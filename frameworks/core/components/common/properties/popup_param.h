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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_POPUP_PARAM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_POPUP_PARAM_H

#include <string>

#include "base/geometry/dimension.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/common/properties/placement.h"
#include "core/event/ace_event_handler.h"
#include "core/event/touch_event.h"

namespace OHOS::Ace {

struct ButtonProperties {
    bool showButton = false;
    std::string value;
    EventMarker actionId;
    //  touchFunc used in Declarative mode.
    TouchEventFunc touchFunc;
};

using StateChangeFunc = std::function<void(const std::string&)>;

class PopupParam : public AceType {
    DECLARE_ACE_TYPE(PopupParam, AceType)

public:
    PopupParam() = default;
    ~PopupParam() override = default;

    void SetIsShow(bool isShow)
    {
        isShow_ = isShow;
    }

    bool IsShow() const
    {
        return isShow_;
    }

    void SetHasAction(bool hasAction)
    {
        hasAction_ = hasAction;
    }

    bool HasAction() const
    {
        return hasAction_;
    }

    void SetPlacement(const Placement& placement)
    {
        placement_ = placement;
    }

    void SetMaskColor(const Color& maskColor)
    {
        maskColor_ = maskColor;
        isMaskColorSetted_ = true;
    }

    void SetBackgroundColor(const Color& backgroundColor)
    {
        backgroundColor_ = backgroundColor;
        isBackgroundColorSetted_ = true;
    }

    void SetOnVisibilityChange(const EventMarker& onVisibilityChange)
    {
        onVisibilityChange_ = onVisibilityChange;
    }

    Placement GetPlacement() const
    {
        return placement_;
    }

    const Color& GetMaskColor() const
    {
        return maskColor_;
    }

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    const EventMarker& GetOnVisibilityChange() const
    {
        return onVisibilityChange_;
    }

    const Edge& GetMargin() const
    {
        return margin_;
    }

    void SetMargin(const Edge& margin)
    {
        margin_ = margin;
    }

    const Edge& GetTargetMargin() const
    {
        return targetMargin_;
    }

    void SetTargetMargin(const Edge& targetMargin)
    {
        targetMargin_ = targetMargin;
    }

    const Edge& GetPadding() const
    {
        return padding_;
    }

    void SetPadding(const Edge& padding)
    {
        padding_ = padding;
    }

    const Border& GetBorder() const
    {
        return border_;
    }

    void SetBorder(const Border& border)
    {
        border_ = border;
    }

    const std::optional<Dimension>& GetArrowOffset() const
    {
        return arrowOffset_;
    }

    void SetArrowOffset(const std::optional<Dimension>& arrowOffset)
    {
        arrowOffset_ = arrowOffset;
    }

    const ComposeId& GetTargetId() const
    {
        return targetId_;
    }

    void SetTargetId(const ComposeId& targetId)
    {
        targetId_ = targetId;
    }

    bool IsMaskColorSetted() const
    {
        return isMaskColorSetted_;
    }

    bool IsBackgroundColorSetted() const
    {
        return isBackgroundColorSetted_;
    }

    bool EnableArrow() const
    {
        return enableArrow_;
    }

    void SetEnableArrow(bool enableArrow)
    {
        enableArrow_ = enableArrow;
    }

    bool IsUseCustom() const
    {
        return useCustom_;
    }

    void SetUseCustomComponent(bool useCustom)
    {
        useCustom_ = useCustom;
    }

    const Dimension& GetTargetSpace() const
    {
        return targetSpace_;
    }

    void SetTargetSpace(const Dimension& targetSpace)
    {
        targetSpace_ = targetSpace;
    }

    void SetMessage(const std::string& msg)
    {
        message_ = msg;
    }

    std::string GetMessage() const
    {
        return message_;
    }

    StateChangeFunc GetOnStateChange()
    {
        return onStateChange_;
    }

    void SetOnStateChange(StateChangeFunc&& onStateChange)
    {
        onStateChange_ = onStateChange;
    }

    void SetPrimaryButtonProperties(const ButtonProperties& prop)
    {
        primaryButtonProperties_ = prop;
    }

    void SetSecondaryButtonProperties(const ButtonProperties& prop)
    {
        secondaryButtonProperties_ = prop;
    }

    const ButtonProperties& GetPrimaryButtonProperties() const
    {
        return primaryButtonProperties_;
    }

    const ButtonProperties& GetSecondaryButtonProperties() const
    {
        return secondaryButtonProperties_;
    }

    void SetShowInSubWindow(bool isShowInSubWindow)
    {
        isShowInSubWindow_ = isShowInSubWindow;
    }

    bool IsShowInSubWindow() const
    {
        return isShowInSubWindow_;
    }

    void SetTargetSize(const Size& targetSize)
    {
        targetSize_ = targetSize;
    }

    const Size& GetTargetSize() const
    {
        return targetSize_;
    }

    void SetTargetOffset(const Offset& targetOffset)
    {
        targetOffset_ = targetOffset;
    }

    const Offset& GetTargetOffset() const
    {
        return targetOffset_;
    }

private:
    bool isShow_ = true;
    bool hasAction_ = false;
    bool enableArrow_ = true;
    bool isMaskColorSetted_ = false;
    bool isBackgroundColorSetted_ = false;
    bool useCustom_ = false;
    bool isShowInSubWindow_ = false;
    Color maskColor_;
    Color backgroundColor_;
    Placement placement_ = Placement::BOTTOM;
    EventMarker onVisibilityChange_;
    Edge padding_;
    Edge margin_;
    Edge targetMargin_;
    Border border_;
    std::optional<Dimension> arrowOffset_;
    ComposeId targetId_;
    Dimension targetSpace_;
    std::string message_;
    Offset targetOffset_;
    Size targetSize_;

    // Used in NG mode
    StateChangeFunc onStateChange_;
    SubInfoUpdateFunc subInfoUpdate_;
    ButtonProperties primaryButtonProperties_;   // first button.
    ButtonProperties secondaryButtonProperties_; // second button.
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_PROPERTIES_POPUP_PARAM_H
