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
#define private public

#include <optional>

#include "gtest/gtest.h"

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/pattern/list/list_item_group_layout_algorithm.h"
#include "core/components_ng/pattern/list/list_item_group_layout_property.h"
#include "core/components_ng/pattern/list/list_item_group_model_ng.h"
#include "core/components_ng/pattern/list/list_layout_algorithm.h"
#include "core/components_ng/pattern/list/list_lanes_layout_algorithm.h"
#include "core/components_ng/pattern/list/list_layout_property.h"
#include "core/components_ng/pattern/list/list_model_ng.h"
#include "core/components_ng/pattern/list/list_position_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t INITIAL_INDEX_VALUE = 0;
constexpr int32_t CACHED_COUNT_VALUE = 10;
constexpr int32_t LIST_ITEM_NUMBER = 10;
constexpr int32_t START_INDEX = 0;
constexpr int32_t END_INDEX = 10;
constexpr int32_t LANES_VALUE = 3;
constexpr int32_t LANES_CONSTRAINT_CASE1_VALUE = 1;
constexpr int32_t LANES_CONSTRAINT_CASE2_VALUE = 2;
constexpr int32_t JUMP_INDEX = 5;
constexpr int32_t JUMP_INDEX_SPECIAL_CASE1 = -1;
constexpr int32_t JUMP_INDEX_SPECIAL_CASE2 = 100;
constexpr float LIST_ITEM_WIDTH = 10.0f;
constexpr float LIST_ITEM_HEIGHT = 30.0f;
constexpr float LIST_WIDTH_CONSTRAINT_CASE1_VALUE = 70.0f;
constexpr float LIST_WIDTH_CONSTRAINT_CASE2_VALUE = 60.0f;
constexpr float LIST_HEIGHT_LIMIT = 200.0f;
constexpr float LIST_ITEM_GROUP_HEAD_VALUE = 50.0f;
constexpr float LIST_ITEM_GROUP_FOOT_VALUE = 50.0f;
constexpr Dimension SPACE_VALUE = Dimension(10, DimensionUnit::PX);
constexpr Dimension LANE_MIN_LENGTH_CASE1_VALUE = Dimension(40, DimensionUnit::PX);
constexpr Dimension LANE_MAX_LENGTH_CASE1_VALUE = Dimension(60, DimensionUnit::PX);
constexpr Dimension LANE_MIN_LENGTH_CASE2_VALUE = Dimension(30, DimensionUnit::PX);
constexpr Dimension LANE_MAX_LENGTH_CASE2_VALUE = Dimension(40, DimensionUnit::PX);
constexpr Dimension DIVIDER_STROKE_WIDTH_VALUE = Dimension(10, DimensionUnit::PX);
constexpr Dimension DIVIDER_START_MARGIN_VALUE = Dimension(0, DimensionUnit::PX);
constexpr Dimension DIVIDER_END_MARGIN_VALUE = Dimension(0, DimensionUnit::PX);
constexpr Color DIVIDER_COLOR_VALUE = Color(0x000000);
constexpr Axis LIST_DIRECTION_CASE1_VALUE = Axis::VERTICAL;
constexpr Axis LIST_DIRECTION_CASE2_VALUE = Axis::HORIZONTAL;
constexpr EdgeEffect EDGE_EFFECT_VALUE = EdgeEffect::NONE;
constexpr V2::ListItemAlign LIST_ITEM_ALIGN_VALUE = V2::ListItemAlign::CENTER;
} // namespace

struct TestProperty {
    std::optional<Dimension> spaceValue = std::nullopt;
    std::optional<int32_t> initialIndexValue = std::nullopt;
    std::optional<RefPtr<ScrollControllerBase>> scrollControllerBasePtr = std::nullopt;
    std::optional<RefPtr<ScrollBarProxy>> scrollBarProxyPtr = std::nullopt;
    std::optional<Axis> listDirectionValue = std::nullopt;
    std::optional<EdgeEffect> edgeEffectValue = std::nullopt;
    std::optional<V2::ItemDivider> dividerValue = std::nullopt;
    std::optional<int32_t> lanesValue = std::nullopt;
    std::optional<Dimension> laneMinLengthValue = std::nullopt;
    std::optional<Dimension> laneMaxlengthValue = std::nullopt;
    std::optional<V2::ListItemAlign> listItemAlignValue = std::nullopt;
    std::optional<int32_t> cachedCountValue = std::nullopt;
};

