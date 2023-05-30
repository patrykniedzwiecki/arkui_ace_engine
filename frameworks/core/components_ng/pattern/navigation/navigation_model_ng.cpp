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

#include "core/components_ng/pattern/navigation/navigation_model_ng.h"

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/divider/divider_layout_property.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/divider/divider_render_property.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/pattern/navigation/bar_item_event_hub.h"
#include "core/components_ng/pattern/navigation/bar_item_node.h"
#include "core/components_ng/pattern/navigation/bar_item_pattern.h"
#include "core/components_ng/pattern/navigation/nav_bar_layout_property.h"
#include "core/components_ng/pattern/navigation/nav_bar_node.h"
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_declaration.h"
#include "core/components_ng/pattern/navigation/navigation_event_hub.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_layout_property.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navigator/navigator_event_hub.h"
#include "core/components_ng/pattern/navigator/navigator_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_group_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_layout_property.h"
#include "core/components_ng/pattern/navrouter/navrouter_group_node.h"
#include "core/components_ng/pattern/option/option_view.h"
#include "core/components_ng/pattern/select/select_model.h"
#include "core/components_ng/pattern/select/select_model_ng.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
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
    textLayoutProperty->UpdateTextAlign(TextAlign::CENTER);
    return textNode;
}

RefPtr<FrameNode> CreateBarItemIconNode(const std::string& src)
{
    int32_t nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    ImageSourceInfo info(src);
    auto iconNode = FrameNode::CreateFrameNode(V2::IMAGE_ETS_TAG, nodeId, AceType::MakeRefPtr<ImagePattern>());
    auto imageLayoutProperty = iconNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_RETURN(imageLayoutProperty, nullptr);
    auto theme = NavigationGetTheme();
    CHECK_NULL_RETURN(theme, nullptr);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navigationGroupNode, nullptr);
    auto hub = navigationGroupNode->GetEventHub<EventHub>();
    CHECK_NULL_RETURN(hub, nullptr);
    if (!hub->IsEnabled()) {
        info.SetFillColor(theme->GetMenuIconColor().BlendOpacity(theme->GetAlphaDisabled()));
    } else {
        info.SetFillColor(theme->GetMenuIconColor());
    }
    imageLayoutProperty->UpdateImageSourceInfo(info);

    auto iconSize = theme->GetMenuIconSize();
    imageLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(iconSize), CalcLength(iconSize)));
    iconNode->MarkModifyDone();
    return iconNode;
}

void UpdateBarItemNodeWithItem(const RefPtr<BarItemNode>& barItemNode, const BarItem& barItem)
{
    if (barItem.text.has_value() && !barItem.text.value().empty()) {
        auto textNode = CreateBarItemTextNode(barItem.text.value());
        barItemNode->SetTextNode(textNode);
        barItemNode->AddChild(textNode);
    }
    if (barItem.icon.has_value() && !barItem.icon.value().empty()) {
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

void BuildMoreIemNode(const RefPtr<BarItemNode>& barItemNode)
{
    int32_t imageNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto imageNode = FrameNode::CreateFrameNode(V2::IMAGE_ETS_TAG, imageNodeId, AceType::MakeRefPtr<ImagePattern>());
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_VOID(imageLayoutProperty);
    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto hub = navigationGroupNode->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto info = ImageSourceInfo("");
    info.SetResourceId(theme->GetMoreResourceId());
    if (!hub->IsEnabled()) {
        info.SetFillColor(theme->GetMenuIconColor().BlendOpacity(theme->GetAlphaDisabled()));
    } else {
        info.SetFillColor(theme->GetMenuIconColor());
    }
    imageLayoutProperty->UpdateImageSourceInfo(info);

    auto iconSize = theme->GetMenuIconSize();
    imageLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(iconSize), CalcLength(iconSize)));
    imageNode->MarkModifyDone();

    barItemNode->SetIconNode(imageNode);
    barItemNode->AddChild(imageNode);
    barItemNode->MarkModifyDone();
}

