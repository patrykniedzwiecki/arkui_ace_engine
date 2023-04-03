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
#include "gtest/gtest.h"

// Add the following two macro definitions to test the private and protected method.
#define private public
#define protected public
#include "base/geometry/ng/size_t.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/checkbox/checkbox_model_ng.h"
#include "core/components_ng/pattern/checkbox/checkbox_paint_property.h"
#include "core/components_ng/pattern/checkbox/checkbox_pattern.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_model_ng.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_paint_method.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_paint_property.h"
#include "core/components_ng/test/mock/rosen/mock_canvas.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_pattern.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string NAME = "checkbox";
const std::string GROUP_NAME = "checkboxGroup";
const std::string GROUP_NAME_CHANGE = "checkboxGroupChange";
const std::string TAG = "CHECKBOX_TAG";
const Dimension WIDTH = 50.0_vp;
const Dimension HEIGHT = 50.0_vp;
const NG::PaddingPropertyF PADDING = NG::PaddingPropertyF();
const bool SELECTED = true;
const Color SELECTED_COLOR = Color::BLUE;
const SizeF CONTENT_SIZE = SizeF(400.0, 500.0);
const OffsetF CONTENT_OFFSET = OffsetF(50.0, 60.0);
constexpr float SHAPE_SCALE_SMALL = 0.3f;
constexpr float SHAPE_SCALE_HALF = 0.5f;
constexpr float SHAPE_SCALE = 1.0f;
constexpr float COMPONENT_WIDTH = 200.0;
constexpr float COMPONENT_HEIGHT = 210.0;
constexpr float BORDER_RADIUS = 100.0;
constexpr float CHECK_STROKE = 200.0;
constexpr float CHECK_MARK_SIZEF = 50.0;
constexpr float CHECK_MARK_WIDTHF = 5.0;
constexpr float CHECKMARK_PAINTSIZE = 400.0;
constexpr double BORDER_WIDTH = 300.0;
constexpr Dimension CHECK_MARK_SIZE = Dimension(CHECK_MARK_SIZEF);
constexpr Dimension NEGATIVE_CHECK_MARK_SIZE = Dimension(-CHECK_MARK_SIZEF);
constexpr Dimension CHECK_MARK_WIDTH = Dimension(CHECK_MARK_WIDTHF);
const Color POINT_COLOR = Color::BLUE;
const Color ACTIVE_COLOR = Color::BLACK;
const Color INACTIVE_COLOR = Color::GREEN;
const Color SHADOW_COLOR = Color::RED;
const Color CLICK_EFFECT_COLOR = Color::WHITE;
const Color HOVER_COLOR = Color::GRAY;
const Color INACTIVE_POINT_COLOR = Color::TRANSPARENT;
constexpr Dimension HOVER_RADIUS = Dimension(3.0);
constexpr Dimension HORIZONTAL_PADDING = Dimension(5.0);
constexpr Dimension VERTICAL_PADDING = Dimension(4.0);
constexpr Dimension SHADOW_WIDTH_FORUPDATE = Dimension(6.0);
} // namespace

class CheckBoxGroupTestNG : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CheckBoxGroupTestNG::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}
void CheckBoxGroupTestNG::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}
void CheckBoxGroupTestNG::SetUp() {}
void CheckBoxGroupTestNG::TearDown() {}
CheckBoxGroupModifier::Parameters CheckBoxGroupCreateDefModifierParam()
{
    CheckBoxGroupModifier::Parameters parameters = {BORDER_WIDTH, BORDER_RADIUS, CHECK_STROKE, CHECKMARK_PAINTSIZE,
        POINT_COLOR, ACTIVE_COLOR, INACTIVE_COLOR, SHADOW_COLOR, CLICK_EFFECT_COLOR, HOVER_COLOR, INACTIVE_POINT_COLOR,
        HOVER_RADIUS, HORIZONTAL_PADDING, VERTICAL_PADDING, SHADOW_WIDTH_FORUPDATE, UIStatus::UNSELECTED,
        CheckBoxGroupPaintProperty::SelectStatus::NONE};
    
    return parameters;
}

