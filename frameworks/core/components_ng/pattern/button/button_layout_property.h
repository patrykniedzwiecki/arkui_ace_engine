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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_BUTTON_BUTTON_LAYOUT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_BUTTON_BUTTON_LAYOUT_PROPERTY_H

#include "base/geometry/dimension.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/text_style.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/property/property.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT ButtonLayoutProperty : public LayoutProperty {
    DECLARE_ACE_TYPE(ButtonLayoutProperty, LayoutProperty);

public:
    ButtonLayoutProperty() = default;

    ~ButtonLayoutProperty() override = default;

    RefPtr<LayoutProperty> Clone() const override
    {
        auto value = MakeRefPtr<ButtonLayoutProperty>();
        value->LayoutProperty::UpdateLayoutProperty(DynamicCast<LayoutProperty>(this));
        value->propType_ = CloneType();
        value->propFontSize_ = CloneFontSize();
        value->propFontWeight_ = CloneFontWeight();
        value->propFontColor_ = CloneFontColor();
        value->propFontFamily_ = CloneFontFamily();
        value->propFontStyle_ = CloneFontStyle();
        value->propLabel_ = CloneLabel();
        value->propBorderRadius_ = CloneBorderRadius();
        return value;
    }

    void Reset() override
    {
        LayoutProperty::Reset();
        ResetType();
        ResetFontSize();
        ResetFontWeight();
        ResetFontColor();
        ResetFontFamily();
        ResetFontStyle();
        ResetLabel();
        ResetBorderRadius();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        LayoutProperty::ToJsonValue(json);

        json->Put("type", ConvertButtonTypeToString(GetType().value_or(ButtonType::CAPSULE)).c_str());
        json->Put("fontSize", GetFontSizeValue(Dimension(0)).ToString().c_str());
        json->Put(
            "fontWeight", V2::ConvertWrapFontWeightToStirng(GetFontWeight().value_or(FontWeight::NORMAL)).c_str());
        json->Put("fontColor", GetFontColor().value_or(Color::BLACK).ColorToString().c_str());
        auto fontFamilyVector = GetFontFamily().value_or<std::vector<std::string>>({ "HarmonyOS Sans" });
        std::string fontFamily = fontFamilyVector.at(0);
        for (uint32_t i = 1; i < fontFamilyVector.size(); ++i) {
            fontFamily += ',' + fontFamilyVector.at(i);
        }
        json->Put("fontFamily", fontFamily.c_str());
        json->Put("fontStyle", GetFontStyle().value_or(Ace::FontStyle::NORMAL) == Ace::FontStyle::NORMAL
                                   ? "FontStyle.Normal"
                                   : "FontStyle.Italic");
        json->Put("label", GetLabelValue("").c_str());
    }

    static std::string ConvertButtonTypeToString(ButtonType buttonType)
    {
        std::string result;
        switch (buttonType) {
            case ButtonType::NORMAL:
                result = "ButtonType.Normal";
                break;
            case ButtonType::CAPSULE:
                result = "ButtonType.Capsule";
                break;
            case ButtonType::CIRCLE:
                result = "ButtonType.Circle";
                break;
            default:
                LOGD("input do not match any ButtonType");
        }
        return result;
    }

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Type, ButtonType, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(FontSize, Dimension, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(FontWeight, Ace::FontWeight, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(FontColor, Color, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(FontFamily, std::vector<std::string>, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(FontStyle, Ace::FontStyle, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Label, std::string, PROPERTY_UPDATE_NORMAL);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(BorderRadius, Dimension, PROPERTY_UPDATE_NORMAL);

private:
    ACE_DISALLOW_COPY_AND_MOVE(ButtonLayoutProperty);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_BUTTON_BUTTON_LAYOUT_PROPERTY_H
