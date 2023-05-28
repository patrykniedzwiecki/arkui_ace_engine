/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/jsview/js_column_split.h"

#include "core/components_ng/pattern/linear_split/linear_split_model_ng.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_linear_split.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"

namespace OHOS::Ace::Framework {

void JSColumnSplit::Create()
{
    LinearSplitModel::GetInstance()->Create(NG::SplitType::COLUMN_SPLIT);
}

void JSColumnSplit::JsResizeable(bool resizeable)
{
    LinearSplitModel::GetInstance()->SetResizeable(NG::SplitType::COLUMN_SPLIT, resizeable);
}

void JSColumnSplit::JSBind(BindingTarget globalObj)
{
    JSClass<JSColumnSplit>::Declare("ColumnSplit");
    JSClass<JSColumnSplit>::StaticMethod("create", &JSColumnSplit::Create, MethodOptions::NONE);
    JSClass<JSColumnSplit>::StaticMethod("resizeable", &JSColumnSplit::JsResizeable, MethodOptions::NONE);
    JSClass<JSColumnSplit>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSColumnSplit>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSColumnSplit>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSColumnSplit>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSColumnSplit>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSColumnSplit>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSColumnSplit>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSColumnSplit>::StaticMethod("remoteMessage", &JSInteractableView::JsCommonRemoteMessage);
    JSClass<JSColumnSplit>::InheritAndBind<JSContainerBase>(globalObj);
}

} // namespace OHOS::Ace::Framework
