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

#include "core/components_ng/layout/layout_property.h"

namespace OHOS::Ace::NG {
void LayoutProperty::Reset() {}
void LayoutProperty::ToJsonValue(std::unique_ptr<JsonValue>& json) const {}

RefPtr<LayoutProperty> LayoutProperty::Clone() const
{
    return nullptr;
}

void LayoutProperty::Clone(RefPtr<LayoutProperty> layoutProperty) const {}
} // namespace OHOS::Ace::NG
