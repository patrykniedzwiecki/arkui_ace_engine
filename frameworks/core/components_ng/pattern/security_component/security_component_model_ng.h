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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SECURITY_COMPONENT_SECURITY_COMPONENT_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SECURITY_COMPONENT_SECURITY_COMPONENT_MODEL_NG_H

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_abstract_model.h"
#include "core/components_ng/pattern/security_component/security_component_common.h"
#include "core/components_ng/pattern/security_component/security_component_theme.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/text_style.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT SecurityComponentModelNG {
public:
    virtual ~SecurityComponentModelNG() = default;
    virtual void Create(int32_t text, int32_t icon,
        int32_t backgroundType) = 0;
    void CreateCommon(const std::string& tag, int32_t text, int32_t icon,
        int32_t backgroundType,
        const std::function<RefPtr<Pattern>(void)>& patternCreator);
    static void SetIconSize(const Dimension& value);
    static void SetIconColor(const Color& value);
    static void SetFontSize(const Dimension& value);
    static void SetFontStyle(const Ace::FontStyle& value);
    static void SetFontWeight(const FontWeight& value);
    static void SetFontFamily(const std::vector<std::string>& fontFamilies);
    static void SetFontColor(const Color& value);
    static void SetBackgroundColor(const Color& value);
    static void SetBackgroundBorderWidth(const Dimension& value);
    static void SetBackgroundBorderColor(const Color& value);
    static void SetBackgroundBorderStyle(const BorderStyle& value);
    static void SetBackgroundBorderRadius(const Dimension& value);
    static void SetBackgroundPadding(const std::optional<Dimension>& left, const std::optional<Dimension>& right,
        const std::optional<Dimension>& top, const std::optional<Dimension>& bottom);
    static void SetBackgroundPadding(const std::optional<Dimension>& padding);
    static void SetTextIconSpace(const Dimension& value);
    static void SetTextIconLayoutDirection(const SecurityComponentLayoutDirection& value);
    static void SetlayoutOrder(const SecSecurityComponentLayoutOrder& value);

    virtual bool GetIconResource(int32_t iconStyle, InternalResource::ResourceId& id)
    {
        return false;
    }

    virtual bool GetTextResource(int32_t textStyle,  std::string& text)
    {
        return false;
    }

protected:
    static RefPtr<SecurityComponentTheme> GetTheme();

private:
    static void SetDefaultIconStyle(const RefPtr<FrameNode>& imageNode, InternalResource::ResourceId id,
        bool isButtonVisible);
    static void SetInvisibleBackgroundButton(const RefPtr<FrameNode>& buttonNode);
    static bool IsBackgroundVisible();
    static void SetDefaultBackgroundButton(const RefPtr<FrameNode>& buttonNode,
        int32_t type);
    static void SetDefaultTextStyle(const RefPtr<FrameNode>& textNode, const std::string& text, bool isButtonVisible);
    static void InitLayoutProperty(RefPtr<FrameNode>& node, int32_t text, int32_t icon,
        int32_t backgroundType);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SECURITY_COMPONENT_SECURITY_COMPONENT_MODEL_NG_H
