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

#include "bridge/declarative_frontend/jsview/js_canvas_gradient.h"
#include "bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSCanvasGradient::JSCanvasGradient() : isColorStopValid_(true) {}

void JSCanvasGradient::Constructor(const JSCallbackInfo& args)
{
    auto jsCanvasGradient = Referenced::MakeRefPtr<JSCanvasGradient>();
    jsCanvasGradient->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsCanvasGradient));
}

void JSCanvasGradient::Destructor(JSCanvasGradient* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
        delete controller->GetGradient();
    }
}

void JSCanvasGradient::JSBind(BindingTarget globalObj)
{
    JSClass<JSCanvasGradient>::Declare("CanvasGradient");
    JSClass<JSCanvasGradient>::CustomMethod("addColorStop", &JSCanvasGradient::addColorStop);
    JSClass<JSCanvasGradient>::Bind(globalObj, JSCanvasGradient::Constructor, JSCanvasGradient::Destructor);
}

void JSCanvasGradient::addColorStop(const JSCallbackInfo& info)
{
    if (!isColorStopValid_ && gradient_->GetColors().empty()) {
        isColorStopValid_ = true;
    }
    if (isColorStopValid_ && info[0]->IsNumber() && info[1]->IsString()) {
        double offset = 0.0;
        JSViewAbstract::ParseJsDouble(info[0], offset);
        if (offset < 0 || offset > 1) {
            LOGE("offset not valid!");
            isColorStopValid_ = false;
            // if the offset is invalid, fill the shape with transparent
            gradient_->ClearColors();
            GradientColor color;
            color.SetColor(Color::TRANSPARENT);
            color.SetDimension(0.0);
            gradient_->AddColor(color);
            gradient_->AddColor(color);
            return;
        }
        std::string jsColor;
        GradientColor color;
        JSViewAbstract::ParseJsString(info[1], jsColor);
        color.SetColor(Color::FromString(jsColor));
        color.SetDimension(offset);
        if (gradient_) {
            gradient_->AddColor(color);
        }
        auto colorSize = gradient_->GetColors().size();
        // prevent setting only one colorStop
        if (colorSize == 1) {
            gradient_->AddColor(color);
        }
    }
}

} // namespace OHOS::Ace::Framework
