/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H

#include <optional>
#include <string>
#include <utility>

#include "base/geometry/dimension.h"
#include "base/geometry/size.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/xcomponent/native_interface_xcomponent_impl.h"
#include "core/components/xcomponent/resource/native_texture.h"
#include "core/components/xcomponent/xcomponent_controller.h"
#include "core/components_ng/event/input_event.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_event_hub.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_layout_algorithm.h"
#include "core/components_ng/pattern/xcomponent/xcomponent_layout_property.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/render_surface.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
class XComponentPattern : public Pattern {
    DECLARE_ACE_TYPE(XComponentPattern, Pattern);

public:
    XComponentPattern() = default;
    XComponentPattern(const std::string& id, XComponentType type, const std::string& libraryname,
        const RefPtr<XComponentController>& xcomponentController);
    ~XComponentPattern() override = default;

    bool IsAtomicNode() const override
    {
        return type_ == XComponentType::SURFACE;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<XComponentLayoutProperty>();
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<XComponentEventHub>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<XComponentLayoutAlgorithm>();
    }

    std::pair<RefPtr<OHOS::Ace::NativeXComponentImpl>, OH_NativeXComponent*> GetNativeXComponent()
    {
        if (!nativeXComponent_ || !nativeXComponentImpl_) {
            if (nativeXComponent_) {
                delete nativeXComponent_;
                nativeXComponent_ = nullptr;
            }
            nativeXComponentImpl_ = AceType::MakeRefPtr<NativeXComponentImpl>();
            nativeXComponent_ = new OH_NativeXComponent(AceType::RawPtr(nativeXComponentImpl_));
        }
        return std::make_pair(nativeXComponentImpl_, nativeXComponent_);
    }

    void NativeXComponentInit()
    {
        CHECK_NULL_VOID(nativeXComponentImpl_);
        CHECK_NULL_VOID(nativeXComponent_);
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto pipelineContext = host->GetContext();
        CHECK_NULL_VOID(pipelineContext);
        auto geometryNode = host->GetGeometryNode();
        CHECK_NULL_VOID(geometryNode);
        auto width = geometryNode->GetContentSize().Width();
        auto height = geometryNode->GetContentSize().Height();

        pipelineContext->GetTaskExecutor()->PostTask(
            [nXCompImpl = nativeXComponentImpl_, nXComp = nativeXComponent_, width, height] {
                if (nXComp && nXCompImpl) {
                    nXCompImpl->SetXComponentWidth(static_cast<int>(width));
                    nXCompImpl->SetXComponentHeight(static_cast<int>(height));
                    auto* surface = const_cast<void*>(nXCompImpl->GetSurface());
                    const auto* callback = nXCompImpl->GetCallback();
                    if (callback && callback->OnSurfaceCreated != nullptr) {
                        callback->OnSurfaceCreated(nXComp, surface);
                    }
                } else {
                    LOGE("Native XComponent nullptr");
                }
            },
            TaskExecutor::TaskType::JS);
    }

    void OnPaint();
    void NativeXComponentOffset(double x, double y);
    void NativeXComponentChange(float width, float height);
    void NativeXComponentDestroy();
    void NativeXComponentDispatchTouchEvent(const OH_NativeXComponent_TouchEvent& touchEvent);
    void NativeXComponentDispatchMouseEvent(const OH_NativeXComponent_MouseEvent& mouseEvent);

    void XComponentSizeInit(float textureWidth, float textureHeight);
    void XComponentSizeChange(float textureWidth, float textureHeight);

    void* GetNativeWindow()
    {
        return renderSurface_->GetNativeWindow();
    }

    const std::string& GetId() const
    {
        return id_;
    }

    const std::string& GetLibraryName() const
    {
        return libraryname_;
    }

    const std::optional<std::string>& GetSoPath() const
    {
        return soPath_;
    }

    void SetSoPath(const std::string& soPath)
    {
        soPath_ = soPath;
    }

private:
    void OnAttachToFrameNode() override;
    void OnDetachFromFrameNode(FrameNode* frameNode) override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;
    void OnRebuildFrame() override;

    void InitEvent();
    void InitTouchEvent(const RefPtr<GestureEventHub>& gestureHub);
    void HandleTouchEvent(const TouchEventInfo& info);
    void InitMouseEvent(const RefPtr<InputEventHub>& inputHub);
    void HandleMouseEvent(const MouseInfo& info);
    void InitMouseHoverEvent(const RefPtr<InputEventHub>& inputHub);
    void HandleMouseHoverEvent(bool isHover);
    ExternalEvent CreateExternalEvent();
    void CreateSurface();
    void SetMethodCall();
    void ConfigSurface(uint32_t surfaceWidth, uint32_t surfaceHeight);
    void SetTouchPoint(
        const std::list<TouchLocationInfo>& touchInfoList, int64_t timeStamp, const TouchType& touchType);

    std::string id_;
    XComponentType type_;
    std::string libraryname_;
    RefPtr<XComponentController> xcomponentController_;
    std::optional<std::string> soPath_;

    RefPtr<RenderSurface> renderSurface_;
    RefPtr<RenderContext> renderContextForSurface_;

    OH_NativeXComponent* nativeXComponent_ = nullptr;
    RefPtr<NativeXComponentImpl> nativeXComponentImpl_;

    bool hasXComponentInit_ = false;

    RefPtr<TouchEventImpl> touchEvent_;
    OH_NativeXComponent_TouchEvent touchEventPoint_;
    RefPtr<InputEvent> mouseEvent_;
    RefPtr<InputEvent> mouseHoverEvent_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_XCOMPONENT_XCOMPONENT_PATTERN_H