void BuildMoreItemNodeAction(const RefPtr<BarItemNode>& barItemNode, const RefPtr<FrameNode>& barMenuNode)
{
    auto eventHub = barItemNode->GetEventHub<BarItemEventHub>();
    CHECK_NULL_VOID(eventHub);

    auto context = PipelineContext::GetCurrentContext();
    auto clickCallback = [weakContext = AceType::WeakClaim(AceType::RawPtr(context)), id = barItemNode->GetId(),
                             weakMenu = AceType::WeakClaim(AceType::RawPtr(barMenuNode)),
                             weakBarItemNode = AceType::WeakClaim(AceType::RawPtr(barItemNode))]() {
        auto context = weakContext.Upgrade();
        CHECK_NULL_VOID(context);

        auto overlayManager = context->GetOverlayManager();
        CHECK_NULL_VOID(overlayManager);

        auto menu = weakMenu.Upgrade();
        CHECK_NULL_VOID(menu);

        auto barItemNode = weakBarItemNode.Upgrade();
        CHECK_NULL_VOID(barItemNode);

        auto imageNode = barItemNode->GetChildAtIndex(0);
        CHECK_NULL_VOID(imageNode);

        auto imageFrameNode = AceType::DynamicCast<FrameNode>(imageNode);
        CHECK_NULL_VOID(imageFrameNode);
        auto imgOffset = imageFrameNode->GetOffsetRelativeToWindow();
        auto imageSize = imageFrameNode->GetGeometryNode()->GetFrameSize();

        auto menuNode = AceType::DynamicCast<FrameNode>(menu->GetChildAtIndex(0));
        CHECK_NULL_VOID(menuNode);
        auto menuLayoutProperty = menuNode->GetLayoutProperty<MenuLayoutProperty>();
        CHECK_NULL_VOID(menuLayoutProperty);
        menuLayoutProperty->UpdateTargetSize(imageSize);

        imgOffset.SetX(imgOffset.GetX());
        imgOffset.SetY(imgOffset.GetY() + imageSize.Height());
        overlayManager->ShowMenu(id, imgOffset, menu);
    };
    eventHub->SetItemAction(clickCallback);
}

void UpdateOldBarItems(const RefPtr<UINode>& oldBarContainer, const std::vector<BarItem>& newBarItems)
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
            oldBarContainer->RemoveChild(oldBarItems.back());
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
        }
    }
    auto container = AceType::DynamicCast<TitleBarNode>(oldBarContainer);
    CHECK_NULL_VOID(container);
    container->MarkModifyDone();
}
} // namespace