/**
 * @tc.name: CheckBoxGroupPaintPropertyTest001
 * @tc.desc: Set CheckBoxGroup value into CheckBoxGroupPaintProperty and get it.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintPropertyTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(std::optional<string>());

    /**
     * @tc.steps: step2. Set parameters to CheckBoxGroup property
     */
    checkBoxGroupModelNG.SetSelectAll(SELECTED);
    checkBoxGroupModelNG.SetSelectedColor(SELECTED_COLOR);
    checkBoxGroupModelNG.SetWidth(WIDTH);
    checkBoxGroupModelNG.SetHeight(HEIGHT);
    checkBoxGroupModelNG.SetPadding(PADDING);

    /**
     * @tc.steps: step3. Get paint property and get CheckBoxGroup property
     * @tc.expected: step3. Check the CheckBoxGroup property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxGroupEventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->SetGroupName(GROUP_NAME);
    EXPECT_EQ(eventHub->GetGroupName(), GROUP_NAME);
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupSelect(), SELECTED);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupSelectedColor(), SELECTED_COLOR);
}

/**
 * @tc.name: CheckBoxGroupPaintPropertyTest002
 * @tc.desc: Verify ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintPropertyTest002, TestSize.Level1)
{
    // create mock theme manager
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto checkboxTheme = AceType::MakeRefPtr<CheckboxTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(checkboxTheme));
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(std::optional<string>());

    /**
     * @tc.steps: step2. Set parameters to CheckBoxGroup property
     */
    checkBoxGroupModelNG.SetSelectAll(SELECTED);
    checkBoxGroupModelNG.SetSelectedColor(SELECTED_COLOR);
    checkBoxGroupModelNG.SetWidth(WIDTH);
    checkBoxGroupModelNG.SetHeight(HEIGHT);
    checkBoxGroupModelNG.SetPadding(PADDING);

    /**
     * @tc.steps: step3. Get paint property and get CheckBoxGroup property
     * @tc.expected: step3. Check the CheckBoxGroup property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxGroupEventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->SetGroupName(GROUP_NAME);
    EXPECT_EQ(eventHub->GetGroupName(), GROUP_NAME);
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupSelect(), SELECTED);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupSelectedColor(), SELECTED_COLOR);
    auto json = JsonUtil::Create(true);
    checkBoxPaintProperty->ToJsonValue(json);
    EXPECT_EQ(json->GetString("selectAll"), "true");
}

/**
 * @tc.name: CheckBoxGroupEventTest002
 * @tc.desc: Test CheckBoxGroup onChange event.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupEventTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG checkBoxGroup;
    checkBoxGroup.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Init change result and onChange function
     */
    std::vector<std::string> vec;
    int status = 0;
    CheckboxGroupResult groupResult(
        std::vector<std::string> { NAME }, int(CheckBoxGroupPaintProperty::SelectStatus::ALL));
    auto onChange = [&vec, &status](const BaseEventInfo* groupResult) {
        const auto* eventInfo = TypeInfoHelper::DynamicCast<CheckboxGroupResult>(groupResult);
        vec = eventInfo->GetNameList();
        status = eventInfo->GetStatus();
    };

    /**
     * @tc.steps: step3. Get event hub and call UpdateChangeEvent function
     * @tc.expected: step3. Check the event result value
     */
    checkBoxGroup.SetOnChange(onChange);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxGroupEventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->UpdateChangeEvent(&groupResult);
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(vec.front(), NAME);
    EXPECT_EQ(status, int(CheckBoxGroupPaintProperty::SelectStatus::ALL));
}

