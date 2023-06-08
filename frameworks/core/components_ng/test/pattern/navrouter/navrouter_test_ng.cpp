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

#include <cstddef>
#include <list>
#include <optional>

#include "gtest/gtest.h"

#include "base/window/drag_window.h"

#define private public
#define protected public
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/test/mock/mock_pixel_map.h"
#include "core/components/counter/counter_theme.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/navigation/bar_item_layout_algorithm.h"
#include "core/components_ng/pattern/navigation/nav_bar_node.h"
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_group_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_layout_algorithm.h"
#include "core/components_ng/pattern/navrouter/navdestination_model.h"
#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"
#include "core/components_ng/pattern/navrouter/navrouter_event_hub.h"
#include "core/components_ng/pattern/navrouter/navrouter_group_node.h"
#include "core/components_ng/pattern/navrouter/navrouter_model.h"
#include "core/components_ng/pattern/navrouter/navrouter_model_ng.h"
#include "core/components_ng/pattern/navrouter/navrouter_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string FONT_CONTEXT = "start";
} // namespace

class NavrouterTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    static RefPtr<FrameNode> CreateSlidingPanel(const TestProperty& testProperty);
};

void NavrouterTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}
void NavrouterTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}

void NavrouterTestNg::SetUp() {}
void NavrouterTestNg::TearDown() {}
/**
 * @tc.name: NavrouterTestNg001
 * @tc.desc: Test NavRouterGroupNode::AddChildToGroup.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNodey.
     */
    NavRouterModelNG navRouterModel;
    navRouterModel.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    /**
     * @tc.steps: step2. create navRouterNode then call navRouterGroupNode->AddChildToGroup.
     * @tc.expected: navRouterGroupNode->children_.empty() is false.
     */
    EXPECT_TRUE(navRouterGroupNode->children_.empty());
    auto navRouterNode = NavRouterGroupNode::GetOrCreateGroupNode(
        "navRouterNode", 11, []() { return AceType::MakeRefPtr<NavRouterPattern>(); });
    navRouterGroupNode->AddChildToGroup(navRouterNode);
    EXPECT_FALSE(navRouterGroupNode->children_.empty());
    /**
     * @tc.steps: step3. create navDestinationNode then call navRouterGroupNode->AddChildToGroup.
     * @tc.expected: navRouterGroupNode->navDestinationNode_ is not nullptr.
     */
    ASSERT_EQ(navRouterGroupNode->navDestinationNode_, nullptr);
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    navRouterGroupNode->AddChildToGroup(navDestinationNode);
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg002
 * @tc.desc: Test NavRouterGroupNode::OnDetachFromMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode and construct parent_ property.
     */
    NavRouterModelNG navRouterModel;
    navRouterModel.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto navRouterPattern = AceType::DynamicCast<NavRouterPattern>(navRouterGroupNode->pattern_);
    navRouterPattern->name_ = "navDestinationNode";
    /**
     * @tc.steps: step2. create navigationPattern and construct navigationStack_ property.
     */
    auto navigationPattern = AceType::MakeRefPtr<NavigationPattern>();
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    std::pair<std::string, RefPtr<UINode>> pair("navDestinationNode", navDestinationNode);
    stack->navPathList_.push_back(pair);
    navigationPattern->navigationStack_ = stack;
    /**
     * @tc.steps: step3. create parentNode and set isOnAnimation_ false.
     */
    auto parentNode = AceType::MakeRefPtr<NavigationGroupNode>("NavigationGroupNode", 11, navigationPattern);
    parentNode->isOnAnimation_ = false;

    auto navRouterNode = NavRouterGroupNode::GetOrCreateGroupNode(
        "navRouterNode", 11, []() { return AceType::MakeRefPtr<NavRouterPattern>(); });
    navRouterNode->children_.push_back(navDestinationNode);
    parentNode->contentNode_ = navRouterNode;

    navRouterGroupNode->parent_ = AceType::WeakClaim<NavigationGroupNode>(AceType::RawPtr(parentNode));
    navRouterGroupNode->OnDetachFromMainTree(false);
    ASSERT_NE(navRouterGroupNode->GetParent(), nullptr);
}

/**
 * @tc.name: NavrouterTestNg003
 * @tc.desc: Test NavRouterGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode and construct parent_ property.
     */
    NavRouterModelNG navRouterModel;
    navRouterModel.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto navRouterPattern = AceType::DynamicCast<NavRouterPattern>(navRouterGroupNode->pattern_);
    navRouterPattern->name_ = "navDestinationNode";
    /**
     * @tc.steps: step2. create navigationPattern and construct navigationStack_ property.
     */
    auto navigationPattern = AceType::MakeRefPtr<NavigationPattern>();
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    std::pair<std::string, RefPtr<UINode>> pair("navDestinationNode", navDestinationNode);
    stack->navPathList_.push_back(pair);
    navigationPattern->navigationStack_ = stack;
    /**
     * @tc.steps: step3. create parentNode and set isOnAnimation_ false.
     */
    auto parentNode = AceType::MakeRefPtr<NavigationGroupNode>("NavigationGroupNode", 11, navigationPattern);
    parentNode->isOnAnimation_ = false;

    auto navRouterNode = NavRouterGroupNode::GetOrCreateGroupNode(
        "navRouterNode", 11, []() { return AceType::MakeRefPtr<NavRouterPattern>(); });
    navRouterNode->children_.push_back(navDestinationNode);
    parentNode->contentNode_ = navRouterNode;

    navRouterGroupNode->parent_ = AceType::WeakClaim<NavigationGroupNode>(AceType::RawPtr(parentNode));

    auto backButton = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    backButton->eventHub_ = AceType::MakeRefPtr<EventHub>();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>("TitleBarNode", 66, AceType::MakeRefPtr<TitleBarPattern>());
    titleBarNode->backButton_ = backButton;

    auto navDestination = AceType::MakeRefPtr<NavDestinationGroupNode>(
        "NavDestinationGroupNode", 55, AceType::MakeRefPtr<NavDestinationPattern>());
    navDestination->titleBarNode_ = titleBarNode;
    navRouterGroupNode->navDestinationNode_ = navDestination;

    navRouterGroupNode->OnAttachToMainTree(false);
    ASSERT_NE(navRouterGroupNode->GetParent(), nullptr);
}

/**
 * @tc.name: NavrouterTestNg004
 * @tc.desc: Test NavRouterGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode and construct parent_ property.
     */
    NavRouterModelNG navRouterModel;
    navRouterModel.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);

    auto navigationPattern = AceType::MakeRefPtr<NavigationPattern>();
    auto parentNode = AceType::MakeRefPtr<NavigationGroupNode>("NavigationGroupNode", 11, navigationPattern);
    navRouterGroupNode->parent_ = AceType::WeakClaim(AceType::RawPtr(parentNode));
    ASSERT_NE(navRouterGroupNode->GetParent(), nullptr);
}

