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

#define private public
#define protected public
#include "gtest/gtest.h"

#include "base/geometry/ng/rect_t.h"
#include "core/components/select/select_theme.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/menu/menu_layout_algorithm.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/syntax/lazy_for_each_model.h"
#include "core/components_ng/syntax/lazy_layout_wrapper_builder.h"
#include "core/components_ng/test/mock/syntax/mock_lazy_for_each_builder.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr float FULL_SCREEN_WIDTH = 720.0f;
constexpr float FULL_SCREEN_HEIGHT = 1136.0f;
constexpr float POSITION_OFFSET = 100.0f;
const SizeF FULL_SCREEN_SIZE(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT);

class MenuLayoutAlgorithmTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MenuLayoutAlgorithmTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}
void MenuLayoutAlgorithmTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}
void MenuLayoutAlgorithmTestNg::SetUp() {}
void MenuLayoutAlgorithmTestNg::TearDown() {}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg001
 * @tc.desc: Verify HorizontalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg001, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(50, 100);
    SizeF size_f(100, 200);
    float clickPosition = 50.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->HorizontalLayout(size, clickPosition);
    EXPECT_EQ(result, clickPosition);
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg002
 * @tc.desc: Verify HorizontalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg002, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(50, 100);
    SizeF size_f(100, 200);
    float clickPosition = 60.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->HorizontalLayout(size, clickPosition);
    EXPECT_EQ(result, clickPosition - size.Width());
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg003
 * @tc.desc: Verify HorizontalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg003, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(70, 100);
    SizeF size_f(100, 200);
    float clickPosition = 60.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->HorizontalLayout(size, clickPosition);
    EXPECT_EQ(result, menuLayoutAlgorithm->screenSize_.Width() - size.Width());
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg004
 * @tc.desc: Verify HorizontalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg004, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(100, 100);
    SizeF size_f(100, 200);
    float clickPosition = 60.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->HorizontalLayout(size, clickPosition);
    EXPECT_EQ(result, 0.0);
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg005
 * @tc.desc: Verify VerticalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg005, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(100, 100);
    SizeF size_f(100, 200);
    float clickPosition = 100.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->VerticalLayout(size, clickPosition);
    EXPECT_EQ(result, clickPosition);
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg006
 * @tc.desc: Verify VerticalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg006, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(100, 100);
    SizeF size_f(100, 200);
    float clickPosition = 150.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->VerticalLayout(size, clickPosition);
    EXPECT_EQ(result, clickPosition - size.Height());
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg007
 * @tc.desc: Verify VerticalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg007, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(100, 150);
    SizeF size_f(100, 200);
    float clickPosition = 100.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->VerticalLayout(size, clickPosition);
    EXPECT_EQ(result, menuLayoutAlgorithm->screenSize_.Height() - size.Height());
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg008
 * @tc.desc: Verify VerticalLayout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg008, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    SizeF size(100, 200);
    SizeF size_f(100, 200);
    float clickPosition = 100.0;
    menuLayoutAlgorithm->screenSize_ = size_f;
    auto result = menuLayoutAlgorithm->VerticalLayout(size, clickPosition);
    EXPECT_EQ(result, 0.0);
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg009
 * @tc.desc: Verify Initialize.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg009, TestSize.Level1)
{
    // set buttonTheme to themeManager before using themeManager to get buttonTheme
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));

    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    const std::string tag = "tag";
    RefPtr<Pattern> pattern = AceType::MakeRefPtr<Pattern>();
    MenuView menuView;
    std::vector<SelectParam> params;
    params.emplace_back("A", "B");
    int32_t targetId = 1;
    auto frameNode = menuView.Create(params, targetId);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    LayoutWrapper* layoutWrapper = new LayoutWrapper(frameNode, geometryNode, frameNode->GetLayoutProperty());
    menuLayoutAlgorithm->Initialize(layoutWrapper);
    EXPECT_EQ(menuLayoutAlgorithm->screenSize_.Width(), 0);
    delete layoutWrapper;
    layoutWrapper = nullptr;
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg010
 * @tc.desc: Verify Layout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg010, TestSize.Level1)
{
    RefPtr<MenuLayoutAlgorithm> menuLayoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    OffsetF offset(10, 10);
    menuLayoutAlgorithm->position_ = offset;
    const std::string tag = "tag";
    int32_t nodeId = 1;
    RefPtr<Pattern> pattern = AceType::MakeRefPtr<Pattern>();
    bool isRoot = false;
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<FrameNode> frameNode = AceType::MakeRefPtr<FrameNode>(tag, nodeId, pattern, isRoot);
    LayoutWrapper* layoutWrapper = new LayoutWrapper(frameNode, geometryNode, frameNode->GetLayoutProperty());
    RefPtr<LazyForEachActuator> actuator = AceType::MakeRefPtr<LazyForEachActuator>();
    auto builder = AceType::DynamicCast<LazyForEachBuilder>(actuator);
    RefPtr<LazyForEachNode> host_ = AceType::MakeRefPtr<LazyForEachNode>(nodeId, builder);
    WeakPtr<LazyForEachNode> host(host_);
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = AceType::MakeRefPtr<LazyLayoutWrapperBuilder>(builder, host);
    wrapperBuilder->OnGetOrCreateWrapperByIndex(nodeId);
    auto children = wrapperBuilder->OnExpandChildLayoutWrapper();
    auto layoutWrapper_ = wrapperBuilder->GetOrCreateWrapperByIndex(nodeId);
    menuLayoutAlgorithm->Layout(layoutWrapper);
    EXPECT_EQ(menuLayoutAlgorithm->position_.GetX(), 10);
    delete layoutWrapper;
    layoutWrapper = nullptr;
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg011
 * @tc.desc: Verify positionOffset of Layout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg011, TestSize.Level1)
{
    std::function<void()> action = [] {};
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem1", "", action);
    optionParams.emplace_back("MenuItem2", "", action);
    MenuParam menuParam;
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), 1, MenuType::MENU, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ASSERT_TRUE(property->GetPositionOffset().has_value());
    EXPECT_EQ(property->GetPositionOffset().value(), OffsetF());
    RefPtr<MenuLayoutAlgorithm> layoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    LayoutWrapper layoutWrapper(menuNode, geometryNode, menuNode->GetLayoutProperty());

    layoutWrapper.GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(FULL_SCREEN_WIDTH), CalcLength(FULL_SCREEN_HEIGHT)));
    LayoutConstraintF parentLayoutConstraint;
    parentLayoutConstraint.maxSize = FULL_SCREEN_SIZE;
    parentLayoutConstraint.percentReference = FULL_SCREEN_SIZE;
    parentLayoutConstraint.selfIdealSize.SetSize(SizeF(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT));
    layoutWrapper.GetLayoutProperty()->UpdateLayoutConstraint(parentLayoutConstraint);
    layoutWrapper.GetLayoutProperty()->UpdateContentConstraint();

    layoutAlgorithm->Measure(&layoutWrapper);
    EXPECT_EQ(layoutAlgorithm->position_, OffsetF());
    EXPECT_EQ(layoutAlgorithm->positionOffset_, OffsetF());
    EXPECT_EQ(layoutAlgorithm->screenSize_, SizeF(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT));
    layoutAlgorithm->Layout(&layoutWrapper);
    EXPECT_EQ(geometryNode->GetMarginFrameOffset(), OffsetF(0.0f, 0.0f));
}

