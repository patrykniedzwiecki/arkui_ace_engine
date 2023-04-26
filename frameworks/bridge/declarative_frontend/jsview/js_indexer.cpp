/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/js_indexer.h"

#include "base/geometry/dimension.h"
#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_scroller.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"
#include "bridge/declarative_frontend/jsview/models/indexer_model_impl.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components_ng/pattern/indexer/indexer_event_hub.h"
#include "core/components_ng/pattern/indexer/indexer_model_ng.h"
#include "core/components_ng/pattern/indexer/indexer_theme.h"
#include "core/components_ng/pattern/indexer/indexer_view.h"
#include "core/components_v2/indexer/indexer_component.h"
#include "core/components_v2/indexer/indexer_event_info.h"
#include "core/components_v2/indexer/render_indexer.h"

namespace OHOS::Ace {
std::unique_ptr<IndexerModel> IndexerModel::instance_ = nullptr;
std::mutex IndexerModel::mutex_;
IndexerModel* IndexerModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::IndexerModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::IndexerModelNG());
            } else {
                instance_.reset(new Framework::IndexerModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}
} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {
namespace {
const std::vector<FontStyle> FONT_STYLES = { FontStyle::NORMAL, FontStyle::ITALIC };
const std::vector<V2::AlignStyle> ALIGN_STYLE = { V2::AlignStyle::LEFT, V2::AlignStyle::RIGHT };
const std::vector<NG::AlignStyle> NG_ALIGN_STYLE = { NG::AlignStyle::LEFT, NG::AlignStyle::RIGHT };
}; // namespace

void JSIndexer::Create(const JSCallbackInfo& args)
{
    if (args.Length() >= 1 && args[0]->IsObject()) {
        size_t length = 0;
        int32_t selectedVal = 0;
        auto param = JsonUtil::ParseJsonString(args[0]->ToString());
        std::unique_ptr<JsonValue> arrayVal;
        if (param && !param->IsNull()) {
            arrayVal = param->GetValue("arrayValue");
            if (arrayVal && arrayVal->IsArray()) {
                length = static_cast<uint32_t>(arrayVal->GetArraySize());
            }
            selectedVal = param->GetInt("selected", 0);
        }
        std::vector<std::string> indexerArray;
        for (size_t i = 0; i < length; i++) {
            auto value = arrayVal->GetArrayItem(i);
            if (!value) {
                return;
            }
            indexerArray.emplace_back(value->GetString());
        }

        IndexerModel::GetInstance()->Create(indexerArray, selectedVal);
        IndexerModel::GetInstance()->SetFocusable(true);
        IndexerModel::GetInstance()->SetFocusNode(true);
        args.ReturnSelf();
    }
}

void JSIndexer::SetSelectedColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }

    IndexerModel::GetInstance()->SetSelectedColor(color);
}

void JSIndexer::SetColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }

    IndexerModel::GetInstance()->SetColor(color);
}

void JSIndexer::SetPopupColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }

    IndexerModel::GetInstance()->SetPopupColor(color);
}

void JSIndexer::SetSelectedBackgroundColor(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }

    IndexerModel::GetInstance()->SetSelectedBackgroundColor(color);
}

void JSIndexer::SetPopupBackground(const JSCallbackInfo& args)
{
    if (args.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color color;
    if (!ParseJsColor(args[0], color)) {
        return;
    }

    IndexerModel::GetInstance()->SetPopupBackground(color);
}

void JSIndexer::SetUsingPopup(bool state)
{
    IndexerModel::GetInstance()->SetUsingPopup(state);
}

void JSIndexer::SetSelectedFont(const JSCallbackInfo& args)
{
    TextStyle textStyle;
    GetFontContent(args, textStyle);
    IndexerModel::GetInstance()->SetSelectedFont(textStyle);
}

void JSIndexer::SetPopupFont(const JSCallbackInfo& args)
{
    TextStyle textStyle;
    GetFontContent(args, textStyle);
    IndexerModel::GetInstance()->SetPopupFont(textStyle);
}

void JSIndexer::SetFont(const JSCallbackInfo& args)
{
    TextStyle textStyle;
    GetFontContent(args, textStyle);
    IndexerModel::GetInstance()->SetFont(textStyle);
}

void JSIndexer::JsOnSelected(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onSelected = [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](
                              const int32_t selected) {
            JAVASCRIPT_EXECUTION_SCOPE(execCtx);
            auto params = ConvertToJSValues(selected);
            func->Call(JSRef<JSObject>(), params.size(), params.data());
        };
        IndexerModel::GetInstance()->SetOnSelected(onSelected);
    }
}