/**
 * @tc.name: NavrouterTestNg005
 * @tc.desc: Test NavRouterGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create mock theme manager.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto theme = AceType::MakeRefPtr<NavigationBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(theme));
    /**
     * @tc.steps: step2. create model then call set function.
     */
    NavDestinationModelNG model;
    model.Create();
    auto func = []() {};
    model.Create(func);
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    model.SetHideTitleBar(false);
    model.SetTitle("myTitle", true);
    auto customNode = CustomNode::CreateCustomNode(11, "customNode");
    navDestinationNode->title_ = customNode;
    model.SetTitle("myTitle", false);
    ASSERT_EQ(navDestinationNode->subtitle_, nullptr);

    model.SetSubtitle("mySubTitle");
    model.SetCustomTitle(customNode);
    model.SetTitleHeight(0);
    model.SetTitleHeight(1);
    auto onShow = []() {};
    model.SetOnShown(onShow);
    auto onHidden = []() {};
    model.SetOnHidden(onHidden);
    auto onBackPressed = []() { return true; };
    model.SetOnBackPressed(onBackPressed);
    ASSERT_NE(customNode, nullptr);
}

/**
 * @tc.name: NavrouterTestNg006
 * @tc.desc: Test NavDestinationGroupNode::AddChildToGroup.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navDestinationNode then call AddChildToGroup.
     * @tc.expected: navDestinationNode->children_.empty() is false.
     */
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 11, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto layoutProperty = AceType::MakeRefPtr<NavDestinationLayoutProperty>();
    navDestinationNode->layoutProperty_ = layoutProperty;
    auto backButton = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    navDestinationNode->AddChildToGroup(backButton);
    EXPECT_FALSE(navDestinationNode->children_.empty());
    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    navDestinationNode->contentNode_ = tempNode;
    navDestinationNode->AddChildToGroup(backButton);
}

/**
 * @tc.name: NavrouterTestNg007
 * @tc.desc: Test NavDestinationGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navDestinationNode then call AddChildToGroup.
     * @tc.expected: builder->executeDeepRenderDone_ is true.
     */
    auto func = []() { return FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>()); };
    auto builder = AceType::MakeRefPtr<ShallowBuilder>(func);
    auto pattern = AceType::MakeRefPtr<NavDestinationPattern>();
    pattern->shallowBuilder_ = builder;
    auto navDestinationNode = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);
    navDestinationNode->OnAttachToMainTree(false);
    EXPECT_TRUE(builder->executeDeepRenderDone_);
    pattern->shallowBuilder_ = builder;
    builder->executeDeepRenderDone_ = true;
    navDestinationNode->OnAttachToMainTree(false);
    EXPECT_TRUE(builder->executeDeepRenderDone_);
    pattern->shallowBuilder_ = nullptr;
    builder->executeDeepRenderDone_ = true;
    navDestinationNode->OnAttachToMainTree(false);
    EXPECT_TRUE(builder->executeDeepRenderDone_);
}
/**
 * @tc.name: NavrouterTestNg008
 * @tc.desc: Test NavDestinationLayoutAlgorithm::Measure.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create algorithm then prepare properties.
     */
    auto algorithm = AceType::MakeRefPtr<NavDestinationLayoutAlgorithm>();
    auto pattern = AceType::MakeRefPtr<NavDestinationPattern>();
    auto navDestinationNode = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>("TitleBarNode", 66, AceType::MakeRefPtr<TitleBarPattern>());
    navDestinationNode->titleBarNode_ = titleBarNode;
    navDestinationNode->children_.push_back(titleBarNode);

    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto layoutProperty = AceType::MakeRefPtr<LayoutProperty>();
    auto navDestinationLayoutProperty = AceType::MakeRefPtr<NavDestinationLayoutProperty>();

    LayoutConstraintF constraint;
    constraint.selfIdealSize.width_ = 20.0f;
    constraint.selfIdealSize.height_ = 30.0f;

    navDestinationLayoutProperty->layoutConstraint_ = constraint;
    navDestinationLayoutProperty->contentConstraint_ = constraint;

    LayoutWrapper* layoutWrapper = new LayoutWrapper(
        AceType::WeakClaim(AceType::RawPtr(navDestinationNode)), geometryNode, navDestinationLayoutProperty);
    SizeF sizeF(5.0f, 5.0f);

    layoutWrapper->currentChildCount_ = 1;
    /**
     * @tc.steps: step2. create navDestinationNode2 geometryNode2  then call Measure.
     */
    auto navDestinationNode2 = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);
    auto geometryNode2 = AceType::MakeRefPtr<GeometryNode>();
    auto navDestinationLayoutProperty2 = AceType::MakeRefPtr<NavDestinationLayoutProperty>();
    layoutWrapper->childrenMap_[0] = AceType::MakeRefPtr<LayoutWrapper>(
        AceType::WeakClaim(AceType::RawPtr(navDestinationNode2)), geometryNode2, navDestinationLayoutProperty2);
    algorithm->Measure(layoutWrapper);
    ASSERT_NE(geometryNode2, nullptr);

    navDestinationLayoutProperty->propHideTitleBar_ = true;
    algorithm->Measure(layoutWrapper);
    EXPECT_TRUE(navDestinationLayoutProperty->propHideTitleBar_.value());
    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    navDestinationNode->subtitle_ = tempNode;
    navDestinationLayoutProperty->propHideTitleBar_ = false;
    algorithm->Measure(layoutWrapper);
    EXPECT_FALSE(navDestinationLayoutProperty->propHideTitleBar_.value());
}

/**
 * @tc.name: NavrouterTestNg009
 * @tc.desc: Test NavDestinationLayoutAlgorithm::Layout.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create algorithm then prepare properties.
     */
    auto algorithm = AceType::MakeRefPtr<NavDestinationLayoutAlgorithm>();
    auto pattern = AceType::MakeRefPtr<NavDestinationPattern>();
    auto navDestinationNode = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>("TitleBarNode", 66, AceType::MakeRefPtr<TitleBarPattern>());
    navDestinationNode->titleBarNode_ = titleBarNode;
    navDestinationNode->children_.push_back(titleBarNode);
    auto tempNode2 = FrameNode::CreateFrameNode("BackButton", 66, AceType::MakeRefPtr<ButtonPattern>());
    navDestinationNode->contentNode_ = tempNode2;

    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto layoutProperty = AceType::MakeRefPtr<LayoutProperty>();
    auto navDestinationLayoutProperty = AceType::MakeRefPtr<NavDestinationLayoutProperty>();

    LayoutConstraintF constraint;
    constraint.selfIdealSize.width_ = 20.0f;
    constraint.selfIdealSize.height_ = 30.0f;

    navDestinationLayoutProperty->layoutConstraint_ = constraint;
    navDestinationLayoutProperty->contentConstraint_ = constraint;

    LayoutWrapper* layoutWrapper = new LayoutWrapper(
        AceType::WeakClaim(AceType::RawPtr(navDestinationNode)), geometryNode, navDestinationLayoutProperty);
    SizeF sizeF(5.0f, 5.0f);

    layoutWrapper->currentChildCount_ = 1;
    /**
     * @tc.steps: step2. create navDestinationNode2 geometryNode2  then call Measure.
     */
    auto navDestinationNode2 = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);
    auto geometryNode2 = AceType::MakeRefPtr<GeometryNode>();
    auto navDestinationLayoutProperty2 = AceType::MakeRefPtr<NavDestinationLayoutProperty>();
    layoutWrapper->childrenMap_[0] = AceType::MakeRefPtr<LayoutWrapper>(
        AceType::WeakClaim(AceType::RawPtr(navDestinationNode2)), geometryNode2, navDestinationLayoutProperty2);
    algorithm->Layout(layoutWrapper);
    ASSERT_NE(geometryNode2, nullptr);

    navDestinationLayoutProperty->propHideTitleBar_ = true;
    algorithm->Layout(layoutWrapper);
    EXPECT_TRUE(navDestinationLayoutProperty->propHideTitleBar_.value());
    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    navDestinationNode->subtitle_ = tempNode;
    navDestinationLayoutProperty->propHideTitleBar_ = false;
    algorithm->Layout(layoutWrapper);
    EXPECT_FALSE(navDestinationLayoutProperty->propHideTitleBar_.value());
}

