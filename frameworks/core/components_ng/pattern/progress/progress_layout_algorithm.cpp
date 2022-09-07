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

#include "core/components_ng/pattern/progress/progress_layout_algorithm.h"

#include <algorithm>

#include "base/geometry/dimension.h"
#include "base/log/log_wrapper.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/progress/progress_component.h"
#include "core/components/progress/progress_theme.h"
#include "core/components_ng/pattern/progress/progress_date.h"
#include "core/components_ng/pattern/progress/progress_layout_property.h"

namespace OHOS::Ace::NG {
ProgressLayoutAlgorithm::ProgressLayoutAlgorithm() = default;

std::optional<SizeF> ProgressLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto frameNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(frameNode, std::nullopt);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    auto themeManager = pipeline->GetThemeManager();
    auto progressTheme = themeManager->GetTheme<ProgressTheme>();
    auto progressLayoutProperty = DynamicCast<ProgressLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(progressLayoutProperty, std::nullopt);
    const auto& progressDate = progressLayoutProperty->GetProgressDate();
    const auto& progressStyle = progressLayoutProperty->GetProgressStyle();
    CHECK_NULL_RETURN(progressDate, std::nullopt);
    CHECK_NULL_RETURN(progressStyle, std::nullopt);
    value_ = progressDate->GetValue().value_or(0);
    maxValue_ = progressDate->GetMaxValue().value_or(100);
    color_ = progressStyle->GetColor().value_or(progressTheme ? progressTheme->GetTrackSelectedColor() : Color::BLUE);
    type_ = progressStyle->GetType().value_or(ProgressType::LINEAR);
    strokeWidth_ = progressStyle->GetStrokeWidth()
                       .value_or(progressTheme ? (type_ == ProgressType::SCALE ? progressTheme->GetScaleLength()
                                                                               : progressTheme->GetTrackThickness())
                                               : Dimension(20))
                       .ConvertToPx();
    scaleWidth_ = progressStyle->GetScaleWidth()
                      .value_or(progressTheme ? progressTheme->GetScaleWidth() : Dimension(2))
                      .ConvertToPx();
    scaleCount_ = progressStyle->GetScaleCount().value_or(progressTheme ? progressTheme->GetScaleNumber() : 20);
    double radius = progressTheme ? progressTheme->GetRingRadius().ConvertToPx() : 20;
    double width_ = progressTheme ? progressTheme->GetTrackWidth().ConvertToPx() : contentConstraint.maxSize.Width();
    if (contentConstraint.selfIdealSize.Width()) {
        width_ = contentConstraint.selfIdealSize.Width().value();
    }
    double height_ = strokeWidth_ * 2;
    LOGI("%{public}lf",height_);
    if (contentConstraint.selfIdealSize.Height()) {
        height_ = contentConstraint.selfIdealSize.Height().value();
    }
    if (type_ == ProgressType::RING || type_ == ProgressType::SCALE || type_ == ProgressType::MOON) {
        if (!contentConstraint.selfIdealSize.Width() && !contentConstraint.selfIdealSize.Height()) {
            width_ = 2 * radius;
            height_ = width_;
        }
        if (contentConstraint.selfIdealSize.Width() && !contentConstraint.selfIdealSize.Height()) {
            height_ = width_;
        }
        if (contentConstraint.selfIdealSize.Height() && !contentConstraint.selfIdealSize.Width()) {
            width_ = height_;
        }
    }
    if (type_ == ProgressType::CAPSULE) {
        if (!contentConstraint.selfIdealSize.Height()) {
            height_ = 2 * radius;
        }
        if (!contentConstraint.selfIdealSize.Width()) {
            width_ = 2 * radius;
        }
    }
    height_ = std::min(height_, static_cast<double>(contentConstraint.maxSize.Height()));
    width_ = std::min(width_, static_cast<double>(contentConstraint.maxSize.Width()));
    if (type_ == ProgressType::LINEAR) {
        strokeWidth_ = std::min(strokeWidth_, height_ / 2);
        strokeWidth_ = std::min(strokeWidth_, width_ / 2);
    }
    LOGI("ProgressLayoutAlgorithm::Type:%{public}d MeasureContent: width_: %{public}fl ,height_: %{public}fl", type_,
        width_, height_);
    return SizeF(width_, height_);
}

double ProgressLayoutAlgorithm::GetValue() const
{
    return value_;
}

double ProgressLayoutAlgorithm::GetMaxValue() const
{
    return maxValue_;
}

Color ProgressLayoutAlgorithm::GetColor() const
{
    return color_;
}

ProgressType ProgressLayoutAlgorithm::GetType() const
{
    return type_;
}

double ProgressLayoutAlgorithm::GetStrokeWidth() const
{
    return strokeWidth_;
}

int32_t ProgressLayoutAlgorithm::GetScaleCount() const
{
    return scaleCount_;
}

double ProgressLayoutAlgorithm::GetScaleWidth() const
{
    return scaleWidth_;
}

} // namespace OHOS::Ace::NG
