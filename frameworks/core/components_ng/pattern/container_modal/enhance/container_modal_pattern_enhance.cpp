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

#include "core/components_ng/pattern/container_modal/enhance/container_modal_pattern_enhance.h"
#include "base/subwindow/subwindow_manager.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
namespace OHOS::Ace::NG {
namespace {
    constexpr int32_t TITLE_LABEL_INDEX = 1;
    constexpr int32_t MAX_RECOVER_BUTTON_INDEX = 3;
    constexpr int32_t MINIMIZE_BUTTON_INDEX = 4;
    constexpr int32_t CLOSE_BUTTON_INDEX = 5;
    constexpr double UNFOCUS_ALPHA = 0.4;
    constexpr double FOCUS_ALPHA = 1.0;
} // namespace

RefPtr<UINode> ContainerModalPatternEnhance::GetTitleItemByIndex(
    const RefPtr<FrameNode>& titleNode, int32_t originIndex)
{
    if (originIndex > TITLE_LABEL_INDEX) {
        return titleNode->GetChildAtIndex(originIndex - 1);
    } else {
        return titleNode->GetChildAtIndex(originIndex);
    }
}

void ContainerModalPatternEnhance::OnWindowFocused()
{
    LOGD("windowOnFocus refresh window");
    ContainerModalPattern::OnWindowFocused();
}

void ContainerModalPatternEnhance::OnWindowUnfocused()
{
    LOGD("OnWindowUnfocused refresh window");
    if (SubwindowManager::GetInstance()->GetCurrentWindow() &&
        SubwindowManager::GetInstance()->GetCurrentWindow()->GetShown()) {
        return;
    }
    ContainerModalPattern::OnWindowUnfocused();
}

void ContainerModalPatternEnhance::ChangeTitle(const RefPtr<FrameNode>& titleNode, bool isFocus)
{
    CHECK_NULL_VOID(titleNode);
    // update title label
    auto titleLabel = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(titleNode, TITLE_LABEL_INDEX));
    auto textLayoutProperty = titleLabel->GetLayoutProperty<TextLayoutProperty>();
    textLayoutProperty->UpdateTextColor(isFocus ? TITLE_TEXT_COLOR : TITLE_TEXT_COLOR_LOST_FOCUS);
    titleLabel->MarkModifyDone();
    titleLabel->MarkDirtyNode(PROPERTY_UPDATE_RENDER);

    // update maximize button
    auto maximizeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(titleNode, MAX_RECOVER_BUTTON_INDEX));
    auto pipeline = PipelineContext::GetCurrentContext();
    auto windowManager = pipeline->GetWindowManager();
    MaximizeMode mode = windowManager->GetCurrentWindowMaximizeMode();
    InternalResource::ResourceId maxId = mode == MaximizeMode::MODE_AVOID_SYSTEM_BAR ?
        InternalResource::ResourceId::IC_WINDOW_RESTORES : InternalResource::ResourceId::IC_WINDOW_MAX;

    ChangeTitleButtonIcon(maximizeButton, maxId, isFocus);
    // update minimize button
    auto minimizeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(titleNode, MINIMIZE_BUTTON_INDEX));
    ChangeTitleButtonIcon(minimizeButton, InternalResource::ResourceId::IC_WINDOW_MIN, isFocus);

    // update close button
    auto closeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(titleNode, CLOSE_BUTTON_INDEX));
    ChangeTitleButtonIcon(closeButton, InternalResource::ResourceId::IC_WINDOW_CLOSE, isFocus);
}

void ContainerModalPatternEnhance::ChangeFloatingTitle(const RefPtr<FrameNode>& floatingNode, bool isFocus)
{
    CHECK_NULL_VOID(floatingNode);

    // update title label
    auto titleLabel = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(floatingNode, TITLE_LABEL_INDEX));
    auto textLayoutProperty = titleLabel->GetLayoutProperty<TextLayoutProperty>();
    textLayoutProperty->UpdateTextColor(isFocus ? TITLE_TEXT_COLOR : TITLE_TEXT_COLOR_LOST_FOCUS);
    titleLabel->MarkModifyDone();
    titleLabel->MarkDirtyNode(PROPERTY_UPDATE_RENDER);

    // update maxRecover button
    auto maxRecoverIcon = windowMode_ == WindowMode::WINDOW_MODE_FULLSCREEN
                                     ? InternalResource::ResourceId::IC_WINDOW_RESTORES
                                     : InternalResource::ResourceId::IC_WINDOW_MAX;
    auto maxRecover = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(floatingNode, MAX_RECOVER_BUTTON_INDEX));
    ChangeTitleButtonIcon(maxRecover, maxRecoverIcon, isFocus);

    // update minimize button
    auto minimizeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(floatingNode, MINIMIZE_BUTTON_INDEX));
    ChangeTitleButtonIcon(minimizeButton, InternalResource::ResourceId::IC_WINDOW_MIN, isFocus);

    // update close button
    auto closeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(floatingNode, CLOSE_BUTTON_INDEX));
    ChangeTitleButtonIcon(closeButton, InternalResource::ResourceId::IC_WINDOW_CLOSE, isFocus);
}

void ContainerModalPatternEnhance::ChangeTitleButtonIcon(
    const RefPtr<FrameNode>& buttonNode, InternalResource::ResourceId icon, bool isFocus)
{
    auto renderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateOpacity(isFocus ? FOCUS_ALPHA : UNFOCUS_ALPHA);
    ContainerModalPattern::ChangeTitleButtonIcon(buttonNode, icon, isFocus);
}
} // namespace OHOS::Ace::NG