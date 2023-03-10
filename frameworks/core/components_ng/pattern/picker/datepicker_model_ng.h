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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_MODEL_NG_VIEW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_MODEL_NG_VIEW_H
#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/components/picker/picker_base_component.h"
#include "core/components_ng/pattern/picker/datepicker_event_hub.h"
#include "core/components_ng/pattern/picker/datepicker_layout_property.h"
#include "core/components_ng/pattern/picker/picker_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT DatePickerModelNG : public DatePickerModel {
public:
    void CreateDatePicker(RefPtr<PickerTheme> theme) override;
    void CreateTimePicker(RefPtr<PickerTheme> theme) override {};
    void SetStartDate(const PickerDate& value) override;
    void SetEndDate(const PickerDate& value) override;
    void SetSelectedDate(const PickerDate& value) override;
    void SetShowLunar(bool lunar) override;
    void SetOnChange(DateChangeEvent&& onChange) override;
    void SetSelectedTime(const PickerTime& selectedTime) override {};
    void SetHour24(bool value) override {};
    void SetDisappearTextStyle(const RefPtr<PickerTheme>& theme, const PickerTextStyle& value) override;
    void SetNormalTextStyle(const RefPtr<PickerTheme>& theme, const PickerTextStyle& value) override;
    void SetSelectedTextStyle(const RefPtr<PickerTheme>& theme, const PickerTextStyle& value) override;
private:
    static RefPtr<FrameNode> CreateStackNode();
    static RefPtr<FrameNode> CreateButtonNode();
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_DATE_PICKER_MODEL_NG_VIEW_H