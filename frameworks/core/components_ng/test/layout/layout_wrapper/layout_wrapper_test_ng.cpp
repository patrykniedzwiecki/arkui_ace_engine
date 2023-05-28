/*
 * Copyright (c) 2023 iSoftStone Information Technology (Group) Co.,Ltd.
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

#include "gtest/gtest.h"

#include <algorithm>

#define protected public
#define private public

#include "base/log/ace_trace.h"
#include "base/memory/ace_type.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/layout/layout_wrapper_builder.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/components_ng/property/property.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/pipeline_ng/ui_task_scheduler.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/flex/flex_layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/syntax/lazy_layout_wrapper_builder.h"
#include "core/components_ng/syntax/lazy_for_each_model.h"

#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t INDEX_NUM_0 = 0;
constexpr int32_t INDEX_ERROR_NUM = -1;
constexpr int32_t NODE_ID_0 = 0;
constexpr int32_t CHILD_COUNT = 1;
constexpr int32_t NODE_ID_1 = 1;
constexpr int32_t NODE_ID_2 = 2;
constexpr int32_t NODE_ID_3 = 3;
constexpr int32_t CACHE_COUNT = 1;
constexpr int32_t HOST_DEPTH = 0;
constexpr int32_t ERROR_HOST_DEPTH = -1;
const std::pair<int32_t, int32_t> RANGE {-1, 0};
const std::pair<int32_t, int32_t> RANGE_0 {0, 0};

constexpr float RK356_WIDTH = 720.0f;
constexpr float RK356_HEIGHT = 1136.0f;
constexpr float ROW_HEIGHT = 120.0f;

const SizeF CONTAINER_SIZE {RK356_WIDTH, RK356_HEIGHT};
SizeF SELF_IDEAL_SIZE {RK356_WIDTH, ROW_HEIGHT};
SizeF FRAME_SIZE {0, 0};
const SizeF TEST_FRAME_SIZE {0, 0};
OptionalSize IDEAL_SIZE {0, 0};

const std::string TEST_TAG = "";
const std::string ROW_FRAME_NODE = "rowFrameNode";
const std::string FIRST_FRAME_NODE = "TabContent";
const std::string FIRST_CHILD_FRAME_NODE = "firstChildFrameNode";
const std::string SECOND_CHILD_FRAME_NODE = "secondChildFrameNode";
const std::string THIRD_CHILD_FRAME_NODE = "thirdChildFrameNode";

constexpr bool TEST_TRUE = true;
constexpr bool TEST_FALSE = false;

RefPtr<LayoutWrapper> CreateLayoutWrapper(const std::string& tag, int32_t nodeId)
{
    auto rowFrameNode =
        FrameNode::CreateFrameNode(tag, nodeId, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    return layoutWrapper;
}

RefPtr<LayoutWrapper> CreateChildLayoutWrapper(const std::string& tag, int32_t nodeId)
{
    auto frameNode =
        FrameNode::CreateFrameNode(tag, nodeId, AceType::MakeRefPtr<Pattern>());
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    return layoutWrapper;
}

RefPtr<LazyLayoutWrapperBuilder> CreateLayoutWrapperBuilder()
{
    RefPtr<LazyForEachActuator> actuator = AceType::MakeRefPtr<LazyForEachActuator>();
    auto builder = AceType::DynamicCast<LazyForEachBuilder>(actuator);
    RefPtr<LazyForEachNode> host_ = AceType::MakeRefPtr<LazyForEachNode>(NODE_ID_1, builder);
    WeakPtr<LazyForEachNode> host(host_);
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = AceType::MakeRefPtr<LazyLayoutWrapperBuilder>(builder, host);

    return wrapperBuilder;
}

void UpdateParentConstraint(RefPtr<LayoutWrapper> layoutWrapper, LayoutConstraintF& parentConstraint)
{
    parentConstraint.maxSize = CONTAINER_SIZE;
    parentConstraint.percentReference = CONTAINER_SIZE;
    parentConstraint.selfIdealSize.SetSize(SizeF(RK356_WIDTH, ROW_HEIGHT));

    layoutWrapper->GetLayoutProperty()->UpdateLayoutConstraint(parentConstraint);
}
}

class LayoutWrapperTestNg : public testing::Test {};

/**
 * @tc.name: LayoutWrapperTest001
 * @tc.desc: Test GetOrCreateChildByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call GetOrCreateChildByIndex and set input index is INDEX_NUM_0.
    * @tc.expected: the return value is null.
    */
    RefPtr<LayoutWrapper> testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_FALSE);
    EXPECT_EQ(testWrapper, nullptr);

    /**
     * @tc.steps: step3. call GetOrCreateChildByIndex and set input index is INDEX_ERROR_NUM.
     * @tc.expected: the return value is null.
     */
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_ERROR_NUM, TEST_FALSE);
    EXPECT_EQ(testWrapper, nullptr);

    /**
     * @tc.steps: step4. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step5. call GetOrCreateChildByIndex and set input index is INDEX_NUM_0.
     * @tc.expected: the return value is the same as secondLayoutWrapper.
     */
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_FALSE);
    EXPECT_EQ(testWrapper, firstChildLayoutWrapper);

    /**
     * @tc.steps: step6. call GetOrCreateChildByIndex and set input addToRenderTree is TEST_TRUE.
     * @tc.expected: testWrapper->isActive_ is true.
     */
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_TRUE);
    EXPECT_TRUE(testWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest002
 * @tc.desc: Test GetOrCreateChildByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call GetOrCreateChildByIndex and set layoutWrapper->currentChildCount_ is CHILD_COUNT.
     * @tc.expected: the return value is null and layoutWrapper->layoutWrapperBuilder_ is null.
     */
    layoutWrapper->currentChildCount_ = CHILD_COUNT;
    RefPtr<LayoutWrapper> testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_FALSE);
    EXPECT_EQ(testWrapper, nullptr);
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);

    /**
     * @tc.steps: step3. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step4. call GetOrCreateChildByIndex and set index is INDEX_NUM_0.
     * @tc.expected: the return value is null.
     */
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_FALSE);
    EXPECT_EQ(testWrapper, nullptr);

    /**
     * @tc.steps: step5. call GetOrCreateChildByIndex and set layoutWrapperBuilder_->wrapperMap_ is not null.
     * @tc.expected: the return value is not null.
     */
    layoutWrapper->layoutWrapperBuilder_->startIndex_ = -1;
    layoutWrapper->layoutWrapperBuilder_->wrapperMap_ = {{1, layoutWrapper}};
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_FALSE);
    ASSERT_NE(testWrapper, nullptr);

    /**
     * @tc.steps: step6. call GetOrCreateChildByIndex and set input addToRenderTree is TEST_TRUE.
     * @tc.expected: the return value is not null.
     */
    testWrapper = layoutWrapper->GetOrCreateChildByIndex(INDEX_NUM_0, TEST_TRUE);
    EXPECT_TRUE(layoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest003
 * @tc.desc: Test SetCacheCount.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call SetCacheCount.
     * @tc.expected: layoutWrapperBuilder_->cacheCount_ is 0.
     */
    layoutWrapper->SetCacheCount(CACHE_COUNT);
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);

    /**
     * @tc.steps: step3. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step4. call SetCacheCount.
     * @tc.expected: layoutWrapperBuilder_->cacheCount_ is equal to CACHE_COUNT.
     */
    layoutWrapper->SetCacheCount(CACHE_COUNT);
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_->cacheCount_, CACHE_COUNT);
}