struct ListItemGroupTestProperty {
    std::optional<Dimension> spaceValue = std::nullopt;
    std::optional<V2::ItemDivider> itemDividerValue = std::nullopt;
};

class ListPatternTestNg : public testing::Test {
public:
    static RefPtr<FrameNode> CreateListParagraph(const TestProperty& testProperty);
    static RefPtr<FrameNode> CreateListItemGroupParagraph(
        const ListItemGroupTestProperty& listItemGroupTestProperty);
};

RefPtr<FrameNode> ListPatternTestNg::CreateListParagraph(const TestProperty& testProperty)
{
    ListModelNG listModelNG;
    listModelNG.Create();
    if (testProperty.spaceValue.has_value()) {
        listModelNG.SetSpace(testProperty.spaceValue.value());
    }
    if (testProperty.initialIndexValue.has_value()) {
        listModelNG.SetInitialIndex(testProperty.initialIndexValue.value());
    }
    if (testProperty.scrollControllerBasePtr.has_value() &&
        testProperty.scrollBarProxyPtr.has_value()) {
            listModelNG.SetScroller(testProperty.scrollControllerBasePtr.value(),
                testProperty.scrollBarProxyPtr.value());
    }
    if (testProperty.listDirectionValue.has_value()) {
        listModelNG.SetListDirection(testProperty.listDirectionValue.value());
    }
    if (testProperty.edgeEffectValue.has_value()) {
        listModelNG.SetEdgeEffect(testProperty.edgeEffectValue.value());
    }
    if (testProperty.dividerValue.has_value()) {
        listModelNG.SetDivider(testProperty.dividerValue.value());
    }
    if (testProperty.lanesValue.has_value()) {
        listModelNG.SetLanes(testProperty.lanesValue.value());
    }
    if (testProperty.laneMinLengthValue.has_value()) {
        listModelNG.SetLaneMinLength(testProperty.laneMinLengthValue.value());
    }
    if (testProperty.laneMaxlengthValue.has_value()) {
        listModelNG.SetLaneMaxLength(testProperty.laneMaxlengthValue.value());
    }
    if (testProperty.listItemAlignValue.has_value()) {
        listModelNG.SetListItemAlign(testProperty.listItemAlignValue.value());
    }
    if (testProperty.cachedCountValue.has_value()) {
        listModelNG.SetCachedCount(testProperty.cachedCountValue.value());
    }

    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->Finish();
    return AceType::DynamicCast<FrameNode>(element);
}

RefPtr<FrameNode> ListPatternTestNg::CreateListItemGroupParagraph(
    const ListItemGroupTestProperty& listItemGroupTestProperty)
{
    ListItemGroupModelNG listItemGroupModelNG;
    listItemGroupModelNG.Create();

    if (listItemGroupTestProperty.spaceValue.has_value()) {
        listItemGroupModelNG.SetSpace(listItemGroupTestProperty.spaceValue.value());
    }
    if (listItemGroupTestProperty.itemDividerValue.has_value()) {
        listItemGroupModelNG.SetDivider(listItemGroupTestProperty.itemDividerValue.value());
    }
    
    RefPtr<UINode> element = ViewStackProcessor::GetInstance()->Finish();
    return AceType::DynamicCast<FrameNode>(element);
}