/**
 * @tc.name: NavrouterTestNg0010
 * @tc.desc: Test NavDestinationGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navDestinationPattern then prepare properties.
     */
    auto pattern = AceType::MakeRefPtr<NavDestinationPattern>();
    auto navDestinationNode = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);

    auto titlePattern = AceType::MakeRefPtr<TitleBarPattern>();

    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    titlePattern->frameNode_ = AceType::WeakClaim(AceType::RawPtr(tempNode));
    auto titleNode = titlePattern->frameNode_.Upgrade();
    ASSERT_NE(titleNode, nullptr);
    auto list = titleNode->GetChildren();
    list.empty();

    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>("TitleBarNode", 66, titlePattern);
    auto layoutProperty = AceType::MakeRefPtr<NavDestinationLayoutProperty>();

    layoutProperty->propNoPixMap_ = true;
    std::string imageInfo = "imageInfo";
    layoutProperty->propImageSource_ = ImageSourceInfo(imageInfo);

    /**
     * @tc.steps: step2. create navDestinationNode then prepare properties.
     */
    navDestinationNode->layoutProperty_ = layoutProperty;
    navDestinationNode->titleBarNode_ = titleBarNode;

    auto titlePattern2 = AceType::MakeRefPtr<TitleBarPattern>();
    auto title = AceType::MakeRefPtr<TitleBarNode>("Title", 234, titlePattern2);
    navDestinationNode->title_ = title;

    auto subTitlePattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto subTitle = AceType::MakeRefPtr<TitleBarNode>("Title", 234, subTitlePattern);
    navDestinationNode->subtitle_ = subTitle;

    auto navDestinationPattern = AceType::MakeRefPtr<NavDestinationPattern>();
    navDestinationPattern->frameNode_ = AceType::WeakClaim(AceType::RawPtr(navDestinationNode));
    navDestinationPattern->OnModifyDone();
    EXPECT_TRUE(layoutProperty->HasNoPixMap());
    /**
     * @tc.steps: step3. change navDestinationLayoutProperty->HasNoPixMap().
     */
    layoutProperty->propNoPixMap_ = false;
    layoutProperty->propImageSource_ = std::nullopt;
    navDestinationNode->propTitleNodeOperation_ = ChildNodeOperation::ADD;
    navDestinationNode->propSubtitleNodeOperation_ = ChildNodeOperation::REPLACE;
    navDestinationPattern->OnModifyDone();
    EXPECT_TRUE(layoutProperty->HasNoPixMap());
    layoutProperty->propNoPixMap_ = std::nullopt;
    navDestinationNode->propTitleNodeOperation_ = ChildNodeOperation::ADD;
    navDestinationNode->propSubtitleNodeOperation_ = ChildNodeOperation::ADD;
    navDestinationPattern->OnModifyDone();
    EXPECT_FALSE(layoutProperty->HasNoPixMap());
}

/**
 * @tc.name: NavrouterTestNg0011
 * @tc.desc: Test NavDestinationGroupNode::OnAttachToMainTree.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navDestinationPattern then prepare properties.
     */
    auto pattern = AceType::MakeRefPtr<NavDestinationPattern>();
    auto navDestinationNode = AceType::MakeRefPtr<NavDestinationGroupNode>("navDestinationNode", 11, pattern);

    auto titlePattern = AceType::MakeRefPtr<TitleBarPattern>();

    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    titlePattern->frameNode_ = AceType::WeakClaim(AceType::RawPtr(tempNode));
    auto titleNode = titlePattern->frameNode_.Upgrade();
    ASSERT_NE(titleNode, nullptr);
    auto list = titleNode->GetChildren();
    list.empty();

    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>("TitleBarNode", 66, titlePattern);

    auto subTitlePattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto subTitle = AceType::MakeRefPtr<TitleBarNode>("Title", 234, subTitlePattern);
    titleBarNode->subtitle_ = subTitle;

    auto layoutProperty = AceType::MakeRefPtr<NavDestinationLayoutProperty>();

    /**
     * @tc.steps: step2. create navDestinationNode then prepare properties.
     */
    layoutProperty->propNoPixMap_ = true;
    auto mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    layoutProperty->propPixelMap_ = mockPixelMap;
    layoutProperty->propHideTitleBar_ = true;
    std::string imageInfo = "imageInfo";
    layoutProperty->propImageSource_ = ImageSourceInfo(imageInfo);

    navDestinationNode->layoutProperty_ = layoutProperty;
    navDestinationNode->titleBarNode_ = titleBarNode;

    auto titlePattern2 = AceType::MakeRefPtr<TitleBarPattern>();
    auto title = AceType::MakeRefPtr<TitleBarNode>("Title", 234, titlePattern2);
    navDestinationNode->title_ = title;
    navDestinationNode->propInspectorId_ = "test";
    navDestinationNode->propTitleNodeOperation_ = ChildNodeOperation::REPLACE;

    auto navDestinationPattern = AceType::MakeRefPtr<NavDestinationPattern>();
    navDestinationPattern->frameNode_ = AceType::WeakClaim(AceType::RawPtr(navDestinationNode));
    navDestinationPattern->OnModifyDone();
    EXPECT_TRUE(layoutProperty->HasNoPixMap());
    /**
     * @tc.steps: step3. change navDestinationLayoutProperty->HasNoPixMap().
     */
    layoutProperty->propNoPixMap_ = false;
    navDestinationNode->propTitleNodeOperation_ = ChildNodeOperation::ADD;
    navDestinationNode->propSubtitleNodeOperation_ = ChildNodeOperation::ADD;
    navDestinationPattern->OnModifyDone();
    EXPECT_TRUE(layoutProperty->HasNoPixMap());
}