/**
 * @tc.name: LayoutWrapperTest004
 * @tc.desc: Test GetAllChildrenWithBuild.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call GetAllChildrenWithBuild.
     * @tc.expected: the return value is empty.
     */
    std::list<RefPtr<LayoutWrapper>> retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_TRUE);
    EXPECT_TRUE(retCachedList.empty());

    /**
     * @tc.steps: step3. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step4. call GetAllChildrenWithBuild.
     * @tc.expected: the return value is not empty.
     */
    retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_TRUE);
    EXPECT_FALSE(retCachedList.empty());
    EXPECT_TRUE(firstChildLayoutWrapper->isActive_);

    /**
     * @tc.steps: step5. call GetAllChildrenWithBuild again.
     * @tc.expected: the return value is not empty.
     */
    retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_TRUE);
    EXPECT_FALSE(retCachedList.empty());
}

/**
 * @tc.name: LayoutWrapperTest005
 * @tc.desc: Test GetAllChildrenWithBuild TEST_TRUE.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call GetAllChildrenWithBuild and set child->isActive_ is TEST_TRUE.
     * @tc.expected: the return value is not empty.
     */
    firstChildLayoutWrapper->isActive_ = TEST_TRUE;
    std::list<RefPtr<LayoutWrapper>> retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_TRUE);
    EXPECT_FALSE(retCachedList.empty());
    EXPECT_TRUE(firstChildLayoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest006
 * @tc.desc: Test the operation of layout_wrapper.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call GetAllChildrenWithBuild.
     * @tc.expected: the return value is the same as layoutWrapper->children_.
     */
    std::list<RefPtr<LayoutWrapper>> retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_FALSE);
    EXPECT_EQ(retCachedList, layoutWrapper->children_);
}

