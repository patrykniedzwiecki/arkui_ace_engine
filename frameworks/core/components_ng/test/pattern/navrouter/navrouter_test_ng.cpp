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

#define private public
#define protected public
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"
#include "core/components_ng/pattern/navrouter/navrouter_pattern.h"
#include "core/components_ng/pattern/navrouter/navrouter_event_hub.h"
#include "core/components_ng/pattern/navrouter/navrouter_group_node.h"
#include "core/components_ng/pattern/navrouter/navrouter_model_ng.h"
#include "core/components_ng/pattern/navrouter/navrouter_model.h"
#include "core/components_ng/pattern/navrouter/navdestination_model.h"
#include "core/components_ng/pattern/navrouter/navdestination_model_ng.h"
#include "core/components_ng/pattern/navrouter/navdestination_group_node.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_layout_algorithm.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/components/counter/counter_theme.h"
#include "base/test/mock/mock_pixel_map.h"
#include "core/components_ng/pattern/custom/custom_node.h"

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
    auto navigationPattern =  AceType::MakeRefPtr<NavigationPattern>();
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
    auto navigationPattern =  AceType::MakeRefPtr<NavigationPattern>();
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

    auto navigationPattern =  AceType::MakeRefPtr<NavigationPattern>();
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
    auto onBackPressed = []() {return true;};
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
    auto func = []() {return FrameNode::CreateFrameNode("BackButton", 123, AceType::MakeRefPtr<ButtonPattern>());};
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
    layoutWrapper->childrenMap_[0] =  AceType::MakeRefPtr<LayoutWrapper>(
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
    layoutWrapper->childrenMap_[0] =  AceType::MakeRefPtr<LayoutWrapper>(
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
    titlePattern->frameNode_ = AceType::WeakClaim(
        AceType::RawPtr(tempNode));
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
    titlePattern->frameNode_ = AceType::WeakClaim(
        AceType::RawPtr(tempNode));
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
        "TitleBarNode", 66, []() {return AceType::MakeRefPtr<TitleBarPattern>();});
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
        "TitleBarNode", 67, []() {return AceType::MakeRefPtr<TitleBarPattern>();});
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
        "TitleBarNode", 66, []() {return AceType::MakeRefPtr<TitleBarPattern>();});
    auto layout = AceType::MakeRefPtr<TextLayoutProperty>();
    layout->propContent_ = "myTitle2";
    titleNode->layoutProperty_ = layout;

    auto subTitle = TitleBarNode::GetOrCreateTitleBarNode(
        "TitleBarNode", 67, []() {return AceType::MakeRefPtr<TitleBarPattern>();});
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
} // namespace OHOS::Ace::NG