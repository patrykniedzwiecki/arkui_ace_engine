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

#include "core/components/common/painter/border_image_painter.h"

#include "include/core/SkImage.h"

#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/border_image_edge.h"



namespace OHOS::Ace {

BorderImagePainter::BorderImagePainter(const Size& paintSize, const RefPtr<Decoration>& decoration,
    const sk_sp<SkImage>& image, double dipscale)
    : paintSize_(paintSize), decoration_(decoration), image_(image), dipscale_(dipscale)
{}

void BorderImagePainter::InitPainter()
{
    imageWidth_ = image_->width();
    imageHeight_ = image_->height();
    auto border = decoration_->GetBorder();
    auto borderImage = decoration_->GetBorderImage();
    InitBorderImageSlice(borderImage);
    InitBorderImageWidth(border, borderImage);
    InitBorderImageOutset(border, borderImage);
    srcRectLeft_ = SkRect::MakeXYWH(0, topSlice_, leftSlice_, std::fabs(imageHeight_ - topSlice_ - bottomSlice_));
    srcRectTop_ = SkRect::MakeXYWH(leftSlice_, 0, std::fabs(imageWidth_ - leftSlice_ - rightSlice_), topSlice_);
    srcRectRight_ = SkRect::MakeXYWH(imageWidth_ - rightSlice_, topSlice_, rightSlice_,
        std::fabs(imageHeight_ - topSlice_ - bottomSlice_));
    srcRectBottom_ = SkRect::MakeXYWH(leftSlice_, imageHeight_ - bottomSlice_,
        std::fabs(imageWidth_ - leftSlice_ - rightSlice_), bottomSlice_);
}

void BorderImagePainter::UpdateExtraOffsetToPaintSize(const Offset& extraOffset)
{
    paintSize_ += Size(extraOffset.GetX() * 2, extraOffset.GetY() * 2);
}

void BorderImagePainter::InitBorderImageWidth(Border& border, RefPtr<BorderImage>& borderImage)
{
    BorderImageEdge imageLeft = borderImage->GetBorderImageEdge(BorderImageDirection::LEFT);
    BorderImageEdge imageTop = borderImage->GetBorderImageEdge(BorderImageDirection::TOP);
    BorderImageEdge imageRight = borderImage->GetBorderImageEdge(BorderImageDirection::RIGHT);
    BorderImageEdge imageBottom = borderImage->GetBorderImageEdge(BorderImageDirection::BOTTOM);

    if (!decoration_->GetHasBorderImageWidth()) {
        border.Left().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), leftWidth_);
        border.Right().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), rightWidth_);
        border.Top().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), topWidth_);
        border.Bottom().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), bottomWidth_);
        return;
    }

    if (GreatNotEqual(imageLeft.GetBorderImageWidth().Value(), 0.0)) {
        imageLeft.GetBorderImageWidth().NormalizeToPx(dipscale_, 0, 0,
            border.Left().GetWidthInPx(dipscale_), leftWidth_);
    } else {
        border.Left().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), leftWidth_);
    }
    if (GreatNotEqual(imageRight.GetBorderImageWidth().Value(), 0.0)) {
        imageRight.GetBorderImageWidth().NormalizeToPx(dipscale_, 0, 0,
            border.Right().GetWidthInPx(dipscale_), rightWidth_);
    } else {
        border.Right().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), rightWidth_);
    }
    if (GreatNotEqual(imageTop.GetBorderImageWidth().Value(), 0.0)) {
        imageTop.GetBorderImageWidth().NormalizeToPx(dipscale_, 0, 0,
            border.Top().GetWidthInPx(dipscale_), topWidth_);
    } else {
        border.Top().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), topWidth_);
    }
    if (GreatNotEqual(imageBottom.GetBorderImageWidth().Value(), 0.0)) {
        imageBottom.GetBorderImageWidth().NormalizeToPx(dipscale_, 0, 0,
            border.Bottom().GetWidthInPx(dipscale_), bottomWidth_);
    } else {
        border.Bottom().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), bottomWidth_);
    }
}