/**
 * @tc.name: LayoutWrapperTest007
 * @tc.desc: Test GetAllChildrenWithBuild TEST_FALSE.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step4. call GetAllChildrenWithBuild and set layoutWrapper->layoutWrapperBuilder_ is not null.
     * @tc.expected: the return value is not empty.
     */
    std::list<RefPtr<LayoutWrapper>> retCachedList = layoutWrapper->GetAllChildrenWithBuild(TEST_FALSE);
    EXPECT_FALSE(retCachedList.empty());
    EXPECT_FALSE(firstChildLayoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest008
 * @tc.desc: Test RemoveChildInRenderTree.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call RemoveChildInRenderTree and set input is null.
     * @tc.expected: layoutWrapper->isActive_ is false.
     */
    layoutWrapper->RemoveChildInRenderTree(nullptr);
    EXPECT_FALSE(layoutWrapper->isActive_);

    /**
     * @tc.steps: step3. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step4. call RemoveChildInRenderTree and set input wrapper is secondLayoutWrapper.
     * @tc.expected: firstChildLayoutWrapper->isActive_ is false.
     */
    layoutWrapper->RemoveChildInRenderTree(firstChildLayoutWrapper);
    EXPECT_FALSE(firstChildLayoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest009
 * @tc.desc: Test RemoveChildInRenderTree.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step3. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call RemoveChildInRenderTree and set input index is NODE_ID_1.
     * @tc.expected: layoutWrapper->isActive_ is false.
     */
    layoutWrapper->RemoveChildInRenderTree(NODE_ID_1);
    EXPECT_FALSE(firstChildLayoutWrapper->isActive_);

    /**
     * @tc.steps: step4. call RemoveChildInRenderTree and set input index is NODE_ID_0.
     * @tc.expected: the return layoutWrapper->isActive_ is true.
     */
    layoutWrapper->isActive_ = TEST_TRUE;
    layoutWrapper->RemoveChildInRenderTree(NODE_ID_0);
    EXPECT_TRUE(layoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest010
 * @tc.desc: Test RemoveAllChildInRenderTree.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call RemoveChildInRenderTree.
     * @tc.expected: layoutWrapper->isActive_ is false.
     */
    layoutWrapper->RemoveAllChildInRenderTree();
    EXPECT_FALSE(layoutWrapper->isActive_);

    /**
     * @tc.steps: step3. create two layoutWrapper and append them to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    RefPtr<LayoutWrapper> secondChildLayoutWrapper = CreateChildLayoutWrapper(SECOND_CHILD_FRAME_NODE, NODE_ID_2);

    firstChildLayoutWrapper->isActive_ = TEST_TRUE;
    secondChildLayoutWrapper->isActive_ = TEST_TRUE;

    layoutWrapper->AppendChild(firstChildLayoutWrapper);
    layoutWrapper->AppendChild(secondChildLayoutWrapper);

    /**
     * @tc.steps: step4. call RemoveAllChildInRenderTree.
     * @tc.expected: the firstChildLayoutWrapper->isActive_ and  secondChildLayoutWrapper->isActive_ are false.
     */
    layoutWrapper->RemoveAllChildInRenderTree();
    EXPECT_FALSE(firstChildLayoutWrapper->isActive_);
    EXPECT_FALSE(secondChildLayoutWrapper->isActive_);

    /**
     * @tc.steps: step5. create thirdChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> thirdChildLayoutWrapper = CreateChildLayoutWrapper(THIRD_CHILD_FRAME_NODE, NODE_ID_3);
    thirdChildLayoutWrapper->isActive_ = TEST_TRUE;
    layoutWrapper->AppendChild(thirdChildLayoutWrapper);

    /**
     * @tc.steps: step6. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step7. call RemoveChildInRenderTree.
     * @tc.expected: thirdChildLayoutWrapper->isActive_ is false.
     */
    layoutWrapper->RemoveAllChildInRenderTree();
    EXPECT_FALSE(thirdChildLayoutWrapper->isActive_);
}

/**
 * @tc.name: LayoutWrapperTest011
 * @tc.desc: Test ResetHostNode.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call ResetHostNode.
     * @tc.expected: layoutWrapper->hostNode_.refCounter_ is null.
     */
    layoutWrapper->ResetHostNode();
    EXPECT_EQ(layoutWrapper->hostNode_.refCounter_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest012
 * @tc.desc: Test GetHostNode.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest012, TestSize.Level1)
{
      /**
      * @tc.steps: step1. create layoutwrapper.
      */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
     * @tc.steps: step2. call GetHostNode.
     * @tc.expected: the return value is the same as rowFrameNode.
     */
    RefPtr<FrameNode> hostNode = layoutWrapper->GetHostNode();
    EXPECT_EQ(hostNode, rowFrameNode);

    /**
     * @tc.steps: step3. call GetWeakHostNode.
     * @tc.expected: the return value is the same as rowFrameNode.
     */
    WeakPtr<FrameNode> weakHostNode = layoutWrapper->GetWeakHostNode();
    EXPECT_EQ(weakHostNode, rowFrameNode);
}

/**
 * @tc.name: LayoutWrapperTest013
 * @tc.desc: Test GetHostTag.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest013, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create layoutwrapper.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
     * @tc.steps: step2. call GetHostTag.
     * @tc.expected: the return retHostTag is the same as ROW_FRAME_NODE.
     */
    std::string hostTag = layoutWrapper->GetHostTag();
    EXPECT_EQ(hostTag, ROW_FRAME_NODE);

    /**
     * @tc.steps: step3. call GetHostTag and set hostNode_ is null.
     * @tc.expected: the return value is the same as TEST_TAG.
     */
    layoutWrapper->hostNode_ = nullptr;
    hostTag = layoutWrapper->GetHostTag();
    EXPECT_EQ(hostTag, TEST_TAG);
}

/**
 * @tc.name: LayoutWrapperTest014
 * @tc.desc: Test GetHostDepth.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest014, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create layoutwrapper.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
     * @tc.steps: step2. call GetHostDepth and set hostNode_ is not null.
     * @tc.expected: the return value is equal to HOST_DEPTH.
     */
    int32_t hostDepth = layoutWrapper->GetHostDepth();
    EXPECT_EQ(hostDepth, HOST_DEPTH);

    /**
     * @tc.steps: step3. call GetHostDepth and set hostNode_ is null.
     * @tc.expected: the return value is equal to ERROR_HOST_DEPTH.
     */
    layoutWrapper->hostNode_ = nullptr;
    hostDepth = layoutWrapper->GetHostDepth();
    EXPECT_EQ(hostDepth, ERROR_HOST_DEPTH);
}