/**
 * @tc.name: NavrouterTestNg0012
 * @tc.desc: Test NavDestinationModelNG::SetTitle.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create mock theme manager.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto theme = AceType::MakeRefPtr<NavigationBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(theme));
    /**
     * @tc.steps: step2. create model then call set function.
     */
    NavDestinationModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto titleNode = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto layout = AceType::MakeRefPtr<TextLayoutProperty>();
    layout->propContent_ = "myTitle2";
    titleNode->layoutProperty_ = layout;
    /**
     * @tc.steps: step3. change properties then call set function.
     */
    navDestinationNode->title_ = titleNode;
    model.SetTitle("myTitle", false);
    layout->propContent_ = "myTitle2";
    model.SetTitle("myTitle2", true);
    EXPECT_EQ(layout->GetContentValue(), "myTitle2");
    auto subTitle = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 67, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->subtitle_ = subTitle;
    model.SetTitle("myTitle", false);
    ASSERT_EQ(navDestinationNode->subtitle_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0013
 * @tc.desc: Test NavDestinationModelNG::SetTitle.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create mock theme manager.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto theme = AceType::MakeRefPtr<NavigationBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(theme));
    /**
     * @tc.steps: step2. create model then call set function.
     */
    NavDestinationModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto titleNode = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto layout = AceType::MakeRefPtr<TextLayoutProperty>();
    layout->propContent_ = "myTitle2";
    titleNode->layoutProperty_ = layout;

    auto subTitle = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 67, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->subtitle_ = subTitle;
    /**
     * @tc.steps: step3. change properties then call set function.
     */
    navDestinationNode->title_ = titleNode;
    model.SetSubtitle("mySubTitle111");
    layout->propContent_ = "myTitle2";
    model.SetSubtitle("mySubTitle222");
    EXPECT_EQ(layout->GetContentValue(), "myTitle2");

    model.SetSubtitle("mySubTitle222");
    ASSERT_NE(navDestinationNode->subtitle_, nullptr);

    auto customNode = CustomNode::CreateCustomNode(11, "customNode");
    navDestinationNode->subtitle_ = customNode;
    model.SetSubtitle("mySubTitle444");
}

/**
 * @tc.name: NavrouterTestNg0014
 * @tc.desc: Test NavDestinationModelNG::SetCustomTitle.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create mock theme manager.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto theme = AceType::MakeRefPtr<NavigationBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(theme));
    /**
     * @tc.steps: step2. create model then call set function.
     */
    NavDestinationModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(frameNode);
    ASSERT_NE(navDestinationNode, nullptr);
    auto titleNode = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto subTitle = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    /**
     * @tc.steps: step3. change properties then call set function.
     */
    model.SetCustomTitle(subTitle);
    navDestinationNode->title_ = titleNode;
    model.SetCustomTitle(subTitle);
    ASSERT_EQ(navDestinationNode->title_->GetId(), subTitle->GetId());
}

