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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_PROGRESS_PROGRESS_MODIFIER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_PROGRESS_PROGRESS_MODIFIER_H

#include "base/geometry/arc.h"
#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/color.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/pattern/progress/progress_date.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/animation_utils.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/paint_wrapper.h"

namespace OHOS::Ace::NG {
struct RingProgressData {
    PointF centerPt;
    float radius = 0.0f;
    float thickness = 0.0f;
    float angle = 0.0f;
    float shadowBlurSigma = 0.0f;
};

class ProgressModifier : public ContentModifier {
    DECLARE_ACE_TYPE(ProgressModifier, ContentModifier);

public:
    ProgressModifier();
    ~ProgressModifier() override = default;
    void onDraw(DrawingContext& context) override;

    void SetStrokeWidth(float width);
    void SetColor(LinearColor color);
    void SetBackgroundColor(LinearColor color);
    void SetBorderColor(LinearColor color);
    void SetProgressType(ProgressType type);
    void SetMaxValue(float value);
    void SetValue(float value);
    void SetScaleWidth(float value);
    void SetScaleCount(int32_t value);
    void SetContentOffset(const OffsetF& offset);
    void SetContentSize(const SizeF& contentSize);
    void SetBorderWidth(float width);
    void SetSweepEffect(bool value);
    void SetRingProgressColor(const Gradient& color);
    void SetPaintShadow(bool paintShadow);
    void SetProgressStatus(ProgressStatus status);
    void SetVisible(bool isVisible);
    void SetRingSweepEffect(bool value);
    void SetLinearSweepEffect(bool value);
    void SetIsItalic(bool isItalic);

private:
    void ContentDrawWithFunction(DrawingContext& context);
    void PaintLinear(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintLinearSweeping(RSCanvas& canvas, const OffsetF& offset, const RSPath& path, bool isHorizontal) const;
    void PaintRing(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintRingBackground(RSCanvas& canvas, const RingProgressData& ringProgressData) const;
    void PaintRingProgressOrShadow(RSCanvas& canvas, const RingProgressData& ringProgressData, bool isShadow) const;
    void PaintRingSweeping(RSCanvas& canvas, const RingProgressData& ringProgressData) const;
    void PaintTrailing(RSCanvas& canvas, const RingProgressData& ringProgressData) const;
    void PaintScaleRing(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintMoon(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintCapsule(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintVerticalCapsule(RSCanvas& canvas, const OffsetF& offset, const SizeF& contentSize) const;
    void PaintCapsuleLightSweep(
        RSCanvas& canvas, const SizeF& contentSize, const OffsetF& offset, const RSPath& path, bool isVertical) const;

    void ProcessSweepingAnimation(ProgressType type, float value);
    void ProcessRingSweepingAnimation(float value);
    void ProcessLinearSweepingAnimation(float value);
    void StartRingSweepingAnimation(float value);
    void StartRingSweepingAnimationImpl(float date, float speed);
    void StartContinuousSweepingAnimation(float currentDate, float newDate, float speed);
    void StartLinearSweepingAnimation(float value);
    void StartLinearSweepingAnimationImpl(float date, float speed);
    void StartCapsuleSweepingAnimation(float value);
    void StartCapsuleSweepingAnimationImpl(float value, float speed);
    void StopSweepingAnimation(float date = 0.0f);
    void StartRingLoadingAnimation();
    void StartRingLoadingHeadAnimation();
    void StartRingLoadingTailAnimation();
    void StopRingLoadingHeadAnimation();
    void StopRingLoadingTailAnimation();
    float CalcRingProgressAdditionalAngle() const;
    float TrailingCoefficient(float base, uint32_t index, uint32_t totalPoints) const;
    Color GenerateTrailingPointColor(const Color& headColor, uint32_t index, uint32_t totalPoints) const;
    void GenerateLinearSweepingGradientInfo(std::vector<RSColorQuad>& colors, std::vector<float>& pos) const;
    void GenerateRingSweepingGradientInfo(std::vector<RSColorQuad>& colors, std::vector<float>& pos) const;
    Gradient CreateCapsuleGradient() const;
    bool PostTask(const TaskExecutor::Task& task);
    Gradient SortGradientColorsByOffset(const Gradient& gradient) const;

    // Animatable
    RefPtr<AnimatablePropertyFloat> strokeWidth_; // After adjusting to the content width and height
    RefPtr<AnimatablePropertyColor> color_;
    RefPtr<AnimatablePropertyColor> bgColor_;
    RefPtr<AnimatablePropertyColor> borderColor_;
    RefPtr<AnimatablePropertyFloat> value_;
    RefPtr<AnimatablePropertyVectorColor> ringProgressColors_;
    RefPtr<AnimatablePropertyFloat> sweepingDate_;
    RefPtr<AnimatablePropertyFloat> trailingHeadDate_;
    RefPtr<AnimatablePropertyFloat> trailingTailDate_;

    // no Animatable
    RefPtr<PropertyOffsetF> offset_;
    RefPtr<PropertySizeF> contentSize_;
    RefPtr<PropertyFloat> maxValue_;
    RefPtr<PropertyFloat> scaleWidth_;
    RefPtr<PropertyInt> scaleCount_;
    RefPtr<PropertyInt> progressType_;
    RefPtr<PropertyFloat> capsuleBorderWidth_;
    RefPtr<PropertyBool> sweepEffect_;
    RefPtr<PropertyBool> ringSweepEffect_;
    RefPtr<PropertyBool> linearSweepEffect_;
    RefPtr<PropertyBool> paintShadow_;
    RefPtr<PropertyInt> progressStatus_;
    RefPtr<PropertyBool> isItalic_;

    bool isVisible_ = true;
    float valueBackup_ = 0.0f;
    bool isLoading_ = false;
    bool isSweeping_ = false;
    float sweepingDateBackup_ = 0.0f;
    uint32_t continuousSweepingCounter_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ProgressModifier);
};

class TailingHeadCurve final : public Curve {
    DECLARE_ACE_TYPE(TailingHeadCurve, Curve);

public:
    float MoveInternal(float time) override
    {
        // v = f(time) = sin(PI * time) + PI
        // => F(v) = - cos(PI * t) / PI + PI * time + C
        // => S = F(time) - F(0) = PI * time + 1 / PI - cos(PI * time) / PI
        // After normalizing, S = (PI * time + 1 / PI - cos(PI * time) / PI) / (PI + 2 / PI)
        return (PI_NUM * time + 1.0f / PI_NUM - std::cos(PI_NUM * time) / PI_NUM) / (PI_NUM + 2.0f / PI_NUM);
    }
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_PROGRESS_PROGRESS_MODIFIER_H