/**
 * @tc.name: CheckBoxGroupPatternTest003
 * @tc.desc: Test CheckBoxGroup onModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Test CheckBoxGroup onModifyDone method
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->SetPreGroup(GROUP_NAME);
    frameNode->MarkModifyDone();
    pattern->SetPreGroup(GROUP_NAME_CHANGE);
    frameNode->MarkModifyDone();
}

/**
 * @tc.name: CheckBoxGroupMeasureTest004
 * @tc.desc: Test CheckBoxGroup Measure.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupMeasureTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Create LayoutWrapper and set CheckBoxGroupLayoutAlgorithm.
     */
    // Create LayoutWrapper and set CheckBoxGroupLayoutAlgorithm.
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, frameNode->GetLayoutProperty());
    auto checkBoxGroupPattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(checkBoxGroupPattern, nullptr);
    auto checkBoxGroupLayoutAlgorithm = checkBoxGroupPattern->CreateLayoutAlgorithm();
    ASSERT_NE(checkBoxGroupLayoutAlgorithm, nullptr);
    layoutWrapper.SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(checkBoxGroupLayoutAlgorithm));

    /**
     * @tc.steps: step3. Test CheckBoxGroup Measure method
     * @tc.expected: step3. Check the CheckBoxGroup frame size and frame offset value
     */
    LayoutConstraintF layoutConstraintSize;
    layoutConstraintSize.selfIdealSize.SetSize(SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
    layoutWrapper.GetLayoutProperty()->UpdateLayoutConstraint(layoutConstraintSize);
    layoutWrapper.GetLayoutProperty()->UpdateContentConstraint();
    checkBoxGroupLayoutAlgorithm->Measure(&layoutWrapper);
    // Test the size set by codes.
    EXPECT_EQ(layoutWrapper.GetGeometryNode()->GetFrameSize(), SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
}

/**
 * @tc.name: CheckBoxGroupPatternTest005
 * @tc.desc: Test CheckBoxGroup pattern method OnTouchUp.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup pattern variable and call OnTouchUp
     * @tc.expected: step3. Check the CheckBoxGroup pattern value
     */
    pattern->isTouch_ = true;
    pattern->OnTouchUp();
    EXPECT_EQ(pattern->isTouch_, false);
}

/**
 * @tc.name: CheckBoxGroupPatternTest006
 * @tc.desc: Test CheckBoxGroup pattern method OnTouchDown.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup pattern variable and call OnTouchDown
     * @tc.expected: step3. Check the CheckBoxGroup pattern value
     */
    pattern->isTouch_ = false;
    pattern->OnTouchDown();
    EXPECT_EQ(pattern->isTouch_, true);
}

/**
 * @tc.name: CheckBoxGroupPatternTest007
 * @tc.desc: Test CheckBoxGroup pattern method OnClick.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup paint property variable and call OnClick
     * @tc.expected: step3. Check the CheckBoxGroup paint property value
     */
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::PART);
    pattern->OnClick();
    auto select1 = checkBoxPaintProperty->GetCheckBoxGroupSelect();
    EXPECT_TRUE(select1.has_value());
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::ALL);
    pattern->OnClick();
    auto select2 = checkBoxPaintProperty->GetCheckBoxGroupSelect();
    EXPECT_TRUE(select2.has_value());
}

/**
 * @tc.name: CheckBoxGroupPatternTest008
 * @tc.desc: Test CheckBoxGroup pattern method HandleMouseEvent.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup pattern variable and call HandleMouseEvent
     * @tc.expected: step3. Check the CheckBoxGroup pattern value
     */
    pattern->isTouch_ = true;
    pattern->isHover_ = false;
    pattern->HandleMouseEvent(true);
    EXPECT_EQ(pattern->isHover_, true);
    EXPECT_EQ(pattern->isTouch_, true);
}