/**
 * @tc.name: LayoutWrapperTest015
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure and set layoutWrapper->layoutProperty_ is null.
     */
    layoutWrapper->layoutProperty_ = nullptr;
    layoutWrapper->Measure(parentLayoutConstraint);

    /**
    * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
    */
    FRAME_SIZE.width_= layoutWrapper->geometryNode_->GetFrameSize().Width();
    FRAME_SIZE.height_ = layoutWrapper->geometryNode_->GetFrameSize().Height();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest016
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure and set layoutWrapper->geometryNode_ is null.
     * @tc.expected: layoutWrapper->geometryNode_ is null.
     */
    layoutWrapper->geometryNode_ = nullptr;
    layoutWrapper->Measure(parentLayoutConstraint);
    EXPECT_EQ(layoutWrapper->geometryNode_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest017
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure and set layoutWrapper->hostNode_ is null.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->hostNode_ = nullptr;
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest018
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest018, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create layoutwrapper.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure and set layoutWrapper->layoutAlgorithm_ is null.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest019
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest019, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create layoutwrapper.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step4. call Measure and set layoutProperty_->geometryTransition_ is not null.
     * @tc.expected: FRAME_SIZE.width_ is RK356_WIDTH and FRAME_SIZE.height_ is ROW_HEIGHT.
     */
    layoutWrapper->GetLayoutProperty()->geometryTransition_ = AceType::MakeRefPtr<GeometryTransition>(rowFrameNode);
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE.width_, RK356_WIDTH);
    EXPECT_EQ(FRAME_SIZE.height_, ROW_HEIGHT);
}