/**
 * @tc.name: NavrouterTestNg0015
 * @tc.desc: Test NavRouterGroupNode::CleanNodeInNavigation.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    navRouterGroupNode->OnDetachFromMainTree(false);
    ASSERT_EQ(navRouterGroupNode->GetParent(), nullptr);
    /**
     * @tc.steps: step2. create parentNode and set properties.
     */
    auto parentNode = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    parentNode->isOnAnimation_ = true;
    navRouterGroupNode->parent_ = AceType::WeakClaim(AceType::RawPtr(parentNode));
    navRouterGroupNode->OnDetachFromMainTree(false);
    EXPECT_TRUE(parentNode->isOnAnimation_);

    parentNode->isOnAnimation_ = false;

    navRouterGroupNode->GetPattern<NavRouterPattern>()->name_ = "navDestination";
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    std::pair<std::string, RefPtr<UINode>> p("navDestination", navDestinationNode);
    ASSERT_NE(parentNode->GetPattern<NavigationPattern>(), nullptr);

    parentNode->GetPattern<NavigationPattern>()->navigationStack_ = AceType::MakeRefPtr<NavigationStack>();
    ASSERT_NE(parentNode->GetPattern<NavigationPattern>()->navigationStack_, nullptr);
    parentNode->GetPattern<NavigationPattern>()->navigationStack_->navPathList_.push_back(p);

    auto contentNode = NavigationGroupNode::GetOrCreateGroupNode(
        "contentNode", 33, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto child = NavDestinationGroupNode::GetOrCreateGroupNode(
        "child", 44, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    contentNode->children_.push_back(child);
    parentNode->contentNode_ = contentNode;

    navRouterGroupNode->OnDetachFromMainTree(false);
    ASSERT_NE(child, navDestinationNode);
}
/**
 * @tc.name: NavrouterTestNg0016
 * @tc.desc: Test NavRouterGroupNode::SetDestinationChangeEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    navRouterGroupNode->OnAttachToMainTree(false);
    ASSERT_EQ(navRouterGroupNode->GetParent(), nullptr);
    /**
     * @tc.steps: step2. create parentNode and set properties.
     */
    auto parentNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 11, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    navRouterGroupNode->parent_ = AceType::WeakClaim(AceType::RawPtr(parentNode));
    navRouterGroupNode->OnAttachToMainTree(false);
    ASSERT_NE(navRouterGroupNode->GetParent(), nullptr);
    /**
     * @tc.steps: step3. prepare event then fire function.
     */
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 22, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    navRouterGroupNode->parent_ = AceType::WeakClaim(AceType::RawPtr(parent));
    auto layoutProperty = AceType::MakeRefPtr<NavigationLayoutProperty>();
    parent->layoutProperty_ = layoutProperty;
    ASSERT_EQ(AceType::DynamicCast<NavigationGroupNode>(navRouterGroupNode->eventHub_), nullptr);
    navRouterGroupNode->eventHub_ = AceType::MakeRefPtr<NavRouterEventHub>();
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    parent->GetPattern<NavigationPattern>()->navigationStack_ = stack;

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    EXPECT_EQ(layoutProperty->GetNavigationModeValue(NavigationMode::AUTO), NavigationMode::AUTO);
    layoutProperty->propNavigationMode_ = NavigationMode::STACK;
    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetPattern<NavRouterPattern>()->routeInfo_ = AceType::MakeRefPtr<RouteInfo>();
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 55, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    preNavDestination->titleBarNode_ = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 33, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    std::pair<std::string, RefPtr<UINode>> p("test", preNavDestination);
    stack->navPathList_.push_back(p);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    EXPECT_TRUE(layoutProperty->propDestinationChange_.value());

    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    navRouterGroupNode->navDestinationNode_ = navDestinationNode;
    auto tempNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    parent->contentNode_ = tempNode;
    ASSERT_EQ(navDestinationNode->GetTitleBarNode(), nullptr);
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 33, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->titleBarNode_ = titleBarNode;

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);

    parent->isOnAnimation_ = false;
    ASSERT_NE(parent->GetPattern<NavigationPattern>()->GetNavDestinationNode(), nullptr);
    navRouterGroupNode->navDestinationNode_ = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 20, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_NE(
        navRouterGroupNode->navDestinationNode_, parent->GetPattern<NavigationPattern>()->GetNavDestinationNode());
    auto cur =
        AceType::DynamicCast<NavDestinationGroupNode>(parent->GetPattern<NavigationPattern>()->GetNavDestinationNode());
    cur->contentNode_ = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    cur->GetPattern<NavDestinationPattern>()->shallowBuilder_ = AceType::MakeRefPtr<ShallowBuilder>(
        []() { return FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>()); });

    AceType::DynamicCast<NavDestinationGroupNode>(navRouterGroupNode->navDestinationNode_)->titleBarNode_ =
        TitleBarNode::GetOrCreateTitleBarNode(
            "titleBarNode34", 34, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });

    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 99, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto tb = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode98", 98, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navBar->titleBarNode_ = tb;
    ASSERT_NE(navBar, nullptr);
    parent->parent_ = AceType::WeakClaim(AceType::RawPtr(navBar));

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);

    parent->isOnAnimation_ = false;
    ASSERT_NE(parent->GetPattern<NavigationPattern>()->GetNavDestinationNode(), nullptr);
    navRouterGroupNode->navDestinationNode_ = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 20, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_EQ(
        navRouterGroupNode->navDestinationNode_, parent->GetPattern<NavigationPattern>()->GetNavDestinationNode());

    AceType::DynamicCast<NavDestinationGroupNode>(navRouterGroupNode->navDestinationNode_)->titleBarNode_ =
        TitleBarNode::GetOrCreateTitleBarNode(
            "titleBarNode34", 34, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    tb->renderContext_ = nullptr;
    navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(navRouterGroupNode->navDestinationNode_);
    auto backButton = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    AceType::DynamicCast<TitleBarNode>(navDestinationNode->titleBarNode_)->backButton_ = backButton;
    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);

    parent->isOnAnimation_ = false;
    ASSERT_NE(parent->GetPattern<NavigationPattern>()->GetNavDestinationNode(), nullptr);
    navRouterGroupNode->navDestinationNode_ = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 20, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_EQ(
        navRouterGroupNode->navDestinationNode_, parent->GetPattern<NavigationPattern>()->GetNavDestinationNode());

    cur = nullptr;
    navBar = nullptr;
    AceType::DynamicCast<NavDestinationGroupNode>(navRouterGroupNode->navDestinationNode_)->titleBarNode_ =
        TitleBarNode::GetOrCreateTitleBarNode(
            "titleBarNode34", 34, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);

    parent->isOnAnimation_ = false;
    ASSERT_NE(parent->GetPattern<NavigationPattern>()->GetNavDestinationNode(), nullptr);
    navRouterGroupNode->navDestinationNode_ = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 20, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    ASSERT_EQ(
        navRouterGroupNode->navDestinationNode_, parent->GetPattern<NavigationPattern>()->GetNavDestinationNode());

    stack->navPathList_.clear();
    layoutProperty->propNavigationMode_ = NavigationMode::SPLIT;
    cur = nullptr;
    navBar = nullptr;
    AceType::DynamicCast<NavDestinationGroupNode>(navRouterGroupNode->navDestinationNode_)->titleBarNode_ =
        TitleBarNode::GetOrCreateTitleBarNode(
            "titleBarNode34", 34, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });

    navRouterGroupNode->OnAttachToMainTree(false);
    navRouterGroupNode->GetEventHub<NavRouterEventHub>()->FireDestinationChangeEvent();
    ASSERT_NE(navRouterGroupNode->navDestinationNode_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0017
 * @tc.desc: Test NavRouterGroupNode::SetBackButtonEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    navRouterGroupNode->OnAttachToMainTree(false);
    ASSERT_EQ(navRouterGroupNode->GetParent(), nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    navRouterGroupNode->parent_ = AceType::WeakClaim(AceType::RawPtr(parent));
    auto navDestinationNode = NavDestinationGroupNode::GetOrCreateGroupNode(
        "navDestinationNode", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    navRouterGroupNode->navDestinationNode_ = navDestinationNode;
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 33, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    navDestinationNode->titleBarNode_ = titleBarNode;
    auto buttonNode = FrameNode::CreateFrameNode("buttonNode", 44, AceType::MakeRefPtr<ButtonPattern>());
    titleBarNode->backButton_ = buttonNode;
    ASSERT_NE(buttonNode->GetEventHub<EventHub>(), nullptr);
    auto eventHub = AceType::MakeRefPtr<NavDestinationEventHub>();
    navDestinationNode->eventHub_ = eventHub;
    auto onBack = []() { return true; };
    eventHub->onBackPressedEvent_ = onBack;
    auto layoutProperty = AceType::MakeRefPtr<NavigationLayoutProperty>();
    parent->layoutProperty_ = layoutProperty;

    navRouterGroupNode->GetPattern<NavRouterPattern>()->name_ = "test";
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 55, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    std::pair<std::string, RefPtr<UINode>> p("test", preNavDestination);
    stack->navPathList_.push_back(p);
    parent->GetPattern<NavigationPattern>()->navigationStack_ = stack;
    navRouterGroupNode->OnAttachToMainTree(false);
}

/**
 * @tc.name: NavrouterTestNg0018
 * @tc.desc: Test NavRouterGroupNode::SetOnStateChangeFalse.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0018, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto pre = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 33, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 44, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());
    navBar->navBarContentNode_ = contentNode;
    parent->navBarNode_ = navBar;

    parent->SetOnStateChangeFalse(navDestination, navDestination);
    ASSERT_NE(navDestination, nullptr);
    parent->SetOnStateChangeFalse(pre, navDestination);
    ASSERT_NE(navDestination, nullptr);
    parent->SetOnStateChangeFalse(pre, navDestination, true);
    ASSERT_NE(navDestination, nullptr);
    /**
     * @tc.steps: step2. fill navigationStack_ property.
     */
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 55, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    std::pair<std::string, RefPtr<UINode>> p("test", preNavDestination);
    stack->navPathList_.push_back(p);
    parent->GetPattern<NavigationPattern>()->navigationStack_ = stack;
    navRouterGroupNode->GetPattern<NavRouterPattern>()->name_ = "test";
    stack->navPathList_.push_back(p);
    parent->SetOnStateChangeFalse(pre, navDestination, true);
    ASSERT_NE(navDestination, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0019
 * @tc.desc: Test NavRouterGroupNode::BackToNavBar.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0019, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    parent->contentNode_ = contentNode;
    parent->navBarNode_ = navBar;
    navRouterGroupNode->navDestinationNode_ = navDestination;

    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());

    navDestination->titleBarNode_ = destinationTitleBarNode;
    navBar->titleBarNode_ = titleBarNode;

    EXPECT_FALSE(parent->GetIsOnAnimation());
    parent->BackToNavBar(navDestination);
    EXPECT_FALSE(parent->GetIsOnAnimation());

    destinationTitleBarNode->backButton_ = backButton;
    parent->BackToNavBar(navDestination);
    EXPECT_FALSE(parent->GetIsOnAnimation());
    parent->isOnAnimation_ = true;
    parent->BackToNavBar(navDestination);
    EXPECT_TRUE(parent->GetIsOnAnimation());
}

/**
 * @tc.name: NavrouterTestNg0020
 * @tc.desc: Test NavRouterGroupNode::BackToPreNavDestination.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0020, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    parent->contentNode_ = contentNode;
    parent->navBarNode_ = navBar;
    navRouterGroupNode->navDestinationNode_ = navDestination;

    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());

    preNavDestination->titleBarNode_ = preDestinationTitleBarNode;
    navDestination->titleBarNode_ = destinationTitleBarNode;

    parent->isOnAnimation_ = false;
    parent->BackToPreNavDestination(
        preNavDestination, navDestination, navRouterGroupNode->GetPattern<NavRouterPattern>());
    EXPECT_FALSE(parent->isOnAnimation_);

    destinationTitleBarNode->backButton_ = nullptr;
    parent->isOnAnimation_ = false;
    auto navDestinationPattern = preNavDestination->GetPattern<NavDestinationPattern>();
    navDestinationPattern->routeInfo_ = AceType::MakeRefPtr<RouteInfo>();

    parent->BackToPreNavDestination(
        preNavDestination, navDestination, navRouterGroupNode->GetPattern<NavRouterPattern>());
    EXPECT_FALSE(parent->isOnAnimation_);
}

/**
 * @tc.name: NavrouterTestNg0021
 * @tc.desc: Test NavRouterGroupNode::NavTransitionInAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0021, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    parent->contentNode_ = contentNode;
    parent->navBarNode_ = navBar;
    navRouterGroupNode->navDestinationNode_ = navDestination;

    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());

    preNavDestination->titleBarNode_ = preDestinationTitleBarNode;
    navDestination->titleBarNode_ = destinationTitleBarNode;

    parent->isOnAnimation_ = true;
    parent->BackToPreNavDestination(
        preNavDestination, navDestination, navRouterGroupNode->GetPattern<NavRouterPattern>());
    parent->NavTransitionInAnimation(preNavDestination, navDestination);
    EXPECT_TRUE(parent->isOnAnimation_);
    parent->NavTransitionOutAnimation(preNavDestination, navDestination, navBar);
    EXPECT_TRUE(parent->isOnAnimation_);
    parent->NavTransitionBackToPreAnimation(preNavDestination, navDestination, navBar);
    EXPECT_TRUE(parent->isOnAnimation_);
}

/**
 * @tc.name: NavrouterTestNg0022
 * @tc.desc: Test NavigationGroupNode::AddChildToGroup.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0022, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    NavRouterModelNG model;
    model.Create();
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    auto navRouterGroupNode = AceType::DynamicCast<NavRouterGroupNode>(frameNode);
    ASSERT_NE(navRouterGroupNode, nullptr);
    auto parent = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    parent->contentNode_ = contentNode;
    parent->navBarNode_ = navBar;
    navRouterGroupNode->navDestinationNode_ = navDestination;

    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());

    ASSERT_EQ(navBar->navBarContentNode_, nullptr);
    parent->AddChildToGroup(backButton);
    ASSERT_NE(navBar->navBarContentNode_, nullptr);
    parent->AddChildToGroup(backButton);
    ASSERT_NE(navBar->navBarContentNode_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0023
 * @tc.desc: Test NavigationGroupNode::AddNavDestinationToNavigation.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navRouterGroupNode.
     */
    auto navigation = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());

    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());

    auto pattern = navigation->GetPattern<NavigationPattern>();

    auto stack = AceType::MakeRefPtr<NavigationStack>();
    pattern->navigationStack_ = stack;
    navigation->navBarNode_ = nullptr;
    navigation->AddNavDestinationToNavigation();
    EXPECT_TRUE(stack->navPathList_.empty());
    ASSERT_EQ(navigation->navBarNode_, nullptr);

    std::pair<std::string, RefPtr<UINode>> p("test", preNavDestination);
    stack->navPathList_.push_back(p);

    navigation->AddNavDestinationToNavigation();
    EXPECT_EQ(stack->navPathList_.size(), 1);
    ASSERT_EQ(navigation->navBarNode_, nullptr);

    navigation->navBarNode_ = navBar;
    navigation->AddNavDestinationToNavigation();
    EXPECT_EQ(stack->navPathList_.size(), 1);
    ASSERT_NE(navigation->navBarNode_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0024
 * @tc.desc: Test NavigationGroupNode::SetBackButtonEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navigation.
     */
    auto navRouterPattern = AceType::MakeRefPtr<NavRouterPattern>();
    auto navigation = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "preNavDestination", 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    auto pattern = navigation->GetPattern<NavigationPattern>();
    /**
     * @tc.steps: step2. call navigation->SetBackButtonEvent.
     */
    navigation->navBarNode_ = nullptr;
    navDestination->titleBarNode_ = destinationTitleBarNode;
    destinationTitleBarNode->backButton_ = backButton;
    backButton->eventHub_ = eventHub;
    navDestination->eventHub_ = AceType::MakeRefPtr<NavDestinationEventHub>();
    navDestination->contentNode_ = nullptr;

    navigation->SetBackButtonEvent(navDestination, nullptr);

    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::STACK;
    auto stack = AceType::MakeRefPtr<NavigationStack>();
    std::pair<std::string, RefPtr<UINode>> p("test", preNavDestination);
    stack->navPathList_.push_back(p);
    stack->navPathList_.push_back(p);
    pattern->navigationStack_ = stack;
    navDestination->GetPattern<NavDestinationPattern>()->name_ = "test";
    ASSERT_EQ(pattern->navigationStack_->navPathList_.size(), 2);
    ASSERT_NE(AceType::DynamicCast<NavDestinationGroupNode>(pattern->GetPreNavDestination("test")), nullptr);

    GestureEvent event;
    navDestination->backButtonEvent_(event);
    EXPECT_FALSE(navigation->GetLayoutProperty<NavigationLayoutProperty>()->propDestinationChange_.value());

    auto onBack = []() { return true; };
    navDestination->GetEventHub<NavDestinationEventHub>()->onBackPressedEvent_ = onBack;
    navDestination->contentNode_ = contentNode;
    eventHub->gestureEventHub_ = AceType::MakeRefPtr<GestureEventHub>(AceType::WeakClaim(AceType::RawPtr(eventHub)));
    navDestination->GetPattern<NavDestinationPattern>()->shallowBuilder_ = AceType::MakeRefPtr<ShallowBuilder>(
        []() { return FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>()); });

    navigation->SetBackButtonEvent(navDestination, navRouterPattern);
    navDestination->backButtonEvent_(event);
    EXPECT_TRUE(navDestination->GetEventHub<NavDestinationEventHub>()->onBackPressedEvent_());

    navDestination->GetEventHub<NavDestinationEventHub>()->onBackPressedEvent_ = nullptr;
    preNavDestination->tag_ = V2::NAVDESTINATION_VIEW_ETS_TAG;
    navDestination->backButtonEvent_(event);
    EXPECT_TRUE(navigation->GetLayoutProperty<NavigationLayoutProperty>()->propDestinationChange_.value());

    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::AUTO;
    navDestination->backButtonEvent_(event);
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::SPLIT;
    preNavDestination->tag_ = "preNavDestination";
    navDestination->backButtonEvent_(event);
    preNavDestination->tag_ = V2::NAVDESTINATION_VIEW_ETS_TAG;
    navDestination->backButtonEvent_(event);
    EXPECT_FALSE(navigation->GetLayoutProperty<NavigationLayoutProperty>()->propDestinationChange_.value());
}

