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

#include "core/components_ng/pattern/marquee/marquee_pattern.h"

#include <math.h>
#include <string>

#include "base/geometry/dimension.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/offset.h"
#include "base/utils/utils.h"
#include "core/animation/curves.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/animation_option.h"
#include "core/components/common/properties/color.h"
#include "core/components/text/text_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/marquee/marquee_layout_property.h"
#include "core/components_ng/pattern/marquee/marquee_paint_property.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/calc_length.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/property/transition_property.h"
#include "core/components_ng/render/animation_utils.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr double DEFAULT_MARQUEE_SCROLL_DELAY = 85.0; // Delay time between each jump.
constexpr float HALF = 0.5f;
inline constexpr int32_t DEFAULT_MARQUEE_LOOP = -1;
} // namespace

void MarqueePattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->GetRenderContext()->SetClipToFrame(true);
}

bool MarqueePattern::OnDirtyLayoutWrapperSwap(
    const RefPtr<LayoutWrapper>& /* dirty */, const DirtySwapConfig& /* config */)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    if (measureChanged_) {
        measureChanged_ = false;
        auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
        CHECK_NULL_RETURN(paintProperty, false);
        auto playStatus = paintProperty->GetPlayerStatus().value_or(false);
        StopMarqueeAnimation(playStatus);
    }
    return false;
}

void MarqueePattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty<MarqueeLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto textChild = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textChild);
    auto textLayoutProperty = textChild->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    auto src = layoutProperty->GetSrc().value_or(" ");
    textLayoutProperty->UpdateContent(src);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID_NOLOG(pipelineContext);
    auto theme = pipelineContext->GetTheme<TextTheme>();
    CHECK_NULL_VOID_NOLOG(theme);
    auto fontSize = layoutProperty->GetFontSize().value_or(theme->GetTextStyle().GetFontSize());
    textLayoutProperty->UpdateFontSize(fontSize);
    textLayoutProperty->UpdateFontWeight(layoutProperty->GetFontWeight().value_or(FontWeight::NORMAL));
    if (layoutProperty->GetFontFamily().has_value()) {
        textLayoutProperty->UpdateFontFamily(layoutProperty->GetFontFamily().value());
    } else {
        textLayoutProperty->ResetFontFamily();
    }
    textLayoutProperty->UpdateTextColor(layoutProperty->GetFontColor().value_or(theme->GetTextStyle().GetTextColor()));
    textChild->MarkModifyDone();
    textChild->MarkDirtyNode();
    if (CheckMeasureFlag(layoutProperty->GetPropertyChangeFlag()) ||
        CheckLayoutFlag(layoutProperty->GetPropertyChangeFlag())) {
        measureChanged_ = true;
    } else if (OnlyPlayStatusChange()) {
        ChangeAnimationPlayStatus();
    } else {
        auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
        CHECK_NULL_VOID(paintProperty);
        auto playStatus = paintProperty->GetPlayerStatus().value_or(false);
        StopMarqueeAnimation(playStatus);
    }
    StoreProperties();
    host->MarkDirtyNode();
    RegistVisibleAreaChangeCallback();
}

void MarqueePattern::StartMarqueeAnimation()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geoNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geoNode);
    auto marqueeSize = geoNode->GetFrameSize();
    auto textNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textNode);
    auto textGeoNode = textNode->GetGeometryNode();
    CHECK_NULL_VOID(textGeoNode);
    auto textWidth = textGeoNode->GetFrameSize().Width();
    auto layoutProperty = host->GetLayoutProperty<MarqueeLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    const auto& padding = layoutProperty->CreatePaddingAndBorder();
    if (GreatOrEqual(marqueeSize.Width(), textWidth)) {
        return;
    }
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto repeatCount = paintProperty->GetLoop().value_or(DEFAULT_MARQUEE_LOOP);
    FireStartEvent();
    auto direction = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
    bool needSecondPlay = false;
    if (repeatCount != 1) {
        needSecondPlay = LessNotEqual(textGeoNode->GetMarginFrameOffset().GetX(), 0.0f);
        if (direction == MarqueeDirection::RIGHT) {
            needSecondPlay = GreatNotEqual(
                textGeoNode->GetMarginFrameOffset().GetX(), marqueeSize.Width() - textGeoNode->GetFrameSize().Width());
        }
    }
    PlayMarqueeAnimation(0.0f, repeatCount, needSecondPlay);
}