void BorderImagePainter::InitBorderImageSlice(RefPtr<BorderImage>& borderImage)
{
    BorderImageEdge imageLeft = borderImage->GetBorderImageEdge(BorderImageDirection::LEFT);
    BorderImageEdge imageTop = borderImage->GetBorderImageEdge(BorderImageDirection::TOP);
    BorderImageEdge imageRight = borderImage->GetBorderImageEdge(BorderImageDirection::RIGHT);
    BorderImageEdge imageBottom = borderImage->GetBorderImageEdge(BorderImageDirection::BOTTOM);

    if (!decoration_->GetHasBorderImageSlice()) {
        leftSlice_ = imageWidth_;
        topSlice_ = imageHeight_;
        rightSlice_ = imageWidth_;
        bottomSlice_ = imageHeight_;
        paintCornersOnly_ = true;
        return;
    }

    if (GreatNotEqual(imageLeft.GetBorderImageSlice().Value(), 0.0)) {
        imageLeft.GetBorderImageSlice().NormalizeToPx(dipscale_, 0, 0, imageWidth_, leftSlice_);
    }
    if (GreatNotEqual(imageRight.GetBorderImageSlice().Value(), 0.0)) {
        imageRight.GetBorderImageSlice().NormalizeToPx(dipscale_, 0, 0, imageWidth_, rightSlice_);
    }
    if (GreatNotEqual(imageTop.GetBorderImageSlice().Value(), 0.0)) {
        imageTop.GetBorderImageSlice().NormalizeToPx(dipscale_, 0, 0, imageHeight_, topSlice_);
    }
    if (GreatNotEqual(imageBottom.GetBorderImageSlice().Value(), 0.0)) {
        imageBottom.GetBorderImageSlice().NormalizeToPx(dipscale_, 0, 0, imageHeight_, bottomSlice_);
    }
    if (GreatNotEqual(leftSlice_, imageWidth_)) {
        leftSlice_ = imageWidth_;
    }
    if (GreatNotEqual(rightSlice_, imageWidth_)) {
        rightSlice_ = imageWidth_;
    }
    if (GreatNotEqual(topSlice_, imageHeight_)) {
        topSlice_ = imageHeight_;
    }
    if (GreatNotEqual(bottomSlice_, imageHeight_)) {
        bottomSlice_ = imageHeight_;
    }
}

void BorderImagePainter::InitBorderImageOutset(Border& border, RefPtr<BorderImage>& borderImage)
{
    BorderImageEdge imageLeft = borderImage->GetBorderImageEdge(BorderImageDirection::LEFT);
    BorderImageEdge imageTop = borderImage->GetBorderImageEdge(BorderImageDirection::TOP);
    BorderImageEdge imageRight = borderImage->GetBorderImageEdge(BorderImageDirection::RIGHT);
    BorderImageEdge imageBottom = borderImage->GetBorderImageEdge(BorderImageDirection::BOTTOM);
    if (!decoration_->GetHasBorderImageOutset()) {
        leftOutset_ = 0.0;
        topOutset_ = 0.0;
        rightOutset_ = 0.0;
        bottomOutset_ = 0.0;
        return;
    }
    if (GreatNotEqual(imageLeft.GetBorderImageOutset().Value(), 0.0)) {
        imageLeft.GetBorderImageOutset().NormalizeToPx(dipscale_, 0, 0,
            border.Left().GetWidthInPx(dipscale_), leftOutset_);
    } else {
        border.Left().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), leftOutset_);
    }

    if (GreatNotEqual(imageRight.GetBorderImageOutset().Value(), 0.0)) {
        imageRight.GetBorderImageOutset().NormalizeToPx(dipscale_, 0, 0,
            border.Right().GetWidthInPx(dipscale_), rightOutset_);
    } else {
        border.Right().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Width(), rightOutset_);
    }

    if (GreatNotEqual(imageTop.GetBorderImageOutset().Value(), 0.0)) {
        imageTop.GetBorderImageOutset().NormalizeToPx(dipscale_, 0, 0,
            border.Top().GetWidthInPx(dipscale_), topOutset_);
    } else {
        border.Top().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), topOutset_);
    }

    if (GreatNotEqual(imageBottom.GetBorderImageOutset().Value(), 0.0)) {
        imageBottom.GetBorderImageOutset().NormalizeToPx(dipscale_, 0, 0,
            border.Bottom().GetWidthInPx(dipscale_), bottomOutset_);
    } else {
        border.Bottom().GetWidth().NormalizeToPx(dipscale_, 0, 0, paintSize_.Height(), bottomOutset_);
    }
}