void NavigationModelNG::Create()
{
    auto* stack = ViewStackProcessor::GetInstance();
    // navigation node
    int32_t nodeId = stack->ClaimNodeId();
    auto navigationGroupNode = NavigationGroupNode::GetOrCreateGroupNode(
        V2::NAVIGATION_VIEW_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    // navBar node
    if (!navigationGroupNode->GetNavBarNode()) {
        int32_t navBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto navBarNode = NavBarNode::GetOrCreateNavBarNode(
            V2::NAVBAR_ETS_TAG, navBarNodeId, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
        auto navBarRenderContext = navBarNode->GetRenderContext();
        CHECK_NULL_VOID(navBarRenderContext);
        navBarRenderContext->UpdateClipEdge(true);
        navigationGroupNode->AddChild(navBarNode);
        navigationGroupNode->SetNavBarNode(navBarNode);

        // titleBar node
        if (!navBarNode->GetTitleBarNode()) {
            int32_t titleBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
            auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
                V2::TITLE_BAR_ETS_TAG, titleBarNodeId, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
            navBarNode->AddChild(titleBarNode);
            navBarNode->SetTitleBarNode(titleBarNode);
        }

        // navBar content node
        if (!navBarNode->GetNavBarContentNode()) {
            int32_t navBarContentNodeId = ElementRegister::GetInstance()->MakeUniqueId();
            auto navBarContentNode = FrameNode::GetOrCreateFrameNode(V2::NAVBAR_CONTENT_ETS_TAG, navBarContentNodeId,
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
            auto navBarContentRenderContext = navBarContentNode->GetRenderContext();
            CHECK_NULL_VOID(navBarContentRenderContext);
            navBarContentRenderContext->UpdateClipEdge(true);
            navBarNode->AddChild(navBarContentNode);
            navBarNode->SetNavBarContentNode(navBarContentNode);
        }

        // toolBar node
        if (!navBarNode->GetToolBarNode()) {
            int32_t toolBarNodeId = ElementRegister::GetInstance()->MakeUniqueId();
            auto toolBarNode = FrameNode::GetOrCreateFrameNode(
                V2::TOOL_BAR_ETS_TAG, toolBarNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
            navBarNode->AddChild(toolBarNode);
            navBarNode->SetToolBarNode(toolBarNode);
            navBarNode->SetPreToolBarNode(toolBarNode);
            navBarNode->UpdatePrevToolBarIsCustom(false);
        }
        auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
        CHECK_NULL_VOID(navBarLayoutProperty);
        navBarLayoutProperty->UpdateTitleMode(NavigationTitleMode::FREE);
    }

    // content node
    if (!navigationGroupNode->GetContentNode()) {
        int32_t contentNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto contentNode = FrameNode::GetOrCreateFrameNode(
            V2::NAVIGATION_CONTENT_ETS_TAG, contentNodeId, []() { return AceType::MakeRefPtr<StackPattern>(); });
        navigationGroupNode->AddChild(contentNode);
        navigationGroupNode->SetContentNode(contentNode);
    }

    // divider node
    if (!navigationGroupNode->GetDividerNode()) {
        int32_t dividerNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto dividerNode = FrameNode::GetOrCreateFrameNode(
            V2::DIVIDER_ETS_TAG, dividerNodeId, []() { return AceType::MakeRefPtr<DividerPattern>(); });
        navigationGroupNode->AddChild(dividerNode);
        navigationGroupNode->SetDividerNode(dividerNode);

        auto dividerLayoutProperty = dividerNode->GetLayoutProperty<DividerLayoutProperty>();
        CHECK_NULL_VOID(dividerLayoutProperty);
        dividerLayoutProperty->UpdateStrokeWidth(DIVIDER_WIDTH);
        dividerLayoutProperty->UpdateVertical(true);
        auto dividerRenderProperty = dividerNode->GetPaintProperty<DividerRenderProperty>();
        CHECK_NULL_VOID(dividerRenderProperty);
        dividerRenderProperty->UpdateDividerColor(DIVIDER_COLOR);
    }

    stack->Push(navigationGroupNode);
    auto navigationLayoutProperty = navigationGroupNode->GetLayoutProperty<NavigationLayoutProperty>();
    navigationLayoutProperty->UpdateNavigationMode(NavigationMode::AUTO);
    navigationLayoutProperty->UpdateNavBarWidth(DEFAULT_NAV_BAR_WIDTH);
}

bool NavigationModelNG::ParseCommonTitle(bool hasSubTitle, bool hasMainTitle, const std::string& subtitle,
    const std::string& title)
{
    bool isCommonTitle = false;
    if (hasSubTitle) {
        SetSubtitle(subtitle);
        isCommonTitle = true;
    }
    if (hasMainTitle) {
        SetTitle(title, hasSubTitle);
        isCommonTitle = true;
    }
    return isCommonTitle;
}

void NavigationModelNG::SetTitle(const std::string& title, bool hasSubTitle)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    do {
        if (!navBarNode->GetTitle()) {
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::ADD);
            break;
        }
        // if previous title is not a frame node, we remove it and create a new node
        auto titleNode = AceType::DynamicCast<FrameNode>(navBarNode->GetTitle());
        if (!titleNode) {
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        auto titleProperty = titleNode->GetLayoutProperty<TextLayoutProperty>();
        // if no subtitle, title's maxLine = 2. if has subtitle, title's maxLine = 1.
        if (!hasSubTitle) {
            if (navBarNode->GetSubtitle()) {
                auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
                CHECK_NULL_VOID(titleBarNode);
                titleBarNode->RemoveChild(navBarNode->GetSubtitle());
                titleBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
                navBarNode->SetSubtitle(nullptr);
            }
            titleProperty->UpdateMaxLines(2); // 2:title's maxLine.
        } else {
            titleProperty->UpdateMaxLines(1); // 1:title's maxLine.
        }
        // previous title is not a text node and might be custom, we remove it and create a new node
        if (!titleProperty) {
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        // text content is the same, do nothing
        if (titleProperty->GetContentValue() == title) {
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        // update title content only without changing node
        titleProperty->UpdateContent(title);
        navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
        navBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        return;
    } while (false);
    int32_t titleNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, titleNodeId, AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = titleNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    textLayoutProperty->UpdateContent(title);

    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_VOID(navBarLayoutProperty);
    if (navBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) == NavigationTitleMode::MINI) {
        textLayoutProperty->UpdateFontSize(theme->GetTitleFontSize());
    } else {
        textLayoutProperty->UpdateFontSize(theme->GetTitleFontSizeBig());
    }
    textLayoutProperty->UpdateTextColor(theme->GetTitleColor());
    textLayoutProperty->UpdateFontWeight(FontWeight::MEDIUM);
    if (!hasSubTitle) {
        textLayoutProperty->UpdateMaxLines(2); // 2:title's maxLine.
    } else {
        textLayoutProperty->UpdateMaxLines(1); // 1:title's maxLine.
    }
    textLayoutProperty->UpdateTextOverflow(TextOverflow::ELLIPSIS);
    navBarNode->SetTitle(titleNode);
    navBarNode->UpdatePrevTitleIsCustom(false);
}

void NavigationModelNG::SetCustomTitle(const RefPtr<AceType>& customNode)
{
    auto customTitle = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customTitle);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    if (navBarNode->GetTitle()) {
        if (customTitle->GetId() == navBarNode->GetTitle()->GetId()) {
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::NONE);
        } else {
            navBarNode->SetTitle(customTitle);
            navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::REPLACE);
            navBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
        return;
    }
    navBarNode->SetTitle(customTitle);
    navBarNode->UpdateTitleNodeOperation(ChildNodeOperation::ADD);
    navBarNode->UpdatePrevTitleIsCustom(true);
}

void NavigationModelNG::SetTitleHeight(const Dimension& height)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_VOID(titleBarLayoutProperty);
    titleBarLayoutProperty->UpdateTitleHeight(height);
}

void NavigationModelNG::SetTitleMode(NG::NavigationTitleMode mode)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_VOID(navBarLayoutProperty);
    bool needAddBackButton = false;
    bool needRemoveBackButton = false;

    do {
        // add back button if current mode is mini and one of the following condition:
        // first create or not first create but previous mode is not mini
        if (navBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) != NavigationTitleMode::MINI &&
            mode == NavigationTitleMode::MINI) {
            needAddBackButton = true;
            break;
        }
        // remove back button if current mode is not mini and previous mode is mini
        if (navBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) == NavigationTitleMode::MINI &&
            mode != NavigationTitleMode::MINI) {
            needRemoveBackButton = true;
            break;
        }
    } while (false);
    navBarLayoutProperty->UpdateTitleMode(static_cast<NG::NavigationTitleMode>(mode));
    if (needAddBackButton) {
        // put component inside navigator pattern to trigger back navigation
        auto navigator = FrameNode::CreateFrameNode(V2::NAVIGATOR_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<NavigatorPattern>());
        auto hub = navigator->GetEventHub<NavigatorEventHub>();
        CHECK_NULL_VOID(hub);
        hub->SetType(NavigatorType::BACK);
        navigator->MarkModifyDone();

        auto backButtonNode = FrameNode::CreateFrameNode(V2::BACK_BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ButtonPattern>());
        CHECK_NULL_VOID(backButtonNode);
        auto backButtonLayoutProperty = backButtonNode->GetLayoutProperty<ButtonLayoutProperty>();
        CHECK_NULL_VOID(backButtonLayoutProperty);
        backButtonLayoutProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(BACK_BUTTON_SIZE.ConvertToPx()), CalcLength(BACK_BUTTON_SIZE.ConvertToPx())));
        backButtonLayoutProperty->UpdateType(ButtonType::NORMAL);
        backButtonLayoutProperty->UpdateBorderRadius(BUTTON_RADIUS);
        backButtonLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
        auto renderContext = backButtonNode->GetRenderContext();
        CHECK_NULL_VOID(renderContext);
        renderContext->UpdateBackgroundColor(Color::TRANSPARENT);

        auto eventHub = backButtonNode->GetOrCreateInputEventHub();
        CHECK_NULL_VOID(eventHub);
        eventHub->SetHoverEffect(HoverEffectType::BOARD);

        PaddingProperty padding;
        padding.left = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.right = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.top = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.bottom = CalcLength(BUTTON_PADDING.ConvertToPx());
        backButtonLayoutProperty->UpdatePadding(padding);

        auto backButtonImageNode = FrameNode::CreateFrameNode(V2::BACK_BUTTON_IMAGE_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
        CHECK_NULL_VOID(backButtonImageNode);
        auto theme = NavigationGetTheme();
        CHECK_NULL_VOID(theme);
        ImageSourceInfo imageSourceInfo;
        imageSourceInfo.SetResourceId(theme->GetBackResourceId());
        auto backButtonImageLayoutProperty = backButtonImageNode->GetLayoutProperty<ImageLayoutProperty>();
        CHECK_NULL_VOID(backButtonImageLayoutProperty);

        auto navigationEventHub = navigationGroupNode->GetEventHub<EventHub>();
        CHECK_NULL_VOID(navigationEventHub);
        if (!navigationEventHub->IsEnabled()) {
            imageSourceInfo.SetFillColor(theme->GetBackButtonIconColor().BlendOpacity(theme->GetAlphaDisabled()));
        } else {
            imageSourceInfo.SetFillColor(theme->GetBackButtonIconColor());
        }
        backButtonImageLayoutProperty->UpdateImageSourceInfo(imageSourceInfo);
        backButtonImageLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);

        backButtonImageNode->MountToParent(backButtonNode);
        backButtonImageNode->MarkModifyDone();
        backButtonNode->MountToParent(navigator);
        backButtonNode->MarkModifyDone();

        navBarNode->SetBackButton(navigator);
        navBarNode->UpdateBackButtonNodeOperation(ChildNodeOperation::ADD);
        return;
    }
    if (needRemoveBackButton) {
        navBarNode->UpdateBackButtonNodeOperation(ChildNodeOperation::REMOVE);
    }
}