/**
 * @tc.name: LayoutWrapperTest020
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest020, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create layoutwrapper.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure and set layoutAlgorithm_->skipMeasure_ is TEST_TRUE.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->layoutAlgorithm_->skipMeasure_ = TEST_TRUE;
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest021
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest021, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Measure.
     * @tc.expected: FRAME_SIZE.Width() is RK356_WIDTH and FRAME_SIZE.Height() is ROW_HEIGHT.
     */
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE.width_, RK356_WIDTH);
    EXPECT_EQ(FRAME_SIZE.height_, ROW_HEIGHT);

    /**
    * @tc.steps: step4. UpdateAspectRatio and UpdateLayoutWeight.
    */
    MagicItemProperty magicItemProperty;
    magicItemProperty.UpdateAspectRatio(0.5);
    magicItemProperty.UpdateLayoutWeight(0.5);

    layoutWrapper->layoutProperty_->magicItemProperty_ = std::make_unique<MagicItemProperty>(magicItemProperty);

    /**
     * @tc.steps: step5. call Measure.
     * @tc.expected: FRAME_SIZE.Height() is twice as much RK356_WIDTH.
     */
    layoutWrapper->Measure(parentLayoutConstraint);
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE.width_, RK356_WIDTH);
    EXPECT_EQ(FRAME_SIZE.height_, RK356_WIDTH * 2);

    /**
     * @tc.steps: step6. call Measure and set layoutProperty_->calcLayoutConstraint_is not null.
     * @tc.expected: SELF_IDEAL_SIZE.Height() is twice as much RK356_WIDTH.
     */
    layoutWrapper->layoutProperty_->calcLayoutConstraint_ = std::make_unique<MeasureProperty>();
    layoutWrapper->Measure(parentLayoutConstraint);
    IDEAL_SIZE.width_ = layoutWrapper->geometryNode_->parentLayoutConstraint_->selfIdealSize.Width();
    IDEAL_SIZE.height_ = layoutWrapper->geometryNode_->parentLayoutConstraint_->selfIdealSize.Height();
    EXPECT_EQ(IDEAL_SIZE.width_, RK356_WIDTH);
    EXPECT_EQ(IDEAL_SIZE.height_, RK356_WIDTH * 2);
}

