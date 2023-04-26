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

#include "core/components_ng/pattern/slider/slider_paint_method.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float HALF = 0.5f;
} // namespace

void SliderPaintMethod::UpdateContentModifier(PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(sliderContentModifier_);
    auto paintProperty = DynamicCast<SliderPaintProperty>(paintWrapper->GetPaintProperty());
    CHECK_NULL_VOID(paintProperty);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto sliderTheme = pipeline->GetTheme<SliderTheme>();
    CHECK_NULL_VOID(sliderTheme);
    sliderContentModifier_->UpdateData(parameters_);
    sliderContentModifier_->JudgeNeedAimate(paintProperty);
    sliderContentModifier_->SetBackgroundSize(parameters_.backStart, parameters_.backEnd);
    sliderContentModifier_->SetSelectSize(parameters_.selectStart, parameters_.selectEnd);
    sliderContentModifier_->SetCircleCenter(parameters_.circleCenter);
    sliderContentModifier_->SetSelectColor(parameters_.selectColor);
    sliderContentModifier_->SetTrackBackgroundColor(parameters_.trackBackgroundColor);
    sliderContentModifier_->SetBlockColor(parameters_.blockColor);
    sliderContentModifier_->SetTrackThickness(parameters_.trackThickness);
    sliderContentModifier_->SetStepRatio(parameters_.stepRatio);
    sliderContentModifier_->SetBoardColor();
    sliderContentModifier_->SetSliderMode(paintProperty->GetSliderModeValue(SliderModelNG::SliderMode::OUTSET));
    auto trackBorderRadius = paintProperty->GetTrackBorderRadiusValue(Dimension(parameters_.trackThickness * HALF));
    if (GreatOrEqual(trackBorderRadius.ConvertToPx(), parameters_.trackThickness * HALF)) {
        trackBorderRadius = Dimension(parameters_.trackThickness * HALF);
    }
    sliderContentModifier_->SetTrackBorderRadius(trackBorderRadius.ConvertToPx());
    auto stepSize = paintProperty->GetStepSizeValue(sliderTheme->GetMarkerSize());
    sliderContentModifier_->SetStepSize(stepSize.ConvertToPx());
    auto stepColor = paintProperty->GetStepColorValue(sliderTheme->GetMarkerColor());
    sliderContentModifier_->SetStepColor(stepColor);
    sliderContentModifier_->SetShowSteps(paintProperty->GetShowStepsValue(false));
    auto blockSize = paintProperty->GetBlockSizeValue(parameters_.blockSize);
    if (paintProperty->GetSliderModeValue(SliderModelNG::SliderMode::OUTSET) != SliderModelNG::SliderMode::OUTSET) {
        blockSize = SizeF(std::min(blockSize.Width(), parameters_.trackThickness),
            std::min(blockSize.Height(), parameters_.trackThickness));
    }
    sliderContentModifier_->SetBlockSize(blockSize);
    sliderContentModifier_->SetBlockBorderColor(paintProperty->GetBlockBorderColorValue(Color::TRANSPARENT));
    sliderContentModifier_->SetBlockBorderWidth(paintProperty->GetBlockBorderWidthValue(Dimension()).ConvertToPx());
    sliderContentModifier_->SetBlockType(paintProperty->GetBlockTypeValue(SliderModelNG::BlockStyleType::DEFAULT));
    sliderContentModifier_->SetBlockShape(paintProperty->GetBlockShapeValue(MakeRefPtr<BasicShape>()));
    sliderContentModifier_->SetDirection(paintProperty->GetDirectionValue(Axis::HORIZONTAL));
}

void SliderPaintMethod::UpdateOverlayModifier(PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(sliderTipModifier_);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SliderTheme>();
    CHECK_NULL_VOID(theme);

    auto paintProperty = DynamicCast<SliderPaintProperty>(paintWrapper->GetPaintProperty());
    if (paintProperty) {
        sliderTipModifier_->SetDirection(paintProperty->GetDirectionValue(Axis::HORIZONTAL));
        sliderTipModifier_->SetTipColor(paintProperty->GetTipColorValue(theme->GetTipColor()));
        sliderTipModifier_->SetTextFont(paintProperty->GetFontSizeValue(theme->GetTipFontSize()));
        sliderTipModifier_->SetTextColor(paintProperty->GetTextColorValue(theme->GetTipTextColor()));
        sliderTipModifier_->SetContent(paintProperty->GetCustomContent().value_or(paintProperty->GetContentValue("")));
    }
    sliderTipModifier_->SetTipFlag(tipParameters_.isDrawTip_);
    sliderTipModifier_->SetContentOffset(paintWrapper->GetContentOffset());
    sliderTipModifier_->SetContentSize(paintWrapper->GetContentSize());
    sliderTipModifier_->SetCircleCenter(tipParameters_.circleCenter_);
}
} // namespace OHOS::Ace::NG