/**
 * @tc.name: CheckBoxGroupPatternTest009
 * @tc.desc: Test CheckBoxGroup pattern Init methods.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup pattern variable and call Init methods
     * @tc.expected: step3. Check the CheckBoxGroup pattern value
     */
    // InitMouseEvent()
    pattern->InitMouseEvent();
    ASSERT_NE(pattern->mouseEvent_, nullptr);
    pattern->InitMouseEvent();
    pattern->mouseEvent_->GetOnHoverEventFunc()(true);
    // InitTouchEvent()
    pattern->InitTouchEvent();
    ASSERT_NE(pattern->touchListener_, nullptr);
    pattern->InitTouchEvent();
    TouchEventInfo info("onTouch");
    TouchLocationInfo touchInfo1(1);
    touchInfo1.SetTouchType(TouchType::DOWN);
    info.AddTouchLocationInfo(std::move(touchInfo1));
    pattern->touchListener_->GetTouchEventCallback()(info);
    TouchLocationInfo touchInfo2(2);
    touchInfo2.SetTouchType(TouchType::UP);
    info.AddTouchLocationInfo(std::move(touchInfo2));
    pattern->touchListener_->GetTouchEventCallback()(info);
    TouchLocationInfo touchInfo3(3);
    touchInfo2.SetTouchType(TouchType::CANCEL);
    info.AddTouchLocationInfo(std::move(touchInfo3));
    pattern->touchListener_->GetTouchEventCallback()(info);
    // InitClickEvent()
    pattern->InitClickEvent();
    ASSERT_NE(pattern->clickListener_, nullptr);
    pattern->InitClickEvent();
}

/**
 * @tc.name: CheckBoxGroupPatternTest010
 * @tc.desc: Test CheckBoxGroup pattern Update methods.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG CheckBoxGroupModelNG;
    CheckBoxGroupModelNG.Create(GROUP_NAME);

    /**
     * @tc.steps: step2. Get CheckBoxGroup pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBoxGroup pattern variable and call Init methods
     * @tc.expected: step3. Check the CheckBoxGroup pattern value
     */
    // UpdateCheckBoxShape(float value)
    pattern->UpdateCheckBoxShape(-1);
    pattern->UpdateCheckBoxShape(2);
    pattern->UpdateCheckBoxShape(0.5);
    EXPECT_EQ(pattern->shapeScale_, 0.5);
    // UpdateUIStatus(bool check)
    pattern->uiStatus_ = UIStatus::ON_TO_OFF;
    pattern->UpdateUIStatus(true);
    EXPECT_EQ(pattern->uiStatus_, UIStatus::OFF_TO_ON);
    // UpdateUnSelect()
    pattern->uiStatus_ = UIStatus::ON_TO_OFF;
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::ALL);
    pattern->UpdateUnSelect();
    EXPECT_EQ(pattern->uiStatus_, UIStatus::ON_TO_OFF);
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::NONE);
    pattern->UpdateUnSelect();
    EXPECT_EQ(pattern->uiStatus_, UIStatus::UNSELECTED);
}

