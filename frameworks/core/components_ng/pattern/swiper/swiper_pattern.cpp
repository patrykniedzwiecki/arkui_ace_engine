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

#include "core/components_ng/pattern/swiper/swiper_pattern.h"
#include <cmath>
#include <stdint.h>

#include "base/geometry/axis.h"
#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/animation/curve.h"
#include "core/animation/curves.h"
#include "core/components/scroll/scrollable.h"
#include "core/components_ng/pattern/swiper/swiper_layout_algorithm.h"
#include "core/components_ng/pattern/swiper/swiper_layout_property.h"
#include "core/components_ng/property/property.h"
#include "core/event/touch_event.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {

constexpr Dimension MIN_TURN_PAGE_VELOCITY = 10.0_vp;
constexpr Dimension MIN_DRAG_DISTANCE = 25.0_vp;

} // namespace

void SwiperPattern::OnAttachToFrameNode()
{
    auto host = frameNode_.Upgrade();
    CHECK_NULL_VOID(host);
    host->GetRenderContext()->SetClipToFrame(true);
}

void SwiperPattern::OnModifyDone()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);

    InitPanEvent(gestureHub);
    InitTouchEvent(gestureHub);
    InitAutoPlay();
}

bool SwiperPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout)
{
    if (skipMeasure && skipLayout) {
        return false;
    }
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);
    auto swiperLayoutAlgorithm = DynamicCast<SwiperLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(swiperLayoutAlgorithm, false);
    startIndex_ = swiperLayoutAlgorithm->GetStartIndex();
    endIndex_ = swiperLayoutAlgorithm->GetEndIndex();
    return false;
}

void SwiperPattern::InitPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    auto actionStartTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        LOGI("Pan event start");
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleDragStart();
        }
    };

    auto actionUpdateTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->UpdateCurrentOffset(static_cast<float>(info.GetMainDelta()));
        }
    };

    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        LOGI("Pan event end mainVelocity: %{public}lf", info.GetMainVelocity());
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleDragEnd(info.GetMainVelocity());
        }
    };

    auto actionCancelTask = [weak = WeakClaim(this)]() {
        LOGI("Pan event cancel");
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleDragEnd(0.0);
        }
    };

    PanDirection panDirection;
    if (GetDirection() == Axis::VERTICAL) {
        panDirection.type = PanDirection::VERTICAL;
    } else {
        panDirection.type = PanDirection::HORIZONTAL;
    }
    if (panEvent_) {
        gestureHub->RemovePanEvent(panEvent_);
    }

    float distance = DEFAULT_PAN_DISTANCE;
    auto host = GetHost();
    if (host) {
        auto context = host->GetContext();
        if (context) {
            distance = static_cast<float>(context->NormalizeToPx(Dimension(DEFAULT_PAN_DISTANCE, DimensionUnit::VP)));
        }
    }
    panEvent_ = MakeRefPtr<PanEvent>(actionStartTask, actionUpdateTask, actionEndTask, actionCancelTask);
    gestureHub->AddPanEvent(panEvent_, panDirection, 1, distance);
}

void SwiperPattern::InitTouchEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleTouchEvent(info);
        }
    };

    if (touchEvent_) {
        gestureHub->RemoveTouchEvent(touchEvent_);
    }
    touchEvent_ = MakeRefPtr<TouchEventImpl>(touchTask);
    gestureHub->AddTouchEvent(touchEvent_);
}

void SwiperPattern::InitAutoPlay()
{
    auto weak = AceType::WeakClaim(this);
    if (!scheduler_) {
        auto&& callback = [weak](uint64_t duration) {
            auto swiper = weak.Upgrade();
            if (swiper) {
                swiper->Tick(duration);
            }
        };
        scheduler_ = SchedulerBuilder::Build(callback, GetHost()->GetContext());
    } else if (scheduler_->IsActive()) {
        scheduler_->Stop();
    }

    if (IsAutoPlay() && !scheduler_->IsActive()) {
        scheduler_->Start();
    }
}

void SwiperPattern::Tick(uint64_t duration)
{
    elapsedTime_ += duration;
    if (elapsedTime_ >= GetInterval()) {
        if (currentIndex_ >= static_cast<int32_t>(GetHost()->GetChildren().size()) - 1 && !IsLoop()) {
            LOGD("already last one, stop auto play because not loop");
            if (scheduler_) {
                scheduler_->Stop();
            }
        } else {
            auto host = GetHost();
            CHECK_NULL_VOID(host);
            auto geometryNode = host->GetGeometryNode();
            CHECK_NULL_VOID(geometryNode);
            auto mainSize = geometryNode->GetFrameSize().MainSize(GetDirection());
            PlayTranslateAnimation(0, -mainSize, currentIndex_ + 1);
        }
        elapsedTime_ = 0;
    }
}

