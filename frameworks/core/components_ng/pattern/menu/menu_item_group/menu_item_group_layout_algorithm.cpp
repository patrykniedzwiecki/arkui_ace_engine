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

#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_layout_algorithm.h"

#include "base/utils/utils.h"
#include "core/components/common/layout/grid_system_manager.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/menu/menu_item_group/menu_item_group_paint_property.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {
void MenuItemGroupLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();

    const auto& props = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(props);
    auto layoutConstraint = props->GetLayoutConstraint();
    CHECK_NULL_VOID(layoutConstraint);

    auto childConstraint = props->CreateChildConstraint();

    if (layoutConstraint->selfIdealSize.Width().has_value()) {
        childConstraint.selfIdealSize.SetWidth(layoutConstraint->selfIdealSize.Width().value());
    }

    // measure children (header, footer, menuItem)
    float maxChildrenWidth = GetChildrenMaxWidth(layoutWrapper->GetAllChildrenWithBuild(), childConstraint);
    SizeF menuItemGroupSize;
    menuItemGroupSize.SetWidth(maxChildrenWidth);

    // measure header
    needHeaderPadding_ = NeedHeaderPadding(host);
    auto paintProperty = host->GetPaintProperty<MenuItemGroupPaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    paintProperty->UpdateNeedHeaderPadding(needHeaderPadding_);
    float headerPadding = needHeaderPadding_ ? GROUP_DIVIDER_PADDING.ConvertToPx() : 0.0;
    float headerHeight = 0.0f;
    if (headerIndex_ >= 0) {
        auto headerWrapper = layoutWrapper->GetOrCreateChildByIndex(headerIndex_);
        headerHeight = headerWrapper->GetGeometryNode()->GetMarginFrameSize().Height();
    }
    // measure menu item
    float totalHeight = headerHeight + headerPadding;
    auto totalItemCount = layoutWrapper->GetTotalChildCount();
    int32_t currentIndex = itemStartIndex_;
    while (currentIndex < totalItemCount) {
        auto item = layoutWrapper->GetOrCreateChildByIndex(currentIndex);
        auto childSize = item->GetGeometryNode()->GetFrameSize();
        // set minimum size
        childSize.SetWidth(maxChildrenWidth);
        item->GetGeometryNode()->SetFrameSize(childSize);

        float itemHeight = childSize.Height();
        float endPos = totalHeight + itemHeight;
        itemPosition_[currentIndex] = { totalHeight, endPos };
        totalHeight = endPos;
        ++currentIndex;
    }

    if (footerIndex_ >= 0) {
        auto footerWrapper = layoutWrapper->GetOrCreateChildByIndex(footerIndex_);
        totalHeight += footerWrapper->GetGeometryNode()->GetMarginFrameSize().Height();
    }
    // set menu size
    needFooterPadding_ = NeedFooterPadding(host);
    paintProperty->UpdateNeedFooterPadding(needHeaderPadding_);
    float footerPadding = needFooterPadding_ ? GROUP_DIVIDER_PADDING.ConvertToPx() : 0.0;
    totalHeight += (headerPadding + footerPadding);
    menuItemGroupSize.SetHeight(totalHeight);

    LOGD("finish measure, menuItemGroup size = %{public}f x %{public}f", menuItemGroupSize.Width(),
        menuItemGroupSize.Height());
    layoutWrapper->GetGeometryNode()->SetFrameSize(menuItemGroupSize);
}

void MenuItemGroupLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    const auto& layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    if (headerIndex_ >= 0) {
        LayoutHeader(layoutWrapper);
    }
    if (footerIndex_ >= 0) {
        LayoutFooter(layoutWrapper);
    }
    LayoutMenuItem(layoutWrapper);
}

void MenuItemGroupLayoutAlgorithm::LayoutMenuItem(LayoutWrapper* layoutWrapper)
{
    // layout items.
    for (auto& pos : itemPosition_) {
        auto wrapper = layoutWrapper->GetOrCreateChildByIndex(pos.first);
        if (!wrapper) {
            LOGI("wrapper is out of boundary");
            continue;
        }
        LayoutIndex(wrapper, OffsetF(0.0, pos.second.first));
    }
}

void MenuItemGroupLayoutAlgorithm::LayoutHeader(LayoutWrapper* layoutWrapper)
{
    auto wrapper = layoutWrapper->GetOrCreateChildByIndex(headerIndex_);
    CHECK_NULL_VOID(wrapper);
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    float headerPadding = needHeaderPadding_ ? GROUP_DIVIDER_PADDING.ConvertToPx() : 0.0;
    LayoutIndex(wrapper, OffsetF(MENU_ITEM_GROUP_PADDING.ConvertToPx(), headerPadding));
}

void MenuItemGroupLayoutAlgorithm::LayoutFooter(LayoutWrapper* layoutWrapper)
{
    auto wrapper = layoutWrapper->GetOrCreateChildByIndex(footerIndex_);
    CHECK_NULL_VOID(wrapper);
    auto footerMainSize = wrapper->GetGeometryNode()->GetFrameSize();
    auto footerHeight = footerMainSize.Height();

    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    auto groupHeight = size.Height();
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    float footerPadding = needFooterPadding_ ? GROUP_DIVIDER_PADDING.ConvertToPx() : 0.0;
    LayoutIndex(wrapper, OffsetF(MENU_ITEM_GROUP_PADDING.ConvertToPx(), (groupHeight - footerHeight - footerPadding)));
}

void MenuItemGroupLayoutAlgorithm::LayoutIndex(const RefPtr<LayoutWrapper>& wrapper, const OffsetF& offset)
{
    CHECK_NULL_VOID_NOLOG(wrapper);
    wrapper->GetGeometryNode()->SetMarginFrameOffset(offset);
    wrapper->Layout();
}

bool MenuItemGroupLayoutAlgorithm::NeedHeaderPadding(const RefPtr<FrameNode>& host)
{
    int32_t hostId = host->GetId();
    auto menu = AceType::DynamicCast<FrameNode>(host->GetParent());
    CHECK_NULL_RETURN(menu, false);
    int32_t index = menu->GetChildIndexById(hostId);
    if (index == 0) {
        return false;
    }
    auto brotherNode = AceType::DynamicCast<FrameNode>(menu->GetChildAtIndex(index - 1));
    CHECK_NULL_RETURN_NOLOG(brotherNode, false);
    return brotherNode->GetTag() != V2::MENU_ITEM_GROUP_ETS_TAG;
}

bool MenuItemGroupLayoutAlgorithm::NeedFooterPadding(const RefPtr<FrameNode>& host)
{
    int32_t hostId = host->GetId();
    auto menu = AceType::DynamicCast<FrameNode>(host->GetParent());
    CHECK_NULL_RETURN(menu, false);
    int32_t index = menu->GetChildIndexById(hostId);
    int32_t menuCount = menu->TotalChildCount();
    return index != menuCount - 1;
}

float MenuItemGroupLayoutAlgorithm::GetChildrenMaxWidth(
    const std::list<RefPtr<LayoutWrapper>>& children, const LayoutConstraintF& layoutConstraint)
{
    float width = layoutConstraint.minSize.Width();

    for (const auto& child : children) {
        child->Measure(layoutConstraint);
        auto childSize = child->GetGeometryNode()->GetFrameSize();
        width = std::max(width, childSize.Width());
    }
    return width;
}
} // namespace OHOS::Ace::NG