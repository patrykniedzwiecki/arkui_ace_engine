/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "core/components_ng/pattern/text_picker/textpicker_dialog_view.h"

#include "base/i18n/localization.h"
#include "base/utils/utils.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/dialog/dialog_view.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_column_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
const uint32_t OPTION_COUNT_PHONE_LANDSCAPE = 3;
} // namespace

RefPtr<FrameNode> TextPickerDialogView::Show(const DialogProperties& dialogProperties,
    const TextPickerSettingData& settingData,
    std::map<std::string, NG::DialogTextEvent> dialogEvent,
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent)
{
    auto contentColumn = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));

    auto textNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto textPickerNode = FrameNode::GetOrCreateFrameNode(
        V2::TEXT_PICKER_ETS_TAG, textNodeId, []() { return AceType::MakeRefPtr<TextPickerPattern>(); });
    ViewStackProcessor::GetInstance()->Push(textPickerNode);
    auto textPickerPattern = textPickerNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, nullptr);
    textPickerPattern->SetColumnsKind(settingData.columnKind);

    auto context = textPickerNode->GetContext();
    CHECK_NULL_RETURN(context, nullptr);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_RETURN(themeManager, nullptr);
    auto pickerTheme = themeManager->GetTheme<PickerTheme>();
    CHECK_NULL_RETURN(pickerTheme, nullptr);

    uint32_t showCount = pickerTheme->GetShowOptionCount();
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE &&
        SystemProperties::GetDeviceOrientation() == DeviceOrientation::LANDSCAPE) {
        showCount = OPTION_COUNT_PHONE_LANDSCAPE;
    }
    if (textPickerNode->GetChildren().empty()) {
        auto columnNode = CreateColumnNode(settingData.columnKind, showCount);
        auto stackNode = CreateStackNode();
        auto buttonNode = CreateButtonNode();
        buttonNode->MountToParent(stackNode);
        columnNode->MountToParent(stackNode);
        columnNode->MarkModifyDone();
        columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
        layoutProperty->UpdateAlignment(Alignment::CENTER);
        stackNode->MountToParent(textPickerNode);
    }
    SetRange(textPickerPattern, settingData.rangeVector);
    SetSelected(textPickerPattern, settingData.selected);
    SetDefaultPickerItemHeight(settingData.height);
    SetTextProperties(pickerTheme, settingData.properties);
    auto changeEvent = dialogEvent["changeId"];
    SetDialogChange(textPickerNode, std::move(changeEvent));
    auto contentRow = CreateButtonNode(textPickerNode, dialogEvent, std::move(dialogCancelEvent));
    ViewStackProcessor::GetInstance()->Finish();
    textPickerNode->MountToParent(contentColumn);

    auto dialogNode = DialogView::CreateDialogNode(dialogProperties, contentColumn);
    CHECK_NULL_RETURN(dialogNode, nullptr);
    auto event = [dialogNode](const GestureEvent& /* info */) {
        auto pipeline = PipelineContext::GetCurrentContext();
        auto overlayManager = pipeline->GetOverlayManager();
        overlayManager->CloseDialog(dialogNode);
    };
    for (const auto& child : contentRow->GetChildren()) {
        auto firstChild = AceType::DynamicCast<FrameNode>(child);
        auto gesturHub = firstChild->GetOrCreateGestureEventHub();
        auto onClick = AceType::MakeRefPtr<NG::ClickEvent>(event);
        gesturHub->AddClickEvent(onClick);
    }
    contentRow->AddChild(CreateDividerNode(textPickerNode), 1);
    contentRow->MountToParent(contentColumn);
    dialogNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    return dialogNode;
}

