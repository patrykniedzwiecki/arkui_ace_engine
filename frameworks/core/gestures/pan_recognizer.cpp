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

#include "core/gestures/pan_recognizer.h"

#include "base/geometry/offset.h"
#include "base/log/log.h"
#include "core/event/axis_event.h"
#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t MAX_PAN_FINGERS = 10;
constexpr double DISTANCE_PER_MOUSE_DEGREE = DP_PER_LINE_DESKTOP * LINE_NUMBER_DESKTOP / MOUSE_WHEEL_DEGREES;
constexpr int32_t AXIS_PAN_FINGERS = 1;
constexpr int32_t RATIO_MS_TO_S = 1000;
constexpr int32_t RATIO_US_TO_MS = 1000;

} // namespace

void PanRecognizer::OnAccepted()
{
    SendCallbackMsg(onActionStart_);

    if (pendingEnd_) {
        LOGD("pan gesture recognizer has pending end event when waiting to be accepted");
        SendCallbackMsg(onActionEnd_);
        Reset();
    } else if (pendingCancel_) {
        SendCancelMsg();
        Reset();
    }
}

void PanRecognizer::OnRejected()
{
    LOGD("pan gesture has been rejected!");
    Reset();
}

void PanRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    LOGD("pan recognizer receives touch down event, begin to detect pan event");
    fingers_ = newFingers_;
    distance_ = newDistance_;
    direction_ = newDirection_;

    if (fingers_ > MAX_PAN_FINGERS) {
        return;
    }

    if (direction_.type == PanDirection::NONE) {
        return;
    }

    deviceId_ = event.deviceId;
    deviceType_ = event.sourceType;
    touchPoints_[event.id] = event;

    if (state_ == DetectState::READY) {
        AddToReferee(event.id, AceType::Claim(this));
        if (static_cast<int32_t>(refereePointers_.size()) == fingers_) {
            state_ = DetectState::DETECTING;
        }
    }
}

void PanRecognizer::HandleTouchDownEvent(const AxisEvent& event)
{
    LOGD("pan recognizer receives touch down event, begin to detect pan event");
    if (fingers_ != AXIS_PAN_FINGERS) {
        return;
    }

    if (direction_.type == PanDirection::NONE) {
        return;
    }

    deviceId_ = event.deviceId;
    deviceType_ = event.sourceType;
    lastAxisEvent_ = event;

    if (state_ == DetectState::READY) {
        state_ = DetectState::DETECTING;
    }
}

void PanRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    LOGD("pan recognizer receives touch up event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    globalPoint_ = Point(event.x, event.y);
    touchPoints_.erase(itr);

    if (state_ == DetectState::READY) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (state_ == DetectState::DETECTING) {
        size_t inRefereeNum = refereePointers_.size();
        bool inReferee = IsInReferee(static_cast<size_t>(event.id));
        if (inReferee) {
            inRefereeNum--;
        }

        if (static_cast<int32_t>(touchPoints_.size()) < fingers_ || inRefereeNum < 1) {
            LOGD("this gesture is not pan, try to reject it");
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
            return;
        }

        if (inReferee) {
            DelFromReferee(event.id, AceType::Claim(this));
        }
        return;
    }

    if (static_cast<int32_t>(touchPoints_.size()) < fingers_) {
        if (refereeState_ == RefereeState::SUCCEED) {
            SendCallbackMsg(onActionEnd_);
            Reset();
        } else {
            pendingEnd_ = true;
        }
    }
}

void PanRecognizer::HandleTouchUpEvent(const AxisEvent& event)
{
    LOGD("pan recognizer receives touch up event");
    if (fingers_ != AXIS_PAN_FINGERS) {
        return;
    }
    globalPoint_ = Point(event.x, event.y);

    if (state_ != DetectState::DETECTED) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    SendCallbackMsg(onActionEnd_);
    Reset();
    pendingEnd_ = true; // TODO: Need confirm
}

void PanRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("pan recognizer receives touch move event");
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    globalPoint_ = Point(event.x, event.y);
    if (state_ == DetectState::READY) {
        touchPoints_[event.id] = event;
        return;
    }

    delta_ = (event.GetOffset() - touchPoints_[event.id].GetOffset()) / touchPoints_.size();
    mainDelta_ = GetMainAxisDelta();
    auto timeInterval = event.time - touchPoints_[event.id].time;
    auto timeIntervalDurationMs =
        std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, RATIO_US_TO_MS>>>(timeInterval);
    speed_ = delta_.GetDistance() / timeIntervalDurationMs.count() * RATIO_MS_TO_S;
    mainSpeed_ = mainDelta_ / timeIntervalDurationMs.count() * RATIO_MS_TO_S;
    averageDistance_ += delta_;
    touchPoints_[event.id] = event;
    time_ = event.time;

    if (state_ == DetectState::DETECTING) {
        auto result = IsPanGestureAccept();
        if (result == GestureAcceptResult::ACCEPT) {
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        } else if (result == GestureAcceptResult::REJECT) {
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        }
    } else if (state_ == DetectState::DETECTED && refereeState_ == RefereeState::SUCCEED) {
        if ((direction_.type & PanDirection::VERTICAL) == 0) {
            averageDistance_.SetY(0.0);
        } else if ((direction_.type & PanDirection::HORIZONTAL) == 0) {
            averageDistance_.SetX(0.0);
        }

        SendCallbackMsg(onActionUpdate_);
    }
}