void SwiperPattern::UpdateCurrentOffset(float offset)
{
    currentOffset_ = currentOffset_ + offset;
    auto host = frameNode_.Upgrade();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_REQUEST_NEW_CHILD_NODE);
}

void SwiperPattern::HandleTouchEvent(const TouchEventInfo& info)
{
    if (info.GetTouches().front().GetTouchType() == TouchType::DOWN) {
        if (controller_ && !controller_->IsStopped()) {
            // Clear stop listener before stop, otherwise the previous swipe will be considered complete.
            controller_->ClearStopListeners();
            controller_->Stop();
        }
    }
}

void SwiperPattern::HandleDragStart()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);

    currentOffset_ = std::fmod(currentOffset_, geometryNode->GetFrame().GetRect().MainSize(GetDirection()));
}

void SwiperPattern::HandleDragEnd(double dragVelocity)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto mainSize = geometryNode->GetFrameSize().MainSize(GetDirection());

    int32_t nextIndex = currentIndex_;
    float start = currentOffset_;
    float end = 0.0;
    if (std::abs(dragVelocity) > context->NormalizeToPx(MIN_TURN_PAGE_VELOCITY) &&
        std::abs(currentOffset_) > context->NormalizeToPx(MIN_DRAG_DISTANCE)) {
        // If dragVelocity if greater than zero, show previous item, otherwise show next items.
        end = GreatNotEqual(dragVelocity, 0.0) ? mainSize : -mainSize;
        nextIndex = GreatNotEqual(dragVelocity, 0.0) ? (nextIndex -1) : (nextIndex + 1);
    }

    PlayTranslateAnimation(start, end, nextIndex);
}

void SwiperPattern::PlayTranslateAnimation(float startPos, float endPos, int32_t nextIndex)
{
    LOGI("Play translate animation startPos: %{public}lf, endPos: %{public}lf, nextIndex: %{public}d", startPos, endPos, nextIndex);
    auto curve = GetCurve();
    if (!curve) {
        curve = Curves::LINEAR;
    }
    auto translate = AceType::MakeRefPtr<CurveAnimation<double>>(startPos, endPos, curve);
    auto weak = AceType::WeakClaim(this);
    translate->AddListener(Animation<double>::ValueCallback([weak, startPos, endPos](double value) {
        auto swiper = weak.Upgrade();
        CHECK_NULL_VOID(swiper);
        if (value != startPos && value != endPos && startPos != endPos) {
            double moveRate = Curves::EASE_OUT->MoveInternal(static_cast<float>((value - startPos) / (endPos - startPos)));
            value = startPos + (endPos - startPos) * moveRate;
        }
        swiper->UpdateCurrentOffset(static_cast<float>(value - swiper->currentOffset_));
    }));

    if (!controller_) {
        controller_ = AceType::MakeRefPtr<Animator>(GetHost()->GetContext());
    }
    controller_->ClearStopListeners();
    controller_->ClearInterpolators();
    controller_->AddStopListener([weak, nextIndex]() {
        auto swiper = weak.Upgrade();
        CHECK_NULL_VOID(swiper);
        swiper->currentOffset_ = 0.0;
        swiper->currentIndex_ = nextIndex;
    });

    controller_->SetDuration(GetDuration());
    controller_->AddInterpolator(translate);
    controller_->Play();
}

Axis SwiperPattern::GetDirection() const
{
    auto swiperLayoutProperty = GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_RETURN(swiperLayoutProperty, Axis::HORIZONTAL);
    return swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
}

bool SwiperPattern::IsAutoPlay() const
{
    auto swiperPaintProperty = GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, false);
    return swiperPaintProperty->GetAutoPlay().value_or(false);
}

int32_t SwiperPattern::GetDuration() const
{
    static const int32_t DEFAULT_DURATION = 400;
    auto swiperPaintProperty = GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, DEFAULT_DURATION);
    return swiperPaintProperty->GetDuration().value_or(DEFAULT_DURATION);
}

int32_t SwiperPattern::GetInterval() const
{
    static const int32_t DEFAULT_INTERVAL = 3000;
    auto swiperPaintProperty = GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, DEFAULT_INTERVAL);
    return swiperPaintProperty->GetAutoPlayInterval().value_or(DEFAULT_INTERVAL);
}

RefPtr<Curve> SwiperPattern::GetCurve() const
{
    auto swiperPaintProperty = GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, nullptr);
    return swiperPaintProperty->GetCurve().value_or(nullptr);
}

bool SwiperPattern::IsLoop() const
{
    static const int32_t DEFAULT_DURATION = 400;
    auto swiperPaintProperty = GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, DEFAULT_DURATION);
    return swiperPaintProperty->GetLoop().value_or(false);
}

} // namespace OHOS::Ace::NG
