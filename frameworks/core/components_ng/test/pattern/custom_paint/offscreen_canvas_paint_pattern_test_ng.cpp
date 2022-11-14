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

#include <memory>
#include <optional>

#include "gtest/gtest.h"

#include "core/common/ace_engine.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/test/unittest/geometry/mock_pipeline_base.h"
#include "base/test/unittest/geometry/mock_container.h"

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/components_ng/test/pattern/custom_paint/common_constants.h"
#include "core/components_ng/test/pattern/custom_paint/mock/mock_paragraph.h"

// Add the following two macro definitions to test the private and protected method.
#define private public
#define protected public

#include "core/components_ng/pattern/custom_paint/custom_paint_paint_method.h"
#include "core/components_ng/pattern/custom_paint/canvas_paint_method.h"
#include "core/components_ng/pattern/custom_paint/offscreen_canvas_paint_method.h"
#include "core/components_ng/pattern/custom_paint/offscreen_canvas_pattern.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {

class OffscreenCanvasPaintPatternTestNg : public testing::Test {
public:
    // Create the pointer of the class OffscreenCanvasPattern
    static RefPtr<OffscreenCanvasPattern> CreateOffscreenCanvasPattern(int32_t width, int32_t height);
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

RefPtr<OffscreenCanvasPattern> OffscreenCanvasPaintPatternTestNg::CreateOffscreenCanvasPattern(
    int32_t width, int32_t height)
{
    ContainerScope scope(DEFAULT_INSTANCE_ID);
    RefPtr<PipelineBase> pipelineContext = AceType::MakeRefPtr<MockPipelineBase>();
    RefPtr<Container> container = AceType::MakeRefPtr<MockContainer>(pipelineContext);
    AceEngine::Get().AddContainer(DEFAULT_INSTANCE_ID, container);
    return AceType::MakeRefPtr<OffscreenCanvasPattern>(width, height);
}

void OffscreenCanvasPaintPatternTestNg::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "OffscreenCanvasPaintPatternTestNg SetUpTestCase";
}

void OffscreenCanvasPaintPatternTestNg::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "OffscreenCanvasPaintPatternTestNg TearDownTestCase";
}

void OffscreenCanvasPaintPatternTestNg::SetUp()
{
    GTEST_LOG_(INFO) << "OffscreenCanvasPaintPatternTestNg SetUp";
}