/**
 * @tc.name: NavrouterTestNg0025
 * @tc.desc: Test NavigationGroupNode::BackToPreNavDestination.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navigation.
     */
    auto navRouterPattern = AceType::MakeRefPtr<NavRouterPattern>();
    auto navigation = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    auto pattern = navigation->GetPattern<NavigationPattern>();
    /**
     * @tc.steps: step2. call navigation->BackToPreNavDestination.
     */
    navigation->isOnAnimation_ = false;
    navigation->contentNode_ = contentNode;
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::STACK;
    preNavDestination->titleBarNode_ = preDestinationTitleBarNode;
    navDestination->titleBarNode_ = destinationTitleBarNode;
    navigation->BackToPreNavDestination(preNavDestination, navDestination, nullptr);
    ASSERT_EQ(
        navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_.value(), NavigationMode::STACK);

    destinationTitleBarNode->backButton_ = backButton;
    navigation->BackToPreNavDestination(preNavDestination, navDestination, nullptr);
    ASSERT_EQ(
        navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_.value(), NavigationMode::STACK);

    navigation->isOnAnimation_ = false;
    destinationTitleBarNode->backButton_ = nullptr;
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::STACK;
    navigation->BackToPreNavDestination(preNavDestination, navDestination, nullptr);

    navigation->isOnAnimation_ = false;
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_ = NavigationMode::SPLIT;
    navigation->BackToPreNavDestination(preNavDestination, navDestination, nullptr);
    ASSERT_EQ(
        navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNavigationMode_.value(), NavigationMode::SPLIT);
}