void BorderImagePainter::PaintBorderImage(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    PaintBorderImageCorners(offset, canvas, paint);
    if (paintCornersOnly_) {
        return;
    }
    switch (decoration_->GetBorderImage()->GetRepeatMode()) {
        case BorderImageRepeat::STRETCH:
            PaintBorderImageStretch(offset, canvas, paint);
            break;
        case BorderImageRepeat::SPACE:
            PaintBorderImageSpace(offset, canvas, paint);
            break;
        case BorderImageRepeat::ROUND:
            PaintBorderImageRound(offset, canvas, paint);
            break;
        case BorderImageRepeat::REPEAT:
            PaintBorderImageRepeat(offset, canvas, paint);
            break;
        default:
            LOGE("Unsupported Border Image repeat mode");
    }
    if (decoration_->GetBorderImage()->GetNeedFillCenter()) {
        FillBorderImageCenter(offset, canvas, paint);
    }
    paint.reset();
}

void BorderImagePainter::FillBorderImageCenter(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    SkRect srcRectCenter = SkRect::MakeXYWH(leftSlice_, topSlice_,
        std::fabs(imageWidth_ - leftSlice_ - rightSlice_), std::fabs(imageHeight_ - topSlice_ - bottomSlice_));
    double horizontalBorderLength = paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_;
    double verticalBorderLength = paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_;
    SkRect desRectCenter = SkRect::MakeXYWH(
        offset.GetX() - leftOutset_ + leftWidth_,
        offset.GetY() - topOutset_ + topWidth_,
        horizontalBorderLength,
        verticalBorderLength);
    canvas->drawImageRect(image_, srcRectCenter, desRectCenter, &paint);
}

void BorderImagePainter::PaintBorderImageCorners(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    double offsetLeftX = offset.GetX() - leftOutset_;
    double offsetRightX = offset.GetX() + paintSize_.Width() + rightOutset_;
    double offsetTopY = offset.GetY() - topOutset_;
    double offsetBottomY = offset.GetY() + paintSize_.Height() + bottomOutset_;

    // top left corner
    SkRect srcRectLeftTop = SkRect::MakeXYWH(0, 0, leftSlice_, topSlice_);
    // top right corner
    SkRect srcRectRightTop = SkRect::MakeXYWH(imageWidth_ - rightSlice_, 0, rightSlice_, topSlice_);
    // left bottom corner
    SkRect srcRectLeftBottom = SkRect::MakeXYWH(0, imageHeight_ - bottomSlice_, leftSlice_, bottomSlice_);
    // right bottom corner
    SkRect srcRectRightBottom =
        SkRect::MakeXYWH(imageWidth_ - rightSlice_, imageHeight_ - bottomSlice_, rightSlice_, bottomSlice_);

    // Draw the four corners of the picture to the four corners of the border
    // left top
    SkRect desRectLeftTop = SkRect::MakeXYWH(offsetLeftX, offsetTopY, leftWidth_, topWidth_);
    canvas->drawImageRect(image_, srcRectLeftTop, desRectLeftTop, &paint);

    // right top
    SkRect desRectRightTop = SkRect::MakeXYWH(offsetRightX - rightWidth_, offsetTopY, rightWidth_, topWidth_);
    canvas->drawImageRect(image_, srcRectRightTop, desRectRightTop, &paint);

    // left bottom
    SkRect desRectLeftBottom =
        SkRect::MakeXYWH(offsetLeftX, offsetBottomY - bottomWidth_, leftWidth_, bottomWidth_);
    canvas->drawImageRect(image_, srcRectLeftBottom, desRectLeftBottom, &paint);

    // right bottom
    SkRect desRectRightBottom =
        SkRect::MakeXYWH(offsetRightX - rightWidth_, offsetBottomY - bottomWidth_,
            rightWidth_, bottomWidth_);
    canvas->drawImageRect(image_, srcRectRightBottom, desRectRightBottom, &paint);
}