void PanRecognizer::HandleTouchMoveEvent(const AxisEvent& event)
{
    LOGD("pan recognizer receives touch move event");
    if (fingers_ != AXIS_PAN_FINGERS) {
        return;
    }
    globalPoint_ = Point(event.x, event.y);
    if (state_ == DetectState::READY) {
        lastAxisEvent_ = event;
        return;
    }

    delta_ = Offset(-event.horizontalAxis * DISTANCE_PER_MOUSE_DEGREE,
        -event.verticalAxis * DISTANCE_PER_MOUSE_DEGREE);
    mainDelta_ = GetMainAxisDelta();
    speed_ = 0.0;
    mainSpeed_ = 0.0;
    averageDistance_ += delta_;
    lastAxisEvent_ = event;
    time_ = event.time;

    if (state_ == DetectState::DETECTING) {
        auto result = IsPanGestureAccept();
        if (result == GestureAcceptResult::ACCEPT) {
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        } else if (result == GestureAcceptResult::REJECT) {
            Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        }
    }
    if (state_ == DetectState::DETECTED) {
        if ((direction_.type & PanDirection::VERTICAL) == 0) {
            averageDistance_.SetY(0.0);
        } else if ((direction_.type & PanDirection::HORIZONTAL) == 0) {
            averageDistance_.SetX(0.0);
        }

        SendCallbackMsg(onActionUpdate_);
    }
}

void PanRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("pan recognizer receives touch cancel event");
    if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
        LOGD("cancel pan gesture detect, try to reject it");
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCancelMsg();
        Reset();
    } else {
        pendingCancel_ = true;
    }
}

void PanRecognizer::HandleTouchCancelEvent(const AxisEvent& event)
{
    if (fingers_ != AXIS_PAN_FINGERS) {
        return;
    }
    return HandleTouchCancelEvent(TouchEvent {});
}

PanRecognizer::GestureAcceptResult PanRecognizer::IsPanGestureAccept() const
{
    if ((direction_.type & PanDirection::ALL) == PanDirection::ALL) {
        double offset = averageDistance_.GetDistance();
        if (fabs(offset) < distance_) {
            return GestureAcceptResult::DETECTING;
        }
        return GestureAcceptResult::ACCEPT;
    }

    if (fabs(averageDistance_.GetX()) > fabs(averageDistance_.GetY())) {
        if ((direction_.type & PanDirection::HORIZONTAL) != 0) {
            double offset = averageDistance_.GetX();
            if (fabs(offset) < distance_) {
                return GestureAcceptResult::DETECTING;
            }
            if ((direction_.type & PanDirection::LEFT) == 0 && offset < 0) {
                return GestureAcceptResult::REJECT;
            }
            if ((direction_.type & PanDirection::RIGHT) == 0 && offset > 0) {
                return GestureAcceptResult::REJECT;
            }
            return GestureAcceptResult::ACCEPT;
        }
        return GestureAcceptResult::DETECTING;
    } else {
        if ((direction_.type & PanDirection::VERTICAL) != 0) {
            double offset = averageDistance_.GetY();
            if (fabs(offset) < distance_) {
                return GestureAcceptResult::DETECTING;
            }
            if ((direction_.type & PanDirection::UP) == 0 && offset < 0) {
                return GestureAcceptResult::REJECT;
            }
            if ((direction_.type & PanDirection::DOWN) == 0 && offset > 0) {
                return GestureAcceptResult::REJECT;
            }
            return GestureAcceptResult::ACCEPT;
        }
        return GestureAcceptResult::DETECTING;
    }
}

void PanRecognizer::Reset()
{
    touchPoints_.clear();
    averageDistance_.Reset();
    state_ = DetectState::READY;
    pendingEnd_ = false;
    pendingCancel_ = false;
}

void PanRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback)
{
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetOffsetX(averageDistance_.GetX());
        info.SetOffsetY(averageDistance_.GetY());
        info.SetGlobalPoint(globalPoint_);
        info.SetDeviceId(deviceId_);
        info.SetSourceDevice(deviceType_);
        info.SetDelta(delta_);
        info.SetMainDelta(mainDelta_);
        info.SetSpeed(speed_);
        info.SetMainSpeed(mainSpeed_);
        (*callback)(info);
    }
}

bool PanRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<PanRecognizer> curr = AceType::DynamicCast<PanRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->fingers_ != fingers_ || curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    direction_.type = curr->direction_.type;
    newDirection_.type = curr->newDirection_.type;
    distance_ = curr->distance_;
    newDistance_ = curr->newDistance_;

    onActionStart_ = std::move(curr->onActionStart_);
    onActionUpdate_ = std::move(curr->onActionUpdate_);
    onActionEnd_ = std::move(curr->onActionEnd_);
    onActionCancel_ = std::move(curr->onActionCancel_);

    return true;
}

void PanRecognizer::ChangeFingers(int32_t fingers)
{
    if (fingers_ != fingers) {
        newFingers_ = fingers;
    }
}

void PanRecognizer::ChangeDirection(const PanDirection& direction)
{
    if (direction_.type != direction.type) {
        direction_.type = direction.type;
        newDirection_.type = direction.type;
    }
}

void PanRecognizer::ChangeDistance(double distance)
{
    if (distance_ != distance) {
        if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
            distance_ = distance;
        }
        newDistance_ = distance;
    }
}

double PanRecognizer::GetMainAxisDelta()
{
    switch (direction_.type) {
        case PanDirection::ALL:
            return delta_.GetDistance();
        case PanDirection::HORIZONTAL:
            return delta_.GetX();
        case PanDirection::VERTICAL:
            return delta_.GetY();
        default:
            return 0.0;
    }
}

} // namespace OHOS::Ace