/**
 * @tc.name: NavrouterTestNg0026
 * @tc.desc: Test NavigationGroupNode::NavTransitionOutAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0026, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navigation.
     */
    auto navRouterPattern = AceType::MakeRefPtr<NavRouterPattern>();
    auto navigation = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    auto pattern = navigation->GetPattern<NavigationPattern>();
    /**
     * @tc.steps: step2. call navigation->NavTransitionOutAnimation.
     */
    navigation->isOnAnimation_ = false;
    ASSERT_NE(navBar->renderContext_, nullptr);
    ASSERT_NE(navDestination->renderContext_, nullptr);
    navigation->NavTransitionOutAnimation(navBar, navDestination, contentNode);
    ASSERT_NE(backButton->renderContext_, nullptr);
    navigation->BackButtonAnimation(backButton, true);
}

/**
 * @tc.name: NavrouterTestNg0027
 * @tc.desc: Test BarItemLayoutAlgorithm::Layout.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0027, TestSize.Level1)
{
    auto algorithm = AceType::MakeRefPtr<BarItemLayoutAlgorithm>();
    auto barItem = AceType::MakeRefPtr<BarItemNode>("barItem", 11);
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto layoutProperty = AceType::MakeRefPtr<LayoutProperty>();
    auto textNode = FrameNode::CreateFrameNode("textNode", 22, AceType::MakeRefPtr<TextPattern>());

    auto layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(AceType::WeakClaim(AceType::RawPtr(barItem)), geometryNode, layoutProperty);

    algorithm->Layout(AceType::RawPtr(layoutWrapper));
    ASSERT_EQ(barItem->text_, nullptr);
    barItem->text_ = textNode;
    algorithm->Layout(AceType::RawPtr(layoutWrapper));
    ASSERT_NE(barItem->text_, nullptr);
}

/**
 * @tc.name: NavrouterTestNg0028
 * @tc.desc: Test TitleBarNode::FastPreviewUpdateChild.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0028, TestSize.Level1)
{
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto menu = FrameNode::CreateFrameNode("menu", 22, AceType::MakeRefPtr<TextPattern>());
    auto newChild = FrameNode::CreateFrameNode("newChild", 33, AceType::MakeRefPtr<TextPattern>());

    ASSERT_TRUE(titleBarNode->children_.empty());
    titleBarNode->FastPreviewUpdateChild(0, newChild);
    ASSERT_FALSE(titleBarNode->children_.empty());
    titleBarNode->FastPreviewUpdateChild(1, menu);
    ASSERT_EQ(titleBarNode->children_.size(), 2);
    titleBarNode->FastPreviewUpdateChild(2, newChild);
    ASSERT_EQ(titleBarNode->children_.size(), 2);
}

/**
 * @tc.name: NavrouterTestNg0029
 * @tc.desc: Test NavBarNode::AddChildToGroup && GetBarItemsString.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0029, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navBar newChild then call AddChildToGroup.
     */
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto newChild = FrameNode::CreateFrameNode("newChild", 22, AceType::MakeRefPtr<TextPattern>());

    ASSERT_EQ(navBar->navBarContentNode_, nullptr);
    navBar->AddChildToGroup(newChild);
    ASSERT_NE(navBar->navBarContentNode_, nullptr);
    navBar->AddChildToGroup(newChild);
    ASSERT_EQ(navBar->navBarContentNode_->children_.size(), 1);

    /**
     * @tc.steps: step2. create menu then construct navBar->menu_->children_.
     */
    auto menu = FrameNode::CreateFrameNode("menu", 33, AceType::MakeRefPtr<TextPattern>());
    auto child1 = FrameNode::CreateFrameNode("child1", 44, AceType::MakeRefPtr<TextPattern>());
    auto barItem = AceType::MakeRefPtr<BarItemNode>("barItem", 55);
    auto barItem2 = AceType::MakeRefPtr<BarItemNode>("barItem", 66);
    auto barItem3 = AceType::MakeRefPtr<BarItemNode>("barItem", 67);
    auto barItem4 = AceType::MakeRefPtr<BarItemNode>("barItem", 68);

    auto icon = FrameNode::CreateFrameNode("icon", 77, AceType::MakeRefPtr<ImagePattern>());
    auto text = FrameNode::CreateFrameNode("text", 88, AceType::MakeRefPtr<TextPattern>());

    auto icon3 = FrameNode::CreateFrameNode("icon", 78, AceType::MakeRefPtr<ImagePattern>());
    auto text3 = FrameNode::CreateFrameNode("text", 89, AceType::MakeRefPtr<TextPattern>());

    auto icon4 = FrameNode::CreateFrameNode("icon", 80, AceType::MakeRefPtr<ImagePattern>());
    auto text4 = FrameNode::CreateFrameNode("text", 81, AceType::MakeRefPtr<TextPattern>());

    auto imageLayoutProperty = AceType::MakeRefPtr<ImageLayoutProperty>();
    auto textLayoutProperty = AceType::MakeRefPtr<TextLayoutProperty>();

    imageLayoutProperty->propImageSourceInfo_ = ImageSourceInfo();

    barItem2->icon_ = icon;
    barItem2->text_ = text;

    barItem3->icon_ = icon3;
    barItem3->text_ = text3;

    barItem4->icon_ = icon4;
    barItem4->text_ = text4;

    icon3->layoutProperty_ = imageLayoutProperty;
    text3->layoutProperty_ = textLayoutProperty;
    text->layoutProperty_ = nullptr;
    icon4->layoutProperty_ = nullptr;

    /**
     * @tc.steps: step3. call navBar->GetBarItemsString.
     */
    navBar->menu_ = menu;
    navBar->GetBarItemsString(true);
    ASSERT_TRUE(navBar->menu_->children_.empty());

    navBar->menu_->children_.push_back(child1);
    navBar->menu_->children_.push_back(barItem);
    navBar->menu_->children_.push_back(barItem2);
    navBar->menu_->children_.push_back(barItem3);
    navBar->menu_->children_.push_back(barItem4);
    navBar->GetBarItemsString(true);
    ASSERT_EQ(navBar->menu_->children_.size(), 5);
}

