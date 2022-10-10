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

#include "core/components_ng/pattern/navigation/navigation_view.h"

#include <cstdint>
#include <iterator>

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/navigation/bar_item_event_hub.h"
#include "core/components_ng/pattern/navigation/bar_item_node.h"
#include "core/components_ng/pattern/navigation/bar_item_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_layout_property.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline/base/element_register.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {
namespace {
RefPtr<FrameNode> CreateBarItemTextNode(const std::string& text)
{
    int32_t nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto textNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, nodeId, AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, nullptr);
    textLayoutProperty->UpdateContent(text);
    textLayoutProperty->UpdateFontSize(TEXT_FONT_SIZE);
    textLayoutProperty->UpdateTextColor(TEXT_COLOR);
    return textNode;
}

RefPtr<FrameNode> CreateBarItemIconNode(const std::string& src)
{
    int32_t nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ImageSourceInfo info(src);
    auto iconNode = FrameNode::CreateFrameNode(V2::IMAGE_ETS_TAG, nodeId, AceType::MakeRefPtr<ImagePattern>(info));
    auto imageLayoutProperty = iconNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_RETURN(imageLayoutProperty, nullptr);
    imageLayoutProperty->UpdateImageSourceInfo(info);
    imageLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(ICON_SIZE), CalcLength(ICON_SIZE)));
    return iconNode;
}

void UpdateBarItemNodeWithItem(const RefPtr<BarItemNode>& barItemNode, const BarItem& barItem)
{
    if (barItem.text.has_value()) {
        auto textNode = CreateBarItemTextNode(barItem.text.value());
        barItemNode->SetTextNode(textNode);
        barItemNode->AddChild(textNode);
    }
    if (barItem.icon.has_value()) {
        auto iconNode = CreateBarItemIconNode(barItem.icon.value());
        barItemNode->SetIconNode(iconNode);
        barItemNode->AddChild(iconNode);
    }
    if (barItem.action) {
        auto eventHub = barItemNode->GetEventHub<BarItemEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->SetItemAction(barItem.action);
    }
    auto barItemPattern = barItemNode->GetPattern<BarItemPattern>();
    barItemNode->MarkModifyDone();
}

void UpdateOldBarItems(const RefPtr<UINode>& oldBarContainer, const std::list<BarItem>& newBarItems)
{
    auto oldBarItems = oldBarContainer->GetChildren();
    auto prevChildrenSize = static_cast<int32_t>(oldBarItems.size());
    auto newChildrenSize = static_cast<int32_t>(newBarItems.size());
    auto oldIter = oldBarItems.begin();
    auto newIter = newBarItems.begin();
    // if old container has m items and incoming array has n items
    // we update first min(m, n) items in the old container
    for (int32_t i = 0; i < std::min(prevChildrenSize, newChildrenSize); i++) {
        do {
            auto oldBarItem = AceType::DynamicCast<BarItemNode>(*oldIter);
            BarItem newBarItem = *newIter;
            if (!oldBarItem) {
                break;
            }
            // TODO: fix error for update condition when add or delete child, and update old bar item will not work
            if (newBarItem.text.has_value()) {
                oldBarItem->UpdateText(newBarItem.text.value());
                if (oldBarItem->GetTextNode()) {
                    auto textNode = AceType::DynamicCast<FrameNode>(oldBarItem->GetTextNode());
                    CHECK_NULL_VOID(textNode);
                    auto textLayoutProperty = textNode->GetLayoutProperty<TextLayoutProperty>();
                    CHECK_NULL_VOID(textLayoutProperty);
                    textLayoutProperty->UpdateContent(newBarItem.text.value());
                    textNode->MarkModifyDone();
                } else {
                    auto textNode = CreateBarItemTextNode(newBarItem.text.value());
                    oldBarItem->SetTextNode(textNode);
                    oldBarItem->AddChild(textNode);
                    oldBarItem->MarkModifyDone();
                }
            } else {
                oldBarItem->ResetText();
                if (oldBarItem->GetTextNode()) {
                    auto textNode = AceType::DynamicCast<FrameNode>(oldBarItem->GetTextNode());
                    CHECK_NULL_VOID(textNode);
                    oldBarItem->RemoveChild(textNode);
                }
            }
            if (newBarItem.icon.has_value()) {
                oldBarItem->UpdateIconSrc(newBarItem.icon.value());
                if (oldBarItem->GetIconNode()) {
                    auto iconNode = AceType::DynamicCast<FrameNode>(oldBarItem->GetIconNode());
                    CHECK_NULL_VOID(iconNode);
                    auto imageLayoutProperty = iconNode->GetLayoutProperty<ImageLayoutProperty>();
                    CHECK_NULL_VOID(imageLayoutProperty);
                    imageLayoutProperty->UpdateImageSourceInfo(ImageSourceInfo(newBarItem.icon.value()));
                    iconNode->MarkModifyDone();
                } else {
                    auto iconNode = CreateBarItemIconNode(newBarItem.icon.value());
                    oldBarItem->SetIconNode(iconNode);
                    oldBarItem->AddChild(iconNode);
                    oldBarItem->MarkModifyDone();
                }
            } else {
                oldBarItem->ResetIconSrc();
                if (oldBarItem->GetIconNode()) {
                    auto iconNode = AceType::DynamicCast<FrameNode>(oldBarItem->GetIconNode());
                    CHECK_NULL_VOID(iconNode);
                    oldBarItem->RemoveChild(iconNode);
                }
            }
        } while (false);
        oldIter++;
        newIter++;
    }
    // if m > n, we remove (m - n) children from the back of old container
    if (prevChildrenSize > newChildrenSize) {
        for (int32_t i = 0; i < prevChildrenSize - newChildrenSize; i++) {
            oldBarItems.pop_back();
        }
    } else if (prevChildrenSize < newChildrenSize) {
        // if m < n, we add (n - m) children created by info in new item list
        for (int32_t i = 0; i < newChildrenSize - prevChildrenSize; i++) {
            auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
            auto barItemNode = AceType::MakeRefPtr<BarItemNode>(V2::BAR_ITEM_ETS_TAG, nodeId);
            barItemNode->InitializePatternAndContext();
            UpdateBarItemNodeWithItem(barItemNode, *newIter);
            oldBarContainer->AddChild(barItemNode);
            newIter++;
            auto container = AceType::DynamicCast<TitleBarNode>(oldBarContainer);
            CHECK_NULL_VOID(container);
            container->MarkModifyDone();
        }
    }
}
} // namespace