/**
 * @tc.name: MenuLayoutAlgorithmTestNg012
 * @tc.desc: Verify positionOffset of Layout.
 * @tc.type: FUNC
 */
HWTEST_F(MenuLayoutAlgorithmTestNg, MenuLayoutAlgorithmTestNg012, TestSize.Level1)
{
    std::vector<OptionParam> optionParams;
    optionParams.emplace_back("MenuItem1", "", nullptr);
    optionParams.emplace_back("MenuItem2", "", nullptr);
    MenuParam menuParam = {"", {POSITION_OFFSET, POSITION_OFFSET}};
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SelectTheme>()));
    auto menuWrapperNode = MenuView::Create(std::move(optionParams), 1, MenuType::MENU, menuParam);
    ASSERT_NE(menuWrapperNode, nullptr);
    ASSERT_EQ(menuWrapperNode->GetChildren().size(), 1);
    auto menuNode = AceType::DynamicCast<FrameNode>(menuWrapperNode->GetChildAtIndex(0));
    ASSERT_NE(menuNode, nullptr);
    auto property = menuNode->GetLayoutProperty<MenuLayoutProperty>();
    ASSERT_NE(property, nullptr);
    ASSERT_TRUE(property->GetPositionOffset().has_value());
    EXPECT_EQ(property->GetPositionOffset().value(), OffsetF(POSITION_OFFSET, POSITION_OFFSET));

    RefPtr<MenuLayoutAlgorithm> layoutAlgorithm = AceType::MakeRefPtr<MenuLayoutAlgorithm>();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    LayoutWrapper layoutWrapper(menuNode, geometryNode, menuNode->GetLayoutProperty());
    layoutWrapper.GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(FULL_SCREEN_WIDTH), CalcLength(FULL_SCREEN_HEIGHT)));
    LayoutConstraintF parentLayoutConstraint;
    parentLayoutConstraint.maxSize = FULL_SCREEN_SIZE;
    parentLayoutConstraint.percentReference = FULL_SCREEN_SIZE;
    parentLayoutConstraint.selfIdealSize.SetSize(SizeF(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT));
    layoutWrapper.GetLayoutProperty()->UpdateLayoutConstraint(parentLayoutConstraint);
    layoutWrapper.GetLayoutProperty()->UpdateContentConstraint();

    layoutAlgorithm->Measure(&layoutWrapper);
    EXPECT_EQ(layoutAlgorithm->position_, OffsetF());
    EXPECT_EQ(layoutAlgorithm->positionOffset_, OffsetF(POSITION_OFFSET, POSITION_OFFSET));
    EXPECT_EQ(layoutAlgorithm->screenSize_, SizeF(FULL_SCREEN_WIDTH, FULL_SCREEN_HEIGHT));
    layoutAlgorithm->Layout(&layoutWrapper);
    EXPECT_EQ(geometryNode->GetMarginFrameOffset(), OffsetF(POSITION_OFFSET, POSITION_OFFSET));
}
} // namespace
} // namespace OHOS::Ace::NG