void BorderImagePainter::PaintBorderImageStretch(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    double offsetLeftX = offset.GetX() - leftOutset_;
    double offsetRightX = offset.GetX() + paintSize_.Width() + rightOutset_;
    double offsetTopY = offset.GetY() - topOutset_;
    double offsetBottomY = offset.GetY() + paintSize_.Height() + bottomOutset_;
    SkRect desRectLeft =
        SkRect::MakeXYWH(offsetLeftX, offsetTopY + topWidth_, leftWidth_,
            paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_);
    canvas->drawImageRect(image_, srcRectLeft_, desRectLeft, &paint);

    SkRect desRectTop =
        SkRect::MakeXYWH(offsetLeftX + leftWidth_, offsetTopY,
            paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_, topWidth_);
    canvas->drawImageRect(image_, srcRectTop_, desRectTop, &paint);

    SkRect desRectRight =
        SkRect::MakeXYWH(offsetRightX - rightWidth_, offsetTopY + topWidth_,
            rightWidth_, paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_);
    canvas->drawImageRect(image_, srcRectRight_, desRectRight, &paint);

    SkRect desRectBottom =
        SkRect::MakeXYWH(offsetLeftX + leftWidth_, offsetBottomY - bottomWidth_,
            paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_, bottomWidth_);
    canvas->drawImageRect(image_, srcRectBottom_, desRectBottom, &paint);
}

void BorderImagePainter::PaintBorderImageRound(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    double offsetLeftX = offset.GetX() - leftOutset_;
    double offsetRightX = offset.GetX() + paintSize_.Width() + rightOutset_;
    double offsetTopY = offset.GetY() - topOutset_;
    double offsetBottomY = offset.GetY() + paintSize_.Height() + bottomOutset_;
    double horizontalBorderLength = paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_;
    double verticalBorderLength = paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_;

    double imageCenterWidth = imageWidth_ - leftSlice_ - rightSlice_;
    double imageCenterHeight = imageHeight_ - topSlice_ - bottomSlice_;

    int32_t roundHorizontalCount = static_cast<int32_t>(horizontalBorderLength / imageCenterWidth);
    int32_t roundVerticalCount = static_cast<int32_t>(verticalBorderLength / imageCenterHeight);

    // Surplus
    if (fmod(horizontalBorderLength, imageCenterWidth) != 0) {
        roundHorizontalCount += 1;
    }

    if (fmod(verticalBorderLength, imageCenterHeight) != 0) {
        roundVerticalCount += 1;
    }

    double roundImageWidth = horizontalBorderLength / roundHorizontalCount;
    double roundImageHeight = verticalBorderLength / roundVerticalCount;

    double roundStartHorizontal = offsetLeftX + leftWidth_;

    // draw shrinked border images on top and bottom edge
    for (int32_t i = 0; i < roundHorizontalCount; i++) {
        // top
        SkRect desRectTopRound =
            SkRect::MakeXYWH(roundStartHorizontal, offsetTopY, roundImageWidth, topWidth_);
        canvas->drawImageRect(image_, srcRectTop_, desRectTopRound, &paint);
        // bottom
        SkRect desRectBottomRound =
            SkRect::MakeXYWH(roundStartHorizontal, offsetBottomY - bottomWidth_, roundImageWidth, bottomWidth_);
        canvas->drawImageRect(image_, srcRectBottom_, desRectBottomRound, &paint);
        roundStartHorizontal += roundImageWidth;
    }
    double roundStartVertical = offsetTopY + topWidth_;
    // draw shrinked border images on left and right edge
    for (int32_t i = 0; i < roundVerticalCount; i++) {
        // left
        SkRect desRectLeftRound =
            SkRect::MakeXYWH(offsetLeftX, roundStartVertical, leftWidth_, roundImageHeight);
        canvas->drawImageRect(image_, srcRectLeft_, desRectLeftRound, &paint);
        // right
        SkRect desRectRightRound =
            SkRect::MakeXYWH(offsetRightX- rightWidth_, roundStartVertical, rightWidth_, roundImageHeight);
        canvas->drawImageRect(image_, srcRectRight_, desRectRightRound, &paint);
        roundStartVertical += roundImageHeight;
    }
}

