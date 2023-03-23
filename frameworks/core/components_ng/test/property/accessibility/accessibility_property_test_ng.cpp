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

#include "gtest/gtest.h"

#define private public
#define protected public
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/event/long_press_event.h"
#include "core/components_ng/property/accessibility_property.h"
#include "core/components_ng/pattern/pattern.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
const size_t ARRAY_SIZE = 1;
} // namespace

class MockPattern : public Pattern {
public:
    MockPattern() = default;
    ~MockPattern() override = default;
};
class AccessibilityPropertyTestNg : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
};

/**
 * @tc.name: AccessibilityPropertyTest001
 * @tc.desc: Set show value into supportActions_ and get SupportAction length.
 * @tc.type: FUNC
 */
HWTEST_F(AccessibilityPropertyTestNg, AccessibilityPropertyTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. supportActions_ Use default values
     * @tc.expected: Make the array length returned by the GetSupportAction function 0
     */
    AccessibilityProperty props;
    auto actions = props.GetSupportAction();
    EXPECT_EQ(actions.size(), 0);

    /**
     * @tc.steps: step2. supportActions_ = AceAction::ACTION_SCROLL_FORWARD
     * @tc.expected: Make the array length returned by the GetSupportAction function 1
     */
    props.supportActions_ = static_cast<uint32_t>(AceAction::ACTION_SCROLL_FORWARD);
    actions = props.GetSupportAction();
    EXPECT_EQ(actions.size(), ARRAY_SIZE);
}

/**
 * @tc.name: AccessibilityPropertyTest002
 * @tc.desc: SetCommonSupportAction method test
 * @tc.type: FUNC
 */
HWTEST_F(AccessibilityPropertyTestNg, AccessibilityPropertyTest002, TestSize.Level1)
{
    auto* stack = ViewStackProcessor::GetInstance();
    RefPtr<MockPattern> pattern = AceType::MakeRefPtr<MockPattern>();
    RefPtr<FrameNode> frameNode = FrameNode::CreateFrameNode("mockPattern", 1, pattern);
    stack->Push(frameNode);
    AccessibilityProperty props;
    /**
     * @tc.steps: step1. host_ is nullptr
     * @tc.expected: supportActions_ = 0
     */
    props.SetCommonSupportAction();
    EXPECT_EQ(props.supportActions_, 0);

    /**
     * @tc.steps: step2. host_ is not nullptr and  gestureEventHub->IsClickable() = true
     * @tc.expected: supportActions_ = AceAction::ACTION_CLICK
     */
    frameNode->eventHub_ = AceType::MakeRefPtr<EventHub>();
    frameNode->eventHub_->GetOrCreateGestureEventHub();
    frameNode->eventHub_->GetGestureEventHub()->CheckClickActuator();
    props.host_ = AceType::WeakClaim(AceType::RawPtr(frameNode));
    props.SetCommonSupportAction();
    EXPECT_EQ(props.supportActions_, 1UL << static_cast<uint32_t>(AceAction::ACTION_CLICK));

    props.supportActions_ = 0;
    frameNode->eventHub_->GetGestureEventHub()->clickEventActuator_ = nullptr;
    GestureEventFunc eventFunc;
    eventFunc = [](GestureEvent& info) {};
    auto longPressEvent = AceType::MakeRefPtr<LongPressEvent>(std::move(eventFunc));
    frameNode->eventHub_->GetGestureEventHub()->SetLongPressEvent(longPressEvent);
    props.SetCommonSupportAction();
    EXPECT_EQ(props.supportActions_, 1UL << static_cast<uint32_t>(AceAction::ACTION_LONG_CLICK));
    props.supportActions_ = 0;
    frameNode->eventHub_->GetGestureEventHub()->longPressEventActuator_ = nullptr;
    frameNode->eventHub_->GetOrCreateFocusHub();
    props.SetCommonSupportAction();
    EXPECT_FALSE(frameNode->GetFocusHub()->IsFocusable());
}
} // namespace OHOS::Ace::NG