void JSIndexer::JsOnRequestPopupData(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto requestPopupData = [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](
                                    const int32_t selected) {
            std::vector<std::string> popupData;
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx, popupData);
            auto params = ConvertToJSValues(selected);
            JSRef<JSArray> result = func->Call(JSRef<JSObject>(), params.size(), params.data());
            if (result.IsEmpty()) {
                LOGE("Error calling onRequestPopupData result is empty.");
                return popupData;
            }

            if (!result->IsArray()) {
                LOGE("Error calling onRequestPopupData result is not array.");
                return popupData;
            }

            for (size_t i = 0; i < result->Length(); i++) {
                if (result->GetValueAt(i)->IsString()) {
                    auto item = result->GetValueAt(i);
                    popupData.emplace_back(item->ToString());
                } else {
                    LOGE("Error calling onRequestPopupData index %{public}zu is not string.", i);
                }
            }
            return popupData;
        };
        IndexerModel::GetInstance()->SetOnRequestPopupData(requestPopupData);
    }
}

void JSIndexer::JsOnPopupSelected(const JSCallbackInfo& args)
{
    if (args[0]->IsFunction()) {
        auto onPopupSelected = [execCtx = args.GetExecutionContext(), func = JSRef<JSFunc>::Cast(args[0])](
                                   const int32_t selected) {
            JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
            auto params = ConvertToJSValues(selected);
            func->Call(JSRef<JSObject>(), params.size(), params.data());
        };
        IndexerModel::GetInstance()->SetOnPopupSelected(onPopupSelected);
    }
}

void JSIndexer::GetFontContent(const JSCallbackInfo& args, TextStyle& textStyle)
{
    JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
    JSRef<JSVal> size = obj->GetProperty("size");
    Dimension fontSize;
    if (ParseJsDimensionVp(size, fontSize)) {
        textStyle.SetFontSize(fontSize);
    }

    JSRef<JSVal> weight = obj->GetProperty("weight");
    if (weight->IsString()) {
        textStyle.SetFontWeight(ConvertStrToFontWeight(weight->ToString()));
    }

    JSRef<JSVal> family = obj->GetProperty("family");
    std::vector<std::string> fontFamilies;
    if (ParseJsFontFamilies(family, fontFamilies)) {
        textStyle.SetFontFamilies(fontFamilies);
    }

    JSRef<JSVal> style = obj->GetProperty("style");
    if (style->IsNumber()) {
        int32_t value = style->ToNumber<int32_t>();
        if (value >= 0 && value < static_cast<int32_t>(FONT_STYLES.size())) {
            textStyle.SetFontStyle(FONT_STYLES[value]);
        }
    }
}

void JSIndexer::SetItemSize(const JSCallbackInfo& args)
{
    Dimension value;
    if (ParseJsDimensionVp(args[0], value)) {
        IndexerModel::GetInstance()->SetItemSize(value);
    }
}

void JSIndexer::SetAlignStyle(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsNumber()) {
        return;
    }
    int32_t value = args[0]->ToNumber<int32_t>();
    if (value >= 0 && value < static_cast<int32_t>(ALIGN_STYLE.size())) {
        IndexerModel::GetInstance()->SetAlignStyle(value);
    }

    Dimension popupHorizontalSpace(-1.0);
    if (args.Length() > 1) {
        ParseJsDimensionVp(args[1], popupHorizontalSpace);
    }
    IndexerModel::GetInstance()->SetPopupHorizontalSpace(popupHorizontalSpace);
}

void JSIndexer::SetSelected(const JSCallbackInfo& args)
{
    if (args.Length() >= 1) {
        int32_t selected = 0;
        if (ParseJsInteger<int32_t>(args[0], selected)) {
            IndexerModel::GetInstance()->SetSelected(selected);
        }
    }
}

void JSIndexer::SetPopupPosition(const JSCallbackInfo& args)
{
    if (args.Length() >= 1) {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        float positionX = 0.0f;
        float positionY = 0.0f;
        if (ConvertFromJSValue(obj->GetProperty("x"), positionX)) {
            IndexerModel::GetInstance()->SetPopupPositionX(Dimension(positionX, DimensionUnit::VP));
        }
        if (ConvertFromJSValue(obj->GetProperty("y"), positionY)) {
            IndexerModel::GetInstance()->SetPopupPositionY(Dimension(positionY, DimensionUnit::VP));
        }
    }
}

void JSIndexer::SetPopupSelectedColor(const JSCallbackInfo& args)
{
    std::optional<Color> selectedColor = std::nullopt;
    if (args.Length() < 1) {
        LOGW("The argv is wrong, it is supposed to have at least 1 argument");
    } else {
        Color color;
        if (ParseJsColor(args[0], color)) {
            selectedColor = color;
        }
        Dimension popupHorizontalSpace(-1.0);
        if (args.Length() > 1) {
            ParseJsDimensionVp(args[1], popupHorizontalSpace);
        }
        NG::IndexerView::SetPopupHorizontalSpace(popupHorizontalSpace);
        return;
    }
    IndexerModel::GetInstance()->SetPopupSelectedColor(selectedColor);
}

