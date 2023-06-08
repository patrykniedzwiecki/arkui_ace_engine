/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "core/components_ng/pattern/window_scene/scene/window_pattern.h"

#include "session_manager/include/scene_session_manager.h"
#include "ui/rs_surface_node.h"

#include "base/utils/system_properties.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {
namespace {
constexpr uint32_t COLOR_BLACK = 0xff000000;
constexpr uint32_t COLOR_WHITE = 0xffffffff;
} // namespace

class LifecycleListener : public Rosen::ILifecycleListener {
public:
    explicit LifecycleListener(const WeakPtr<WindowPattern>& windowPattern)
        : windowPattern_(windowPattern) {}
    virtual ~LifecycleListener() = default;

    void OnConnect() override
    {
        auto windowPattern = windowPattern_.Upgrade();
        CHECK_NULL_VOID(windowPattern);
        windowPattern->OnConnect();
    }

    void OnForeground() override
    {
        auto windowPattern = windowPattern_.Upgrade();
        CHECK_NULL_VOID(windowPattern);
        windowPattern->OnForeground();
    }

    void OnBackground() override
    {
        auto windowPattern = windowPattern_.Upgrade();
        CHECK_NULL_VOID(windowPattern);
        windowPattern->OnBackground();
    }

private:
    WeakPtr<WindowPattern> windowPattern_;
};

WindowPattern::WindowPattern()
{
    instanceId_ = Container::CurrentId();
}

void WindowPattern::RegisterLifecycleListener()
{
    CHECK_NULL_VOID(session_);
    lifecycleListener_ = std::make_shared<LifecycleListener>(WeakClaim(this));
    session_->RegisterLifecycleListener(lifecycleListener_);
}

void WindowPattern::UnregisterLifecycleListener()
{
    CHECK_NULL_VOID(session_);
    session_->UnregisterLifecycleListener(lifecycleListener_);
}

void WindowPattern::InitContent()
{
    contentNode_ = FrameNode::CreateFrameNode(
        V2::WINDOW_SCENE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    contentNode_->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);

    CHECK_NULL_VOID(session_);
    auto surfaceNode = session_->GetSurfaceNode();
    if (surfaceNode) {
        auto context = AceType::DynamicCast<NG::RosenRenderContext>(contentNode_->GetRenderContext());
        CHECK_NULL_VOID(context);
        context->SetRSNode(surfaceNode);
    }

    auto state = session_->GetSessionState();
    LOGI("Session state is %{public}u.", state);
    switch (state) {
        case Rosen::SessionState::STATE_DISCONNECT: {
            CreateStartingNode();
            break;
        }
        case Rosen::SessionState::STATE_BACKGROUND: {
            CreateSnapshotNode();
            break;
        }
        default: {
            auto host = GetHost();
            CHECK_NULL_VOID(host);
            host->AddChild(contentNode_);
            break;
        }
    }
}

void WindowPattern::CreateStartingNode()
{
    if (CreatePersistentNode() || !HasStartingPage()) {
        return;
    }

    auto host = GetHost();
    CHECK_NULL_VOID(host);

    startingNode_ = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    auto imageLayoutProperty = startingNode_->GetLayoutProperty<ImageLayoutProperty>();
    imageLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    host->AddChild(startingNode_);

    std::string startPagePath;
    auto backgroundColor = SystemProperties::GetColorMode() == ColorMode::DARK ? COLOR_BLACK : COLOR_WHITE;
    auto sessionInfo = session_->GetSessionInfo();
    Rosen::SceneSessionManager::GetInstance().GetStartPage(sessionInfo, startPagePath, backgroundColor);
    LOGI("start page path %{public}s, background color %{public}x", startPagePath.c_str(), backgroundColor);

    startingNode_->GetRenderContext()->UpdateBackgroundColor(Color(backgroundColor));
    imageLayoutProperty->UpdateImageSourceInfo(
        ImageSourceInfo(startPagePath, sessionInfo.bundleName_, sessionInfo.moduleName_));
    imageLayoutProperty->UpdateImageFit(ImageFit::NONE);
    startingNode_->MarkModifyDone();
}