void NavigationModelNG::SetSubtitle(const std::string& subtitle)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    do {
        if (!navBarNode->GetSubtitle()) {
            navBarNode->UpdateSubtitleNodeOperation(ChildNodeOperation::ADD);
            break;
        }
        auto subtitleNode = AceType::DynamicCast<FrameNode>(navBarNode->GetSubtitle());
        if (!subtitleNode) {
            navBarNode->UpdateSubtitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        auto subtitleProperty = subtitleNode->GetLayoutProperty<TextLayoutProperty>();
        if (!subtitleProperty) {
            navBarNode->UpdateSubtitleNodeOperation(ChildNodeOperation::REPLACE);
            break;
        }
        if (subtitleProperty->GetContentValue() == subtitle) {
            navBarNode->UpdateSubtitleNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        subtitleProperty->UpdateContent(subtitle);
        navBarNode->UpdateSubtitleNodeOperation(ChildNodeOperation::NONE);
        navBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        return;
    } while (false);
    int32_t subtitleNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto subtitleNode =
        FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, subtitleNodeId, AceType::MakeRefPtr<TextPattern>());
    auto textLayoutProperty = subtitleNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(textLayoutProperty);
    textLayoutProperty->UpdateContent(subtitle);
    textLayoutProperty->UpdateFontSize(SUBTITLE_FONT_SIZE);
    textLayoutProperty->UpdateTextColor(SUBTITLE_COLOR);
    textLayoutProperty->UpdateFontWeight(FontWeight::REGULAR); // ohos_id_text_font_family_regular
    textLayoutProperty->UpdateMaxLines(1);
    textLayoutProperty->UpdateTextOverflow(TextOverflow::ELLIPSIS);
    navBarNode->SetSubtitle(subtitleNode);
}