void BorderImagePainter::PaintBorderImageSpace(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    double offsetLeftX = offset.GetX() - leftOutset_;
    double offsetRightX = offset.GetX() + paintSize_.Width() + rightOutset_;
    double offsetTopY = offset.GetY() - topOutset_;
    double offsetBottomY = offset.GetY() + paintSize_.Height() + bottomOutset_;
    double horizontalBorderLength = paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_;
    double verticalBorderLength = paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_;

    double imageCenterWidth = std::fabs(imageWidth_ - leftSlice_ - rightSlice_);
    double imageCenterHeight = std::fabs(imageHeight_ - topSlice_ - bottomSlice_);

    // calculate maximum count of image pieces can fit in border
    int32_t roundHorizontalCount = static_cast<int32_t>(horizontalBorderLength / imageCenterWidth);
    int32_t roundVerticalCount = static_cast<int32_t>(verticalBorderLength / imageCenterHeight);
    if (roundHorizontalCount == 0.0) {
        LOGW("Border image center width exceeds horizontal border center length, left and right side will not paint");
    }
    if (roundVerticalCount == 0.0) {
        LOGW("Border image center height exceeds vertical border center length, top and bottom side will not paint");
    }
    // fmod(horizontalBorderLength, imageCenterWidth) will return total blank length,
    // and there are roundHorizontalCount + 1 blanks
    double blankHorizontalSize = fmod(horizontalBorderLength, imageCenterWidth) / (roundHorizontalCount + 1);
    double blankVerticalSize = fmod(verticalBorderLength, imageCenterHeight) / (roundVerticalCount + 1);

    double roundStartHorizontal = offsetLeftX + leftWidth_ + blankHorizontalSize;
    for (int32_t i = 0; i < roundHorizontalCount; i++) {
        // top
        SkRect desRectTopRound =
            SkRect::MakeXYWH(roundStartHorizontal, offsetTopY, imageCenterWidth, topWidth_);
        canvas->drawImageRect(image_, srcRectTop_, desRectTopRound, &paint);
        // bottom
        SkRect desRectBottomRound =
            SkRect::MakeXYWH(roundStartHorizontal, offsetBottomY - bottomWidth_, imageCenterWidth, bottomWidth_);
        canvas->drawImageRect(image_, srcRectBottom_, desRectBottomRound, &paint);

        roundStartHorizontal += imageCenterWidth + blankHorizontalSize;
    }

    double roundStartVertical = offsetTopY + topWidth_ + blankVerticalSize;
    for (int32_t i = 0; i < roundVerticalCount; i++) {
        // left
        SkRect desRectLeftRound =
            SkRect::MakeXYWH(offsetLeftX, roundStartVertical, leftWidth_, imageCenterHeight);
        canvas->drawImageRect(image_, srcRectLeft_, desRectLeftRound, &paint);
        // right
        SkRect desRectRightRound =
            SkRect::MakeXYWH(offsetRightX - rightWidth_, roundStartVertical, rightWidth_, imageCenterHeight);
        canvas->drawImageRect(image_, srcRectRight_, desRectRightRound, &paint);
        roundStartVertical += imageCenterHeight + blankVerticalSize;
    }
}