void JSIndexer::SetPopupUnselectedColor(const JSCallbackInfo& args)
{
    std::optional<Color> unselectedColor = std::nullopt;
    if (args.Length() < 1) {
        LOGW("The argv is wrong, it is supposed to have at least 1 argument");
    } else {
        Color color;
        if (ParseJsColor(args[0], color)) {
            unselectedColor = color;
        }
    }
    IndexerModel::GetInstance()->SetPopupUnselectedColor(unselectedColor);
}

void JSIndexer::SetPopupItemFont(const JSCallbackInfo& args)
{
    Dimension fontSize;
    std::string weight;
    if (args.Length() < 1 || !args[0]->IsObject()) {
        LOGW("The argv is wrong, it is supposed to have at least 1 object argument");
    } else {
        JSRef<JSObject> obj = JSRef<JSObject>::Cast(args[0]);
        JSRef<JSVal> size = obj->GetProperty("size");
        if (!size->IsNull()) {
            ParseJsDimensionVp(size, fontSize);
        }

        auto jsWeight = obj->GetProperty("weight");
        if (!jsWeight->IsNull()) {
            if (jsWeight->IsNumber()) {
                weight = std::to_string(jsWeight->ToNumber<int32_t>());
            } else {
                ParseJsString(jsWeight, weight);
            }
        }
    }
    IndexerModel::GetInstance()->SetFontSize(fontSize);
    IndexerModel::GetInstance()->SetFontWeight(ConvertStrToFontWeight(weight, FontWeight::MEDIUM));
}

void JSIndexer::SetPopupItemBackgroundColor(const JSCallbackInfo& args)
{
    std::optional<Color> backgroundColor = std::nullopt;
    if (args.Length() < 1) {
        LOGW("The argv is wrong, it is supposed to have at least 1 argument");
    } else {
        Color color;
        if (ParseJsColor(args[0], color)) {
            backgroundColor = color;
        }
    }
    IndexerModel::GetInstance()->SetPopupItemBackground(backgroundColor);
}

void JSIndexer::JSBind(BindingTarget globalObj)
{
    MethodOptions opt = MethodOptions::NONE;
    JSClass<JSIndexer>::Declare("AlphabetIndexer");
    JSClass<JSIndexer>::StaticMethod("create", &JSIndexer::Create);
    // API7 onSelected deprecated
    JSClass<JSIndexer>::StaticMethod("onSelected", &JSIndexer::JsOnSelected);
    JSClass<JSIndexer>::StaticMethod("onSelect", &JSIndexer::JsOnSelected);
    JSClass<JSIndexer>::StaticMethod("color", &JSIndexer::SetColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedColor", &JSIndexer::SetSelectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupColor", &JSIndexer::SetPopupColor, opt);
    JSClass<JSIndexer>::StaticMethod("selectedBackgroundColor", &JSIndexer::SetSelectedBackgroundColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupBackground", &JSIndexer::SetPopupBackground, opt);
    JSClass<JSIndexer>::StaticMethod("usingPopup", &JSIndexer::SetUsingPopup, opt);
    JSClass<JSIndexer>::StaticMethod("selectedFont", &JSIndexer::SetSelectedFont);
    JSClass<JSIndexer>::StaticMethod("font", &JSIndexer::SetFont);
    JSClass<JSIndexer>::StaticMethod("popupFont", &JSIndexer::SetPopupFont);
    JSClass<JSIndexer>::StaticMethod("itemSize", &JSIndexer::SetItemSize, opt);
    JSClass<JSIndexer>::StaticMethod("alignStyle", &JSIndexer::SetAlignStyle, opt);
    JSClass<JSIndexer>::StaticMethod("onRequestPopupData", &JSIndexer::JsOnRequestPopupData, opt);
    JSClass<JSIndexer>::StaticMethod("selected", &JSIndexer::SetSelected, opt);
    JSClass<JSIndexer>::StaticMethod("popupPosition", &JSIndexer::SetPopupPosition, opt);
    JSClass<JSIndexer>::StaticMethod("popupSelectedColor", &JSIndexer::SetPopupSelectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupUnselectedColor", &JSIndexer::SetPopupUnselectedColor, opt);
    JSClass<JSIndexer>::StaticMethod("popupItemFont", &JSIndexer::SetPopupItemFont);
    JSClass<JSIndexer>::StaticMethod("popupItemBackgroundColor", &JSIndexer::SetPopupItemBackgroundColor, opt);
    // keep compatible, need remove after
    JSClass<JSIndexer>::StaticMethod("onPopupSelected", &JSIndexer::JsOnPopupSelected, opt);
    JSClass<JSIndexer>::StaticMethod("onPopupSelect", &JSIndexer::JsOnPopupSelected, opt);
    JSClass<JSIndexer>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSIndexer>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSIndexer>::Inherit<JSViewAbstract>();
    JSClass<JSIndexer>::Bind(globalObj);
}
} // namespace OHOS::Ace::Framework