/**
 * @tc.name: LayoutWrapperTest022
 * @tc.desc: Test Measure.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest022, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step4. call Measure and set layoutWrapper->needForceMeasureAndLayout_ is TEST_FALSE.
     * @tc.expected: layoutWrapper->skipMeasureContent_ is true.
     */
    layoutWrapper->needForceMeasureAndLayout_ = std::make_optional(TEST_FALSE);
    layoutWrapper->Measure(parentLayoutConstraint);
    EXPECT_TRUE(layoutWrapper->skipMeasureContent_);
}

/**
 * @tc.name: LayoutWrapperTest023
 * @tc.desc: Test the operation of layout_wrapper.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest023, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Layout and set layoutWrapper->layoutProperty_ is null.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->layoutProperty_ = nullptr;
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest024
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest024, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Layout and set layoutWrapper->geometryNode_ is null.
     * @tc.expected: FRAME_SIZE is the default value.
     */
    layoutWrapper->geometryNode_ = nullptr;
    layoutWrapper->Layout();
    EXPECT_EQ(layoutWrapper->geometryNode_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest025
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest025, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Layout and set layoutWrapper->hostNode_ is null.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->hostNode_ = nullptr;
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest026
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest026, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step3. call Layout and set layoutWrapper->layoutAlgorithm_ is not null.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest027
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest027, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step4. call Layout and set layoutWrapper->layoutAlgorithm_->skipLayout_ is TEST_TRUE.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->layoutAlgorithm_->skipLayout_ = TEST_TRUE;
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest028
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest028, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step4. call Layout and set layoutWrapper->skipMeasureContent_ is default value.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);

    /**
     * @tc.steps: step5. call Layout and set layoutWrapper->skipMeasureContent_ is TEST_TRUE.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->skipMeasureContent_ = TEST_TRUE;
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest029
 * @tc.desc: Test Layout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest029, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
    * @tc.steps: step3. call UpdateParentConstraint update parentLayoutConstraint.
    */
    LayoutConstraintF parentLayoutConstraint;
    UpdateParentConstraint(layoutWrapper, parentLayoutConstraint);

    /**
     * @tc.steps: step4. call Layout and set layoutWrapper->skipMeasureContent_ is default value.
     * @tc.expected: FRAME_SIZE is the same as TEST_FRAME_SIZE.
     */
    layoutWrapper->Layout();
    FRAME_SIZE = layoutWrapper->geometryNode_->GetFrameSize();
    EXPECT_EQ(FRAME_SIZE, TEST_FRAME_SIZE);
}

/**
 * @tc.name: LayoutWrapperTest030
 * @tc.desc: Test SkipMeasureContent.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest030, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
    * @tc.steps: step2. set layoutWrapper->layoutAlgorithm_ is not null.
    */
    auto rowLayoutPattern = rowFrameNode->GetPattern<LinearLayoutPattern>();
    auto rowLayoutAlgorithm = rowLayoutPattern->CreateLayoutAlgorithm();
    layoutWrapper->SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(rowLayoutAlgorithm));

    /**
     * @tc.steps: step3. call SkipMeasureContent.
     * @tc.expected: the return value is false.
     */
    bool retSkipMeasureContent = layoutWrapper->SkipMeasureContent();
    EXPECT_FALSE(retSkipMeasureContent);

    /**
     * @tc.steps: step4. call SkipMeasureContent and set layoutWrapper->skipMeasureContent_ is true.
     * @tc.expected: the return value is true.
     */
    layoutWrapper->skipMeasureContent_ = std::make_optional(TEST_TRUE);
    retSkipMeasureContent = layoutWrapper->SkipMeasureContent();
    EXPECT_TRUE(retSkipMeasureContent);
}