void NavigationModelNG::SetHideTitleBar(bool hideTitleBar)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_VOID(navBarLayoutProperty);
    navBarLayoutProperty->UpdateHideTitleBar(hideTitleBar);
}

void NavigationModelNG::SetHideNavBar(bool hideNavBar)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, HideNavBar, hideNavBar);
}

void NavigationModelNG::SetBackButtonIcon(const std::string& src, bool noPixMap, RefPtr<PixelMap>& pixMap)
{
    ImageSourceInfo imageSourceInfo(src);
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, NoPixMap, noPixMap);
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, ImageSource, imageSourceInfo);
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, PixelMap, pixMap);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);

    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarContentNode = navBarNode->GetNavBarContentNode();
    CHECK_NULL_VOID(navBarContentNode);
    if (navBarContentNode->GetChildren().empty()) {
        return;
    }
    for (const auto& navBarContentChild : navBarContentNode->GetChildren()) {
        auto navBarContentChildFrameNode = AceType::DynamicCast<FrameNode>(navBarContentChild);
        CHECK_NULL_VOID(navBarContentChildFrameNode);
        if (navBarContentChildFrameNode->GetTag() != V2::NAVROUTER_VIEW_ETS_TAG) {
            return;
        }

        auto navigationPattern = navigationGroupNode->GetPattern<NavigationPattern>();
        CHECK_NULL_VOID(navigationPattern);
        auto navDestinationNode =
            AceType::DynamicCast<NavDestinationGroupNode>(navigationPattern->GetNavDestinationNode());
        CHECK_NULL_VOID(navDestinationNode);
        auto navDestinationLayoutProperty = navDestinationNode->GetLayoutProperty<NavDestinationLayoutProperty>();
        navDestinationLayoutProperty->UpdateImageSource(imageSourceInfo);
        navDestinationLayoutProperty->UpdateNoPixMap(noPixMap);
        navDestinationLayoutProperty->UpdatePixelMap(pixMap);
        navDestinationNode->MarkModifyDone();
    }
}

