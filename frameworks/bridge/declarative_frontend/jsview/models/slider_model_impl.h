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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SLIDER_MODEL_IMPL_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SLIDER_MODEL_IMPL_H

#include "core/components_ng/pattern/slider/slider_model.h"

namespace OHOS::Ace::Framework {

class ACE_EXPORT SliderModelImpl : public SliderModel {
public:
    void Create(float value, float step, float min, float max) override;
    void SetSliderMode(const SliderMode& value) override;
    void SetDirection(Axis value) override;
    void SetReverse(bool value) override;
    void SetBlockColor(const Color& value) override;
    void SetTrackBackgroundColor(const Color& value) override;
    void SetSelectColor(const Color& value) override;
    void SetMinLabel(float value) override;
    void SetMaxLabel(float value) override;
    void SetShowSteps(bool value) override;
    void SetShowTips(bool value, const std::optional<std::string>& content) override;
    void SetThickness(const Dimension& value) override;
    void SetBlockBorderColor(const Color& value) override;
    void SetBlockBorderWidth(const Dimension& value) override;
    void SetStepColor(const Color& value) override;
    void SetTrackBorderRadius(const Dimension& value) override;
    void SetBlockSize(const Size& value) override;
    void SetBlockType(BlockStyleType value) override;
    void SetBlockImage(const std::string& value) override;
    void SetBlockShape(const RefPtr<BasicShape>& value) override;
    void SetStepSize(const Dimension& value) override;
    void SetOnChange(std::function<void(float, int32_t)>&& eventOnChange) override;
    void SetOnChangeEvent(std::function<void(float)>&& onChangeEvent) override {};
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SLIDER_MODEL_IMPL_H