void NavigationView::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    // navigation node
    int32_t nodeId = stack->ClaimNodeId();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(
        V2::NAVIGATION_VIEW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    // titleBar node
    int32_t titleBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        V2::TITLE_BAR_ETS_TAG, titleBarNodeId, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    // content node
    int32_t contentNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto contentNode = FrameNode::GetOrCreateFrameNode(
        V2::NAVIGATION_CONTENT_ETS_TAG, contentNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
    // toolBar node
    int32_t toolBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto toolBarNode = FrameNode::GetOrCreateFrameNode(
        V2::TOOL_BAR_ETS_TAG, toolBarNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });

    navigationGroupNode->AddChild(titleBarNode);
    navigationGroupNode->SetTitleBarNode(titleBarNode);
    navigationGroupNode->AddChild(contentNode);
    navigationGroupNode->SetContentNode(contentNode);
    navigationGroupNode->AddChild(toolBarNode);
    navigationGroupNode->SetToolBarNode(toolBarNode);
    navigationGroupNode->SetPreToolBarNode(toolBarNode);
    stack->Push(navigationGroupNode);
    auto layoutProperty = navigationGroupNode->GetLayoutProperty<NavigationLayoutProperty>();
    layoutProperty->UpdateTitleMode(NavigationTitleMode::FREE);
}

