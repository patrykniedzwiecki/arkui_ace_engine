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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_PATTERN_TEXTFIELD_TEXTFIELD_TEST_NG_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_PATTERN_TEXTFIELD_TEXTFIELD_TEST_NG_UTILS_H

#include "core/components_ng/base/view_stack_processor.h"

namespace OHOS::Ace::NG {
const std::string PLACEHOLDER = "DEFAULT PLACEHOLDER";
const std::string EMPTY_TEXT_VALUE;
class TextFieldTestNgUtils {
public:
    static RefPtr<FrameNode> CreatTextFieldNode(const std::optional<std::string>& placeholder = PLACEHOLDER,
        const std::optional<std::string>& value = EMPTY_TEXT_VALUE, bool isTextArea = false);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_PATTERN_TEXTFIELD_TEXTFIELD_TEST_NG_UTILS_H