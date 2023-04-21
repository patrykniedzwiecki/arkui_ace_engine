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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_WATER_FLOW_MODEL_IMPL_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_WATER_FLOW_MODEL_IMPL_H

#include "core/components_ng/pattern/waterflow/water_flow_model.h"

namespace OHOS::Ace::Framework {
class WaterFlowModelImpl : public WaterFlowModel {
public:
    void Create() override;
    void SetFooter(std::function<void()>&& footer) override;
    RefPtr<ScrollControllerBase> CreateScrollController() override;
    RefPtr<ScrollProxy> CreateScrollBarProxy() override;
    void SetScroller(RefPtr<ScrollControllerBase> scroller, RefPtr<ScrollProxy> proxy) override;

    void SetColumnsTemplate(const std::string& value) override;
    void SetRowsTemplate(const std::string& value) override;

    void SetItemMinWidth(const Dimension& minWidth) override;
    void SetItemMinHeight(const Dimension& minHeight) override;
    void SetItemMaxWidth(const Dimension& maxWidth) override;
    void SetItemMaxHeight(const Dimension& maxHeight) override;

    void SetColumnsGap(const Dimension& value) override;
    void SetRowsGap(const Dimension& value) override;

    void SetLayoutDirection(FlexDirection value) override;

    void SetOnReachStart(OnReachEvent&& onReachStart) override;
    void SetOnReachEnd(OnReachEvent&& onReachEnd) override;
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_WATER_FLOW_MODEL_IMPL_H