/**
 * @tc.name: LayoutWrapperTest031
 * @tc.desc: Test CheckNeedForceMeasureAndLayout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest031, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call CheckNeedForceMeasureAndLayout.
     * @tc.expected: the return retCheck is true.
     */
    bool retCheck = layoutWrapper->CheckNeedForceMeasureAndLayout();
    EXPECT_TRUE(retCheck);

    /**
     * @tc.steps: step3. call CheckNeedForceMeasureAndLayout again.
     * @tc.expected: the return retCheck is true.
     */
    retCheck = layoutWrapper->CheckNeedForceMeasureAndLayout();
    EXPECT_TRUE(retCheck);
}

/**
 * @tc.name: LayoutWrapperTest032
 * @tc.desc: Test CheckNeedForceMeasureAndLayout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest032, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call CheckNeedForceMeasureAndLayout again.
     * @tc.expected: the return retCheck is false.
     */
    layoutWrapper->layoutProperty_->propertyChangeFlag_ = PROPERTY_UPDATE_NORMAL;
    bool retCheck = layoutWrapper->CheckNeedForceMeasureAndLayout();
    EXPECT_FALSE(retCheck);
}

/**
 * @tc.name: LayoutWrapperTest033
 * @tc.desc: Test CheckNeedForceMeasureAndLayout.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest033, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    RefPtr<LayoutWrapper> firstChildLayoutWrapper = CreateChildLayoutWrapper(FIRST_CHILD_FRAME_NODE, NODE_ID_1);
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call CheckChildNeedForceMeasureAndLayout.
     * @tc.expected: the return retCheck is true.
     */
    bool retCheck = layoutWrapper->CheckChildNeedForceMeasureAndLayout();
    EXPECT_TRUE(retCheck);
}