void NavigationView::SetTitle(const std::string& title)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    do {
        if (!navigationGroupNode->GetTitle()) {
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::ADD);
            break;
        }
        // if previous title is not a frame node, we remove it and create a new node
        auto titleNode = AceType::DynamicCast<FrameNode>(navigationGroupNode->GetTitle());
        if (!titleNode) {
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        auto titleProperty = titleNode->GetLayoutProperty<TextLayoutProperty>();
        // previous title is not a text node and might be custom, we remove it and create a new node
        if (!titleProperty) {
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        // text content is the same, do nothing
        if (titleProperty->GetContentValue() == title) {
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        // update title content only without changing node
        titleProperty->UpdateContent(title);
        navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
        return;
    } while (false);
    int32_t titleNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, titleNodeId, AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = titleNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    textLayoutProperty->UpdateContent(title);
    textLayoutProperty->UpdateFontSize(TITLE_FONT_SIZE);
    textLayoutProperty->UpdateTextColor(TITLE_COLOR);
    navigationGroupNode->SetTitle(titleNode);
    navigationGroupNode->UpdatePrevTitleIsCustom(false);
}

void NavigationView::SetCustomTitle(const RefPtr<UINode>& customTitle)
{
    CHECK_NULL_VOID(customTitle);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    if (navigationGroupNode->GetTitle()) {
        if (customTitle->GetId() == navigationGroupNode->GetTitle()->GetId()) {
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
        } else {
            navigationGroupNode->SetTitle(customTitle);
            navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
        }
        return;
    }
    navigationGroupNode->SetTitle(customTitle);
    navigationGroupNode->UpdateTitleNodeOperation(ChildNodeOperation::ADD);
    navigationGroupNode->UpdatePrevTitleIsCustom(true);
}

void NavigationView::SetSubtitle(const std::string& subtitle)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    do {
        if (!navigationGroupNode->GetSubtitle()) {
            navigationGroupNode->UpdateSubtitleNodeOperation(ChildNodeOperation::ADD);
            break;
        }
        auto subtitleNode = AceType::DynamicCast<FrameNode>(navigationGroupNode->GetSubtitle());
        if (!subtitleNode) {
            navigationGroupNode->UpdateSubtitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        auto subtitleProperty = subtitleNode->GetLayoutProperty<TextLayoutProperty>();
        if (!subtitleProperty) {
            navigationGroupNode->UpdateSubtitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        if (subtitleProperty->GetContentValue() == subtitle) {
            navigationGroupNode->UpdateSubtitleNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        subtitleProperty->UpdateContent(subtitle);
        navigationGroupNode->UpdateSubtitleNodeOperation(ChildNodeOperation::NONE);
        return;
    } while (false);
    int32_t subtitleNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto subtitleNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, subtitleNodeId, AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = subtitleNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    textLayoutProperty->UpdateContent(subtitle);
    textLayoutProperty->UpdateFontSize(SUBTITLE_FONT_SIZE);
    textLayoutProperty->UpdateTextColor(SUBTITLE_COLOR);
    navigationGroupNode->SetSubtitle(subtitleNode);
}

void NavigationView::SetMenuItems(std::list<BarItem>&& menuItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    // if previous menu is custom, just remove it and create new menu, otherwise update old menu
    if (navigationGroupNode->GetPrevMenuIsCustom().value_or(false)) {
        navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navigationGroupNode->GetMenu()) {
            auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navigationGroupNode->GetTitleBarNode());
            CHECK_NULL_VOID(titleBarNode);
            UpdateOldBarItems(titleBarNode->GetMenu(), menuItems);
            navigationGroupNode->SetMenu(titleBarNode->GetMenu());
            navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
    }
    int32_t menuNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto menuNode = FrameNode::GetOrCreateFrameNode(
        V2::NAVIGATION_MENU_ETS_TAG, menuNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    auto rowProperty = menuNode->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(rowProperty);
    rowProperty->UpdateMainAxisAlign(FlexAlign::FLEX_END);
    for (const auto& menuItem : menuItems) {
        int32_t barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto barItemNode = AceType::MakeRefPtr<BarItemNode>(V2::BAR_ITEM_ETS_TAG, barItemNodeId);
        barItemNode->InitializePatternAndContext();
        UpdateBarItemNodeWithItem(barItemNode, menuItem);
        menuNode->AddChild(barItemNode);
    }
    navigationGroupNode->SetMenu(menuNode);
    navigationGroupNode->UpdatePrevMenuIsCustom(false);
}

void NavigationView::SetCustomMenu(const RefPtr<UINode>& customMenu)
{
    CHECK_NULL_VOID(customMenu);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    // if previous menu exists, remove it if their ids are not the same
    // if previous node is not custom, their ids must not be the same
    if (navigationGroupNode->GetMenu()) {
        if (customMenu->GetId() == navigationGroupNode->GetMenu()->GetId()) {
            navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navigationGroupNode->SetMenu(customMenu);
        navigationGroupNode->UpdatePrevMenuIsCustom(true);
        navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        return;
    }
    navigationGroupNode->SetMenu(customMenu);
    navigationGroupNode->UpdatePrevMenuIsCustom(true);
    navigationGroupNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
}

void NavigationView::SetTitleMode(NavigationTitleMode mode)
{
    if (mode != NavigationTitleMode::MINI) {
        return;
    }
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto layoutProperty = navigationGroupNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    bool needAddBackButton = false;
    bool needRemoveBackButton = false;
    do {
        // add back button if current mode is mini and one of the following condition:
        // first create or not first create but previous mode is not mini
        if (layoutProperty->GetTitleModeValue() != NavigationTitleMode::MINI && mode == NavigationTitleMode::MINI) {
            needAddBackButton = true;
            break;
        }
        // remove back button if current mode is not mini and previous mode is mini
        if (layoutProperty->GetTitleModeValue() == NavigationTitleMode::MINI && mode != NavigationTitleMode::MINI) {
            needRemoveBackButton = true;
            break;
        }
    } while (false);
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, TitleMode, static_cast<NG::NavigationTitleMode>(mode));
    if (needAddBackButton) {
        int32_t backButtonNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto backButtonNode = FrameNode::CreateFrameNode(
            V2::BACK_BUTTON_ETS_TAG, backButtonNodeId, AceType::MakeRefPtr<TextPattern>());
        auto textLayoutProperty = backButtonNode->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        textLayoutProperty->UpdateContent(BACK_BUTTON);
        navigationGroupNode->SetBackButton(backButtonNode);
        navigationGroupNode->UpdateBackButtonNodeOperation(ChildNodeOperation::ADD);
        return;
    }
    if (needRemoveBackButton) {
        navigationGroupNode->UpdateBackButtonNodeOperation(ChildNodeOperation::REMOVE);
    }
}

void NavigationView::SetNavigationMode(NavigationMode mode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, NavigationMode, static_cast<NG::NavigationMode>(mode));
}

void NavigationView::SetHideTitleBar(bool hideTitleBar)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, HideTitleBar, hideTitleBar);
}