RefPtr<FrameNode> TextPickerDialogView::CreateColumnNode(uint32_t columnKind, uint32_t showCount)
{
    auto columnNode =
        FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPickerColumnPattern>(); });
    if (columnKind == ICON) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto row = FrameNode::CreateFrameNode(
                V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<LinearLayoutPattern>(false));
            CHECK_NULL_RETURN(row, nullptr);
            auto layoutProps = row->GetLayoutProperty<LinearLayoutProperty>();
            CHECK_NULL_RETURN(layoutProps, nullptr);
            layoutProps->UpdateMainAxisAlign(FlexAlign::CENTER);
            layoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);

            auto imageNode = FrameNode::CreateFrameNode(
                V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<ImagePattern>());
            CHECK_NULL_RETURN(imageNode, nullptr);
            imageNode->MountToParent(row);
            row->MountToParent(columnNode);
        }
    } else if (columnKind == TEXT) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto textNode = FrameNode::CreateFrameNode(
                V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<TextPattern>());
            CHECK_NULL_RETURN(textNode, nullptr);
            textNode->MountToParent(columnNode);
        }
    } else if (columnKind == MIXTURE) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto row = FrameNode::CreateFrameNode(
                V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<LinearLayoutPattern>(false));
            CHECK_NULL_RETURN(row, nullptr);

            auto imageNode = FrameNode::CreateFrameNode(
                V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<ImagePattern>());
            CHECK_NULL_RETURN(imageNode, nullptr);
            imageNode->MountToParent(row);

            auto textNode = FrameNode::CreateFrameNode(
                V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<TextPattern>());
            CHECK_NULL_RETURN(textNode, nullptr);
            textNode->MountToParent(row);
            row->MountToParent(columnNode);
        }
    }
    return columnNode;
}

RefPtr<FrameNode> TextPickerDialogView::CreateStackNode()
{
    auto stackId = ElementRegister::GetInstance()->MakeUniqueId();
    return FrameNode::GetOrCreateFrameNode(
        V2::STACK_ETS_TAG, stackId, []() { return AceType::MakeRefPtr<StackPattern>(); });
}

RefPtr<FrameNode> TextPickerDialogView::CreateButtonNode()
{
    auto buttonId = ElementRegister::GetInstance()->MakeUniqueId();
    return FrameNode::GetOrCreateFrameNode(
        V2::BUTTON_ETS_TAG, buttonId, []() { return AceType::MakeRefPtr<ButtonPattern>(); });
}

RefPtr<FrameNode> TextPickerDialogView::CreateDividerNode(const RefPtr<FrameNode>& dateNode)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto dialogTheme = pipeline->GetTheme<DialogTheme>();
    auto dividerNode = FrameNode::GetOrCreateFrameNode(V2::DIVIDER_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<DividerPattern>(); });
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_RETURN(dividerRenderContext, nullptr);
    dividerRenderContext->UpdateBackgroundColor(dialogTheme->GetDividerColor());

    MarginProperty margin;
    margin.top = CalcLength(dialogTheme->GetDividerHeight());
    margin.bottom = CalcLength(dialogTheme->GetDividerPadding().Bottom());
    dividerNode->GetLayoutProperty()->UpdateMargin(margin);
    dividerNode->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(dialogTheme->GetDividerWidth()), CalcLength(dialogTheme->GetDividerHeight())));

    return dividerNode;
}

RefPtr<FrameNode> TextPickerDialogView::CreateButtonNode(const RefPtr<FrameNode>& frameNode,
    std::map<std::string, NG::DialogTextEvent> dialogEvent,
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent)
{
    auto acceptEvent = dialogEvent["acceptId"];
    auto cancelEvent = dialogCancelEvent["cancelId"];
    auto contentRow = FrameNode::CreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    CHECK_NULL_RETURN(contentRow, nullptr);
    auto layoutProps = contentRow->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_RETURN(layoutProps, nullptr);
    layoutProps->UpdateMainAxisAlign(FlexAlign::SPACE_AROUND);
    layoutProps->UpdateMeasureType(MeasureType::MATCH_PARENT_MAIN_AXIS);

    auto buttonCancelNode = CreateCancelNode(cancelEvent);
    auto buttonConfirmNode = CreateConfirmNode(frameNode, acceptEvent);

    buttonCancelNode->MountToParent(contentRow);
    buttonConfirmNode->MountToParent(contentRow);
    return contentRow;
}

