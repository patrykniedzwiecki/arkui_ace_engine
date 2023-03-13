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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATA_PANEL_DATA_PANEL_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATA_PANEL_DATA_PANEL_PATTERN_H

#include "core/components_ng/pattern/data_panel/data_panel_layout_algorithm.h"
#include "core/components_ng/pattern/data_panel/data_panel_modifier.h"
#include "core/components_ng/pattern/data_panel/data_panel_paint_method.h"
#include "core/components_ng/pattern/data_panel/data_panel_paint_property.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class DataPanelPattern : public Pattern {
    DECLARE_ACE_TYPE(DataPanelPattern, Pattern);

public:
    DataPanelPattern() = default;
    ~DataPanelPattern() override = default;

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        if (!dataPanelModifier_) {
            dataPanelModifier_ = AceType::MakeRefPtr<DataPanelModifier>();
        }
        return MakeRefPtr<DataPanelPaintMethod>(dataPanelModifier_);
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<DataPanelPaintProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<DataPanelLayoutAlgorithm>();
    }

    void OnModifyDone() override;

private:
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, bool skipMeasure, bool skipLayout) override;

    RefPtr<DataPanelModifier> dataPanelModifier_;
    ACE_DISALLOW_COPY_AND_MOVE(DataPanelPattern);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATA_PANEL_DATA_PANEL_PATTERN_H