void NavigationModelNG::SetHideBackButton(bool hideBackButton)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_VOID(navBarLayoutProperty);
    navBarLayoutProperty->UpdateHideBackButton(hideBackButton);
}

void NavigationModelNG::SetHideToolBar(bool hideToolBar)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_VOID(navBarLayoutProperty);
    navBarLayoutProperty->UpdateHideToolBar(hideToolBar);
}

void NavigationModelNG::SetCustomToolBar(const RefPtr<AceType>& customNode)
{
    auto customToolBar = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customToolBar);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    if (navBarNode->GetPrevToolBarIsCustom().value_or(false)) {
        if (customToolBar->GetId() == navBarNode->GetToolBarNode()->GetId()) {
            navBarNode->UpdateToolBarNodeOperation(ChildNodeOperation::NONE);
            navBarNode->UpdatePrevToolBarIsCustom(true);
            return;
        }
    }
    navBarNode->UpdateToolBarNodeOperation(ChildNodeOperation::REPLACE);
    auto toolBarNode = navBarNode->GetToolBarNode();
    CHECK_NULL_VOID(toolBarNode);
    customToolBar->MountToParent(toolBarNode);
    navBarNode->UpdatePrevToolBarIsCustom(true);
}

bool NavigationModelNG::NeedSetItems()
{
    return true;
}

void NavigationModelNG::SetToolBarItems(std::vector<NG::BarItem>&& toolBarItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    if (navBarNode->GetPrevToolBarIsCustom().value_or(false)) {
        navBarNode->UpdateToolBarNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navBarNode->GetPreToolBarNode() &&
            static_cast<int32_t>(navBarNode->GetPreToolBarNode()->GetChildren().size()) != 0) {
            UpdateOldBarItems(navBarNode->GetPreToolBarNode(), toolBarItems);
            navBarNode->SetToolBarNode(navBarNode->GetPreToolBarNode());
            navBarNode->UpdateToolBarNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navBarNode->UpdateToolBarNodeOperation(ChildNodeOperation::REPLACE);
    }
    auto toolBarNode = AceType::DynamicCast<FrameNode>(navBarNode->GetPreToolBarNode());
    CHECK_NULL_VOID(toolBarNode);
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
    navBarNode->SetToolBarNode(toolBarNode);
    navBarNode->SetPreToolBarNode(toolBarNode);
    navBarNode->UpdatePrevToolBarIsCustom(false);
}