/**
 * @tc.name: LayoutWrapperTest034
 * @tc.desc: Test MountToHostOnMainThread.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest034, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call MountToHostOnMainThread.
     * @tc.expected: layoutWrapper->layoutWrapperBuilder_ is null.
     */
    layoutWrapper->MountToHostOnMainThread();
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);

    /**
     * @tc.steps: step3. call MountToHostOnMainThread and set layoutWrapper->isActive_ is TEST_TRUE.
     * @tc.expected: layoutWrapper->layoutWrapperBuilder_ is null.
     */
    layoutWrapper->isActive_ = TEST_TRUE;
    layoutWrapper->MountToHostOnMainThread();
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);

    /**
    * @tc.steps: step4. create a testLayoutWrapper pointer and set tag is FIRST_CHILD_FRAME_NODE.
    */
    auto frameNode =
        FrameNode::CreateFrameNode(FIRST_CHILD_FRAME_NODE, NODE_ID_1, AceType::MakeRefPtr<Pattern>());
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> testLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    /**
     * @tc.steps: step5. call MountToHostOnMainThread.
     * @tc.expected: testLayoutWrapper->layoutWrapperBuilder is null.
     */
    testLayoutWrapper->MountToHostOnMainThread();
    EXPECT_EQ(testLayoutWrapper->layoutWrapperBuilder_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest035
 * @tc.desc: Test MountToHostOnMainThread.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest035, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    auto frameNode =
        FrameNode::CreateFrameNode(FIRST_CHILD_FRAME_NODE, NODE_ID_1, AceType::MakeRefPtr<Pattern>());
    RefPtr<LayoutWrapper> firstChildLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call MountToHostOnMainThread.
     * @tc.expected: layoutWrapper->layoutWrapperBuilder_ is null.
     */
    layoutWrapper->isActive_ = TEST_TRUE;
    layoutWrapper->MountToHostOnMainThread();
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest036
 * @tc.desc: Test MountToHostOnMainThread.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest036, TestSize.Level1)
{
    /**
    * @tc.steps: step1. create a layoutwrapper pointer.
    */
    auto rowFrameNode =
        FrameNode::CreateFrameNode(ROW_FRAME_NODE, NODE_ID_0, AceType::MakeRefPtr<LinearLayoutPattern>(false));
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(rowFrameNode, geometryNode, rowFrameNode->GetLayoutProperty());

    /**
     * @tc.steps: step2. create firstChildLayoutWrapper and append it to layoutWrapper.
     */
    auto frameNode =
        FrameNode::CreateFrameNode(FIRST_CHILD_FRAME_NODE, NODE_ID_1, AceType::MakeRefPtr<Pattern>());
    RefPtr<LayoutWrapper> firstChildLayoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());

    firstChildLayoutWrapper->hostNode_ = nullptr;
    layoutWrapper->AppendChild(firstChildLayoutWrapper);

    /**
     * @tc.steps: step3. call MountToHostOnMainThread.
     * @tc.expected: layoutWrapper->layoutWrapperBuilder_ is null.
     */
    layoutWrapper->MountToHostOnMainThread();
    EXPECT_EQ(layoutWrapper->layoutWrapperBuilder_, nullptr);

    /**
     * @tc.steps: step4. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step5. call MountToHostOnMainThread.
     * @tc.expected: layoutWrapper->layoutWrapperBuilder_ is not null.
     */
    layoutWrapper->isActive_ = TEST_TRUE;

    layoutWrapper->MountToHostOnMainThread();
    ASSERT_NE(layoutWrapper->layoutWrapperBuilder_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest037
 * @tc.desc: Test BuildLazyItem.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest037, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
    * @tc.steps: step2. set layoutWrapper->lazyBuildFunction_ is not null.
    */
    auto lazyBuildFunction = [] (RefPtr<LayoutWrapper> testLayoutWrapper) {
        testLayoutWrapper = nullptr;
    };
    layoutWrapper->lazyBuildFunction_ = lazyBuildFunction;

    /**
     * @tc.steps: step3. call GetLazyBuildRange.
     * @tc.expected: the return retRange.first is -1 and retRange.second is 0.
     */
    layoutWrapper->BuildLazyItem();
    EXPECT_EQ(layoutWrapper->lazyBuildFunction_, nullptr);
}

/**
 * @tc.name: LayoutWrapperTest038
 * @tc.desc: Test GetLazyBuildRange.
 * @tc.type: FUNC
 */
HWTEST_F(LayoutWrapperTestNg, LayoutWrapperTest038, TestSize.Level1)
{
    /**
     * @tc.steps: step1. call CreateLayoutWrapper create a layoutwrapper pointer.
     */
    RefPtr<LayoutWrapper> layoutWrapper = CreateLayoutWrapper(ROW_FRAME_NODE, NODE_ID_0);

    /**
     * @tc.steps: step2. call GetLazyBuildRange.
     * @tc.expected: the return value is the same as RANGE.
     */
    std::pair<int32_t, int32_t> retRange = layoutWrapper->GetLazyBuildRange();
    EXPECT_EQ(retRange, RANGE);

    /**
     * @tc.steps: step3. create wrapperBuilder and set it to layoutWrapper->layoutWrapperBuilder_.
     */
    RefPtr<LazyLayoutWrapperBuilder> wrapperBuilder = CreateLayoutWrapperBuilder();
    layoutWrapper->layoutWrapperBuilder_ = wrapperBuilder;

    /**
     * @tc.steps: step4. call GetLazyBuildRange.
     * @tc.expected: the return value is the same as RANGE_0.
     */
    retRange = layoutWrapper->GetLazyBuildRange();
    EXPECT_EQ(retRange, RANGE_0);
}
} // namespace OHOS::Ace::NG