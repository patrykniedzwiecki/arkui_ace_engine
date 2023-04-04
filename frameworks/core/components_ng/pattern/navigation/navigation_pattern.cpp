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

#include "core/components_ng/pattern/navigation/navigation_pattern.h"

#include "core/components_ng/pattern/navigation/navigation_event_hub.h"
#include "core/components_ng/pattern/navigation/nav_bar_layout_property.h"
#include "core/components_ng/pattern/navigation/nav_bar_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_group_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_layout_property.h"
#include "core/components_ng/pattern/navrouter/navrouter_group_node.h"

namespace OHOS::Ace::NG {

constexpr int32_t MAX_NAVIGATION_CHILDREN_SIZE = 3;

namespace {

void MountNavBar(const RefPtr<NavigationGroupNode>& hostNode)
{
    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    navBarNode->MarkModifyDone();
}

}

void NavigationPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    MountNavBar(hostNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    navBarNode->MarkModifyDone();
}

bool NavigationPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipMeasure && config.skipLayout) {
        return false;
    }
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);
    auto navigationLayoutAlgorithm =
        DynamicCast<NavigationLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(navigationLayoutAlgorithm, false);
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, false);
    auto navigationLayoutProperty = AceType::DynamicCast<NavigationLayoutProperty>(hostNode->GetLayoutProperty());
    CHECK_NULL_RETURN(navigationLayoutProperty, false);
    navigationLayoutProperty->UpdateNavigationMode(navigationLayoutAlgorithm->GetNavigationMode());

    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_RETURN(navBarNode, false);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_RETURN(navBarLayoutProperty, false);
    auto eventHub = hostNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_RETURN(eventHub, false);
    if (navigationLayoutProperty->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE) {
        eventHub->FireNavBarStateChangeEvent(false);
    } else {
        if (navigationLayoutAlgorithm->GetNavigationMode() == NavigationMode::SPLIT) {
            if (navigationLayoutProperty->GetHideNavBar().value_or(false)) {
                navBarLayoutProperty->UpdateVisibility(VisibleType::GONE);
                eventHub->FireNavBarStateChangeEvent(false);
            } else {
                navBarLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
                eventHub->FireNavBarStateChangeEvent(true);
            }
        }
    }

    auto navigationMode = navigationLayoutAlgorithm->GetNavigationMode();
    auto navigationChildrenSize = hostNode->GetChildren().size();
    if (navigationMode == NavigationMode::STACK) {
        auto contentNode = hostNode->GetContentNode();
        CHECK_NULL_RETURN(contentNode, false);
        auto contentChildSize = contentNode->GetChildren().size();
        if (contentChildSize != 0) {
            auto contentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
            CHECK_NULL_RETURN(contentNode, false);
            hostNode->AddChild(contentNode);
            auto dividerNode = AceType::DynamicCast<FrameNode>(hostNode->GetDividerNode());
            CHECK_NULL_RETURN(dividerNode, false);
            hostNode->AddChild(dividerNode);
            hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        } else {
            auto contentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
            CHECK_NULL_RETURN(contentNode, false);
            hostNode->RemoveChild(contentNode);
            auto dividerNode = AceType::DynamicCast<FrameNode>(hostNode->GetDividerNode());
            CHECK_NULL_RETURN(dividerNode, false);
            hostNode->RemoveChild(dividerNode);
        }
    } else {
        if (navigationChildrenSize != MAX_NAVIGATION_CHILDREN_SIZE) {
            auto contentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
            CHECK_NULL_RETURN(contentNode, false);
            hostNode->AddChild(contentNode);
            auto dividerNode = AceType::DynamicCast<FrameNode>(hostNode->GetDividerNode());
            CHECK_NULL_RETURN(dividerNode, false);
            hostNode->AddChild(dividerNode);
            hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
    }

    return false;
}

} // namespace OHOS::Ace::NG