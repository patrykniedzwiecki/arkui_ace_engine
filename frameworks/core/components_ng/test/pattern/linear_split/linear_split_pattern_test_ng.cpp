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

#include <cstdint>
#include <optional>

#include "gtest/gtest.h"

#include "core/components_ng/pattern/linear_split/linear_split_layout_property.h"

#define private public
#define protected public
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/linear_split/linear_split_model.h"
#include "core/components_ng/pattern/linear_split/linear_split_pattern.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr Dimension WIDTH = 500.0_vp;
constexpr Dimension HEIGHT = 500.0_vp;
constexpr std::size_t DEFAULT_DRAG_INDEX = -1;
const float RK356_WIDTH = 720.0f;
const float RK356_HEIGHT = 1136.0f;
const float NO_PADDING = 0.0f;
const float ZERO = 0.0f;
const float SMALL_ITEM_HEIGHT = 50.0f;
const float SMALL_ITEM_WIDTH = 50.0f;
const float COLUMN_HEIGHT = 150.0f;
const float DEFAULT_SPLIT_WIDTH = 2.0f;
const float DEFAULT_SPLIT_HEIGHT = 2.0f;
const int32_t FIVE_ITEM_SIZE = 5;
const SizeF CONTAINER_SIZE(RK356_WIDTH, RK356_HEIGHT);
const OffsetF OFFSET_TOP_LEFT = OffsetF(ZERO, ZERO);
} // namespace

class LinearSplitPatternTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
void LinearSplitPatternTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}

void LinearSplitPatternTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}
void LinearSplitPatternTestNg::SetUp() {}

void LinearSplitPatternTestNg::TearDown() {}

/**
 * @tc.name: LinearSplitCreatorTest001
 * @tc.desc: Test all the property of split
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitCreatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);

    /**
     * @tc.steps: step2. Get split frameNode.
     */
    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->Finish();
    auto frameNode = AceType::DynamicCast<FrameNode>(element);
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step3. Get layoutProperty
     * @tc.expected: Confirm that the properties are correct.
     */
    auto renderProperty = frameNode->GetLayoutProperty<LinearSplitLayoutProperty>();
    EXPECT_NE(renderProperty, nullptr);
    EXPECT_EQ(renderProperty->GetResizeableValue(), true);
}

/**
 * @tc.name: LinearSplitPatternTest001
 * @tc.desc: Test linearSplit pattern OnDirtyLayoutWrapperSwap function.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    /**
     * @tc.steps: step2. Get linearSplitPattern and linearSplitWrapper.
     */
    RefPtr<LinearSplitPattern> linearSPlitPattern = frameNode->GetPattern<LinearSplitPattern>();
    EXPECT_NE(linearSPlitPattern, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
    frameNode->SetGeometryNode(geometryNode);
    auto linearSplitLayoutProperty = frameNode->GetLayoutProperty<LinearSplitLayoutProperty>();
    EXPECT_NE(linearSplitLayoutProperty, nullptr);
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, linearSplitLayoutProperty);
    EXPECT_NE(layoutWrapper, nullptr);
    layoutWrapper->skipMeasureContent_ = false;
    std::vector<float> dragSplitOffset;
    RefPtr<LinearSplitLayoutAlgorithm> linearLayoutAlgorithm =
        AceType::MakeRefPtr<LinearSplitLayoutAlgorithm>(SplitType::COLUMN_SPLIT, dragSplitOffset, false);

    /**
     * @tc.steps: step3. call linearSplitPattern OnDirtyLayoutWrapperSwap function, compare result.
     * @tc.expected: OnDirtyLayoutWrapperSwap success and result correct.
     */

    /**
     *     case 1: LayoutWrapper::SkipMeasureContent = false , skipMeasure = true;
     */
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(linearLayoutAlgorithm, true);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    bool firstCase = linearSPlitPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, true, false);
    EXPECT_FALSE(firstCase);

    /**
     *     case 2: LayoutWrapper::SkipMeasureContent = false , skipMeasure = false;
     */

    layoutAlgorithmWrapper = AceType::MakeRefPtr<LayoutAlgorithmWrapper>(linearLayoutAlgorithm, false);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    bool secondCase = linearSPlitPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, false, false);
    EXPECT_TRUE(secondCase);
}