void MarqueePattern::PlayMarqueeAnimation(float start, int32_t playCount, bool needSecondPlay)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geoNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geoNode);
    auto marqueeSize = geoNode->GetFrameSize();
    auto textNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textNode);
    auto textGeoNode = textNode->GetGeometryNode();
    CHECK_NULL_VOID(textGeoNode);
    auto textWidth = textGeoNode->GetFrameSize().Width();
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto step = paintProperty->GetScrollAmount().value_or(DEFAULT_MARQUEE_SCROLL_AMOUNT.ConvertToPx());
    auto direction = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
    auto end = CalculateEnd();
    auto duration = static_cast<int32_t>(std::abs(end - start) * DEFAULT_MARQUEE_SCROLL_DELAY);
    if (GreatNotEqual(step, 0.0)) {
        duration = static_cast<int32_t>(duration / step);
    }
    AnimationOption option;
    option.SetCurve(Curves::LINEAR);
    option.SetDuration(duration);
    if (needSecondPlay) {
        option.SetIteration(1);
    } else {
        option.SetIteration(playCount);
    }
    SetTextOffset(start);
    animationId_++;
    animation_ = AnimationUtils::StartAnimation(
        option,
        [weak = AceType::WeakClaim(this), end]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->SetTextOffset(end);
        },
        [weak = AceType::WeakClaim(this), animationId = animationId_, needSecondPlay, playCount]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            if (animationId != pattern->animationId_) {
                return;
            }
            if (!needSecondPlay) {
                pattern->OnAnimationFinish();
                return;
            }
            auto newPlayCount = playCount > 0 ? playCount - 1 : playCount;
            if (newPlayCount == 0) {
                return;
            }
            auto newStart = pattern->CalculateStart();
            pattern->PlayMarqueeAnimation(newStart, newPlayCount, false);
        },
        [weak = AceType::WeakClaim(this)]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->FireBounceEvent();
        });
}

void MarqueePattern::OnAnimationFinish()
{
    FireFinishEvent();
    SetTextOffset(0.0f);
}

void MarqueePattern::StopMarqueeAnimation(bool stopAndStart)
{
    animation_ = nullptr;
    AnimationOption option;
    option.SetCurve(Curves::LINEAR);
    option.SetDuration(0);
    AnimationUtils::Animate(
        option,
        [weak = AceType::WeakClaim(this)]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->SetTextOffset(0.0f);
        },
        [weak = AceType::WeakClaim(this), restart = stopAndStart]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            if (restart) {
                pattern->StartMarqueeAnimation();
            }
        });
}

void MarqueePattern::FireStartEvent() const
{
    auto marqueeEventHub = GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(marqueeEventHub);
    marqueeEventHub->FireStartEvent();
}

void MarqueePattern::FireBounceEvent() const
{
    auto marqueeEventHub = GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(marqueeEventHub);
    marqueeEventHub->FireBounceEvent();
}

void MarqueePattern::FireFinishEvent() const
{
    auto marqueeEventHub = GetEventHub<MarqueeEventHub>();
    CHECK_NULL_VOID(marqueeEventHub);
    marqueeEventHub->FireFinishEvent();
}

void MarqueePattern::SetTextOffset(float offsetX)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto textNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textNode);
    auto renderContext = textNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateTransformTranslate({ offsetX, 0.0f, 0.0f });
}

void MarqueePattern::RegistVisibleAreaChangeCallback()
{
    if (isRegistedAreaCallback_) {
        return;
    }
    isRegistedAreaCallback_ = true;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto callback = [weak = WeakClaim(this)](bool visible, double ratio) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->OnVisibleAreaChange(visible);
    };
    pipeline->AddVisibleAreaChangeNode(host, 0.0f, callback, false);
}

void MarqueePattern::OnVisibleAreaChange(bool visible)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    CHECK_NULL_VOID(animation_);
    if (visible) {
        AnimationUtils::ResumeAnimation(animation_);
    } else {
        AnimationUtils::PauseAnimation(animation_);
    }
}

bool MarqueePattern::OnlyPlayStatusChange()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto playStatus = paintProperty->GetPlayerStatus().value_or(false);
    auto scrollAmount = paintProperty->GetScrollAmount().value_or(DEFAULT_MARQUEE_SCROLL_AMOUNT.ConvertToPx());
    auto loop = paintProperty->GetLoop().value_or(DEFAULT_MARQUEE_LOOP);
    auto direction = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
    if (!NearEqual(scrollAmount_, scrollAmount) || loop_ != loop || direction_ != direction) {
        return false;
    }
    if (playStatus_ != playStatus) {
        return true;
    }
    return false;
}