/**
 * @tc.name: ListFrameNodeCreator001
 * @tc.desc: Test all the properties of list.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.spaceValue = std::make_optional(SPACE_VALUE);
    testProperty.initialIndexValue = std::make_optional(INITIAL_INDEX_VALUE);
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.edgeEffectValue = std::make_optional(EDGE_EFFECT_VALUE);
    testProperty.lanesValue = std::make_optional(LANES_VALUE);
    testProperty.laneMinLengthValue = std::make_optional(LANE_MIN_LENGTH_CASE1_VALUE);
    testProperty.laneMaxlengthValue = std::make_optional(LANE_MAX_LENGTH_CASE1_VALUE);
    testProperty.listItemAlignValue = std::make_optional(LIST_ITEM_ALIGN_VALUE);
    testProperty.cachedCountValue = std::make_optional(CACHED_COUNT_VALUE);
    V2::ItemDivider itemDivider = V2::ItemDivider();
    itemDivider.strokeWidth = DIVIDER_STROKE_WIDTH_VALUE;
    itemDivider.startMargin = DIVIDER_START_MARGIN_VALUE;
    itemDivider.endMargin = DIVIDER_END_MARGIN_VALUE;
    itemDivider.color = DIVIDER_COLOR_VALUE;
    testProperty.dividerValue = std::make_optional(itemDivider);

    /**
     * @tc.steps: step2. create list frameNode and get listLayoutProperty.
     * @tc.expected: step2. get listLayoutProperty success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    RefPtr<ListLayoutProperty> listLayoutProperty = AceType::DynamicCast<ListLayoutProperty>(layoutProperty);
    EXPECT_NE(listLayoutProperty, nullptr);

    /**
     * @tc.steps: step3. compare list properties and expected value.
     * @tc.expected: step3. list properties equals expected value.
     */
    EXPECT_EQ(listLayoutProperty->GetSpace().value(), SPACE_VALUE);
    EXPECT_EQ(listLayoutProperty->GetInitialIndex().value(), INITIAL_INDEX_VALUE);
    EXPECT_EQ(listLayoutProperty->GetListDirection().value(), LIST_DIRECTION_CASE1_VALUE);
    EXPECT_EQ(listLayoutProperty->GetEdgeEffect().value(), EDGE_EFFECT_VALUE);
    EXPECT_EQ(listLayoutProperty->GetLanes().value(), LANES_VALUE);
    EXPECT_EQ(listLayoutProperty->GetLaneMinLength().value(), LANE_MIN_LENGTH_CASE1_VALUE);
    EXPECT_EQ(listLayoutProperty->GetLaneMaxLength().value(), LANE_MAX_LENGTH_CASE1_VALUE);
    EXPECT_EQ(listLayoutProperty->GetListItemAlign().value(), LIST_ITEM_ALIGN_VALUE);
    EXPECT_EQ(listLayoutProperty->GetCachedCount().value(), CACHED_COUNT_VALUE);
    EXPECT_EQ(listLayoutProperty->GetDivider().value().strokeWidth, DIVIDER_STROKE_WIDTH_VALUE);
    EXPECT_EQ(listLayoutProperty->GetDivider().value().startMargin, DIVIDER_START_MARGIN_VALUE);
    EXPECT_EQ(listLayoutProperty->GetDivider().value().endMargin, DIVIDER_END_MARGIN_VALUE);
    EXPECT_EQ(listLayoutProperty->GetDivider().value().color, DIVIDER_COLOR_VALUE);
}

