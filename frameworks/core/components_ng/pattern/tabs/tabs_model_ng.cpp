/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/tabs/tabs_model_ng.h"

#include <type_traits>

#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/swiper/swiper_controller.h"
#include "core/components_ng/base/group_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/swiper/swiper_layout_property.h"
#include "core/components_ng/pattern/swiper/swiper_paint_property.h"
#include "core/components_ng/pattern/swiper/swiper_pattern.h"
#include "core/components_ng/pattern/tabs/tab_bar_paint_property.h"
#include "core/components_ng/pattern/tabs/tab_bar_pattern.h"
#include "core/components_ng/pattern/tabs/tabs_node.h"
#include "core/components_ng/pattern/tabs/tabs_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/divider/divider_layout_property.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/divider/divider_render_property.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {

void TabsModelNG::Create(BarPosition barPosition, int32_t index, const RefPtr<TabController>& /*tabController*/,
    const RefPtr<SwiperController>& swiperController)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto tabsNode = GetOrCreateTabsNode(V2::TABS_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TabsPattern>(); });

    bool hasSwiperNode = tabsNode->HasSwiperNode();
    bool hasTabBarNode = tabsNode->HasTabBarNode();
    bool hasDividerNode = tabsNode->HasDividerNode();
    auto swiperId = tabsNode->GetSwiperId();
    auto tabBarId = tabsNode->GetTabBarId();
    auto dividerId = tabsNode->GetDividerId();
    // Create Swiper node to contain TabContent.
    auto swiperNode = FrameNode::GetOrCreateFrameNode(
        V2::SWIPER_ETS_TAG, swiperId, []() { return AceType::MakeRefPtr<SwiperPattern>(); });
    auto swiperPaintProperty = swiperNode->GetPaintProperty<SwiperPaintProperty>();
    swiperPaintProperty->UpdateLoop(false);
    swiperPaintProperty->UpdateEdgeEffect(EdgeEffect::SPRING);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto tabTheme = pipelineContext->GetTheme<TabTheme>();
    CHECK_NULL_VOID(tabTheme);
    swiperPaintProperty->UpdateDuration(tabTheme->GetTabContentAnimationDuration());
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    swiperLayoutProperty->UpdateCachedCount(0);
    swiperLayoutProperty->UpdateShowIndicator(false);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    auto controller = swiperController;
    if (!controller) {
        controller = AceType::MakeRefPtr<SwiperController>();
    }
    swiperPattern->SetSwiperController(controller);

    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, dividerId, []() { return AceType::MakeRefPtr<DividerPattern>(); });

    // Create TabBar to contain TabBar of TabContent.
    auto tabBarNode = FrameNode::GetOrCreateFrameNode(
        V2::TAB_BAR_ETS_TAG, tabBarId, [controller]() { return AceType::MakeRefPtr<TabBarPattern>(controller); });
    if (!hasTabBarNode) {
        tabBarNode->MountToParent(tabsNode);
    }
    if (!hasDividerNode) {
        dividerNode->MountToParent(tabsNode);
    }
    if (!hasSwiperNode) {
        swiperNode->MountToParent(tabsNode);
    }

    ViewStackProcessor::GetInstance()->Push(tabsNode);

    SetTabBarPosition(barPosition);
    SetIndex(index);
}

void TabsModelNG::SetTabBarPosition(BarPosition tabBarPosition)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, TabBarPosition, tabBarPosition);
}

void TabsModelNG::SetTabBarMode(TabBarMode tabBarMode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, TabBarMode, tabBarMode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateTabBarMode(tabBarMode);
}

void TabsModelNG::SetTabBarWidth(const Dimension& tabBarWidth)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarWidth, tabBarWidth);
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (tabBarWidth.ConvertToPx() < 0.0) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(true, false);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(NG::CalcLength(tabBarWidth), std::nullopt));
    }
    tabBarLayoutProperty->UpdateTabBarWidth(tabBarWidth);
}

void TabsModelNG::SetTabBarHeight(const Dimension& tabBarHeight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarHeight, tabBarHeight);
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (tabBarHeight.ConvertToPx() < 0.0) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(false, true);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, NG::CalcLength(tabBarHeight)));
    }
    tabBarLayoutProperty->UpdateTabBarHeight(tabBarHeight);
}

void TabsModelNG::SetIsVertical(bool isVertical)
{
    auto axis = isVertical ? Axis::VERTICAL : Axis::HORIZONTAL;
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, Axis, axis);

    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateAxis(axis);

    auto swiperLayoutProperty = GetSwiperLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateDirection(axis);
}