void NavigationModelNG::SetMenuItems(std::vector<NG::BarItem>&& menuItems)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    // if previous menu is custom, just remove it and create new menu, otherwise update old menu
    if (navBarNode->GetPrevMenuIsCustom().value_or(false)) {
        navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
    } else {
        if (navBarNode->GetMenu()) {
            navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        } else {
            navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
        }
    }
    int32_t menuNodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto menuNode = FrameNode::GetOrCreateFrameNode(
        V2::NAVIGATION_MENU_ETS_TAG, menuNodeId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    auto rowProperty = menuNode->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(rowProperty);
    rowProperty->UpdateMainAxisAlign(FlexAlign::SPACE_BETWEEN);
    auto theme = NavigationGetTheme();
    auto mostMenuItemCount = theme->GetMostMenuItemCountInBar();
    bool needMoreButton = menuItems.size() > mostMenuItemCount ? true : false;
    int32_t count = 0;
    std::vector<OptionParam> params;
    for (const auto& menuItem : menuItems) {
        ++count;
        if (needMoreButton && (count > mostMenuItemCount - 1)) {
            params.push_back({ menuItem.text.value(), menuItem.icon.value(), menuItem.action });
        } else {
            auto menuItemNode = FrameNode::CreateFrameNode(V2::MENU_ITEM_ETS_TAG,
                ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ButtonPattern>());
            CHECK_NULL_VOID(menuItemNode);
            auto menuItemLayoutProperty = menuItemNode->GetLayoutProperty<ButtonLayoutProperty>();
            CHECK_NULL_VOID(menuItemLayoutProperty);
            menuItemLayoutProperty->UpdateUserDefinedIdealSize(
                CalcSize(CalcLength(BACK_BUTTON_SIZE.ConvertToPx()), CalcLength(BACK_BUTTON_SIZE.ConvertToPx())));
            menuItemLayoutProperty->UpdateType(ButtonType::NORMAL);
            menuItemLayoutProperty->UpdateBorderRadius(BUTTON_RADIUS);
            auto renderContext = menuItemNode->GetRenderContext();
            CHECK_NULL_VOID(renderContext);
            renderContext->UpdateBackgroundColor(Color::TRANSPARENT);

            auto eventHub = menuItemNode->GetOrCreateInputEventHub();
            CHECK_NULL_VOID(eventHub);
            eventHub->SetHoverEffect(HoverEffectType::BOARD);

            PaddingProperty padding;
            padding.left = CalcLength(BUTTON_PADDING.ConvertToPx());
            padding.right = CalcLength(BUTTON_PADDING.ConvertToPx());
            padding.top = CalcLength(BUTTON_PADDING.ConvertToPx());
            padding.bottom = CalcLength(BUTTON_PADDING.ConvertToPx());
            menuItemLayoutProperty->UpdatePadding(padding);

            int32_t barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
            auto barItemNode = AceType::MakeRefPtr<BarItemNode>(V2::BAR_ITEM_ETS_TAG, barItemNodeId);
            barItemNode->InitializePatternAndContext();
            UpdateBarItemNodeWithItem(barItemNode, menuItem);
            auto barItemLayoutProperty = barItemNode->GetLayoutProperty();
            CHECK_NULL_VOID(barItemLayoutProperty);
            barItemLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);

            barItemNode->MountToParent(menuItemNode);
            barItemNode->MarkModifyDone();
            menuItemNode->MarkModifyDone();
            menuNode->AddChild(menuItemNode);
        }
    }

    // build more button
    if (needMoreButton) {
        int32_t barItemNodeId = ElementRegister::GetInstance()->MakeUniqueId();
        auto barItemNode = AceType::MakeRefPtr<BarItemNode>(V2::BAR_ITEM_ETS_TAG, barItemNodeId);
        barItemNode->InitializePatternAndContext();
        auto barItemLayoutProperty = barItemNode->GetLayoutProperty();
        CHECK_NULL_VOID(barItemLayoutProperty);
        barItemLayoutProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);

        BuildMoreIemNode(barItemNode);

        auto barMenuNode =
            MenuView::Create(std::move(params), barItemNodeId, V2::BAR_ITEM_ETS_TAG, MenuType::NAVIGATION_MENU);
        BuildMoreItemNodeAction(barItemNode, barMenuNode);
        auto menuItemNode = FrameNode::CreateFrameNode(V2::MENU_ITEM_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ButtonPattern>());
        CHECK_NULL_VOID(menuItemNode);
        auto menuItemLayoutProperty = menuItemNode->GetLayoutProperty<ButtonLayoutProperty>();
        CHECK_NULL_VOID(menuItemLayoutProperty);
        menuItemLayoutProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(BACK_BUTTON_SIZE.ConvertToPx()), CalcLength(BACK_BUTTON_SIZE.ConvertToPx())));
        menuItemLayoutProperty->UpdateType(ButtonType::NORMAL);
        menuItemLayoutProperty->UpdateBorderRadius(BUTTON_RADIUS);
        auto renderContext = menuItemNode->GetRenderContext();
        CHECK_NULL_VOID(renderContext);
        renderContext->UpdateBackgroundColor(Color::TRANSPARENT);

        auto eventHub = menuItemNode->GetOrCreateInputEventHub();
        CHECK_NULL_VOID(eventHub);
        eventHub->SetHoverEffect(HoverEffectType::BOARD);

        PaddingProperty padding;
        padding.left = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.right = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.top = CalcLength(BUTTON_PADDING.ConvertToPx());
        padding.bottom = CalcLength(BUTTON_PADDING.ConvertToPx());
        menuItemLayoutProperty->UpdatePadding(padding);

        barItemNode->MountToParent(menuItemNode);
        barItemNode->MarkModifyDone();
        menuItemNode->MarkModifyDone();
        menuNode->AddChild(menuItemNode);
        navBarNode->SetMenuNode(barMenuNode);
    }

    navBarNode->SetMenu(menuNode);
    navBarNode->UpdatePrevMenuIsCustom(false);
}