/**
 * @tc.name: ListFrameNodeCreator002
 * @tc.desc: Test list measure and layout function, set space initialIndex direction cachedCount.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.spaceValue = std::make_optional(SPACE_VALUE);
    testProperty.initialIndexValue = std::make_optional(INITIAL_INDEX_VALUE);
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.cachedCountValue = std::make_optional(CACHED_COUNT_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos,
            (index - INITIAL_INDEX_VALUE) * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT));
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].endPos,
            (index - INITIAL_INDEX_VALUE) * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT) + LIST_ITEM_HEIGHT);
    }
    
    listLayoutAlgorithm.Layout(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), 0);
        EXPECT_EQ(frameOffset.GetY(), (index - INITIAL_INDEX_VALUE) * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT));
    }
}

/**
 * @tc.name: ListFrameNodeCreator003
 * @tc.desc: Test list measure and layout function, set direction lanes listItemAlign.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.lanesValue = std::make_optional(LANES_VALUE);

    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    OptionalSizeF idealSize = OptionalSizeF(LIST_ITEM_WIDTH * LANES_VALUE,
        LIST_ITEM_HEIGHT * LIST_ITEM_NUMBER);
    layoutConstraint.UpdateIllegalSelfIdealSizeWithCheck(idealSize);
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLanesLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    listLayoutAlgorithm.Measure(&layoutWrapper);
    listLayoutAlgorithm.Layout(&layoutWrapper);

    EXPECT_EQ(LANES_VALUE, listLayoutAlgorithm.GetLanes());
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), (index % LANES_VALUE) * LIST_ITEM_WIDTH);
        EXPECT_EQ(frameOffset.GetY(), (index / LANES_VALUE) * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator004
 * @tc.desc: Test list measure and layout function, set direction laneConstraint{laneMinLength, laneMaxLength} case1.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.laneMinLengthValue = std::make_optional(LANE_MIN_LENGTH_CASE1_VALUE);
    testProperty.laneMaxlengthValue = std::make_optional(LANE_MAX_LENGTH_CASE1_VALUE);

    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    OptionalSizeF idealSize = OptionalSizeF(LIST_WIDTH_CONSTRAINT_CASE1_VALUE,
        LIST_ITEM_HEIGHT * LIST_ITEM_NUMBER);
    layoutConstraint.UpdateIllegalSelfIdealSizeWithCheck(idealSize);
    layoutConstraint.maxSize = SizeF(LIST_WIDTH_CONSTRAINT_CASE1_VALUE,
        LIST_ITEM_HEIGHT * LIST_ITEM_NUMBER);
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLanesLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    listLayoutAlgorithm.Measure(&layoutWrapper);
    listLayoutAlgorithm.Layout(&layoutWrapper);

    EXPECT_EQ(LANES_CONSTRAINT_CASE1_VALUE, listLayoutAlgorithm.GetLanes());
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), (index % LANES_CONSTRAINT_CASE1_VALUE) * LANE_MAX_LENGTH_CASE1_VALUE.Value());
        EXPECT_EQ(frameOffset.GetY(), (index / LANES_CONSTRAINT_CASE1_VALUE) * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator005
 * @tc.desc: Test list measure and layout function, set direction laneConstraint{laneMinLength, laneMaxLength} case2.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.laneMinLengthValue = std::make_optional(LANE_MIN_LENGTH_CASE2_VALUE);
    testProperty.laneMaxlengthValue = std::make_optional(LANE_MAX_LENGTH_CASE2_VALUE);

    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    OptionalSizeF idealSize = OptionalSizeF(LIST_WIDTH_CONSTRAINT_CASE2_VALUE,
        LIST_ITEM_HEIGHT * LIST_ITEM_NUMBER);
    layoutConstraint.UpdateIllegalSelfIdealSizeWithCheck(idealSize);
    layoutConstraint.maxSize = SizeF(LIST_WIDTH_CONSTRAINT_CASE2_VALUE,
        LIST_ITEM_HEIGHT * LIST_ITEM_NUMBER);
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLanesLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    listLayoutAlgorithm.Measure(&layoutWrapper);
    listLayoutAlgorithm.Layout(&layoutWrapper);

    EXPECT_EQ(LANES_CONSTRAINT_CASE2_VALUE, listLayoutAlgorithm.GetLanes());
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), (index % LANES_CONSTRAINT_CASE2_VALUE) * LANE_MIN_LENGTH_CASE2_VALUE.Value());
        EXPECT_EQ(frameOffset.GetY(), (index / LANES_CONSTRAINT_CASE2_VALUE) * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator006
 * @tc.desc: Test list measure and layout function, set direction, special direction(Axis::HORIZONTAL).
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.initialIndexValue = std::make_optional(INITIAL_INDEX_VALUE);
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE2_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos, index * LIST_ITEM_WIDTH);
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].endPos, (index + 1) * LIST_ITEM_WIDTH);
    }
    
    listLayoutAlgorithm.Layout(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), index * LIST_ITEM_WIDTH);
        EXPECT_EQ(frameOffset.GetY(), 0);
    }
}

/**
 * @tc.name: ListFrameNodeCreator007
 * @tc.desc: Test list measure function, set direction, jumpIndex_ set JUMP_INDEX.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. jump to JUMP_INDEX. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = JUMP_INDEX;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = JUMP_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos,
            (index - JUMP_INDEX) * LIST_ITEM_HEIGHT);
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].endPos,
            (index - JUMP_INDEX + 1) * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator008
 * @tc.desc: Test list measure and layout function, special case: jumpIndex < 0. List jump to top.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }
    
    /**
     * @tc.steps: step4. jump to 0. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = JUMP_INDEX_SPECIAL_CASE1;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos, index * LIST_ITEM_HEIGHT);
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].endPos, (index + 1) * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator009
 * @tc.desc: Test list measure and layout function, special case: jumpIndex > listItemCount.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.cachedCountValue = std::make_optional(CACHED_COUNT_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    OptionalSizeF idealSize = OptionalSizeF(LIST_ITEM_WIDTH, LIST_HEIGHT_LIMIT);
    layoutConstraint.UpdateIllegalSelfIdealSizeWithCheck(idealSize);
    layoutConstraint.maxSize = SizeF(LIST_ITEM_WIDTH, LIST_HEIGHT_LIMIT);
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to list frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. jump to end and layout back. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = JUMP_INDEX_SPECIAL_CASE2;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    int32_t layoutNumber = LIST_HEIGHT_LIMIT / LIST_ITEM_HEIGHT;
    for (int32_t index = END_INDEX - 1; index >= END_INDEX - layoutNumber; index--) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos, LIST_ITEM_HEIGHT * (index + 1 - END_INDEX));
    }
}

/**
 * @tc.name: ListFrameNodeCreator010
 * @tc.desc: Test list measure function, special case: list has no listItem.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create testProperty and set properties of list.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    testProperty.cachedCountValue = std::make_optional(CACHED_COUNT_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    OptionalSizeF idealSize = OptionalSizeF(LIST_ITEM_WIDTH, LIST_HEIGHT_LIMIT);
    layoutConstraint.UpdateIllegalSelfIdealSizeWithCheck(idealSize);
    layoutConstraint.maxSize = SizeF(LIST_ITEM_WIDTH, LIST_HEIGHT_LIMIT);
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. no listItem. do listLayoutAlgorithm Measure and compare values.
     * @tc.expected: step3. layout result equals expected result.
     */
    auto listLayoutAlgorithm = ListLayoutAlgorithm();
    listLayoutAlgorithm.jumpIndex_ = 0;
    
    listLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].startPos, 0);
        EXPECT_EQ(listLayoutAlgorithm.itemPosition_[index].endPos, 0);
    }
}

