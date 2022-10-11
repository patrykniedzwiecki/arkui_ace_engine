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

#include "bridge/declarative_frontend/jsview/models/tabs_model_impl.h"

#include "bridge/declarative_frontend/jsview/js_container_base.h"
#include "bridge/declarative_frontend/view_stack_processor.h"
#include "core/components/tab_bar/tab_bar_component.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/components/tab_bar/tab_theme.h"
#include "core/components_v2/tabs/tabs_component.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_utils.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_view_abstract.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension DEFAULT_TAB_BAR_HEIGHT = 56.0_vp;

} // namespace

void TabsModelImpl::Create(BarPosition barPosition, int32_t /*index*/, const RefPtr<TabController>& tabController,
    const RefPtr<SwiperController>& /*swiperController*/)
{
    std::list<RefPtr<Component>> children;
    auto tabsComponent = AceType::MakeRefPtr<V2::TabsComponent>(children, barPosition, tabController);
    auto tabBar = tabsComponent->GetTabBarChild();
    if (tabBar) {
        tabBar->InitStyle(GetTheme());
        auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
        if (box) {
            box->SetHeight(DEFAULT_TAB_BAR_HEIGHT);
        }
    }
    ViewStackProcessor::GetInstance()->PushTabs(tabsComponent);
    ViewStackProcessor::GetInstance()->Push(tabsComponent);
}

void TabsModelImpl::Pop()
{
    ViewStackProcessor::GetInstance()->PopTabs();
    JSContainerBase::Pop();
}

void TabsModelImpl::SetIndex(int32_t index)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        LOGE("can not find tabs component");
        return;
    }
    auto controller = component->GetTabsController();
    if (controller) {
        controller->SetPendingIndex(index);
    } else {
        LOGE("can not find controller");
    }
}

void TabsModelImpl::SetTabBarPosition(BarPosition tabBarPosition)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (tabBar) {
        tabBar->SetBarPosition(tabBarPosition);
    }
}

void TabsModelImpl::SetTabBarMode(TabBarMode tabBarMode)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (tabBar) {
        tabBar->SetMode(tabBarMode);
    }
}

void TabsModelImpl::SetTabBarWidth(const Dimension& tabBarWidth)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (!tabBar) {
        LOGE("can not find tab bar component");
        return;
    }
    auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
    if (box) {
        box->SetWidth(tabBarWidth);
    } else {
        LOGE("can not find box component");
    }
}

void TabsModelImpl::SetTabBarHeight(const Dimension& tabBarHeight)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabBar = component->GetTabBarChild();
    if (!tabBar) {
        LOGE("can not find tab bar component");
        return;
    }
    auto box = AceType::DynamicCast<BoxComponent>(tabBar->GetParent().Upgrade());
    if (box) {
        box->SetHeight(tabBarHeight);
    } else {
        LOGE("can not find box component");
    }
}

void TabsModelImpl::SetIsVertical(bool isVertical)
{
    auto tabsComponent = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!tabsComponent) {
        return;
    }
    if (isVertical) {
        tabsComponent->SetDirection(FlexDirection::ROW);
    } else {
        tabsComponent->SetDirection(FlexDirection::COLUMN);
    }
    auto tabBar = tabsComponent->GetTabBarChild();
    if (tabBar) {
        tabBar->SetVertical(isVertical);
    }
    auto tabContent = tabsComponent->GetTabContentChild();
    if (tabContent) {
        tabContent->SetVertical(isVertical);
    }
}

void TabsModelImpl::SetScrollable(bool scrollable)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabContent = component->GetTabContentChild();
    if (tabContent) {
        tabContent->SetScrollable(scrollable);
    }
}

void TabsModelImpl::SetAnimationDuration(float duration)
{
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (!component) {
        return;
    }
    auto tabContent = component->GetTabContentChild();
    if (!tabContent) {
        return;
    }
    tabContent->SetScrollDuration(duration);
}

void TabsModelImpl::SetOnChange(std::function<void(const BaseEventInfo*)>&& onChange)
{
    auto onChangeEvent = EventMarker(std::move(onChange));
    auto component = AceType::DynamicCast<V2::TabsComponent>(ViewStackProcessor::GetInstance()->GetMainComponent());
    if (component) {
        auto tabContent = component->GetTabContentChild();
        if (tabContent) {
            tabContent->SetChangeEventId(onChangeEvent);
        }
    }
}

RefPtr<TabTheme> TabsModelImpl::GetTheme() const
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    if (!pipelineContext) {
        LOGE("pipelineContext is null!");
        return nullptr;
    }
    auto themeManager = pipelineContext->GetThemeManager();
    if (!themeManager) {
        LOGE("themeManager is null!");
        return nullptr;
    }
    return themeManager->GetTheme<TabTheme>();
}

} // namespace OHOS::Ace::Framework