void TabsModelNG::SetIndex(int32_t index)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().back());
    CHECK_NULL_VOID(swiperNode);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateIndex(index);
    auto tabContentNum = swiperNode->TotalChildCount();
    if (tabContentNum == 0) {
        return;
    }
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (index > tabContentNum - 1 || index < 0) {
        index = 0;
    }
    tabBarLayoutProperty->UpdateIndicator(index);
    tabBarPattern->UpdateTextColor(index);
    swiperLayoutProperty->UpdateIndex(index);
}

void TabsModelNG::SetScrollable(bool scrollable)
{
    auto swiperPaintProperty = GetSwiperPaintProperty();
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateDisableSwipe(!scrollable);
}

void TabsModelNG::SetAnimationDuration(float duration)
{
    if (duration < 0) {
        return;
    }
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    tabBarPattern->SetAnimationDuration(static_cast<int32_t>(duration));
    auto swiperPaintProperty = GetSwiperPaintProperty();
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateDuration(static_cast<int32_t>(duration));
}

void TabsModelNG::SetFadingEdge(bool fadingEdge)
{
    auto tabBarPaintProperty = GetTabBarPaintProperty();
    CHECK_NULL_VOID(tabBarPaintProperty);
    tabBarPaintProperty->UpdateFadingEdge(fadingEdge);
}

void TabsModelNG::SetOnChange(std::function<void(const BaseEventInfo*)>&& onChange)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnChangeEvent(std::move(onChange));
}

void TabsModelNG::SetDivider(const TabsItemDivider& divider)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider);
}

RefPtr<TabBarLayoutProperty> TabsModelNG::GetTabBarLayoutProperty()
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_RETURN(tabBarLayoutProperty, nullptr);
    return tabBarLayoutProperty;
}

RefPtr<TabBarPaintProperty> TabsModelNG::GetTabBarPaintProperty()
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarPaintProperty = tabBarNode->GetPaintProperty<TabBarPaintProperty>();
    CHECK_NULL_RETURN(tabBarPaintProperty, nullptr);
    return tabBarPaintProperty;
}

RefPtr<SwiperLayoutProperty> TabsModelNG::GetSwiperLayoutProperty()
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().back());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_RETURN(swiperLayoutProperty, nullptr);
    return swiperLayoutProperty;
}

RefPtr<SwiperPaintProperty> TabsModelNG::GetSwiperPaintProperty()
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().back());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperPaintProperty = swiperNode->GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, nullptr);
    return swiperPaintProperty;
}

void TabsModelNG::Pop()
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().front());
    CHECK_NULL_VOID(tabBarNode);

    tabBarNode->MarkModifyDone();
    tabBarNode->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);

    auto childNode = tabsNode->GetChildAtIndex(1);
    CHECK_NULL_VOID(childNode);
    auto dividerNode = AceType::DynamicCast<FrameNode>(childNode);
    auto layoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);

    auto axis = layoutProperty->GetAxis().value_or((Axis::HORIZONTAL));
    TabsItemDivider defaultDivider;
    auto divider = layoutProperty->GetDivider().value_or(defaultDivider);
    auto dividerColor = divider.color;
    auto dividerStrokeWidth = divider.strokeWidth;

    auto dividerHub = dividerNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(dividerHub);

    auto dividerRenderProperty = dividerNode->GetPaintProperty<DividerRenderProperty>();
    CHECK_NULL_VOID(dividerRenderProperty);
    dividerRenderProperty->UpdateDividerColor(dividerColor);

    auto dividerLayoutProperty = dividerNode->GetLayoutProperty<DividerLayoutProperty>();
    CHECK_NULL_VOID(dividerLayoutProperty);
    dividerLayoutProperty->UpdateVertical(axis == Axis::VERTICAL);
    dividerLayoutProperty->UpdateStrokeWidth(dividerStrokeWidth);
    CHECK_NULL_VOID(dividerNode);
    dividerNode->MarkModifyDone();

    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetChildren().back());
    CHECK_NULL_VOID(swiperNode);
    swiperNode->MarkModifyDone();

    ViewStackProcessor::GetInstance()->PopContainer();
}

RefPtr<TabsNode> TabsModelNG::GetOrCreateTabsNode(
    const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator)
{
    auto tabsNode = ElementRegister::GetInstance()->GetSpecificItemById<TabsNode>(nodeId);
    if (tabsNode) {
        if (tabsNode->GetTag() == tag) {
            return tabsNode;
        }
        ElementRegister::GetInstance()->RemoveItemSilently(nodeId);
        auto parent = tabsNode->GetParent();
        if (parent) {
            parent->RemoveChild(tabsNode);
        }
    }

    auto pattern = patternCreator ? patternCreator() : AceType::MakeRefPtr<Pattern>();
    tabsNode = AceType::MakeRefPtr<TabsNode>(tag, nodeId, pattern, false);
    tabsNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(tabsNode);
    return tabsNode;
}

} // namespace OHOS::Ace::NG
