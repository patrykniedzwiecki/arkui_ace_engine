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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_EVENT_HUB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_EVENT_HUB_H

#include "base/memory/ace_type.h"
#include "core/components_ng/event/event_hub.h"

namespace OHOS::Ace::NG {

using SliderOnChangeEvent = std::function<void(float, int32_t)>;

class SliderEventHub : public EventHub {
    DECLARE_ACE_TYPE(SliderEventHub, EventHub)
public:
    SliderEventHub() = default;
    ~SliderEventHub() override = default;
    void SetOnChange(SliderOnChangeEvent&& changeEvent)
    {
        changeEvent_ = std::move(changeEvent);
    }

    void FireChangeEvent(float value, int32_t mode)
    {
        CHECK_NULL_VOID(changeEvent_);
        changeEvent_(value, mode);
        SetValue(value);
    }

    void SetValue(float value)
    {
        value_ = value;
    }

    float GetValue() const
    {
        return value_;
    }

private:
    SliderOnChangeEvent changeEvent_;
    float value_ = .0f;
    ACE_DISALLOW_COPY_AND_MOVE(SliderEventHub);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_EVENT_HUB_H