void MarqueePattern::ChangeAnimationPlayStatus()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto playStatus = paintProperty->GetPlayerStatus().value_or(false);
    if (playStatus) {
        if (!animation_) {
            StartMarqueeAnimation();
            return;
        }
        AnimationUtils::ResumeAnimation(animation_);
    } else {
        CHECK_NULL_VOID(animation_);
        AnimationUtils::PauseAnimation(animation_);
    }
}

void MarqueePattern::StoreProperties()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    playStatus_ = paintProperty->GetPlayerStatus().value_or(false);
    scrollAmount_ = paintProperty->GetScrollAmount().value_or(DEFAULT_MARQUEE_SCROLL_AMOUNT.ConvertToPx());
    loop_ = paintProperty->GetLoop().value_or(DEFAULT_MARQUEE_LOOP);
    direction_ = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
}

float MarqueePattern::CalculateStart()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geoNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geoNode);
    auto marqueeSize = geoNode->GetFrameSize();
    auto textNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textNode);
    auto textGeoNode = textNode->GetGeometryNode();
    CHECK_NULL_VOID(textGeoNode);
    auto textWidth = textGeoNode->GetFrameSize().Width();
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto direction = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
    auto layoutProperty = host->GetLayoutProperty<MarqueeLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    Alignment align = Alignment::CENTER_LEFT;
    if (layoutProperty->GetPositionProperty()) {
        align = layoutProperty->GetPositionProperty()->GetAlignment().value_or(align);
    }
    const auto& padding = layoutProperty->CreatePaddingAndBorder();
    auto start = marqueeSize.Width();
    if (direction == MarqueeDirection::LEFT) {
        if (NearEqual(align.GetHorizontal(), -1.0)) {
            start = marqueeSize.Width() - padding.left.value_or(0);
        } else if (NearEqual(align.GetHorizontal(), 0.0)) {
            start = (marqueeSize.Width() + textWidth) * HALF;
        } else {
            start = textWidth + padding.right.value_or(0);
        }
    } else {
        if (NearEqual(align.GetHorizontal(), -1.0)) {
            start = -1 * textWidth - padding.left.value_or(0);
        } else if (NearEqual(align.GetHorizontal(), 0.0)) {
            start = -1 * (marqueeSize.Width() + textWidth) * HALF;
        } else {
            start = -1 * marqueeSize.Width() + padding.right.value_or(0);
        }
    }
    return start;
}

float MarqueePattern::CalculateEnd()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geoNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geoNode);
    auto marqueeSize = geoNode->GetFrameSize();
    auto textNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(textNode);
    auto textGeoNode = textNode->GetGeometryNode();
    CHECK_NULL_VOID(textGeoNode);
    auto textWidth = textGeoNode->GetFrameSize().Width();
    auto paintProperty = host->GetPaintProperty<MarqueePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto layoutProperty = host->GetLayoutProperty<MarqueeLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto direction = paintProperty->GetDirection().value_or(MarqueeDirection::LEFT);
    const auto& padding = layoutProperty->CreatePaddingAndBorder();

    Alignment align = Alignment::CENTER_LEFT;
    if (layoutProperty->GetPositionProperty()) {
        align = layoutProperty->GetPositionProperty()->GetAlignment().value_or(align);
    }
    float end = 0.0f;
    if (direction == MarqueeDirection::LEFT) {
        if (NearEqual(align.GetHorizontal(), -1.0)) {
            end = -1 * textWidth - padding.left.value_or(0);
        } else if (NearEqual(align.GetHorizontal(), 0.0)) {
            end = -1 * (marqueeSize.Width() + textWidth) * HALF;
        } else {
            end = -1 * marqueeSize.Width() + padding.right.value_or(0);
        }
    } else {
        if (NearEqual(align.GetHorizontal(), -1.0)) {
            end = marqueeSize.Width() - padding.left.value_or(0);
        } else if (NearEqual(align.GetHorizontal(), 0.0)) {
            end = (marqueeSize.Width() + textWidth) * HALF;
        } else {
            end = textWidth + padding.right.value_or(0);
        }
    }
    return end;
}
} // namespace OHOS::Ace::NG
