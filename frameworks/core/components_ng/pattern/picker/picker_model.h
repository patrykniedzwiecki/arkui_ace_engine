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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_MODEL_H

#include "core/components/picker/picker_data.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"

namespace OHOS::Ace {
using DateChangeEvent = std::function<void(const BaseEventInfo* info)>;
struct PickerDialogInfo {
    bool isLunar;
    PickerDate parseStartDate;
    PickerDate parseEndDate;
    PickerDate parseSelectedDate;
    PickerTime pickerTime;
    bool isUseMilitaryTime;
    bool isSelectedTime;
    bool isStartDate;
    bool isEndDate;
    bool isSelectedDate;
};
class DatePickerModel {
public:
    static DatePickerModel* GetInstance();
    virtual ~DatePickerModel() = default;

    virtual void CreateDatePicker(RefPtr<PickerTheme> theme) = 0;
    virtual void CreateTimePicker(RefPtr<PickerTheme> theme) = 0;
    virtual void SetStartDate(const PickerDate& value) = 0;
    virtual void SetEndDate(const PickerDate& value) = 0;
    virtual void SetSelectedDate(const PickerDate& value) = 0;
    virtual void SetShowLunar(bool lunar) = 0;
    virtual void SetOnChange(DateChangeEvent&& onChange) = 0;
    virtual void SetSelectedTime(const PickerTime& selectedTime) = 0;
    virtual void SetHour24(bool value) = 0;
    virtual void SetDisappearTextStyle(const RefPtr<PickerTheme>& theme, const NG::PickerTextStyle& value) = 0;
    virtual void SetNormalTextStyle(const RefPtr<PickerTheme>& theme, const NG::PickerTextStyle& value) = 0;
    virtual void SetSelectedTextStyle(const RefPtr<PickerTheme>& theme, const NG::PickerTextStyle& value) = 0;
    virtual void SetBackgroundColor(const Color& color) = 0;
    virtual void SetChangeEvent(DateChangeEvent&& onChange) = 0;

private:
    static std::unique_ptr<DatePickerModel> datePickerInstance_;
};

class DatePickerDialogModel {
public:
    static DatePickerDialogModel* GetInstance();
    virtual ~DatePickerDialogModel() = default;

    virtual void SetDatePickerDialogShow(PickerDialogInfo& pickerDialog, NG::DatePickerSettingData& settingData,
        std::function<void()>&& onCancel, std::function<void(const std::string&)>&& onAccept,
        std::function<void(const std::string&)>&& onChange, DatePickerType pickerType) = 0;

private:
    static std::unique_ptr<DatePickerDialogModel> datePickerDialogInstance_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PICKER_PICKER_MODEL_H
