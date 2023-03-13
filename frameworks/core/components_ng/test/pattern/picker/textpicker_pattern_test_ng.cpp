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

#include <functional>
#include <optional>
#include <securec.h>

#include "gtest/gtest.h"

#define private public
#define protected public
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_dialog_view.h"
#include "core/components_ng/pattern/text_picker/textpicker_model_ng.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_column_pattern.h"
#include "core/components_ng/test/mock/pattern/picker/mock_picker_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr size_t THREE_CHILDREN = 3;
constexpr size_t FIVE_CHILDREN = 5;
constexpr size_t THREE = 3;
constexpr uint32_t SELECTED_INDEX_1 = 1;
constexpr uint32_t SELECTED_INDEX_2 = 2;
constexpr double HALF = 0.5;
constexpr double FONT_SIZE_5 = 5.0;
constexpr double FONT_SIZE_10 = 10.0;
constexpr double FONT_SIZE_20 = 20.0;
constexpr double FONT_SIZE_INVALID = -1.0;
} // namespace

class TextPickerPatternTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

class TestNode : public UINode {
    DECLARE_ACE_TYPE(TestNode, UINode);

public:
    static RefPtr<TestNode> CreateTestNode(int32_t nodeId)
    {
        auto spanNode = MakeRefPtr<TestNode>(nodeId);
        return spanNode;
    }

    bool IsAtomicNode() const override
    {
        return true;
    }

    explicit TestNode(int32_t nodeId) : UINode("TestNode", nodeId) {}
    ~TestNode() override = default;
};

void TextPickerPatternTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
    auto themeManager = AceType::MakeRefPtr<MockPickerThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    PipelineContext::GetCurrentContext()->SetThemeManager(themeManager);
}

void TextPickerPatternTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
    PipelineContext::GetCurrentContext()->SetThemeManager(nullptr);
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions001
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(kind:TEXT).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("1", content);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollUp001
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, move up).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollUp001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_2);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = 0 - jump * HALF;
    double offset2 = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("1", content);
    ASSERT_TRUE(textLayoutProperty->HasFontSize());
    double fontSize = textLayoutProperty->GetFontSize().value().Value();
    double expectFontSize = FONT_SIZE_5 * HALF + FONT_SIZE_5 * HALF * HALF;
    EXPECT_EQ(expectFontSize, fontSize);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollDown001
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, move down).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollDown001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_2);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = jump * HALF;
    double offset2 = jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("5", content);
    ASSERT_TRUE(textLayoutProperty->HasFontSize());
    double fontSize = textLayoutProperty->GetFontSize().value().Value();
    double expectFontSize = FONT_SIZE_5 * HALF + FONT_SIZE_5 * HALF * HALF;
    EXPECT_EQ(expectFontSize, fontSize);
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions002
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(kind:ICON).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", ""},
        {"/demo/demo2.jpg", ""}, {"/demo/demo3.jpg", ""}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_TRUE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollUp002
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:ICON, move up).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollUp002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", ""}, {"/demo/demo2.jpg", ""}, {"/demo/demo3.jpg", ""}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = 0 - jump * HALF;
    double offset2 = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto linearNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(linearNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollDown002
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:ICON, move down).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollDown002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", ""}, {"/demo/demo2.jpg", ""}, {"/demo/demo3.jpg", ""}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = jump * HALF;
    double offset2 = jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto linearNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(linearNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearNode->GetFirstChild());
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions003
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(kind:MIXTURE).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_TRUE(imageLayoutProperty->HasImageSourceInfo());

    auto textNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("test1", content);
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions004
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is invalid).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->SetColumnKind(0);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions005
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is TEXT, option's size is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions005, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    EXPECT_FALSE(textLayoutProperty->HasContent());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions006
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is ICON, option's size is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions006, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions007
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is MIXTURE, option's size is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions007, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions008
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is TEXT, showCount > optionCount).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions008, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("", content);
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions009
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is ICON, showCount > optionCount).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions009, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions010
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is MIXTURE, showCount > optionCount).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions010, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions011
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is TEXT, showCount != column item count).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions011, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(textPickerPattern, nullptr);
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->SetColumnKind(TEXT);
    columnPattern->SetOptions(range);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    columnPattern->FlushCurrentOptions(false, false);
    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    EXPECT_FALSE(textLayoutProperty->HasContent());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions012
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is ICON, showCount != column item count).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions012, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(textPickerPattern, nullptr);
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->SetColumnKind(ICON);
    columnPattern->SetOptions(range);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions013
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is MIXTURE, showCount != column item count).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions013, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(textPickerPattern, nullptr);
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->SetColumnKind(MIXTURE);
    columnPattern->SetOptions(range);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions014
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions
 *          (column kind is MIXTURE, linearNode's children size is not 2).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions014, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(textPickerPattern, nullptr);
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->SetColumnKind(MIXTURE);
    columnPattern->SetOptions(range);
    columnPattern->FlushCurrentOptions(false, false);
    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternFlushCurrentOptions015
 * @tc.desc: Test TextPickerColumnPattern FlushCurrentOptions(column kind is TEXT, set text properties).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternFlushCurrentOptions015, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"}, {"/demo/demo2.jpg", "test2"},
        {"/demo/demo3.jpg", "test3"}, {"/demo/demo4.jpg", "test2"}, {"/demo/demo5.jpg", "test2"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.textColor = Color::RED;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    disappearTextStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.textColor = Color::RED;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    textStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.textColor = Color::RED;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    selectedTextStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);
    auto textNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasFontWeight());
    EXPECT_EQ(Ace::FontWeight::BOLD, textLayoutProperty->GetFontWeight().value());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollUp003
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:MIXTURE, move up).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollUp003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"},
        {"/demo/demo2.jpg", "test2"}, {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = 0 - jump * HALF;
    double offset2 = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto textNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    std::string content = textLayoutProperty->GetContent().value();
    EXPECT_EQ("test1", content);
    ASSERT_TRUE(textLayoutProperty->HasFontSize());
    double fontSize = textLayoutProperty->GetFontSize().value().Value();
    double expectFontSize = FONT_SIZE_5 * HALF + FONT_SIZE_5 * HALF * HALF;
    EXPECT_EQ(expectFontSize, fontSize);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollUp004
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, move up(jump then up)).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollUp004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_2);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = 0 - jump * HALF;
    double offset2 = 0 - jump;
    double offset3 = 0 - jump / HALF;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);
    columnPattern->UpdateColumnChildPosition(offset3);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasFontSize());
    double fontSize = textLayoutProperty->GetFontSize().value().Value();
    EXPECT_EQ(FONT_SIZE_5, fontSize);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScrollDown003
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:MIXTURE, move down).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScrollDown003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"},
        {"/demo/demo2.jpg", "test2"}, {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    disappearTextStyle.textColor = Color::BLACK;
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    textStyle.textColor = Color::BLUE;
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    selectedTextStyle.textColor = Color::RED;
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset1 = jump * HALF;
    double offset2 = jump;
    columnPattern->UpdateColumnChildPosition(offset1);
    columnPattern->UpdateColumnChildPosition(offset2);

    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto textNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    std::string content = textLayoutProperty->GetContent().value_or("");
    EXPECT_EQ("test3", content);
    ASSERT_TRUE(textLayoutProperty->HasFontSize());
    double fontSize = textLayoutProperty->GetFontSize().value().Value();
    double expectFontSize = FONT_SIZE_5 * HALF + FONT_SIZE_5 * HALF * HALF;
    EXPECT_EQ(expectFontSize, fontSize);
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScroll001
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, OptionCount is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScroll001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    columnPattern->FlushCurrentOptions(false, false);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    EXPECT_FALSE(textLayoutProperty->HasContent());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScroll002
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, showCount != column item count).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScroll002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    columnPattern->SetOptions(range);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    EXPECT_FALSE(textLayoutProperty->HasContent());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScroll003
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:MIXTURE, linearNode's children size is not 2)).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScroll003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->OnModifyDone();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    columnPattern->SetOptions(range);
    columnPattern->SetColumnKind(MIXTURE);
    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset);

    auto linearLayoutNode = AceType::DynamicCast<FrameNode>(child->GetFirstChild());
    ASSERT_NE(linearLayoutNode, nullptr);
    auto imageNode = AceType::DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
    ASSERT_NE(imageNode, nullptr);
    auto imagePattern = imageNode->GetPattern<ImagePattern>();
    ASSERT_NE(imagePattern, nullptr);
    auto imageLayoutProperty = imagePattern->GetLayoutProperty<ImageLayoutProperty>();
    ASSERT_NE(imageLayoutProperty, nullptr);
    EXPECT_FALSE(imageLayoutProperty->HasImageSourceInfo());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScroll004
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, move up, animationProperties_'s size is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScroll004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    columnPattern->SetOptions(range);
    columnPattern->SetColumnKind(TEXT);
    columnPattern->FlushCurrentOptions(false, true);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset = 0 - jump * HALF;
    columnPattern->UpdateColumnChildPosition(offset);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    EXPECT_FALSE(textLayoutProperty->HasTextColor());
}