/**
 * @tc.name: LinearSplitPatternTest002
 * @tc.desc: Test linearSplit pattern OnModifyDone function.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Get linearSplitPattern and.
     */
    RefPtr<LinearSplitPattern> linearSPlitPattern = frameNode->GetPattern<LinearSplitPattern>();
    EXPECT_NE(linearSPlitPattern, nullptr);

    /**
     * @tc.steps: step3. call linearSplitPattern OnModifyDone function, compare result.
     * @tc.expected: OnModifyDone success and result correct.
     */
    linearSPlitPattern->OnModifyDone();
    EXPECT_TRUE(linearSPlitPattern->resizeable_);
    EXPECT_NE(linearSPlitPattern->panEvent_, nullptr);
    EXPECT_NE(linearSPlitPattern->mouseEvent_, nullptr);
}

/**
 * @tc.name: LinearSplitPatternTest003
 * @tc.desc: Test linerSplit pattern OnDirtyLayoutWrapperSwap function.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Get linearSplitPattern.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);

    /**
     * @tc.steps: step3. call linearSplitPattern OnDirtyLayoutWrapperSwap function, compare result.
     * @tc.expected: the properties of linearSplitPattern are updated rightly
     */
    RefPtr<LayoutWrapper> linearLayoutWrapper = frameNode->CreateLayoutWrapper(true, true);
    linearSplitPattern->OnDirtyLayoutWrapperSwap(linearLayoutWrapper, true, true);
    EXPECT_EQ(linearSplitPattern->splitLength_, 0.0f);
    EXPECT_EQ(linearSplitPattern->isOverParent_, false);
}
/**
 * @tc.name: LinearSplitPatternTest004
 * @tc.desc: Test linerSplit pattern HandlePanEvent  when resizeable is false.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, false);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Get linearSplitPattern.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);

    /**
     * @tc.steps: step3. Construct GestureEvent and Call HandlePanStart, HandlePanUpdate.
     * @tc.expected: functions exit normally
     */
    GestureEvent info;
    linearSplitPattern->HandlePanStart(info);
    linearSplitPattern->HandlePanUpdate(info);
    EXPECT_FALSE(linearSplitPattern->isDraged_);

    /**
     * @tc.steps: step4. Construct MouseEvent and Call HandleMouseEvent.
     * @tc.expected: functions exit normally
     */
    MouseInfo info2;
    linearSplitPattern->HandleMouseEvent(info2);
    EXPECT_FALSE(linearSplitPattern->isDraged_);
}
/**
 * @tc.name: LinearSplitPatternTest005
 * @tc.desc: Test linerSplit pattern HandlePanEvent  when resizeable is true.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create split and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Get linearSplitPattern.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);

    /**
     * @tc.steps: step3. Construct GestureEvent and Call HandlePanEvent function.
     * @tc.expected: functions exit normally when spiltRects is null
     */
    GestureEvent info;
    Offset globalLocation(1, 1);
    info.SetGlobalLocation(globalLocation);
    linearSplitPattern->HandlePanStart(info);
    EXPECT_TRUE(linearSplitPattern->isDraged_);
    linearSplitPattern->HandlePanUpdate(info);
    EXPECT_EQ(linearSplitPattern->preOffset_, info.GetOffsetY());
    linearSplitPattern->HandlePanEnd(info);
    EXPECT_EQ(linearSplitPattern->dragedSplitIndex_, DEFAULT_DRAG_INDEX);
}
/**
 * @tc.name: LinearSplitPatternTest006
 * @tc.desc: Test LinearSplit Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest006, TestSize.Level1)
{
    std::vector<SplitType> splitType = { SplitType::COLUMN_SPLIT, SplitType::ROW_SPLIT };
    for (int turn = 0; turn < splitType.size(); turn++) {
        /**
         * @tc.steps: step1. Create split and initialize related properties.
         */
        LinearSplitModelNG model;
        model.Create(splitType[turn]);
        model.SetResizeable(splitType[turn], true);

        auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
        ASSERT_NE(frameNode, nullptr);

        /**
         * @tc.steps: step2. Get linearSplitPattern and layoutWrapper.
         */
        RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
        ASSERT_NE(linearSplitPattern, nullptr);
        RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
        ASSERT_NE(geometryNode, nullptr);
        geometryNode->SetContentSize(SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
        frameNode->SetGeometryNode(geometryNode);
        auto linearSplitLayoutProperty = frameNode->GetLayoutProperty<LinearSplitLayoutProperty>();
        ASSERT_NE(linearSplitLayoutProperty, nullptr);
        LayoutWrapper* layoutWrapper = new LayoutWrapper(frameNode, geometryNode, linearSplitLayoutProperty);
        ASSERT_NE(layoutWrapper, nullptr);
        std::vector<float> dragSplitOffset = { 0.0f, 2.0f };
        RefPtr<LinearSplitLayoutAlgorithm> linearLayoutAlgorithm =
            AceType::MakeRefPtr<LinearSplitLayoutAlgorithm>(splitType[turn], dragSplitOffset, false);
        RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
            AceType::MakeRefPtr<LayoutAlgorithmWrapper>(linearLayoutAlgorithm, false);
        layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
        /**
         * @tc.steps: steps3. Call  LinearSplitLayoutAlgorithm Layout without children.
         * @tc.expected: SplitRects is empty.
         */
        linearLayoutAlgorithm->Layout(layoutWrapper);
        EXPECT_TRUE(linearSplitPattern->splitRects_.empty());
        /**
         * @tc.steps: step4. Call  LinearSplitLayoutAlgorithm Measure without children.
         * @tc.expected: frameSize is right.
         */
        linearLayoutAlgorithm->Measure(layoutWrapper);
        EXPECT_EQ(layoutWrapper->GetGeometryNode()->GetFrameSize().Width(), 0);
    }
}
/**
 * @tc.name: LinearSplitPatternTest007
 * @tc.desc: Test ColumnSplit Layout with children nodes.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create columnSplit and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    /**
     * @tc.steps: step2. Get linearSplitPattern and layoutWrapper.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    frameNode->SetGeometryNode(geometryNode);
    auto linearSplitLayoutProperty = frameNode->GetLayoutProperty<LinearSplitLayoutProperty>();
    ASSERT_NE(linearSplitLayoutProperty, nullptr);
    LayoutWrapper* layoutWrapper = new LayoutWrapper(frameNode, geometryNode, linearSplitLayoutProperty);
    ASSERT_NE(layoutWrapper, nullptr);
    std::vector<float> dragSplitOffset;
    RefPtr<LinearSplitLayoutAlgorithm> linearLayoutAlgorithm =
        AceType::MakeRefPtr<LinearSplitLayoutAlgorithm>(SplitType::COLUMN_SPLIT, dragSplitOffset, false);
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(linearLayoutAlgorithm, false);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);

    layoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(RK356_WIDTH), CalcLength(RK356_HEIGHT)));
    LayoutConstraintF parentLayoutConstraint;
    parentLayoutConstraint.maxSize = CONTAINER_SIZE;
    parentLayoutConstraint.percentReference = CONTAINER_SIZE;

    PaddingProperty noPadding;
    noPadding.left = CalcLength(NO_PADDING);
    noPadding.right = CalcLength(NO_PADDING);
    noPadding.top = CalcLength(NO_PADDING);
    noPadding.bottom = CalcLength(NO_PADDING);
    layoutWrapper->GetLayoutProperty()->UpdatePadding(noPadding);
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(parentLayoutConstraint);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();

    auto childLayoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    childLayoutConstraint.maxSize = CONTAINER_SIZE;
    childLayoutConstraint.minSize = SizeF(ZERO, ZERO);
    /**
     * @tc.steps: step3. Create childFrameNode and Get childLayoutWrapper.
     */
    /* corresponding ets code:
        ColumnSplit(){
            Text('1').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center).width('100%')
            Text('2').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).width('100%')
            Text('3').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center).width('100%')
            Text('4').height(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).width('100%')
            Text('5').height(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center).width('100%')
        }
        .backgroundColor(Color.Pink)
        .resizeable(true)
        .width('100%')
        .height('100%')
    */
    for (int32_t i = 0; i < FIVE_ITEM_SIZE; i++) {
        auto itemFrameNode = FrameNode::CreateFrameNode(V2::BLANK_ETS_TAG, i + 1, AceType::MakeRefPtr<Pattern>());
        RefPtr<GeometryNode> itemGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        itemGeometryNode->Reset();
        RefPtr<LayoutWrapper> itemLayoutWrapper =
            AceType::MakeRefPtr<LayoutWrapper>(itemFrameNode, itemGeometryNode, itemFrameNode->GetLayoutProperty());
        itemLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
        itemLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(RK356_WIDTH), CalcLength(SMALL_ITEM_HEIGHT)));
        itemLayoutWrapper->GetLayoutProperty()->UpdatePadding(noPadding);
        auto boxLayoutAlgorithm = itemFrameNode->GetPattern<Pattern>()->CreateLayoutAlgorithm();
        EXPECT_FALSE(boxLayoutAlgorithm == nullptr);
        itemLayoutWrapper->SetLayoutAlgorithm(
            AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(boxLayoutAlgorithm));
        frameNode->AddChild(itemFrameNode);
        layoutWrapper->AppendChild(itemLayoutWrapper);
    }
    /**
     * @tc.steps: step4. Call Measure and Layout.
     * @tc.expected: size and offset are right
     */
    linearLayoutAlgorithm->Measure(layoutWrapper);
    linearLayoutAlgorithm->Layout(layoutWrapper);
    EXPECT_EQ(layoutWrapper->GetGeometryNode()->GetFrameSize(), SizeF(RK356_WIDTH, RK356_HEIGHT));
    EXPECT_EQ(layoutWrapper->GetGeometryNode()->GetFrameOffset(), OFFSET_TOP_LEFT);

    auto verticalRemaining = RK356_HEIGHT - SMALL_ITEM_HEIGHT * FIVE_ITEM_SIZE;
    for (int32_t i = 0; i < FIVE_ITEM_SIZE; i++) {
        auto childWrapper = layoutWrapper->GetOrCreateChildByIndex(i);
        auto childSize = childWrapper->GetGeometryNode()->GetFrameSize();
        auto childOffset = childWrapper->GetGeometryNode()->GetFrameOffset();
        EXPECT_EQ(childSize, SizeF(RK356_WIDTH, SMALL_ITEM_HEIGHT));
        EXPECT_EQ(childOffset, OffsetF(ZERO, verticalRemaining / 2 + i * (SMALL_ITEM_HEIGHT + DEFAULT_SPLIT_HEIGHT)));
    }
    linearSplitPattern->splitRects_ = linearLayoutAlgorithm->GetSplitRects();
    EXPECT_TRUE(linearSplitPattern->splitRects_[1].IsInRegion(Point(10, 543)));

    /**
     * @tc.steps: step5. Construct GestureEvent and Call HandlePanEvent function.
     * @tc.expected: functions exit normally when gestureEventInfo is not in splitRects region
     */
    GestureEvent info;
    Offset globalLocation(0, 500);
    info.SetGlobalLocation(globalLocation);
    linearSplitPattern->HandlePanStart(info);
    EXPECT_TRUE(linearSplitPattern->isDraged_);
    linearSplitPattern->HandlePanUpdate(info);
    EXPECT_EQ(linearSplitPattern->preOffset_, info.GetOffsetY());

    /**
     * @tc.steps: step6. Construct MouseInfo and Call HandleMouseEvent function.
     * @tc.expected: return normally when isDraged is true
     */
    MouseInfo mouseInfo;
    mouseInfo.SetGlobalLocation(globalLocation);
    linearSplitPattern->HandleMouseEvent(mouseInfo);
    EXPECT_EQ(linearSplitPattern->mouseDragedSplitIndex_, DEFAULT_DRAG_INDEX);
}
/**
 * @tc.name: LinearSplitPatternTest008
 * @tc.desc: Test RowSplit Layout with children nodes.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create rowSplit and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::ROW_SPLIT);
    model.SetResizeable(SplitType::ROW_SPLIT, true);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    /**
     * @tc.steps: step2. Get linearSplitPattern and layoutWrapper.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    frameNode->SetGeometryNode(geometryNode);
    auto linearSplitLayoutProperty = frameNode->GetLayoutProperty<LinearSplitLayoutProperty>();
    ASSERT_NE(linearSplitLayoutProperty, nullptr);
    LayoutWrapper* layoutWrapper = new LayoutWrapper(frameNode, geometryNode, linearSplitLayoutProperty);
    ASSERT_NE(layoutWrapper, nullptr);
    std::vector<float> dragSplitOffset;
    RefPtr<LinearSplitLayoutAlgorithm> linearLayoutAlgorithm =
        AceType::MakeRefPtr<LinearSplitLayoutAlgorithm>(SplitType::ROW_SPLIT, dragSplitOffset, false);
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(linearLayoutAlgorithm, false);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);

    layoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(RK356_WIDTH), CalcLength(COLUMN_HEIGHT)));
    LayoutConstraintF parentLayoutConstraint;
    parentLayoutConstraint.maxSize = CONTAINER_SIZE;
    parentLayoutConstraint.percentReference = CONTAINER_SIZE;

    PaddingProperty noPadding;
    noPadding.left = CalcLength(NO_PADDING);
    noPadding.right = CalcLength(NO_PADDING);
    noPadding.top = CalcLength(NO_PADDING);
    noPadding.bottom = CalcLength(NO_PADDING);
    layoutWrapper->GetLayoutProperty()->UpdatePadding(noPadding);
    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(parentLayoutConstraint);
    layoutWrapper->GetLayoutProperty()->UpdateContentConstraint();

    auto childLayoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    childLayoutConstraint.maxSize = CONTAINER_SIZE;
    childLayoutConstraint.minSize = SizeF(ZERO, ZERO);
    /**
     * @tc.steps: step3. Create childFrameNode and Get childLayoutWrapper.
     */
    /* corresponding ets code:
        RowSplit(){
            Text('1').width(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).height('100%')
            Text('2').width(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).height('100%')
            Text('3').width(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).height('100%')
            Text('4').width(50).backgroundColor(0xD2B48C).textAlign(TextAlign.Center).height('100%')
            Text('5').width(50).backgroundColor(0xF5DEB3).textAlign(TextAlign.Center).height('100%')
        }
        .resizeable(true)
        .width('100%')
        .height(150)
        .backgroundColor(Color.Pink)
    */
    for (int32_t i = 0; i < FIVE_ITEM_SIZE; i++) {
        auto itemFrameNode = FrameNode::CreateFrameNode(V2::BLANK_ETS_TAG, i + 1, AceType::MakeRefPtr<Pattern>());
        RefPtr<GeometryNode> itemGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        itemGeometryNode->Reset();
        RefPtr<LayoutWrapper> itemLayoutWrapper =
            AceType::MakeRefPtr<LayoutWrapper>(itemFrameNode, itemGeometryNode, itemFrameNode->GetLayoutProperty());
        itemLayoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(childLayoutConstraint);
        itemLayoutWrapper->GetLayoutProperty()->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(SMALL_ITEM_WIDTH), CalcLength(COLUMN_HEIGHT)));
        itemLayoutWrapper->GetLayoutProperty()->UpdatePadding(noPadding);
        auto boxLayoutAlgorithm = itemFrameNode->GetPattern<Pattern>()->CreateLayoutAlgorithm();
        EXPECT_FALSE(boxLayoutAlgorithm == nullptr);
        itemLayoutWrapper->SetLayoutAlgorithm(
            AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(boxLayoutAlgorithm));
        frameNode->AddChild(itemFrameNode);
        layoutWrapper->AppendChild(itemLayoutWrapper);
    }
    /**
     * @tc.steps: step4. Call Measure and Layout.
     * @tc.expected: size and offset are right
     */
    linearLayoutAlgorithm->Measure(layoutWrapper);
    linearLayoutAlgorithm->Layout(layoutWrapper);
    EXPECT_EQ(layoutWrapper->GetGeometryNode()->GetFrameSize(), SizeF(RK356_WIDTH, COLUMN_HEIGHT));
    EXPECT_EQ(layoutWrapper->GetGeometryNode()->GetFrameOffset(), OFFSET_TOP_LEFT);

    auto horizontalRemaining = RK356_WIDTH - FIVE_ITEM_SIZE * SMALL_ITEM_WIDTH;
    for (int32_t i = 0; i < FIVE_ITEM_SIZE; i++) {
        auto childWrapper = layoutWrapper->GetOrCreateChildByIndex(i);
        auto childSize = childWrapper->GetGeometryNode()->GetFrameSize();
        auto childOffset = childWrapper->GetGeometryNode()->GetFrameOffset();
        EXPECT_EQ(childSize, SizeF(SMALL_ITEM_WIDTH, COLUMN_HEIGHT));
        EXPECT_EQ(childOffset, OffsetF(horizontalRemaining / 2 + i * (SMALL_ITEM_WIDTH + DEFAULT_SPLIT_WIDTH), ZERO));
    }
}
/**
 * @tc.name: LinearSplitPatternTest009
 * @tc.desc: Test linerSplit pattern HandleMouseEvent  when resizeable is true.
 * @tc.type: FUNC
 */