/**
 * @tc.name: CheckBoxGroupPatternTest011
 * @tc.desc: Test CheckBoxGroup onModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest011, TestSize.Level1)
{
    // create mock theme manager
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto checkboxTheme = AceType::MakeRefPtr<CheckboxTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(checkboxTheme));
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(GROUP_NAME);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->SetPreGroup(GROUP_NAME);
    frameNode->MarkModifyDone();
    pattern->SetPreGroup(GROUP_NAME_CHANGE);
    frameNode->MarkModifyDone();
    ASSERT_NE(pattern, nullptr);
}

/**
 * @tc.name: CheckBoxGroupPatternTest012
 * @tc.desc: Test UpdateCheckBoxStatus.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest012, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG1;
    checkBoxModelNG1.Create(NAME, GROUP_NAME, TAG);
    auto frameNode1 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode1, nullptr);
    CheckBoxModelNG checkBoxModelNG2;
    checkBoxModelNG2.Create(NAME, GROUP_NAME, TAG);
    auto frameNode2 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode2, nullptr);
    CheckBoxModelNG checkBoxModelNG3;
    checkBoxModelNG3.Create(NAME, GROUP_NAME, TAG);
    auto frameNode3 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode3, nullptr);
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(GROUP_NAME);
    auto groupFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(groupFrameNode, nullptr);
    auto pattern = groupFrameNode->GetPattern<CheckBoxGroupPattern>();
    EXPECT_NE(pattern, nullptr);

    std::unordered_map<std::string, std::list<WeakPtr<FrameNode>>> checkBoxGroupMap;
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode1);
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode2);
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode3);
    checkBoxGroupMap[GROUP_NAME].push_back(groupFrameNode);
    bool isSelected = true;
    pattern->UpdateCheckBoxStatus(groupFrameNode, checkBoxGroupMap, GROUP_NAME, isSelected);
    auto checkBoxPaintProperty1 = frameNode1->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty1, nullptr);
    checkBoxPaintProperty1->UpdateCheckBoxSelect(true);
    auto checkBoxPaintProperty2 = frameNode2->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty2, nullptr);
    checkBoxPaintProperty2->UpdateCheckBoxSelect(false);
    pattern->UpdateCheckBoxStatus(groupFrameNode, checkBoxGroupMap, GROUP_NAME, isSelected);
    isSelected = false;
    pattern->UpdateCheckBoxStatus(groupFrameNode, checkBoxGroupMap, GROUP_NAME, isSelected);
    checkBoxPaintProperty2->UpdateCheckBoxSelect(true);
    auto checkBoxPaintProperty3 = frameNode3->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty3, nullptr);
    checkBoxPaintProperty3->UpdateCheckBoxSelect(true);
    isSelected = true;
    pattern->UpdateCheckBoxStatus(groupFrameNode, checkBoxGroupMap, GROUP_NAME, isSelected);
}

/**
 * @tc.name: CheckBoxGroupPatternTest013
 * @tc.desc: Test UpdateRepeatedGroupStatus.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest013, TestSize.Level1)
{
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(GROUP_NAME);
    auto groupFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(groupFrameNode, nullptr);
    auto pattern = groupFrameNode->GetPattern<CheckBoxGroupPattern>();
    EXPECT_NE(pattern, nullptr);
    std::unordered_map<std::string, std::list<WeakPtr<FrameNode>>> checkBoxGroupMap;
    checkBoxGroupMap[GROUP_NAME].push_back(groupFrameNode);
    bool isSelected = true;
    pattern->UpdateRepeatedGroupStatus(groupFrameNode, isSelected);
    auto paintProperty = groupFrameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    EXPECT_NE(paintProperty, nullptr);
    EXPECT_EQ(paintProperty->GetSelectStatus(), CheckBoxGroupPaintProperty::SelectStatus::ALL);
    isSelected = false;
    pattern->UpdateRepeatedGroupStatus(groupFrameNode, isSelected);
    EXPECT_EQ(paintProperty->GetSelectStatus(), CheckBoxGroupPaintProperty::SelectStatus::NONE);
}

/**
 * @tc.name: CheckBoxGroupPatternTest014
 * @tc.desc: Test GetInnerFocusPaintRect.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxPatternTest014, TestSize.Level1)
{
    CheckBoxGroupModelNG checkBoxModelNG;
    checkBoxModelNG.Create(GROUP_NAME);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    EXPECT_NE(pattern, nullptr);
    RefPtr<EventHub> eventHub = AccessibilityManager::MakeRefPtr<EventHub>();
    RefPtr<FocusHub> focusHub = AccessibilityManager::MakeRefPtr<FocusHub>(eventHub, FocusType::DISABLE, false);
    pattern->InitOnKeyEvent(focusHub);
    RoundRect paintRect;
    pattern->GetInnerFocusPaintRect(paintRect);
}

/**
 * @tc.name: CheckBoxGroupPatternTest015
 * @tc.desc: Test UpdateAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest015, TestSize.Level1)
{
    CheckBoxGroupModelNG checkBoxModelNG;
    checkBoxModelNG.Create(GROUP_NAME);
    checkBoxModelNG.SetSelectAll(false);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);
    checkBoxModelNG.SetSelectAll(true);
    pattern->UpdateAnimation(true);
    ASSERT_NE(pattern->controller_, nullptr);
    pattern->UpdateAnimation(true);
    pattern->controller_->NotifyStopListener();
    pattern->translate_->NotifyListener(1);
}

/**
 * @tc.name: CheckBoxGroupPatternTest016
 * @tc.desc: Test UpdateAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest016, TestSize.Level1)
{
    CheckBoxGroupModelNG checkBoxModelNG;
    checkBoxModelNG.Create(GROUP_NAME);
    checkBoxModelNG.SetSelectAll(true);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);
    checkBoxModelNG.SetSelectAll(false);
    pattern->UpdateAnimation(false);
    ASSERT_NE(pattern->controller_, nullptr);
    pattern->UpdateAnimation(false);
}

/**
 * @tc.name: CheckBoxGroupPatternTest017
 * @tc.desc: Test SetUnSelectedColor, SetCheckMarkColor, SetCheckMarkSize, SetCheckMarkWidth.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPatternTest017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBoxGroup node
     */
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(std::optional<string>());

    /**
     * @tc.steps: step2. Set parameters to CheckBoxGroup property
     */
    Color unselected = Color::BLACK;

    checkBoxGroupModelNG.SetUnSelectedColor(unselected);
    checkBoxGroupModelNG.SetCheckMarkColor(unselected);
    checkBoxGroupModelNG.SetCheckMarkSize(CHECK_MARK_SIZE);
    checkBoxGroupModelNG.SetCheckMarkWidth(CHECK_MARK_WIDTH);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxGroupEventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->SetGroupName(GROUP_NAME);
    EXPECT_EQ(eventHub->GetGroupName(), GROUP_NAME);
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupUnSelectedColor(), unselected);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupCheckMarkColor(), unselected);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupCheckMarkSize(), CHECK_MARK_SIZE);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxGroupCheckMarkWidth(), CHECK_MARK_WIDTH);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest001
 * @tc.desc: Test CheckBoxGroup Modifier DrawTouchBoard.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest001, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    /**
     *  @tc.case: case. When isTouch is true, CheckBoxGroupModifier will call DrawTouchBoard.
     */
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, true, false, 0.0, UIStatus::FOCUS, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    SizeF size;
    OffsetF offset;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawRoundRect(_)).Times(1);
    checkBoxGroupModifier_->DrawTouchBoard(canvas, size, offset);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest002
 * @tc.desc: Test CheckBoxGroup Modifier DrawHoverBoard.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest002, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    /**
     * @tc.case: case. When isHover is true, CheckBoxGroupModifier's PaintCheckBox will call DrawHoverBoard.
     */
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, false, true, 0.0, UIStatus::FOCUS, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    SizeF size;
    OffsetF offset;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawRoundRect(_)).Times(1);
    checkBoxGroupModifier_->DrawHoverBoard(canvas, size, offset);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest003
 * @tc.desc: Test CheckBoxGroup Modifier DrawAnimationOffToOn.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest003, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    /**
     * @tc.case: case. When uiStatus_ == UIStatus::OFF_TO_ON, CheckBoxGroupModifier's  will call
     * DrawAnimationOffToOn.
     */
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(true, false, false, 0.0, UIStatus::OFF_TO_ON, checkBoxGroupModifier_);
    Testing::MockCanvas canvas;
    OffsetF offset;
    RSPen pen;
    SizeF size;
    /**
     * @tc.case: case1. shapeScale_ < CHECK_MARK_LEFT_ANIMATION_PERCENT. and shapeScale_ !=
     * DEFAULT_MAX_CHECKBOX_SHAPE_SCALE.
     */
    checkBoxPaintMethod.shapeScale_ = SHAPE_SCALE_SMALL;
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawPath(_)).Times(2);
    checkBoxGroupModifier_->DrawAnimationOffToOn(canvas, offset, pen, size);
    /**
     * @tc.case: case2. shapeScale_ == DEFAULT_MAX_CHECKBOX_SHAPE_SCALE. and shapeScale_ !=
     * DEFAULT_MAX_CHECKBOX_SHAPE_SCALE.
     */
    checkBoxPaintMethod.shapeScale_ = SHAPE_SCALE;
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawPath(_)).Times(4);
    checkBoxGroupModifier_->DrawAnimationOffToOn(canvas, offset, pen, size);
    /**
     * @tc.case: case3. shapeScale_ > CHECK_MARK_LEFT_ANIMATION_PERCENT.
     */
    checkBoxPaintMethod.shapeScale_ = SHAPE_SCALE_HALF;
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawPath(_)).Times(4);
    checkBoxGroupModifier_->DrawAnimationOffToOn(canvas, offset, pen, size);
}


