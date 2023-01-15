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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_MODEL_H

#include <functional>
#include <memory>

#include "base/geometry/axis.h"
#include "base/utils/macros.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {
class ACE_EXPORT SliderModel {
public:
    enum class SliderMode {
        OUTSET,  // block on track, track is thin
        INSET,   // block inside track, track is rough
        CAPSULE, // capsule slider.
    };

    static SliderModel* GetInstance();
    virtual ~SliderModel() = default;

    virtual void Create(float value, float step, float min, float max) = 0;
    virtual void SetSliderMode(const SliderMode& value) = 0;
    virtual void SetDirection(Axis value) = 0;
    virtual void SetReverse(bool value) = 0;
    virtual void SetBlockColor(const Color& value) = 0;
    virtual void SetTrackBackgroundColor(const Color& value) = 0;
    virtual void SetSelectColor(const Color& value) = 0;
    virtual void SetMinLabel(float value) = 0;
    virtual void SetMaxLabel(float value) = 0;
    virtual void SetShowSteps(bool value) = 0;
    virtual void SetShowTips(bool value) = 0;
    virtual void SetThickness(const Dimension& value) = 0;
    virtual void SetOnChange(std::function<void(float, int32_t)>&& eventOnChange) = 0;

private:
    static std::unique_ptr<SliderModel> instance_;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SLIDER_SLIDER_MODEL_H