/**
 * @tc.name: NavrouterTestNg0030
 * @tc.desc: Test NavBarLayoutAlgorithm MeasureTitleBar.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0030, TestSize.Level1)
{
    auto algorithm = AceType::MakeRefPtr<NavBarLayoutAlgorithm>();
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto geometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto layoutProperty = AceType::MakeRefPtr<NavBarLayoutProperty>();
    auto layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(AceType::WeakClaim(AceType::RawPtr(navBar)), geometryNode, layoutProperty);

    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 22, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto text4 = FrameNode::CreateFrameNode("text", 22, AceType::MakeRefPtr<TextPattern>());
    auto titleGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto titleLayoutProperty = AceType::MakeRefPtr<TitleBarLayoutProperty>();
    auto titleBarNode2 = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 23, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto childWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        AceType::WeakClaim(AceType::RawPtr(titleBarNode2)), titleGeometryNode, titleLayoutProperty);
    auto subTitle =
        TitleBarNode::GetOrCreateTitleBarNode("subTitle", 22, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto toolBarNode = FrameNode::CreateFrameNode("text", 22, AceType::MakeRefPtr<TextPattern>());
    auto navBarContentNode = FrameNode::CreateFrameNode("text", 22, AceType::MakeRefPtr<TextPattern>());
    auto toolBarChild = FrameNode::CreateFrameNode("text", 99, AceType::MakeRefPtr<TextPattern>());

    LayoutConstraintF constraint;
    LayoutConstraintF constraint2;
    constraint.selfIdealSize.width_ = 20.0f;
    constraint.selfIdealSize.height_ = 30.0f;
    layoutProperty->layoutConstraint_ = constraint;
    layoutProperty->propHideTitleBar_ = true;
    layoutProperty->contentConstraint_ = constraint2;
    // toolBarNode_ navBarContentNode_

    navBar->navBarContentNode_ = navBarContentNode;
    navBar->toolBarNode_ = toolBarNode;
    navBar->titleBarNode_ = titleBarNode;
    navBar->children_.push_back(text4);
    layoutWrapper->childrenMap_[0] = childWrapper;
    layoutWrapper->currentChildCount_ = 1;
    layoutProperty->propHideToolBar_ = false;

    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    algorithm->Layout(AceType::RawPtr(layoutWrapper));
    ASSERT_TRUE(layoutProperty->propHideTitleBar_.value());

    layoutProperty->propHideToolBar_ = true;
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = Dimension();
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    algorithm->Layout(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_TRUE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    layoutProperty->propHideToolBar_ = false;
    toolBarNode->children_.push_back(toolBarChild);
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::MINI;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    // subtitle_
    navBar->subtitle_ = subTitle;
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::MINI;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::FULL;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    titleBarNode->GetPattern<TitleBarPattern>()->tempTitleBarHeight_ = 5.0f;
    navBar->subtitle_ = nullptr;
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::FREE;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    titleBarNode->GetPattern<TitleBarPattern>()->tempTitleBarHeight_ = 5.0f;
    navBar->subtitle_ = subTitle;
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::FREE;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());

    titleBarNode->GetPattern<TitleBarPattern>()->tempTitleBarHeight_ = 0.0f;
    navBar->subtitle_ = nullptr;
    layoutProperty->propHideTitleBar_ = false;
    titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propTitleHeight_ = std::nullopt;
    layoutProperty->propTitleMode_ = NavigationTitleMode::FREE;
    algorithm->Measure(AceType::RawPtr(layoutWrapper));
    ASSERT_FALSE(layoutProperty->propHideTitleBar_.value());
    ASSERT_FALSE(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->HasTitleHeight());
}

/**
 * @tc.name: NavrouterTestNg0031
 * @tc.desc: Test NavigationGroupNode::AddBackButtonIconToNavDestination.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navigation.
     */
    auto navRouterPattern = AceType::MakeRefPtr<NavRouterPattern>();
    auto navigation = NavigationGroupNode::GetOrCreateGroupNode(
        "parentNode", 11, []() { return AceType::MakeRefPtr<NavigationPattern>(); });
    auto navDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        "NavDestination", 22, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto preNavDestination = NavDestinationGroupNode::GetOrCreateGroupNode(
        V2::NAVDESTINATION_VIEW_ETS_TAG, 23, []() { return AceType::MakeRefPtr<NavDestinationPattern>(); });
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 33, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto contentNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());
    auto preDestinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 55, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto destinationTitleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 66, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 77, AceType::MakeRefPtr<ButtonPattern>());
    auto eventHub = AceType::MakeRefPtr<EventHub>();
    auto pattern = navigation->GetPattern<NavigationPattern>();
    /**
     * @tc.steps: step2. call navigation->NavTransitionOutAnimation.
     */
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propNoPixMap_ = false;
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propImageSource_ = std::nullopt;
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propPixelMap_ = std::nullopt;
    navigation->AddBackButtonIconToNavDestination(navDestination);
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propImageSource_ = ImageSourceInfo();
    auto mockPixelMap = AceType::MakeRefPtr<MockPixelMap>();
    navigation->GetLayoutProperty<NavigationLayoutProperty>()->propPixelMap_ = mockPixelMap;
    navigation->AddBackButtonIconToNavDestination(navDestination);
    ASSERT_FALSE(navDestination->GetLayoutProperty<NavDestinationLayoutProperty>()->GetNoPixMapValue());
}

/**
 * @tc.name: NavrouterTestNg0032
 * @tc.desc: Test NavBarPattern::OnModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(NavrouterTestNg, NavrouterTestNg0032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create navBar.
     */
    auto navBar =
        NavBarNode::GetOrCreateNavBarNode("navBarNode", 11, []() { return AceType::MakeRefPtr<NavBarPattern>(); });
    auto titleBarNode = TitleBarNode::GetOrCreateTitleBarNode(
        "titleBarNode", 22, []() { return AceType::MakeRefPtr<TitleBarPattern>(); });
    auto backButton = FrameNode::CreateFrameNode("BackButton", 33, AceType::MakeRefPtr<ButtonPattern>());
    auto toolBarNode = FrameNode::CreateFrameNode("BackButton", 44, AceType::MakeRefPtr<ButtonPattern>());

    auto pattern = navBar->GetPattern<NavBarPattern>();

    navBar->toolBarNode_ = toolBarNode;
    navBar->titleBarNode_ = titleBarNode;
    /**
     * @tc.steps: step2. call pattern->OnModifyDone();.
     */
    
    navBar->backButton_ = backButton;
    pattern->OnModifyDone();
    navBar->propToolBarNodeOperation_ = ChildNodeOperation::REPLACE;
    navBar->GetLayoutProperty<NavBarLayoutProperty>()->propHideTitleBar_ = true;
    navBar->GetLayoutProperty<NavBarLayoutProperty>()->propHideToolBar_ = true;
    pattern->OnModifyDone();
    ASSERT_EQ(titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>()->propVisibility_.value(), VisibleType::GONE);
}
} // namespace OHOS::Ace::NG