void BorderImagePainter::PaintBorderImageRepeat(const Offset& offset, SkCanvas* canvas, SkPaint& paint)
{
    double imageWidth = image_->width();
    double imageHeight = image_->height();

    double offsetLeftX = offset.GetX() - leftOutset_;
    double offsetRightX = offset.GetX() + paintSize_.Width() + rightOutset_;
    double offsetTopY = offset.GetY() - topOutset_;
    double offsetBottomY = offset.GetY() + paintSize_.Height() + bottomOutset_;

    double horizontalBorderLength = paintSize_.Width() - leftWidth_ - rightWidth_ + leftOutset_ + rightOutset_;
    double imageCenterWidth = imageWidth - leftSlice_ - rightSlice_;
    double widthFactor = 0.0;
    if (GreatNotEqual(imageCenterWidth, 0.0)) {
        widthFactor = horizontalBorderLength / imageCenterWidth;
        if (GreatNotEqual(widthFactor, 0.0) && LessOrEqual(widthFactor, 1.0)) {
            double halfSurplusImageCenterWidth = (imageCenterWidth - horizontalBorderLength) / 2;
            SkRect srcRectTop = SkRect::MakeXYWH(halfSurplusImageCenterWidth + leftSlice_, 0,
                horizontalBorderLength, topSlice_);
            SkRect desRectTop =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_, offsetTopY, horizontalBorderLength, topWidth_);
            canvas->drawImageRect(image_, srcRectTop, desRectTop, &paint);

            SkRect srcRectBottom =
                SkRect::MakeXYWH(halfSurplusImageCenterWidth + leftSlice_, imageHeight - bottomSlice_,
                    horizontalBorderLength, bottomSlice_);
            SkRect desRectBottom =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_,
                    offset.GetY() + paintSize_.Height() - bottomWidth_ + bottomOutset_,
                    horizontalBorderLength, bottomWidth_);
            canvas->drawImageRect(image_, srcRectBottom, desRectBottom, &paint);
        } else if (GreatNotEqual(widthFactor, 1.0)) {
            double halfSurplusHorizontalLength = 0;
            halfSurplusHorizontalLength = (horizontalBorderLength - (int)(widthFactor) * imageCenterWidth) / 2;
            SkRect srcRectTopLeft = SkRect::MakeXYWH(imageWidth - rightSlice_ - halfSurplusHorizontalLength, 0,
                halfSurplusHorizontalLength, topSlice_);
            SkRect desRectTopLeftEnd =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_, offsetTopY, halfSurplusHorizontalLength, topWidth_);
            canvas->drawImageRect(image_, srcRectTopLeft, desRectTopLeftEnd, &paint);

            SkRect srcRectTopRight = SkRect::MakeXYWH(leftSlice_, 0, halfSurplusHorizontalLength, topSlice_);
            SkRect desRectTopRightEnd =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_ + horizontalBorderLength - halfSurplusHorizontalLength,
                    offsetTopY, halfSurplusHorizontalLength, topWidth_);
            canvas->drawImageRect(image_, srcRectTopRight, desRectTopRightEnd, &paint);

            SkRect srcRectBottomLeft = SkRect::MakeXYWH(imageWidth - rightSlice_ - halfSurplusHorizontalLength,
                imageHeight - bottomSlice_, halfSurplusHorizontalLength, bottomSlice_);
            SkRect desRectBottomLeftEnd =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_, offsetBottomY - bottomWidth_,
                    halfSurplusHorizontalLength, bottomWidth_);
            canvas->drawImageRect(image_, srcRectBottomLeft, desRectBottomLeftEnd, &paint);

            SkRect srcRectBottomRight =  SkRect::MakeXYWH(leftSlice_, imageHeight - bottomSlice_,
                halfSurplusHorizontalLength, bottomSlice_);
            SkRect desRectBottomRightEnd =
                SkRect::MakeXYWH(offsetLeftX + leftWidth_ + horizontalBorderLength - halfSurplusHorizontalLength,
                    offsetBottomY - bottomWidth_, halfSurplusHorizontalLength, bottomWidth_);
            canvas->drawImageRect(image_, srcRectBottomRight, desRectBottomRightEnd, &paint);

            double repeatHorizontalStart = offsetLeftX + leftWidth_ + halfSurplusHorizontalLength;
            for (int32_t i = 0; i < static_cast<int32_t>(widthFactor); i++) {
                // top
                SkRect desRectTopRepeat = SkRect::MakeXYWH(repeatHorizontalStart, offsetTopY,
                    imageCenterWidth, topWidth_);
                canvas->drawImageRect(image_, srcRectTop_, desRectTopRepeat, &paint);

                // bottom
                SkRect desRectBottomRepeat = SkRect::MakeXYWH(repeatHorizontalStart, offsetBottomY - bottomWidth_,
                    imageCenterWidth, bottomWidth_);
                canvas->drawImageRect(image_, srcRectBottom_, desRectBottomRepeat, &paint);

                repeatHorizontalStart += imageCenterWidth;
            }
        }
    }

    double verticalBorderLength = paintSize_.Height() - topWidth_ - bottomWidth_ + topOutset_ + bottomOutset_;
    double imageCenterHeight = imageHeight - topSlice_ - bottomSlice_;
    double heightFactor = 0.0;
    if (GreatNotEqual(imageCenterHeight, 0.0)) {
        heightFactor = verticalBorderLength / imageCenterHeight;
        if (GreatNotEqual(heightFactor, 0.0) && LessOrEqual(heightFactor, 1.0)) {
            double halfSurplusImageCenterHeight = (imageCenterHeight - verticalBorderLength) / 2;
            SkRect srcRectLeft =
                SkRect::MakeXYWH(0, topSlice_ + halfSurplusImageCenterHeight, leftSlice_, verticalBorderLength);
            SkRect desRectLeft =
                SkRect::MakeXYWH(offsetLeftX, offsetTopY + topWidth_, leftWidth_, verticalBorderLength);
            canvas->drawImageRect(image_, srcRectLeft, desRectLeft, &paint);

            SkRect srcRectRight =
                SkRect::MakeXYWH(imageWidth - rightSlice_, topSlice_ + halfSurplusImageCenterHeight, rightSlice_,
                    verticalBorderLength);
            SkRect desRectRight =
                SkRect::MakeXYWH(offset.GetX() + paintSize_.Width() - rightWidth_ + rightOutset_,
                    offset.GetY() - topOutset_ + topWidth_, rightWidth_, verticalBorderLength);
            canvas->drawImageRect(image_, srcRectRight, desRectRight, &paint);
        } else if (GreatNotEqual(heightFactor, 1.0)) {
            double halfSurplusVerticalLength = 0;
            halfSurplusVerticalLength = (verticalBorderLength - (int)(heightFactor) * imageCenterHeight) / 2;
            SkRect srcRectLeftTop =
                SkRect::MakeXYWH(0, imageHeight - bottomSlice_ - halfSurplusVerticalLength,
                    leftSlice_, halfSurplusVerticalLength);
            SkRect desRectLeftTopStart =
                SkRect::MakeXYWH(offsetLeftX, offsetTopY + topWidth_, leftWidth_, halfSurplusVerticalLength);
            canvas->drawImageRect(image_, srcRectLeftTop, desRectLeftTopStart, &paint);

            SkRect srcRectRightTop =
                SkRect::MakeXYWH(imageWidth - rightSlice_, imageHeight - bottomSlice_ - halfSurplusVerticalLength,
                    rightSlice_, halfSurplusVerticalLength);
            SkRect desRectRightTopStart =
                SkRect::MakeXYWH(offsetRightX - rightWidth_, offsetTopY + topWidth_,
                    rightWidth_, halfSurplusVerticalLength);
            canvas->drawImageRect(image_, srcRectRightTop, desRectRightTopStart, &paint);

            SkRect srcRectLeftBottom = SkRect::MakeXYWH(0, topSlice_, leftSlice_, halfSurplusVerticalLength);
            SkRect desRectLeftBottomEnd = SkRect::MakeXYWH(offsetLeftX,
                offsetBottomY - bottomWidth_ - halfSurplusVerticalLength, leftWidth_, halfSurplusVerticalLength);
            canvas->drawImageRect(image_, srcRectLeftBottom, desRectLeftBottomEnd, &paint);

            SkRect srcRectRightBottom =
                SkRect::MakeXYWH(imageWidth - rightSlice_, topSlice_, rightSlice_, halfSurplusVerticalLength);
            SkRect desRectRightBottomEnd = SkRect::MakeXYWH(offsetRightX - rightWidth_,
                offsetBottomY - bottomWidth_ - halfSurplusVerticalLength, rightWidth_, halfSurplusVerticalLength);
            canvas->drawImageRect(image_, srcRectRightBottom, desRectRightBottomEnd, &paint);

            double repeatVerticalStart = offsetTopY + topWidth_ + halfSurplusVerticalLength;
            for (int32_t i = 0; i < static_cast<int32_t>(heightFactor); i++) {
                // left
                SkRect desRectLeftRepeat =
                    SkRect::MakeXYWH(offsetLeftX, repeatVerticalStart, leftWidth_, imageCenterHeight);
                canvas->drawImageRect(image_, srcRectLeft_, desRectLeftRepeat, &paint);

                // right
                SkRect desRectRightRepeat =
                    SkRect::MakeXYWH(offsetRightX - rightWidth_, repeatVerticalStart, rightWidth_, imageCenterHeight);
                canvas->drawImageRect(image_, srcRectRight_, desRectRightRepeat, &paint);

                repeatVerticalStart += imageCenterHeight;
            }
        }
    }
    paint.reset();
}
}