/**
 * @tc.name: ListFrameNodeCreator011
 * @tc.desc: Test all the properties of listItemGroup.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of listItemGroup.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    listItemGroupTestProperty.spaceValue = std::make_optional(SPACE_VALUE);
    V2::ItemDivider itemDivider = V2::ItemDivider();
    itemDivider.strokeWidth = DIVIDER_STROKE_WIDTH_VALUE;
    itemDivider.startMargin = DIVIDER_START_MARGIN_VALUE;
    itemDivider.endMargin = DIVIDER_END_MARGIN_VALUE;
    itemDivider.color = DIVIDER_COLOR_VALUE;
    listItemGroupTestProperty.itemDividerValue = std::make_optional(itemDivider);

    /**
     * @tc.steps: step2. create listItemGroup frameNode and get listItemGroupLayoutProperty.
     * @tc.expected: step2. get listItemGroupLayoutProperty success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    RefPtr<ListItemGroupLayoutProperty> listItemGroupLayoutProperty =
        AceType::DynamicCast<ListItemGroupLayoutProperty>(layoutProperty);
    EXPECT_NE(listItemGroupLayoutProperty, nullptr);

    /**
     * @tc.steps: step3. compare listItemGroup properties and expected value.
     * @tc.expected: step3. listItemGroups properties equals expected value.
     */
    EXPECT_EQ(listItemGroupLayoutProperty->GetSpace().value(), SPACE_VALUE);
    EXPECT_EQ(listItemGroupLayoutProperty->GetDivider().value().strokeWidth, DIVIDER_STROKE_WIDTH_VALUE);
    EXPECT_EQ(listItemGroupLayoutProperty->GetDivider().value().startMargin, DIVIDER_START_MARGIN_VALUE);
    EXPECT_EQ(listItemGroupLayoutProperty->GetDivider().value().endMargin, DIVIDER_END_MARGIN_VALUE);
    EXPECT_EQ(listItemGroupLayoutProperty->GetDivider().value().color, DIVIDER_COLOR_VALUE);
}