void OffscreenCanvasPaintPatternTestNg::TearDown()
{
    GTEST_LOG_(INFO) << "OffscreenCanvasPaintPatternTestNg TearDown";
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg001
 * @tc.desc: Test functions of the object customPattern.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg001, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);
    std::unique_ptr<MockParagraph> mockParagraph = std::make_unique<MockParagraph>();
    EXPECT_CALL(*mockParagraph, GetMaxIntrinsicWidth()).WillRepeatedly(testing::Return(DEFAULT_DOUBLE10));
    EXPECT_CALL(*mockParagraph, GetAlphabeticBaseline()).WillRepeatedly(testing::Return(DEFAULT_DOUBLE10));
    EXPECT_CALL(*mockParagraph, GetIdeographicBaseline()).WillRepeatedly(testing::Return(DEFAULT_DOUBLE10));
    EXPECT_CALL(*mockParagraph, GetHeight()).WillRepeatedly(testing::Return(DEFAULT_DOUBLE10));
    std::unique_ptr<txt::Paragraph> paragraph(std::move(mockParagraph));

    /**
     * @tc.steps2: Test functions GetWidth and GetHeight.
     * @tc.expected: The return values are equal to which used to create the object of OffscreenCanvasPattern.
     */
    EXPECT_EQ(offscreenCanvasPattern->GetWidth(), CANVAS_WIDTH);
    EXPECT_EQ(offscreenCanvasPattern->GetHeight(), CANVAS_HEIGHT);

    /**
     * @tc.steps3: Test functions GetTextDirection.
     * @tc.expected: The function GetTextDirection always returns TextDirection::LTR..
     */
    for (const std::string& item : CANDIDATE_STRINGS) {
        EXPECT_EQ(paintMethod->GetTextDirection(item), TextDirection::LTR);
    }

    /**
     * @tc.steps4: Test functions GetAlignOffset.
     * @tc.expected: The return value is affected by the second parameter.
     */
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::LEFT, paragraph), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::RIGHT, paragraph), -DEFAULT_DOUBLE10);
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::CENTER, paragraph), -DEFAULT_DOUBLE10/2);
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::JUSTIFY, paragraph), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::START, paragraph), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->GetAlignOffset(DEFAULT_STR, TextAlign::END, paragraph), -DEFAULT_DOUBLE10);

    /**
     * @tc.steps5: Test functions GetBaselineOffset.
     * @tc.expected: The return value is affected by the first parameter.
     */
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::ALPHABETIC, paragraph), -DEFAULT_DOUBLE10);
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::IDEOGRAPHIC, paragraph), -DEFAULT_DOUBLE10);
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::TOP, paragraph), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::BOTTOM, paragraph), -DEFAULT_DOUBLE10);
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::MIDDLE, paragraph), -DEFAULT_DOUBLE10 / 2);
    EXPECT_DOUBLE_EQ(paintMethod->GetBaselineOffset(TextBaseline::HANGING, paragraph), DEFAULT_DOUBLE0);
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg002
 * @tc.desc: Test all properties related to the variable fillState_.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg002, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);

    /**
     * @tc.steps2: Test all interfaces beginning with "Get" and "Set", which related to the variable fillState_.
     * @tc.expected: The return values of interfaces beginning with "Get" are equal to the input parameters of
     *               corresponding interfaces beginning with "Set"
     */
    for (auto color : CANDIDATE_COLORS) {
        offscreenCanvasPattern->SetFillColor(color);
        EXPECT_EQ(paintMethod->fillState_.GetColor(), color);
        EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetTextColor(), color);
    }

    Ace::Pattern imagePattern;
    offscreenCanvasPattern->SetFillPattern(imagePattern);
    EXPECT_DOUBLE_EQ(paintMethod->fillState_.GetPattern().GetImageWidth(), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->fillState_.GetPattern().GetImageHeight(), DEFAULT_DOUBLE0);
    EXPECT_EQ(paintMethod->fillState_.GetPattern().GetImgSrc(), NULL_STR);
    EXPECT_EQ(paintMethod->fillState_.GetPattern().GetRepetition(), NULL_STR);
    imagePattern.SetImageWidth(IDEAL_WIDTH);
    imagePattern.SetImageHeight(IDEAL_HEIGHT);
    imagePattern.SetImgSrc(IMAGE_SRC);
    imagePattern.SetRepetition(REPETITION_STR);
    offscreenCanvasPattern->SetFillPattern(imagePattern);
    EXPECT_DOUBLE_EQ(paintMethod->fillState_.GetPattern().GetImageWidth(), IDEAL_WIDTH);
    EXPECT_DOUBLE_EQ(paintMethod->fillState_.GetPattern().GetImageHeight(), IDEAL_HEIGHT);
    EXPECT_EQ(paintMethod->fillState_.GetPattern().GetImgSrc(), IMAGE_SRC);
    EXPECT_EQ(paintMethod->fillState_.GetPattern().GetRepetition(), REPETITION_STR);
    
    Ace::Gradient gradient;
    offscreenCanvasPattern->SetFillGradient(gradient);
    EXPECT_FALSE(paintMethod->fillState_.GetGradient().GetRepeat());
    gradient.SetRepeat(true);
    offscreenCanvasPattern->SetFillGradient(gradient);
    EXPECT_TRUE(paintMethod->fillState_.GetGradient().GetRepeat());

    for (auto textAlign : CANDIDATE_TEXT_ALIGNS) {
        offscreenCanvasPattern->SetTextAlign(textAlign);
        EXPECT_EQ(paintMethod->fillState_.GetTextAlign(), textAlign);
    }
    for (auto textBaseline : CANDIDATE_TEXT_BASELINES) {
        offscreenCanvasPattern->SetTextBaseline(textBaseline);
        EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetTextBaseline(), textBaseline);
    }
    for (auto fontSize : CANDIDATE_DIMENSIONS) {
        offscreenCanvasPattern->SetFontSize(fontSize);
        EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetFontSize(), fontSize);
    }
    for (auto fontStyle : CANDIDATE_FONT_STYLES) {
        offscreenCanvasPattern->SetFontStyle(fontStyle);
        EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetFontStyle(), fontStyle);
    }
    for (auto fontWeight : CANDIDATE_FONT_WEIGHTS) {
        offscreenCanvasPattern->SetFontWeight(fontWeight);
        EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetFontWeight(), fontWeight);
    }
    offscreenCanvasPattern->SetFontFamilies(FONT_FAMILY);
    EXPECT_EQ(paintMethod->fillState_.GetTextStyle().GetFontFamilies(), FONT_FAMILY);
    EXPECT_TRUE(paintMethod->saveStates_.empty());
    offscreenCanvasPattern->Save();
    EXPECT_FALSE(paintMethod->saveStates_.empty());
    offscreenCanvasPattern->Restore();
    EXPECT_TRUE(paintMethod->saveStates_.empty());
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg003
 * @tc.desc: Test all properties related to the variable strokeState_.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg003, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);

    /**
     * @tc.steps2: Test all interfaces beginning with "Get" and "Set", which related to the variable strokeState_.
     * @tc.expected: The return values of interfaces beginning with "Get" are equal to the input parameters of
     *               corresponding interfaces beginning with "Set"
     */
    for (auto color : CANDIDATE_COLORS) {
        offscreenCanvasPattern->SetStrokeColor(color);
        EXPECT_EQ(paintMethod->strokeState_.GetColor(), color);
    }

    Ace::Pattern imagePattern;
    offscreenCanvasPattern->SetStrokePattern(imagePattern);
    EXPECT_DOUBLE_EQ(paintMethod->strokeState_.GetPattern().GetImageWidth(), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(paintMethod->strokeState_.GetPattern().GetImageHeight(), DEFAULT_DOUBLE0);
    EXPECT_EQ(paintMethod->strokeState_.GetPattern().GetImgSrc(), NULL_STR);
    EXPECT_EQ(paintMethod->strokeState_.GetPattern().GetRepetition(), NULL_STR);
    imagePattern.SetImageWidth(IDEAL_WIDTH);
    imagePattern.SetImageHeight(IDEAL_HEIGHT);
    imagePattern.SetImgSrc(IMAGE_SRC);
    imagePattern.SetRepetition(REPETITION_STR);
    offscreenCanvasPattern->SetStrokePattern(imagePattern);
    EXPECT_DOUBLE_EQ(paintMethod->strokeState_.GetPattern().GetImageWidth(), IDEAL_WIDTH);
    EXPECT_DOUBLE_EQ(paintMethod->strokeState_.GetPattern().GetImageHeight(), IDEAL_HEIGHT);
    EXPECT_EQ(paintMethod->strokeState_.GetPattern().GetImgSrc(), IMAGE_SRC);
    EXPECT_EQ(paintMethod->strokeState_.GetPattern().GetRepetition(), REPETITION_STR);

    Ace::Gradient gradient;
    offscreenCanvasPattern->SetStrokeGradient(gradient);
    EXPECT_FALSE(paintMethod->strokeState_.GetGradient().GetRepeat());
    gradient.SetRepeat(true);
    offscreenCanvasPattern->SetStrokeGradient(gradient);
    EXPECT_TRUE(paintMethod->strokeState_.GetGradient().GetRepeat());

    for (auto item : CANDIDATE_LINE_CAP_STYLES) {
        offscreenCanvasPattern->SetLineCap(item);
        EXPECT_EQ(paintMethod->strokeState_.GetLineCap(), item);
    }

    for (auto item : CANDIDATE_LINE_JOIN_STYLES) {
        offscreenCanvasPattern->SetLineJoin(item);
        EXPECT_EQ(paintMethod->strokeState_.GetLineJoin(), item);
    }

    for (double item : CANDIDATE_DOUBLES) {
        offscreenCanvasPattern->SetLineWidth(item);
        EXPECT_EQ(paintMethod->strokeState_.GetLineWidth(), item);
        offscreenCanvasPattern->SetMiterLimit(item);
        EXPECT_EQ(paintMethod->strokeState_.GetMiterLimit(), item);
    }

    offscreenCanvasPattern->SetLineDash(CANDIDATE_DOUBLES);
    for (uint32_t i = 1; i < CANDIDATE_DOUBLES.size(); ++i) {
        EXPECT_EQ(offscreenCanvasPattern->GetLineDash().lineDash[i], CANDIDATE_DOUBLES[i]);
    }
    for (double item : CANDIDATE_DOUBLES) {
        offscreenCanvasPattern->SetLineDashOffset(item);
        EXPECT_EQ(offscreenCanvasPattern->GetLineDash().dashOffset, item);
    }

    for (auto textAlign : CANDIDATE_TEXT_ALIGNS) {
        offscreenCanvasPattern->SetTextAlign(textAlign);
        EXPECT_EQ(paintMethod->strokeState_.GetTextAlign(), textAlign);
    }
    for (auto textBaseline : CANDIDATE_TEXT_BASELINES) {
        offscreenCanvasPattern->SetTextBaseline(textBaseline);
        EXPECT_EQ(paintMethod->strokeState_.GetTextStyle().GetTextBaseline(), textBaseline);
    }
    for (auto fontSize : CANDIDATE_DIMENSIONS) {
        offscreenCanvasPattern->SetFontSize(fontSize);
        EXPECT_EQ(paintMethod->strokeState_.GetTextStyle().GetFontSize(), fontSize);
    }
    for (auto fontStyle : CANDIDATE_FONT_STYLES) {
        offscreenCanvasPattern->SetFontStyle(fontStyle);
        EXPECT_EQ(paintMethod->strokeState_.GetTextStyle().GetFontStyle(), fontStyle);
    }
    for (auto fontWeight : CANDIDATE_FONT_WEIGHTS) {
        offscreenCanvasPattern->SetFontWeight(fontWeight);
        EXPECT_EQ(paintMethod->strokeState_.GetTextStyle().GetFontWeight(), fontWeight);
    }
    offscreenCanvasPattern->SetFontFamilies(FONT_FAMILY);
    EXPECT_EQ(paintMethod->strokeState_.GetTextStyle().GetFontFamilies(), FONT_FAMILY);
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg004
 * @tc.desc: Test all properties related to the variable globalState_.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg004, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);

    /**
     * @tc.steps2: Test all interfaces beginning with "Get" and "Set", which related to the variable globalState_.
     * @tc.expected: The return values of interfaces beginning with "Get" are equal to the input parameters of
     *               corresponding interfaces beginning with "Set"
     */
    for (double item : CANDIDATE_DOUBLES) {
        offscreenCanvasPattern->SetAlpha(item);
        EXPECT_DOUBLE_EQ(paintMethod->globalState_.GetAlpha(), item);
    }
    for (auto compositeOperation : CANDIDATE_COMPOSITE_OPERATIONS) {
        offscreenCanvasPattern->SetCompositeType(compositeOperation);
        EXPECT_EQ(paintMethod->globalState_.GetType(), compositeOperation);
    }
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg005
 * @tc.desc: Test all properties related to the variable shadow_.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg005, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);

    /**
     * @tc.steps2: Test all interfaces beginning with "Get" and "Set", which related to the variable shadow_.
     * @tc.expected: The return values of interfaces beginning with "Get" are equal to the input parameters of
     *               corresponding interfaces beginning with "Set"
     */
    for (auto color : CANDIDATE_COLORS) {
        offscreenCanvasPattern->SetShadowColor(color);
        EXPECT_EQ(paintMethod->shadow_.GetColor(), color);
    }
    offscreenCanvasPattern->SetShadowOffsetX(CANDIDATE_DOUBLES[0]);
    offscreenCanvasPattern->SetShadowOffsetY(CANDIDATE_DOUBLES[0]);
    offscreenCanvasPattern->SetShadowBlur(CANDIDATE_DOUBLES[0]);
    EXPECT_FALSE(paintMethod->HasShadow());
    EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetOffset().GetX(), CANDIDATE_DOUBLES[0]);
    EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetOffset().GetY(), CANDIDATE_DOUBLES[0]);
    EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetBlurRadius(), CANDIDATE_DOUBLES[0]);
    for (uint32_t i = 1; i < CANDIDATE_DOUBLES.size(); ++i) {
        offscreenCanvasPattern->SetShadowOffsetX(CANDIDATE_DOUBLES[i]);
        offscreenCanvasPattern->SetShadowOffsetY(CANDIDATE_DOUBLES[i]);
        offscreenCanvasPattern->SetShadowBlur(CANDIDATE_DOUBLES[i]);
        EXPECT_TRUE(paintMethod->HasShadow());
        EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetOffset().GetX(), CANDIDATE_DOUBLES[i]);
        EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetOffset().GetY(), CANDIDATE_DOUBLES[i]);
        EXPECT_DOUBLE_EQ(paintMethod->shadow_.GetBlurRadius(), CANDIDATE_DOUBLES[i]);
    }
}