HWTEST_F(LinearSplitPatternTestNg, LinearSplitPatternTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create columnSplit and initialize related properties.
     */
    LinearSplitModelNG model;
    model.Create(SplitType::COLUMN_SPLIT);
    model.SetResizeable(SplitType::COLUMN_SPLIT, true);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    /**
     * @tc.steps: step2. Get linearSplitPattern.
     */
    RefPtr<LinearSplitPattern> linearSplitPattern = frameNode->GetPattern<LinearSplitPattern>();
    ASSERT_NE(linearSplitPattern, nullptr);
    linearSplitPattern->HandleHoverEvent(true);
    /**
     * @tc.steps: step3. Construct MouseEvent and Call HandleMouseEvent function.
     * @tc.expected: function exits normally when splitRects is empty
     */
    MouseInfo info;
    Offset globalLocation(0, 500);
    info.SetGlobalLocation(globalLocation);
    linearSplitPattern->HandleHoverEvent(false);
    linearSplitPattern->HandleMouseEvent(info);
    EXPECT_EQ(linearSplitPattern->mouseDragedSplitIndex_, DEFAULT_DRAG_INDEX);
    /**
     * @tc.steps: step4. Construct GestureEvent and Call HandlePanEnd function.
     * @tc.expected: function exits normally when sourceDevice is SourceType::MOUSE
     */
    GestureEvent gestureInfo;
    gestureInfo.SetSourceDevice(SourceType::MOUSE);
    gestureInfo.SetGlobalLocation(globalLocation);
    linearSplitPattern->HandlePanEnd(gestureInfo);
    EXPECT_EQ(linearSplitPattern->dragedSplitIndex_, DEFAULT_DRAG_INDEX);
}
} // namespace OHOS::Ace::NG