/**
 * @tc.name: ListFrameNodeCreator012
 * @tc.desc: Test listItemGroup measure and layout function. ListItemGroup only has listItem.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of it.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    listItemGroupTestProperty.spaceValue = std::make_optional(SPACE_VALUE);
    
    /**
     * @tc.steps: step2. create listItemGroup frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add listItem layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. do layoutAlgorithm Measure and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    auto listItemGroupLayoutAlgorithm = ListItemGroupLayoutAlgorithm(-1, -1, 0);
    
    listItemGroupLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].first,
            index * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT));
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].second,
            index * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT) + LIST_ITEM_HEIGHT);
    }
    
    /**
     * @tc.steps: step4. do layoutAlgorithm Layout and compare values.
     * @tc.expected: step4. layout result equals expected result.
     */
    listItemGroupLayoutAlgorithm.Layout(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childWrapper = layoutWrapper.GetOrCreateChildByIndex(index);
        auto childGeometryNode = childWrapper->GetGeometryNode();
        EXPECT_NE(childGeometryNode, nullptr);
        auto frameOffset = childGeometryNode->GetMarginFrameOffset();
        EXPECT_EQ(frameOffset.GetX(), 0);
        EXPECT_EQ(frameOffset.GetY(), index * (SPACE_VALUE.Value() + LIST_ITEM_HEIGHT));
    }
}

/**
 * @tc.name: ListFrameNodeCreator013
 * @tc.desc: Test listItemGroup measure function. ListItemGroup has listItem and head.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of it.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    
    /**
     * @tc.steps: step2. create listItemGroup frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);

    /**
     * @tc.steps: step3. add head layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    auto headFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(headFrameNode);
    RefPtr<GeometryNode> headGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    headGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_HEAD_VALUE, LIST_ITEM_GROUP_HEAD_VALUE));
    RefPtr<LayoutProperty> headLayoutProperty = headFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> headLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        headFrameNode, headGeometryNode, headLayoutProperty);
    layoutWrapper.AppendChild(std::move(headLayoutWrapper));

    /**
     * @tc.steps: step4. add listItem layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step4. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step5. do layoutAlgorithm Measure and compare values.
     * @tc.expected: step5. layout result equals expected result.
     */
    auto listItemGroupLayoutAlgorithm = ListItemGroupLayoutAlgorithm(0, -1, 1);
    listItemGroupLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].first,
            index * LIST_ITEM_HEIGHT + LIST_ITEM_GROUP_HEAD_VALUE);
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].second,
            (index + 1) * LIST_ITEM_HEIGHT + LIST_ITEM_GROUP_HEAD_VALUE);
    }
}

