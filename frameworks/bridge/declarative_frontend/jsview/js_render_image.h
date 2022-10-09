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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDER_IMAGE_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDER_IMAGE_H

#include "base/memory/referenced.h"
#include "bridge/declarative_frontend/engine/bindings_defines.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {

class JSRenderImage : public Referenced {
public:
    JSRenderImage();
    ~JSRenderImage() override = default;

    static void JSBind(BindingTarget globalObj);
    static void Constructor(const JSCallbackInfo& args);
    static void Destructor(JSRenderImage* scroller);

    void JsSetWidth(const JSCallbackInfo& info);
    void JsSetHeight(const JSCallbackInfo& info);
    void JsGetWidth(const JSCallbackInfo& info);
    void JsGetHeight(const JSCallbackInfo& info);
    void JsClose(const JSCallbackInfo& info);
    double GetWidth();
    double GetHeight();
    std::string GetSrc();

    ACE_DISALLOW_COPY_AND_MOVE(JSRenderImage);

private:
    std::string src_;
    double width_ = 0;
    double height_ = 0;
};

} // namespace OHOS::Ace::Framework

#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_RENDER_IMAGE_H