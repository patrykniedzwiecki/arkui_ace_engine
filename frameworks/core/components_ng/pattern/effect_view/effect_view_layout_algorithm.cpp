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

#include "core/components_ng/pattern/effect_view/effect_view_layout_algorithm.h"

namespace OHOS::Ace::NG {

std::optional<SizeF> EffectViewLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* /*layoutWrapper*/)
{
    OptionalSizeF contentSize;
    do {
        // Use idea size first if it is valid.
        contentSize.UpdateSizeWithCheck(contentConstraint.selfIdealSize);
        if (contentSize.IsValid()) {
            break;
        }
        contentSize.UpdateIllegalSizeWithCheck(contentConstraint.parentIdealSize);
        // use max is parent ideal size is invalid.
        contentSize.UpdateIllegalSizeWithCheck(contentConstraint.maxSize);
    } while (false);

    return contentSize.ConvertToSizeT();
}

} // namespace OHOS::Ace::NG