RefPtr<FrameNode> TextPickerDialogView::CreateConfirmNode(const RefPtr<FrameNode>& dateNode, DialogEvent& acceptEvent)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto dialogTheme = pipeline->GetTheme<DialogTheme>();
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();

    auto buttonConfirmNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    auto textConfirmNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(buttonConfirmNode, nullptr);
    CHECK_NULL_RETURN(textConfirmNode, nullptr);
    auto textLayoutProperty = textConfirmNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, nullptr);
    textLayoutProperty->UpdateContent(Localization::GetInstance()->GetEntryLetters("common.ok"));
    textLayoutProperty->UpdateTextColor(pickerTheme->GetOptionStyle(true, false).GetTextColor());
    textLayoutProperty->UpdateFontSize(pickerTheme->GetOptionStyle(false, false).GetFontSize());
    textLayoutProperty->UpdateFontWeight(pickerTheme->GetOptionStyle(true, false).GetFontWeight());
    auto buttonConfirmEventHub = buttonConfirmNode->GetEventHub<ButtonEventHub>();
    CHECK_NULL_RETURN(buttonConfirmEventHub, nullptr);
    buttonConfirmEventHub->SetStateEffect(true);

    auto buttonConfirmLayoutProperty = buttonConfirmNode->GetLayoutProperty<ButtonLayoutProperty>();
    buttonConfirmLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT_MAIN_AXIS);
    buttonConfirmLayoutProperty->UpdateType(ButtonType::CAPSULE);
    buttonConfirmLayoutProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(pickerTheme->GetButtonWidth()), CalcLength(pickerTheme->GetButtonHeight())));
    auto buttonConfirmRenderContext = buttonConfirmNode->GetRenderContext();
    buttonConfirmRenderContext->UpdateBackgroundColor(Color::TRANSPARENT);

    MarginProperty margin;
    margin.right = CalcLength(dialogTheme->GetDividerPadding().Right());
    margin.top = CalcLength(dialogTheme->GetDividerHeight());
    margin.bottom = CalcLength(dialogTheme->GetDividerPadding().Bottom());
    buttonConfirmNode->GetLayoutProperty()->UpdateMargin(margin);

    textConfirmNode->MountToParent(buttonConfirmNode);
    auto eventConfirmHub = buttonConfirmNode->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(eventConfirmHub, nullptr);
    CHECK_NULL_RETURN(dateNode, nullptr);
    SetDialogAcceptEvent(dateNode, std::move(acceptEvent));
    auto clickCallback = [dateNode](const GestureEvent& /* info */) {
        auto pickerPattern = dateNode->GetPattern<TextPickerPattern>();
        CHECK_NULL_VOID(pickerPattern);
        auto str = pickerPattern->GetSelectedObject(false);
        auto textPickerEventHub = pickerPattern->GetEventHub<TextPickerEventHub>();
        CHECK_NULL_VOID(textPickerEventHub);
        textPickerEventHub->FireDialogAcceptEvent(str);
    };
    eventConfirmHub->AddClickEvent(AceType::MakeRefPtr<NG::ClickEvent>(clickCallback));
    buttonConfirmNode->MarkModifyDone();
    return buttonConfirmNode;
}

RefPtr<FrameNode> TextPickerDialogView::CreateCancelNode(NG::DialogGestureEvent& cancelEvent)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, nullptr);
    auto dialogTheme = pipeline->GetTheme<DialogTheme>();
    auto pickerTheme = pipeline->GetTheme<PickerTheme>();
    auto buttonCancelNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    CHECK_NULL_RETURN(buttonCancelNode, nullptr);
    auto textCancelNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_RETURN(textCancelNode, nullptr);
    auto textCancelLayoutProperty = textCancelNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textCancelLayoutProperty, nullptr);
    textCancelLayoutProperty->UpdateContent(Localization::GetInstance()->GetEntryLetters("common.cancel"));
    textCancelLayoutProperty->UpdateTextColor(pickerTheme->GetOptionStyle(true, false).GetTextColor());
    textCancelLayoutProperty->UpdateFontSize(pickerTheme->GetOptionStyle(false, false).GetFontSize());
    textCancelLayoutProperty->UpdateFontWeight(pickerTheme->GetOptionStyle(true, false).GetFontWeight());
    textCancelNode->MountToParent(buttonCancelNode);
    auto eventCancelHub = buttonCancelNode->GetOrCreateGestureEventHub();
    CHECK_NULL_RETURN(eventCancelHub, nullptr);
    eventCancelHub->AddClickEvent(AceType::MakeRefPtr<NG::ClickEvent>(std::move(cancelEvent)));

    auto buttonCancelEventHub = buttonCancelNode->GetEventHub<ButtonEventHub>();
    CHECK_NULL_RETURN(buttonCancelEventHub, nullptr);
    buttonCancelEventHub->SetStateEffect(true);

    MarginProperty margin;
    margin.left = CalcLength(dialogTheme->GetDividerPadding().Left());
    margin.top = CalcLength(dialogTheme->GetDividerHeight());
    margin.bottom = CalcLength(dialogTheme->GetDividerPadding().Bottom());
    buttonCancelNode->GetLayoutProperty()->UpdateMargin(margin);

    auto buttonCancelLayoutProperty = buttonCancelNode->GetLayoutProperty<ButtonLayoutProperty>();
    buttonCancelLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT_MAIN_AXIS);
    buttonCancelLayoutProperty->UpdateType(ButtonType::CAPSULE);
    buttonCancelLayoutProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(pickerTheme->GetButtonWidth()), CalcLength(pickerTheme->GetButtonHeight())));

    auto buttonCancelRenderContext = buttonCancelNode->GetRenderContext();
    buttonCancelRenderContext->UpdateBackgroundColor(Color::TRANSPARENT);
    buttonCancelNode->MarkModifyDone();
    return buttonCancelNode;
}