void NavigationView::SetHideBackButton(bool hideBackButton)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, HideBackButton, hideBackButton);
}

void NavigationView::SetHideToolBar(bool hideToolBar)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, HideToolBar, hideToolBar);
}

void NavigationView::SetToolBarItems(std::list<BarItem>&& toolBarItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    if (navigationGroupNode->GetPrevToolBarIsCustom().value_or(false)) {
        navigationGroupNode->UpdateToolBarNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navigationGroupNode->GetPreToolBarNode() &&
            static_cast<int32_t>(navigationGroupNode->GetPreToolBarNode()->GetChildren().size()) != 0) {
            UpdateOldBarItems(navigationGroupNode->GetPreToolBarNode(), toolBarItems);
            navigationGroupNode->SetToolBarNode(navigationGroupNode->GetPreToolBarNode());
            navigationGroupNode->UpdateToolBarNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navigationGroupNode->UpdateToolBarNodeOperation(ChildNodeOperation::ADD);
    }
    int32_t toolBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto toolBarNode = FrameNode::GetOrCreateFrameNode(
        V2::TOOL_BAR_ETS_TAG, toolBarNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    auto rowProperty = toolBarNode->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(rowProperty);
    rowProperty->UpdateMainAxisAlign(FlexAlign::SPACE_EVENLY);
    for (const auto& toolBarItem : toolBarItems) {
        int32_t barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto barItemNode = AceType::MakeRefPtr<BarItemNode>(V2::BAR_ITEM_ETS_TAG, barItemNodeId);
        barItemNode->InitializePatternAndContext();
        UpdateBarItemNodeWithItem(barItemNode, toolBarItem);
        toolBarNode->AddChild(barItemNode);
    }
    navigationGroupNode->SetToolBarNode(toolBarNode);
    navigationGroupNode->UpdatePrevToolBarIsCustom(false);
}

void NavigationView::SetCustomToolBar(const RefPtr<UINode>& customToolBar)
{
    CHECK_NULL_VOID(customToolBar);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto layoutProperty = navigationGroupNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    if (navigationGroupNode->HasPrevToolBarIsCustom() && navigationGroupNode->GetPrevToolBarIsCustomValue()) {
        if (customToolBar->GetId() == navigationGroupNode->GetToolBarNode()->GetId()) {
            navigationGroupNode->UpdateToolBarNodeOperation(ChildNodeOperation::NONE);
            navigationGroupNode->UpdatePrevToolBarIsCustom(true);
            return;
        }
    }
    navigationGroupNode->UpdateToolBarNodeOperation(ChildNodeOperation::REPLACE);
    navigationGroupNode->SetToolBarNode(customToolBar);
    navigationGroupNode->UpdatePrevToolBarIsCustom(true);
}

void NavigationView::SetOnTitleModeChange(std::function<void(NavigationTitleMode)>&& onTitleModeChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto eventHub = navigationGroupNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnTitleModeChange(std::move(onTitleModeChange));
}

} // namespace OHOS::Ace::NG