/**
 * @tc.name: CheckBoxGroupPaintMethodTest004
 * @tc.desc: Test CheckBoxGroup Modifier paintCheckBox when enabled_ == false..
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest004, TestSize.Level1)
{
    /**
     * @tc.case: case1. When uiStatus_ == UIStatus::OFF_TO_ON and enabled_ == false.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, true, true, 0.0, UIStatus::OFF_TO_ON, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest005
 * @tc.desc: Test CheckBoxGroup Modifier will paintCheckBox when UIStatus is ON_TO_OFF.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest005, TestSize.Level1)
{
    /**
     * @tc.case: case1. When uiStatus_ == UIStatus::ON_TO_OFF and enabled_ == true.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(true, false, false, SHAPE_SCALE,
        UIStatus::ON_TO_OFF, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest006
 * @tc.desc: Test CheckBoxGroup  Modifier will paintCheckBox when UIStatus is UNSELECTED.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest006, TestSize.Level1)
{
    /**
     * @tc.case: case1. When uiStatus_ == UIStatus::UNSELECTED and enabled_ == true.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(true, false, false, 0.0, UIStatus::UNSELECTED, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest007
 * @tc.desc: Test CheckBoxGroup Modifier PaintCheckBox PaintCheckBoxGroupPartStatus.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest007, TestSize.Level1)
{
    /**
     * @tc.case: case1. When status == CheckBoxGroupPaintProperty::SelectStatus::PART and enabled_ == true.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::PART);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(true, false, false, 0.0, UIStatus::UNSELECTED, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest008
 * @tc.desc: Test CheckBoxGroup Modifier  paintCheckBox when UIStatus is PART_TO_OFF.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest008, TestSize.Level1)
{
    /**
     * @tc.case: case1. When uiStatus_ == UIStatus::PART_TO_OFF and enabled_ == false.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, false, false, 0.0,
        UIStatus::PART_TO_OFF, checkBoxGroupModifier_);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest009
 * @tc.desc: Test checkBoxPaintMethod  UpdateContentModifier.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest009, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(COMPONENT_WIDTH, COMPONENT_WIDTH));
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, false, true, 0.0, UIStatus::FOCUS, checkBoxGroupModifier_);
    checkBoxPaintProperty->UpdateCheckBoxGroupSelectedColor(ACTIVE_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupUnSelectedColor(INACTIVE_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkColor(POINT_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkSize(NEGATIVE_CHECK_MARK_SIZE);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkWidth(CHECK_MARK_WIDTH);
    /**
      * @tc.expected: step1. Check the CheckBoxGroup property value.
      */
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    EXPECT_EQ(checkBoxGroupModifier_->activeColor_->Get(), LinearColor(ACTIVE_COLOR));
    EXPECT_EQ(checkBoxGroupModifier_->inactiveColor_->Get(), LinearColor(INACTIVE_COLOR));
    EXPECT_EQ(checkBoxGroupModifier_->pointColor_->Get(), LinearColor(POINT_COLOR));
    EXPECT_EQ(checkBoxGroupModifier_->checkMarkPaintSize_->Get(), COMPONENT_WIDTH);
    EXPECT_EQ(checkBoxGroupModifier_->checkStroke_->Get(), CHECK_MARK_WIDTHF);
    /**
      * @tc.expected: step2.Test GetCheckBoxGroupCheckMarkSizeValue().ConvertToPx() >= 0.
      */
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkSize(CHECK_MARK_SIZE);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    EXPECT_EQ(checkBoxGroupModifier_->checkMarkPaintSize_->Get(), CHECK_MARK_SIZEF);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest010
 * @tc.desc: Test CheckBoxGroupPattern UpdateModifierParam.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest010, TestSize.Level1)
{
    const std::optional<std::string> groupName;
    CheckBoxGroupModelNG checkboxGroupModel;
    checkboxGroupModel.Create(groupName);
    auto groupFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(groupFrameNode, nullptr);
    auto checkBoxPaintProperty = groupFrameNode->GetPaintProperty<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    checkBoxPaintProperty->UpdateCheckBoxGroupSelectedColor(ACTIVE_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupUnSelectedColor(INACTIVE_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkColor(POINT_COLOR);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkSize(CHECK_MARK_SIZE);
    checkBoxPaintProperty->UpdateCheckBoxGroupCheckMarkWidth(CHECK_MARK_WIDTH);
    auto pattern = groupFrameNode->GetPattern<CheckBoxGroupPattern>();
    ASSERT_NE(pattern, nullptr);
    auto paintMethod = pattern->CreateNodePaintMethod();
    ASSERT_NE(paintMethod, nullptr);
    auto modifier_ = pattern->checkBoxGroupModifier_;
    ASSERT_NE(modifier_, nullptr);
    EXPECT_EQ(modifier_->activeColor_->Get(), LinearColor(ACTIVE_COLOR));
    EXPECT_EQ(modifier_->inactiveColor_->Get(), LinearColor(INACTIVE_COLOR));
    EXPECT_EQ(modifier_->pointColor_->Get(), LinearColor(POINT_COLOR));
    EXPECT_EQ(modifier_->checkMarkPaintSize_->Get(), CHECK_MARK_SIZEF);
    EXPECT_EQ(modifier_->checkStroke_->Get(), CHECK_MARK_WIDTHF);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest011
 * @tc.desc:  Test CheckBoxGroupModifier  paintCheckBox when enabled_ == false.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest011, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, false, false, 0.0, UIStatus::ON_TO_OFF, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}

/**
 * @tc.name: CheckBoxGroupPaintMethodTest012
 * @tc.desc:  Test CheckBoxGroupModifier PaintCheckBox when enabled_ == false.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxGroupTestNG, CheckBoxGroupPaintMethodTest012, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxGroupPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    checkBoxPaintProperty->SetSelectStatus(CheckBoxGroupPaintProperty::SelectStatus::PART);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    CheckBoxGroupModifier::Parameters parameters = CheckBoxGroupCreateDefModifierParam();
    auto checkBoxGroupModifier_= AceType::MakeRefPtr<CheckBoxGroupModifier>(parameters);
    CheckBoxGroupPaintMethod checkBoxPaintMethod(false, false, false, 0.0,
        UIStatus::UNSELECTED, checkBoxGroupModifier_);
    auto modifier_ = checkBoxPaintMethod.GetContentModifier(&paintWrapper);
    ASSERT_NE(modifier_, nullptr);
    checkBoxPaintMethod.UpdateContentModifier(&paintWrapper);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    DrawingContext context {canvas, COMPONENT_WIDTH, COMPONENT_HEIGHT};
    checkBoxGroupModifier_->PaintCheckBox(context, CONTENT_OFFSET, CONTENT_SIZE);
}
} // namespace OHOS::Ace::NG