/**
 * @tc.name: ListFrameNodeCreator014
 * @tc.desc: Test listItemGroup measure function. ListItemGroup has listItem and foot.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of it.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    
    /**
     * @tc.steps: step2. create listItemGroup frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);
    
    /**
     * @tc.steps: step3. add listItem layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        EXPECT_NE(childFrameNode, nullptr);
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        EXPECT_NE(childGeometryNode, nullptr);
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        EXPECT_NE(childLayoutProperty, nullptr);
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step4. add foot layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step4. create layoutWrapper success.
     */
    auto footFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(footFrameNode);
    RefPtr<GeometryNode> footGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    footGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_FOOT_VALUE, LIST_ITEM_GROUP_FOOT_VALUE));
    RefPtr<LayoutProperty> footLayoutProperty = footFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> footLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        footFrameNode, footGeometryNode, footLayoutProperty);
    layoutWrapper.AppendChild(std::move(footLayoutWrapper));

    /**
     * @tc.steps: step5. do layoutAlgorithm Measure and compare values.
     * @tc.expected: step5. layout result equals expected result.
     */
    auto listItemGroupLayoutAlgorithm = ListItemGroupLayoutAlgorithm(-1, 0, 1);
    listItemGroupLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].first,
            index * LIST_ITEM_HEIGHT);
    }
}

/**
 * @tc.name: ListFrameNodeCreator015
 * @tc.desc: Test listItemGroup measure function. ListItemGroup has listItem and head and foot.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of it.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    
    /**
     * @tc.steps: step2. create listItemGroup frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);
    
    /**
     * @tc.steps: step3. add head layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    auto headFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(headFrameNode);
    RefPtr<GeometryNode> headGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    headGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_HEAD_VALUE, LIST_ITEM_GROUP_HEAD_VALUE));
    RefPtr<LayoutProperty> headLayoutProperty = headFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> headLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        headFrameNode, headGeometryNode, headLayoutProperty);
    layoutWrapper.AppendChild(std::move(headLayoutWrapper));

    /**
     * @tc.steps: step4. add listItem layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step4. create layoutWrapper success.
     */
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        auto childFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
            AceType::MakeRefPtr<Pattern>());
        ViewStackProcessor::GetInstance()->Push(childFrameNode);
        RefPtr<GeometryNode> childGeometryNode = AceType::MakeRefPtr<GeometryNode>();
        childGeometryNode->SetFrameSize(SizeF(LIST_ITEM_WIDTH, LIST_ITEM_HEIGHT));
        RefPtr<LayoutProperty> childLayoutProperty = childFrameNode->GetLayoutProperty();
        RefPtr<LayoutWrapper> childLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
            childFrameNode, childGeometryNode, childLayoutProperty);
        layoutWrapper.AppendChild(std::move(childLayoutWrapper));
    }

    /**
     * @tc.steps: step5. add foot layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step5. create layoutWrapper success.
     */
    auto footFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(footFrameNode);
    RefPtr<GeometryNode> footGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    footGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_FOOT_VALUE, LIST_ITEM_GROUP_FOOT_VALUE));
    RefPtr<LayoutProperty> footLayoutProperty = footFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> footLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        footFrameNode, footGeometryNode, footLayoutProperty);
    layoutWrapper.AppendChild(std::move(footLayoutWrapper));

    /**
     * @tc.steps: step6. do layoutAlgorithm Measure and compare values.
     * @tc.expected: step6. layout result equals expected result.
     */
    auto listItemGroupLayoutAlgorithm = ListItemGroupLayoutAlgorithm(0, 1, 2);
    listItemGroupLayoutAlgorithm.Measure(&layoutWrapper);
    for (int32_t index = START_INDEX; index < END_INDEX; index++) {
        EXPECT_EQ(listItemGroupLayoutAlgorithm.itemPosition_[index].first,
            index * LIST_ITEM_HEIGHT + LIST_ITEM_GROUP_HEAD_VALUE);
    }
}

