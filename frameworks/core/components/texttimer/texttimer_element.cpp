/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/texttimer/texttimer_element.h"

#include "core/components/texttimer/render_texttimer.h"

namespace OHOS::Ace {
void TextTimerElement::PerformBuild()
{
    auto option = AceType::DynamicCast<TextTimerComponent>(component_);
    if (option) {
        option->Initialize();
    }
    SoleChildElement::PerformBuild();
    auto render = AceType::DynamicCast<RenderTextTimer>(renderNode_);
    if (!render) {
        LOGE("render texttimer is null.");
        return;
    }
    render->UpdateRenders();
}
} // namespace OHOS::Ace