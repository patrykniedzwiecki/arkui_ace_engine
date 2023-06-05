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

#include "core/components_ng/pattern/blank/blank_pattern.h"

#include "base/geometry/dimension.h"
#include "base/json/json_util.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/flex/flex_layout_property.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline/base/element_register.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {

namespace {
FlexDirection GetFlexDirection(const RefPtr<UINode>& node)
{
    auto tag = node->GetTag();
    if (tag == V2::ROW_ETS_TAG) {
        return FlexDirection::ROW;
    }
    if (tag == V2::COLUMN_ETS_TAG) {
        return FlexDirection::COLUMN;
    }
    if (tag != V2::FLEX_ETS_TAG) {
        return FlexDirection::ROW;
    }
    auto frameNode = AceType::DynamicCast<FrameNode>(node);
    CHECK_NULL_RETURN_NOLOG(frameNode, FlexDirection::ROW);
    auto layoutProperty = frameNode->GetLayoutProperty<FlexLayoutProperty>();
    CHECK_NULL_RETURN_NOLOG(layoutProperty, FlexDirection::ROW);
    return layoutProperty->GetFlexDirection().value_or(FlexDirection::ROW);
}
} // namespace

std::string BlankPattern::GetColorString() const
{
    std::string color;
    auto renderContext = GetHost()->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, "NA");
    return renderContext->GetBackgroundColor().value_or(Color::WHITE).ColorToString();
}

void BlankPattern::ToJsonValue(std::unique_ptr<JsonValue>& json) const
{
    json->Put("color", GetColorString().c_str());
}

void BlankPattern::OnMountToParentDone()
{
    auto host = GetHost();
    CHECK_NULL_VOID_NOLOG(host);
    auto parent = host->GetParent();
    CHECK_NULL_VOID_NOLOG(parent);
    auto layoutProp = host->GetLayoutProperty<BlankLayoutProperty>();
    CHECK_NULL_VOID_NOLOG(layoutProp);
    CHECK_NULL_VOID_NOLOG(layoutProp->GetMinSize().has_value());
    auto blankMin = layoutProp->GetMinSize().value_or(Dimension());
    auto& calcConstraint = layoutProp->GetCalcLayoutConstraint();
    auto isParentRow = GetFlexDirection(parent) == FlexDirection::ROW;
    if (calcConstraint && calcConstraint->minSize.has_value()) {
        // skip if constraint min size on main axis is set already
        if (isParentRow && calcConstraint->minSize.value().Width().has_value()) {
            return;
        } else if (!isParentRow && calcConstraint->minSize.value().Height().has_value()) {
            return;
        }
    }
    if (isParentRow) {
        layoutProp->UpdateCalcMinSize(CalcSize(CalcLength(blankMin), std::nullopt));
    } else {
        layoutProp->UpdateCalcMinSize(CalcSize(std::nullopt, CalcLength(blankMin)));
    }
}

} // namespace OHOS::Ace::NG