/**
 * @tc.name: TextPickerColumnPatternInnerHandleScroll005
 * @tc.desc: Test TextPickerColumnPattern InnerHandleScroll(kind:TEXT, move up, options'size is 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerColumnPatternInnerHandleScroll005, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    auto child = textPickerPattern->GetColumnNode();
    ASSERT_NE(child, nullptr);
    auto columnPattern = AceType::DynamicCast<FrameNode>(child)->GetPattern<TextPickerColumnPattern>();
    ASSERT_NE(columnPattern, nullptr);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}, {"", "4"}, {"", "5"}};
    columnPattern->SetOptions(range);
    columnPattern->SetColumnKind(TEXT);
    columnPattern->SetCurrentIndex(SELECTED_INDEX_2);
    columnPattern->FlushCurrentOptions(false, false);
    range.clear();
    columnPattern->SetOptions(range);

    double jump = theme->GetJumpInterval().ConvertToPx();
    double offset = 0 - jump;
    columnPattern->UpdateColumnChildPosition(offset);

    auto textNode = AceType::DynamicCast<FrameNode>(child->GetLastChild());
    ASSERT_NE(textNode, nullptr);
    auto textPattern = textNode->GetPattern<TextPattern>();
    ASSERT_NE(textPattern, nullptr);
    auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    ASSERT_TRUE(textLayoutProperty->HasContent());
    EXPECT_EQ("5", textLayoutProperty->GetContent().value());
}

/**
 * @tc.name: TextPickerPatternToJsonValue001
 * @tc.desc: Test TextPickerPattern ToJsonValue(range is not empty).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerPatternToJsonValue001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    std::vector<NG::RangeContent> range = {{"/demo/demo1.jpg", "test1"},
        {"/demo/demo2.jpg", "test2"}, {"/demo/demo3.jpg", "test3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    PickerTextStyle disappearTextStyle;
    disappearTextStyle.fontSize = Dimension(FONT_SIZE_5);
    disappearTextStyle.textColor = Color::BLACK;
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, disappearTextStyle);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    textStyle.textColor = Color::BLUE;
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    PickerTextStyle selectedTextStyle;
    selectedTextStyle.fontSize = Dimension(FONT_SIZE_20);
    selectedTextStyle.textColor = Color::RED;
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, selectedTextStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    std::unique_ptr<JsonValue> json = std::make_unique<JsonValue>();
    textPickerPattern->ToJsonValue(json);
    ASSERT_NE(json, nullptr);
}

/**
 * @tc.name: TextPickerPatternToJsonValue002
 * @tc.desc: Test TextPickerPattern ToJsonValue(range is empty).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerPatternToJsonValue002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    std::unique_ptr<JsonValue> json = std::make_unique<JsonValue>();
    textPickerPattern->ToJsonValue(json);
    ASSERT_NE(json, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow001
 * @tc.desc: Test TextPickerDialogView Show(column kind is MIXTURE).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow001, TestSize.Level1)
{
    PickerTextProperties properties;
    properties.disappearTextStyle_.textColor = Color::RED;
    properties.disappearTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.disappearTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.normalTextStyle_.textColor = Color::RED;
    properties.normalTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.selectedTextStyle_.textColor = Color::RED;
    properties.selectedTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    auto func = [](const std::string& info) {
        (void)info;
    };
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    dialogEvent["changeId"] = func;
    dialogEvent["acceptId"] = func;

    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    settingData.columnKind = MIXTURE;
    settingData.height = Dimension(FONT_SIZE_10);
    memcpy_s(&settingData.properties, sizeof(PickerTextProperties), &properties, sizeof(PickerTextProperties));
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow002
 * @tc.desc: Test TextPickerDialogView Show(do not set callback).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow002, TestSize.Level1)
{
    PickerTextProperties properties;
    properties.disappearTextStyle_.textColor = Color::RED;
    properties.disappearTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.disappearTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.normalTextStyle_.textColor = Color::RED;
    properties.normalTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.selectedTextStyle_.textColor = Color::RED;
    properties.selectedTextStyle_.fontSize = Dimension(FONT_SIZE_10);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    settingData.columnKind = MIXTURE;
    settingData.height = Dimension(FONT_SIZE_10);
    memcpy_s(&settingData.properties, sizeof(PickerTextProperties), &properties, sizeof(PickerTextProperties));
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow003
 * @tc.desc: Test TextPickerDialogView Show(do not set properties).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow003, TestSize.Level1)
{
    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    memset_s(&settingData, sizeof(TextPickerSettingData), 0x00, sizeof(TextPickerSettingData));
    settingData.columnKind = MIXTURE;
    settingData.height = Dimension(FONT_SIZE_10);
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow004
 * @tc.desc: Test TextPickerDialogView Show(column kind is TEXT).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow004, TestSize.Level1)
{
    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    memset_s(&settingData, sizeof(TextPickerSettingData), 0x00, sizeof(TextPickerSettingData));
    settingData.columnKind = TEXT;
    settingData.height = Dimension(FONT_SIZE_10);
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow005
 * @tc.desc: Test TextPickerDialogView Show(column kind is ICON).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow005, TestSize.Level1)
{
    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    memset_s(&settingData, sizeof(TextPickerSettingData), 0x00, sizeof(TextPickerSettingData));
    settingData.columnKind = ICON;
    settingData.height = Dimension(FONT_SIZE_10);
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow006
 * @tc.desc: Test TextPickerDialogView Show(column kind is invalid).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow006, TestSize.Level1)
{
    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    memset_s(&settingData, sizeof(TextPickerSettingData), 0x00, sizeof(TextPickerSettingData));
    settingData.columnKind = 0;
    settingData.height = Dimension(FONT_SIZE_10);
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerDialogViewShow007
 * @tc.desc: Test TextPickerDialogView Show(Invailid font size).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerDialogViewShow007, TestSize.Level1)
{
    PickerTextProperties properties;
    properties.disappearTextStyle_.textColor = Color::RED;
    properties.disappearTextStyle_.fontSize = Dimension(FONT_SIZE_INVALID);
    properties.disappearTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.normalTextStyle_.textColor = Color::RED;
    properties.normalTextStyle_.fontSize = Dimension(FONT_SIZE_INVALID);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    properties.selectedTextStyle_.textColor = Color::RED;
    properties.selectedTextStyle_.fontSize = Dimension(FONT_SIZE_INVALID);
    properties.normalTextStyle_.fontWeight = Ace::FontWeight::BOLD;

    auto cancelFunc = [](const GestureEvent& info) { (void)info; };
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogCancelEvent["cancelId"] = cancelFunc;

    TextPickerSettingData settingData;
    settingData.columnKind = MIXTURE;
    settingData.height = Dimension(FONT_SIZE_10);
    memcpy_s(&settingData.properties, sizeof(PickerTextProperties), &properties, sizeof(PickerTextProperties));
    settingData.rangeVector = {{"", "1"}, {"", "2"}, {"", "3"}};
    settingData.selected = 0;

    DialogProperties dialogProperties;
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    auto frameNode = TextPickerDialogView::Show(dialogProperties, settingData, dialogEvent, dialogCancelEvent);
    ASSERT_NE(frameNode, nullptr);
}

/**
 * @tc.name: TextPickerPatternOnAttachToFrameNode001
 * @tc.desc: Test TextPickerPattern OnAttachToFrameNode.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGCreateTextPicker001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, ICON);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto textPickerPattern = AceType::MakeRefPtr<TextPickerPattern>();
    textPickerPattern->AttachToFrameNode(frameNode);
    textPickerPattern->OnAttachToFrameNode();
    auto host = textPickerPattern->GetHost();
    ASSERT_NE(host, nullptr);
}

/**
 * @tc.name: TextPickerModelNGSetDisappearTextStyle001
 * @tc.desc: Test TextPickerModelNG SetDisappearTextStyle(set Color).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetDisappearTextStyle001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.textColor = Color::RED;
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasDisappearColor());
    EXPECT_EQ(Color::RED, pickerProperty->GetDisappearColor().value());
}

/**
 * @tc.name: TextPickerModelNGSetDisappearTextStyle002
 * @tc.desc: Test TextPickerModelNG SetDisappearTextStyle(set FontSize).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetDisappearTextStyle002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasDisappearFontSize());
    EXPECT_EQ(Dimension(FONT_SIZE_10), pickerProperty->GetDisappearFontSize().value());
}

/**
 * @tc.name: TextPickerModelNGSetDisappearTextStyle003
 * @tc.desc: Test TextPickerModelNG SetDisappearTextStyle(set FontSize 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetDisappearTextStyle003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(0);
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    EXPECT_TRUE(pickerProperty->HasDisappearFontSize());
}

/**
 * @tc.name: TextPickerModelNGSetDisappearTextStyle004
 * @tc.desc: Test TextPickerModelNG SetDisappearTextStyle(set FontWeight).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetDisappearTextStyle004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetDisappearTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasDisappearWeight());
    EXPECT_EQ(Ace::FontWeight::BOLD, pickerProperty->GetDisappearWeight().value());
}

/**
 * @tc.name: TextPickerModelNGSetNormalTextStyle001
 * @tc.desc: Test TextPickerModelNG SetNormalTextStyle(set Color).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetNormalTextStyle001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.textColor = Color::RED;
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasColor());
    EXPECT_EQ(Color::RED, pickerProperty->GetColor().value());
}

/**
 * @tc.name: TextPickerModelNGSetNormalTextStyle002
 * @tc.desc: Test TextPickerModelNG SetNormalTextStyle(set FontSize).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetNormalTextStyle002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasFontSize());
    EXPECT_EQ(Dimension(FONT_SIZE_10), pickerProperty->GetFontSize().value());
}

/**
 * @tc.name: TextPickerModelNGSetNormalTextStyle003
 * @tc.desc: Test TextPickerModelNG SetNormalTextStyle(set FontSize 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetNormalTextStyle003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(0);
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    EXPECT_TRUE(pickerProperty->HasFontSize());
}

/**
 * @tc.name: TextPickerModelNGSetNormalTextStyle004
 * @tc.desc: Test TextPickerModelNG SetNormalTextStyle(set FontWeight).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetNormalTextStyle004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetNormalTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasWeight());
    EXPECT_EQ(Ace::FontWeight::BOLD, pickerProperty->GetWeight().value());
}

/**
 * @tc.name: TextPickerModelNGSetSelectedTextStyle001
 * @tc.desc: Test TextPickerModelNG SetSelectedTextStyle(set Color).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetSelectedTextStyle001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.textColor = Color::RED;
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasSelectedColor());
    EXPECT_EQ(Color::RED, pickerProperty->GetSelectedColor().value());
}

/**
 * @tc.name: TextPickerModelNGSetSelectedTextStyle002
 * @tc.desc: Test TextPickerModelNG SetSelectedTextStyle(set FontSize).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetSelectedTextStyle002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(FONT_SIZE_10);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasSelectedFontSize());
    EXPECT_EQ(Dimension(FONT_SIZE_10), pickerProperty->GetSelectedFontSize().value());
}

/**
 * @tc.name: TextPickerModelNGSetSelectedTextStyle003
 * @tc.desc: Test TextPickerModelNG SetSelectedTextStyle(set FontSize 0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetSelectedTextStyle003, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontSize = Dimension(0);
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    EXPECT_TRUE(pickerProperty->HasSelectedFontSize());
}

/**
 * @tc.name: TextPickerModelNGSetSelectedTextStyle004
 * @tc.desc: Test TextPickerModelNG SetSelectedTextStyle(set FontWeight).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetSelectedTextStyle004, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    PickerTextStyle textStyle;
    textStyle.fontWeight = Ace::FontWeight::BOLD;
    TextPickerModelNG::GetInstance()->SetSelectedTextStyle(theme, textStyle);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasSelectedWeight());
    EXPECT_EQ(Ace::FontWeight::BOLD, pickerProperty->GetSelectedWeight().value());
}

/**
 * @tc.name: TextPickerModelNGSetSelected001
 * @tc.desc: Test TextPickerModelNG SetSelected.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetSelected001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    TextPickerModelNG::GetInstance()->SetSelected(SELECTED_INDEX_1);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasSelected());
    EXPECT_EQ(1, pickerProperty->GetSelected().value());
}

/**
 * @tc.name: TextPickerModelNGSetRange001
 * @tc.desc: Test TextPickerModelNG SetRange.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetRange001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range = {{"", "1"}, {"", "2"}, {"", "3"}};
    TextPickerModelNG::GetInstance()->SetRange(range);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(pickerPattern, nullptr);
    EXPECT_EQ(THREE, pickerPattern->GetRange().size());
}

/**
 * @tc.name: TextPickerModelNGSetRange002
 * @tc.desc: Test TextPickerModelNG SetRange.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetRange002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    std::vector<NG::RangeContent> range;
    TextPickerModelNG::GetInstance()->SetRange(range);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    auto pickerPattern = frameNode->GetPattern<TextPickerPattern>();
    ASSERT_NE(pickerPattern, nullptr);
    EXPECT_TRUE(pickerPattern->GetRange().empty());
}

/**
 * @tc.name: TextPickerModelNGCreate001
 * @tc.desc: Test TextPickerModelNG Create(DeviceType::PHONE, DeviceOrientation::LANDSCAPE).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGCreate001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(static_cast<int32_t>(DeviceOrientation::LANDSCAPE));
    TextPickerModelNG::GetInstance()->Create(theme, ICON);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto stackNode = AceType::DynamicCast<FrameNode>(frameNode->GetFirstChild());
    ASSERT_NE(stackNode, nullptr);
    auto columnNode = AceType::DynamicCast<FrameNode>(stackNode->GetLastChild());
    ASSERT_NE(columnNode, nullptr);
    auto columnChildren = columnNode->GetChildren();
    EXPECT_EQ(THREE_CHILDREN, columnChildren.size());
}

/**
 * @tc.name: TextPickerModelNGCreate002
 * @tc.desc: Test TextPickerModelNG Create(DeviceType::PHONE, DeviceOrientation::0).
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGCreate002, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    SystemProperties::SetDeviceOrientation(0);
    TextPickerModelNG::GetInstance()->Create(theme, MIXTURE);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto stackNode = AceType::DynamicCast<FrameNode>(frameNode->GetFirstChild());
    ASSERT_NE(stackNode, nullptr);
    auto columnNode = AceType::DynamicCast<FrameNode>(stackNode->GetLastChild());
    ASSERT_NE(columnNode, nullptr);
    auto columnChildren = columnNode->GetChildren();
    EXPECT_EQ(FIVE_CHILDREN, columnChildren.size());
}

/**
 * @tc.name: TextPickerModelNGSetDefaultAttributes001
 * @tc.desc: Test TextPickerModelNG SetDefaultAttributes.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerModelNGSetDefaultAttributes001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    TextPickerModelNG::GetInstance()->SetDefaultAttributes(theme);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    ASSERT_TRUE(pickerProperty->HasSelectedColor());
    EXPECT_EQ(Color::RED, pickerProperty->GetSelectedColor().value());
}

/**
 * @tc.name: TextPickerLayoutPropertyToJsonValue001
 * @tc.desc: Test TextPickerLayoutProperty ToJsonValue.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerLayoutPropertyToJsonValue001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    TextPickerModelNG::GetInstance()->SetDefaultAttributes(theme);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    std::unique_ptr<JsonValue> json = std::make_unique<JsonValue>();
    pickerProperty->ToJsonValue(json);
    ASSERT_NE(json, nullptr);
}

/**
 * @tc.name: TextPickerLayoutPropertyClone001
 * @tc.desc: Test TextPickerLayoutProperty Clone.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerLayoutPropertyClone001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    TextPickerModelNG::GetInstance()->SetDefaultAttributes(theme);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    auto layoutProperty = pickerProperty->Clone();
    ASSERT_NE(layoutProperty, nullptr);
}

/**
 * @tc.name: TextPickerLayoutPropertyReset001
 * @tc.desc: Test TextPickerLayoutProperty Reset.
 * @tc.type: FUNC
 */
HWTEST_F(TextPickerPatternTestNg, TextPickerLayoutPropertyReset001, TestSize.Level1)
{
    auto theme = MockPipelineBase::GetCurrent()->GetTheme<PickerTheme>();
    TextPickerModelNG::GetInstance()->Create(theme, TEXT);
    TextPickerModelNG::GetInstance()->SetDefaultAttributes(theme);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pickerProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    ASSERT_NE(pickerProperty, nullptr);
    pickerProperty->Reset();
    EXPECT_FALSE(pickerProperty->HasDisappearFontSize());
}
} // namespace OHOS::Ace::NG