void NavigationModelNG::SetCustomMenu(const RefPtr<AceType>& customNode)
{
    auto customMenu = AceType::DynamicCast<NG::UINode>(customNode);
    CHECK_NULL_VOID(customMenu);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    // if previous menu exists, remove it if their ids are not the same
    // if previous node is not custom, their ids must not be the same
    if (navBarNode->GetMenu()) {
        if (customMenu->GetId() == navBarNode->GetMenu()->GetId()) {
            navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::NONE);
            return;
        }
        navBarNode->SetMenu(customMenu);
        navBarNode->UpdatePrevMenuIsCustom(true);
        navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::REPLACE);
        return;
    }
    navBarNode->SetMenu(customMenu);
    navBarNode->UpdatePrevMenuIsCustom(true);
    navBarNode->UpdateMenuNodeOperation(ChildNodeOperation::ADD);
}

void NavigationModelNG::SetOnTitleModeChange(std::function<void(NG::NavigationTitleMode)>&& onTitleModeChange,
    std::function<void(const BaseEventInfo* baseInfo)>&& eventInfo)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto eventHub = navigationGroupNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnTitleModeChange(std::move(onTitleModeChange));
}

void NavigationModelNG::SetUsrNavigationMode(NavigationMode mode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, UsrNavigationMode, mode);
}

void NavigationModelNG::SetNavBarPosition(NG::NavBarPosition mode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, NavBarPosition, static_cast<NG::NavBarPosition>(mode));
}

void NavigationModelNG::SetNavBarWidth(const Dimension& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, NavBarWidth, value);
}

void NavigationModelNG::SetOnNavBarStateChange(std::function<void(bool)>&& onNavBarStateChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationEventHub = AceType::DynamicCast<NavigationEventHub>(frameNode->GetEventHub<EventHub>());
    CHECK_NULL_VOID(navigationEventHub);
    navigationEventHub->SetOnNavBarStateChange(std::move(onNavBarStateChange));
}

void NavigationModelNG::SetNavigationMode(NavigationMode mode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(NavigationLayoutProperty, NavigationMode, mode);
}

void NavigationModelNG::SetNavigationStack(RefPtr<NG::NavigationStack>&& navigationStack)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto pattern = navigationGroupNode->GetPattern<NavigationPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetNavigationStack(std::move(navigationStack));
}

void NavigationModelNG::SetNavigationStack()
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto pattern = navigationGroupNode->GetPattern<NavigationPattern>();
    CHECK_NULL_VOID(pattern);
    auto navigationStack = pattern->GetNavigationStack();
    if (!navigationStack) {
        auto navigationStack = AceType::MakeRefPtr<NavigationStack>();
        pattern->SetNavigationStack(std::move(navigationStack));
    }
}

void NavigationModelNG::SetNavDestination(std::function<void(std::string)>&& builder)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_VOID(navigationGroupNode);
    auto pattern = navigationGroupNode->GetPattern<NavigationPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetNavDestination(std::move(builder));
}

RefPtr<NG::NavigationStack> NavigationModelNG::GetNavigationStack()
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    CHECK_NULL_RETURN(navigationGroupNode, nullptr);
    auto pattern = navigationGroupNode->GetPattern<NavigationPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    return pattern->GetNavigationStack();
}

void NavigationModelNG::SetMenuCount(int32_t menuCount)
{
    return;
}
} // namespace OHOS::Ace::NG