void TextPickerDialogView::SetSelected(const RefPtr<TextPickerPattern>& textPickerPattern, uint32_t value)
{
    textPickerPattern->SetSelected(value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Selected, value);
}

void TextPickerDialogView::SetRange(
    const RefPtr<TextPickerPattern>& textPickerPattern, const std::vector<NG::RangeContent>& value)
{
    textPickerPattern->SetRange(value);
}

void TextPickerDialogView::SetTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const PickerTextProperties& properties)
{
    CHECK_NULL_VOID(pickerTheme);
    auto selectedStyle = pickerTheme->GetOptionStyle(true, false);
    auto disappearStyle = pickerTheme->GetDisappearOptionStyle();
    auto normalStyle = pickerTheme->GetOptionStyle(false, false);

    Color disappearColor = properties.hasValueFlag & FLAG_DISAPPEAR_COLOR ?
        properties.disappearColor_ : disappearStyle.GetTextColor();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearColor, disappearColor);
    Color normalColor = properties.hasValueFlag & FLAG_COLOR ?
        properties.color_ : normalStyle.GetTextColor();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Color, normalColor);
    Color selectedColor = properties.hasValueFlag & FLAG_SELECTED_COLOR ?
        properties.selectedColor_ : selectedStyle.GetTextColor();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedColor, selectedColor);

    Dimension disappearFontSize = (properties.hasValueFlag & FLAG_DISAPPEAR_FONTSIZE) &&
        properties.disappearFontSize_.IsValid() ?
        properties.disappearFontSize_ : disappearStyle.GetFontSize();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontSize, disappearFontSize);
    Dimension normalFontSize = (properties.hasValueFlag & FLAG_FONTSIZE) &&
        properties.fontSize_.IsValid() ?
        properties.fontSize_ : normalStyle.GetFontSize();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize, normalFontSize);
    Dimension selectedFontSize = (properties.hasValueFlag & FLAG_SELECTED_FONTSIZE) &&
        properties.selectedFontSize_.IsValid() ?
        properties.selectedFontSize_ : selectedStyle.GetFontSize();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontSize, selectedFontSize);

    FontWeight disappearWeight = properties.hasValueFlag & FLAG_DISAPPEAR_WEIGHT ?
        properties.disappearWeight_ : disappearStyle.GetFontWeight();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearWeight, disappearWeight);
    FontWeight normalWeight = properties.hasValueFlag & FLAG_WEIGHT ?
        properties.weight_ : normalStyle.GetFontWeight();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Weight, normalWeight);
    FontWeight selectedWeight = properties.hasValueFlag & FLAG_SELECTED_WEIGHT ?
        properties.selectedWeight_ : selectedStyle.GetFontWeight();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedWeight, selectedWeight);
}

void TextPickerDialogView::SetDialogChange(const RefPtr<FrameNode>& frameNode, DialogTextEvent&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetDialogChange(std::move(onChange));
}

void TextPickerDialogView::SetDefaultPickerItemHeight(const Dimension& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultPickerItemHeight, value);
}

void TextPickerDialogView::SetDialogAcceptEvent(const RefPtr<FrameNode>& frameNode, DialogTextEvent&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetDialogAcceptEvent(std::move(onChange));
}

} // namespace OHOS::Ace::NG