/**
 * @tc.name: OffscreenCanvasPaintPatternTestNg006
 * @tc.desc: Test all functions about OffscreenCanvasPattern.
 * @tc.type: FUNC
 */
HWTEST_F(OffscreenCanvasPaintPatternTestNg, OffscreenCanvasPaintPatternTestNg006, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     * @tc.expected: All pointer is non-null.
     */
    auto offscreenCanvasPattern = CreateOffscreenCanvasPattern(CANVAS_WIDTH, CANVAS_HEIGHT);
    ASSERT_NE(offscreenCanvasPattern, nullptr);
    auto paintMethod = offscreenCanvasPattern->offscreenPaintMethod_;
    ASSERT_NE(paintMethod, nullptr);

    /**
     * @tc.steps2: Test the function ToDataURL.
     * @tc.expected: The return value is affected by the first parameter.
     */
    std::string result = offscreenCanvasPattern->ToDataURL(IMAGE_PNG, DEFAULT_DOUBLE1);
    EXPECT_EQ(result.substr(URL_PREFIX.size(), IMAGE_PNG.size()), IMAGE_PNG);
    result = offscreenCanvasPattern->ToDataURL(IMAGE_JPEG, -DEFAULT_DOUBLE2);
    EXPECT_EQ(result.substr(URL_PREFIX.size(), IMAGE_JPEG.size()), IMAGE_JPEG);
    result = offscreenCanvasPattern->ToDataURL(IMAGE_WEBP, -DEFAULT_DOUBLE1);
    EXPECT_EQ(result.substr(URL_PREFIX.size(), IMAGE_WEBP.size()), IMAGE_WEBP);

    /**
     * @tc.steps3: Test functions SetFillRuleForPath and SetFillRuleForPath2D.
     * @tc.expected: The corresponding attributes are modified.
     */
    for (uint32_t i = 0; i < CANDIDATE_CANVAS_FILL_RULES.size(); ++i) {
        offscreenCanvasPattern->SetFillRuleForPath(CANDIDATE_CANVAS_FILL_RULES[i]);
        EXPECT_EQ(paintMethod->skPath_.getFillType(), CANDIDATE_SKPATH_FILL_TYPES[i]);
        offscreenCanvasPattern->SetFillRuleForPath2D(CANDIDATE_CANVAS_FILL_RULES[i]);
        EXPECT_EQ(paintMethod->skPath2d_.getFillType(), CANDIDATE_SKPATH_FILL_TYPES[i]);
    }

    /**
     * @tc.steps4: Test functions SetSmoothingEnabled and SetAntiAlias.
     * @tc.expected: The corresponding attributes are modified.
     */
    offscreenCanvasPattern->SetSmoothingEnabled(true);
    EXPECT_TRUE(paintMethod->smoothingEnabled_);
    offscreenCanvasPattern->SetSmoothingEnabled(false);
    EXPECT_FALSE(paintMethod->smoothingEnabled_);
    offscreenCanvasPattern->SetAntiAlias(true);
    EXPECT_TRUE(paintMethod->antiAlias_);
    offscreenCanvasPattern->SetAntiAlias(false);
    EXPECT_FALSE(paintMethod->antiAlias_);

    /**
     * @tc.steps4: Test the function GetImageData.
     * @tc.expected: The attributes dirtyWidth and dirtyHeight of return value are equal to DEFAULT_DOUBLE1.
     */
    auto imageData1 = offscreenCanvasPattern->GetImageData(
        DEFAULT_DOUBLE1, DEFAULT_DOUBLE1, DEFAULT_DOUBLE1, DEFAULT_DOUBLE1);
    EXPECT_DOUBLE_EQ(imageData1->dirtyWidth, DEFAULT_DOUBLE1);
    EXPECT_DOUBLE_EQ(imageData1->dirtyHeight, DEFAULT_DOUBLE1);

    /**
     * @tc.steps5: Call the function MeasureTextMetrics.
     * @tc.expected: All parts of the return TextMetrics object are equal to zero.
     */
    PaintState paintState;
    TextMetrics textMetrics = offscreenCanvasPattern->MeasureTextMetrics(DEFAULT_STR, paintState);
    EXPECT_DOUBLE_EQ(textMetrics.width, DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(textMetrics.height, DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(textMetrics.actualBoundingBoxLeft, DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(textMetrics.actualBoundingBoxRight, DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(textMetrics.actualBoundingBoxAscent, DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(textMetrics.actualBoundingBoxDescent, DEFAULT_DOUBLE0);

    /**
     * @tc.steps6: Call functions MeasureText and MeasureTextHeight.
     * @tc.expected: All return values are equal to zero.
     */
    EXPECT_DOUBLE_EQ(offscreenCanvasPattern->MeasureText(DEFAULT_STR, paintState), DEFAULT_DOUBLE0);
    EXPECT_DOUBLE_EQ(offscreenCanvasPattern->MeasureTextHeight(DEFAULT_STR, paintState), DEFAULT_DOUBLE0);
}

} // namespace OHOS::Ace::NG