void WindowPattern::CreateSnapshotNode()
{
    snapshotNode_ = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    auto imageLayoutProperty = snapshotNode_->GetLayoutProperty<ImageLayoutProperty>();
    imageLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    auto backgroundColor = SystemProperties::GetColorMode() == ColorMode::DARK ? COLOR_BLACK : COLOR_WHITE;
    snapshotNode_->GetRenderContext()->UpdateBackgroundColor(Color(backgroundColor));

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->AddChild(snapshotNode_);

    CHECK_NULL_VOID(session_);
    auto snapshot = session_->GetSnapshot();
    auto pixelMap = PixelMap::CreatePixelMap(&snapshot);

    CHECK_NULL_VOID(pixelMap);
    imageLayoutProperty->UpdateImageSourceInfo(ImageSourceInfo(pixelMap));
    imageLayoutProperty->UpdateImageFit(ImageFit::FILL);
    snapshotNode_->MarkModifyDone();
}

void WindowPattern::OnConnect()
{
    ContainerScope scope(instanceId_);
    CHECK_NULL_VOID(session_);
    auto surfaceNode = session_->GetSurfaceNode();
    CHECK_NULL_VOID(surfaceNode);

    CHECK_NULL_VOID(contentNode_);
    auto context = AceType::DynamicCast<NG::RosenRenderContext>(contentNode_->GetRenderContext());
    CHECK_NULL_VOID(context);
    context->SetRSNode(surfaceNode);

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->AddChild(contentNode_, 0);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);

    if (!HasStartingPage()) {
        return;
    }
    surfaceNode->SetBufferAvailableCallback([weak = WeakClaim(this)]() {
        LOGI("RSSurfaceNode buffer available callback");
        auto windowPattern = weak.Upgrade();
        CHECK_NULL_VOID(windowPattern);
        windowPattern->BufferAvailableCallback();
    });
}

void WindowPattern::BufferAvailableCallback()
{
    ContainerScope scope(instanceId_);

    auto host = GetHost();
    CHECK_NULL_VOID(host);

    host->RemoveChild(startingNode_);
    startingNode_.Reset();
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

void WindowPattern::OnAttachToFrameNode()
{
    InitContent();

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

bool WindowPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (!config.frameSizeChange) {
        LOGI("frame size not changed, just return");
        return false;
    }
    CHECK_NULL_RETURN(dirty, false);
    auto geometryNode = dirty->GetGeometryNode();
    auto windowRect = geometryNode->GetFrameRect();
    Rosen::WSRect rect = {
        .posX_ = std::round(windowRect.GetX()),
        .posY_ = std::round(windowRect.GetY()),
        .width_ = std::round(windowRect.Width()),
        .height_ = std::round(windowRect.Height())
    };

    CHECK_NULL_RETURN(session_, false);
    session_->UpdateRect(rect, Rosen::SizeChangeReason::UNDEFINED);
    return false;
}

void WindowPattern::DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    CHECK_NULL_VOID(session_);
    session_->TransferPointerEvent(pointerEvent);
}

void WindowPattern::DispatchKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    CHECK_NULL_VOID(session_);
    session_->TransferKeyEvent(keyEvent);
}

bool WindowPattern::CreatePersistentNode()
{
    CHECK_NULL_RETURN(session_, false);
    snapshotNode_ = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    auto imageLayoutProperty = snapshotNode_->GetLayoutProperty<ImageLayoutProperty>();
    imageLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);

    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    host->AddChild(snapshotNode_);
    if (session_->GetScenePersistence() != nullptr && session_->GetScenePersistence()->IsSnapshotExisted()) {
        imageLayoutProperty->UpdateImageSourceInfo(
            ImageSourceInfo(std::string("file:/").append(session_->GetScenePersistence()->GetSnapshotFilePath())));
        imageLayoutProperty->UpdateImageFit(ImageFit::COVER);
        snapshotNode_->MarkModifyDone();
        return true;
    }
    return false;
}
} // namespace OHOS::Ace::NG