/**
 * @tc.name: ListFrameNodeCreator016
 * @tc.desc: Test listItemGroup measure function. Special Case: listItemGroup has only head and foot.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listItemGroupTestProperty and set properties of it.
     */
    ListItemGroupTestProperty listItemGroupTestProperty;
    
    /**
     * @tc.steps: step2. create listItemGroup frameNode and layoutWrapper.
     * @tc.expected: step2. create layoutWrapper success.
     */
    RefPtr<FrameNode> frameNode = CreateListItemGroupParagraph(listItemGroupTestProperty);
    EXPECT_NE(frameNode, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = frameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    LayoutConstraintF layoutConstraint;
    layoutConstraint.Reset();
    layoutProperty->UpdateLayoutConstraint(layoutConstraint);
    layoutProperty->UpdateContentConstraint();
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, layoutProperty);
    
    /**
     * @tc.steps: step3. add head layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step3. create layoutWrapper success.
     */
    auto headFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(headFrameNode);
    RefPtr<GeometryNode> headGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    headGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_HEAD_VALUE, LIST_ITEM_GROUP_HEAD_VALUE));
    RefPtr<LayoutProperty> headLayoutProperty = headFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> headLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        headFrameNode, headGeometryNode, headLayoutProperty);
    layoutWrapper.AppendChild(std::move(headLayoutWrapper));
    
    /**
     * @tc.steps: step4. add foot layoutWrapper to listItemGroup frameNode layoutWrapper.
     * @tc.expected: step4. create layoutWrapper success.
     */
    auto footFrameNode = FrameNode::CreateFrameNode(LIST_ITEM_TYPE, 0,
        AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(footFrameNode);
    RefPtr<GeometryNode> footGeometryNode = AceType::MakeRefPtr<GeometryNode>();
    footGeometryNode->SetFrameSize(SizeF(LIST_ITEM_GROUP_FOOT_VALUE, LIST_ITEM_GROUP_FOOT_VALUE));
    RefPtr<LayoutProperty> footLayoutProperty = footFrameNode->GetLayoutProperty();
    RefPtr<LayoutWrapper> footLayoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        footFrameNode, footGeometryNode, footLayoutProperty);
    layoutWrapper.AppendChild(std::move(footLayoutWrapper));

    /**
     * @tc.steps: step5. do layoutAlgorithm Measure and compare values.
     * @tc.expected: step5. layout result equals expected result.
     */
    auto listItemGroupLayoutAlgorithm = ListItemGroupLayoutAlgorithm(0, 1, 2);
    listItemGroupLayoutAlgorithm.Measure(&layoutWrapper);
    listItemGroupLayoutAlgorithm.Layout(&layoutWrapper);
    
    auto frameSize = geometryNode->GetMarginFrameSize();
    EXPECT_EQ(frameSize.Height(), LIST_ITEM_GROUP_HEAD_VALUE + LIST_ITEM_GROUP_FOOT_VALUE);
}

/**
 * @tc.name: ListFrameNodeCreator017
 * @tc.desc: Test list eventHub function. ListEventHub getDirection.
 * @tc.type: FUNC
 */
HWTEST_F(ListPatternTestNg, ListFrameNodeCreator017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create listTestProperty and set properties of it.
     */
    TestProperty testProperty;
    testProperty.listDirectionValue = std::make_optional(LIST_DIRECTION_CASE1_VALUE);
    
    /**
     * @tc.steps: step2. create list frameNode and eventHub.
     * @tc.expected: step2. create eventHub success.
     */
    RefPtr<FrameNode> frameNode = CreateListParagraph(testProperty);
    EXPECT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<EventHub>();
    EXPECT_NE(eventHub, nullptr);
    auto listEventHub = AceType::DynamicCast<ListEventHub>(eventHub);
    EXPECT_NE(listEventHub, nullptr);

    /**
     * @tc.steps: step3. compare function result and expected result.
     * @tc.expected: step3. function result equals expected result.
     */
    EXPECT_EQ(listEventHub->GetDirection(), LIST_DIRECTION_CASE1_VALUE);
}
} // namespace OHOS::Ace::NG
