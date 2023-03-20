/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_LONG_PRESS_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_LONG_PRESS_RECOGNIZER_H

#include "base/thread/cancelable_callback.h"
#include "core/components_ng/gestures/recognizers/multi_fingers_recognizer.h"

#ifdef OHOS_BUILD_ENABLE_COORDINATION
#include "core/components_ng/event/gesture_event_hub.h"
#endif

namespace OHOS::Ace::NG {

#ifdef OHOS_BUILD_ENABLE_COORDINATION
class GestureEventHub;
#endif

class LongPressInfo : public TouchLocationInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LongPressInfo, TouchLocationInfo);

public:
    explicit LongPressInfo(int32_t fingerId) : TouchLocationInfo("onLongPress", fingerId) {}
    ~LongPressInfo() override = default;
};

using OnLongPress = std::function<void(const LongPressInfo&)>;
using LongPressCallback = std::function<void(const GestureEvent&)>;
using LongPressNoParamCallback = std::function<void()>;

class LongPressRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(LongPressRecognizer, MultiFingersRecognizer);

public:
    explicit LongPressRecognizer() = default;
    LongPressRecognizer(
        int32_t duration, int32_t fingers, bool repeat, bool isForDrag = false, bool isDisableMouseLeft = false)
        : MultiFingersRecognizer(fingers), duration_(duration), repeat_(repeat), isForDrag_(isForDrag),
          isDisableMouseLeft_(isDisableMouseLeft)
    {}
    LongPressRecognizer(bool isForDrag = false, bool isDisableMouseLeft = false)
        : isForDrag_(isForDrag), isDisableMouseLeft_(isDisableMouseLeft)
    {}
    ~LongPressRecognizer() override = default;

    void OnAccepted() override;
    void OnRejected() override;

    void SetOnLongPress(const OnLongPress& onLongPress)
    {
        onLongPress_ = onLongPress;
    }

    void SetUseCatchMode(bool useCatchMode)
    {
        useCatchMode_ = useCatchMode;
    }

    void SetDuration(int32_t duration)
    {
        duration_ = duration;
    }

#ifdef OHOS_BUILD_ENABLE_COORDINATION
    void SetGestureHub(WeakPtr<GestureEventHub> gestureHub);
#endif

private:
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    bool ReconcileFrom(const RefPtr<NGGestureRecognizer>& recognizer) override;
    void HandleOverdueDeadline(bool isCatchMode);
    void DeadlineTimer(int32_t time, bool isCatchMode);
    void DoRepeat();
    void StartRepeatTimer();
    void SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback, bool isRepeat);
    void OnResetStatus() override;
    double ConvertPxToVp(double offset) const;
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    void ThumbnailTimer(int32_t time);
    void GetThumbnailPixelMap();

    RefPtr<GestureEventHub> gestureHub_;
    CancelableCallback<void()> thumbnailTimer_;
    int32_t thumbnailDeadline = 150;
#endif
    OnLongPress onLongPress_;
    CancelableCallback<void()> deadlineTimer_;
    CancelableCallback<void()> timer_;
    int32_t duration_ = 500;
    bool repeat_ = false;
    TimeStamp time_;
    bool useCatchMode_ = true;
    bool isForDrag_ = false;
    bool isDisableMouseLeft_ = false;
    Point globalPoint_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_LONG_PRESS_RECOGNIZER_H
