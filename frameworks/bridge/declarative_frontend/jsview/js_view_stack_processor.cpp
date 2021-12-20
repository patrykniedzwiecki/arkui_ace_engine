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

#include "bridge/declarative_frontend/jsview/js_view_stack_processor.h"

#include "frameworks/bridge/declarative_frontend/engine/bindings.h"
#include "frameworks/bridge/declarative_frontend/engine/js_types.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSViewStackProcessor::JSVisualState(const JSCallbackInfo& info)
{
    LOGD("JSVisualState");
    if (info.Length() < 1) {
        ViewStackProcessor::GetInstance()->ClearVisualState();
        return;
    }

    if (!info[0]->IsString()) {
        LOGE("JSVisualState: is not a string.");
        ViewStackProcessor::GetInstance()->ClearVisualState();
        return;
    }
    std::string state = info[0]->ToString();
    ViewStackProcessor::GetInstance()->SetVisualState(state);
}

// public static emthods exposed to JS
void JSViewStackProcessor::JSBind(BindingTarget globalObj)
{
    LOGD("JSViewStackProcessor::Bind");
    JSClass<JSViewStackProcessor>::Declare("ViewStackProcessor");
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSViewStackProcessor>::StaticMethod("visualState", JSVisualState, opt);
    JSClass<JSViewStackProcessor>::Bind<>(globalObj);
}

} // namespace OHOS::Ace::Framework