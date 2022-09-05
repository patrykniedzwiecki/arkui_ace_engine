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

#include "core/components_ng/pattern/progress/progress_paint_method.h"

#include <algorithm>

#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/log/log_wrapper.h"

namespace OHOS::Ace::NG {
void ProgressPaintMethod::PaintLinear(RSCanvas& canvas, const OffsetF& offset, const SizeF& frameSize) const
{
    PointF start = PointF(offset.GetX() + strokeWidth_ / 2, offset.GetY() + strokeWidth_ / 2);
    PointF end = PointF(offset.GetX() + frameSize.Width() - strokeWidth_ / 2, offset.GetY() + strokeWidth_ / 2);
    PointF end2 = PointF(offset.GetX() + strokeWidth_ / 2 + (frameSize.Width() - strokeWidth_) * value_ / maxValue_,
        offset.GetY() + strokeWidth_ / 2);
    RSPen pen;
    pen.SetAntiAlias(true);
    pen.SetWidth(strokeWidth_);
    pen.SetCapStyle(ToRSCapStyle(LineCap::ROUND));
    pen.SetColor(ToRSColor((Color::GRAY)));
    canvas.AttachPen(pen);
    if (end2 != end) {
        canvas.DrawLine(ToRSPonit(end2), ToRSPonit(end));
    }
    pen.SetColor(ToRSColor((color_)));
    canvas.AttachPen(pen);
    if (start != end2) {
        canvas.DrawLine(ToRSPonit(start), ToRSPonit(end2));
    }
}

void ProgressPaintMethod::PaintRing(RSCanvas& canvas, const OffsetF& offset, const SizeF& frameSize) const
{
    static int32_t totalDegree = 360;
    PointF centerPt = PointF(frameSize.Width() / 2, frameSize.Height() / 2) + offset;
    double radius = std::min(frameSize.Width() / 2, frameSize.Height() / 2) - strokeWidth_ / 2;
    RSPen pen;
    pen.SetAntiAlias(true);
    pen.SetWidth(strokeWidth_);
    pen.SetCapStyle(ToRSCapStyle(LineCap::ROUND));
    pen.SetColor(ToRSColor((Color::GRAY)));
    canvas.AttachPen(pen);
    canvas.DrawCircle(ToRSPonit(centerPt), radius);
    pen.SetColor(ToRSColor((color_)));
    canvas.AttachPen(pen);
    double angle = (value_ / maxValue_) * totalDegree;
    canvas.DrawArc(
        { centerPt.GetX() - radius, centerPt.GetY() - radius, centerPt.GetX() + radius, centerPt.GetY() + radius }, 270,
        angle);
}

void ProgressPaintMethod::PaintScaleRing(RSCanvas& canvas, const OffsetF& offset, const SizeF& frameSize) const
{
    static int32_t totalDegree = 360;
    PointF centerPt = PointF(frameSize.Width() / 2, frameSize.Height() / 2) + offset;
    double radius = std::min(frameSize.Width() / 2, frameSize.Height() / 2) - strokeWidth_ / 2;
    double pathDistance = 2.0 * M_PI * radius / scaleCount_;
    RSPen pen;
    Rosen::Drawing::Path path;
    double widthOfLine = std::min(scaleWidth_, pathDistance);
    path.AddRoundRect({ 0, 0, widthOfLine, strokeWidth_ }, 90, 90, rosen::PathDirection::CW_DIRECTION);
    pen.SetAntiAlias(true);
    pen.SetWidth(scaleWidth_);
    pen.SetCapStyle(ToRSCapStyle(LineCap::ROUND));
    pen.SetPathEffect(rosen::PathEffect::CreatePathDashEffect(path, pathDistance, 0.0f, rosen::PathDashStyle::ROTATE));
    pen.SetColor(ToRSColor((Color::GRAY)));
    canvas.AttachPen(pen);
    canvas.DrawArc(
        { centerPt.GetX() - radius, centerPt.GetY() - radius, centerPt.GetX() + radius, centerPt.GetY() + radius }, 270,
        totalDegree);
    pen.SetColor(ToRSColor((color_)));
    canvas.AttachPen(pen);
    double angle = (value_ / maxValue_) * totalDegree;
    canvas.DrawArc(
        { centerPt.GetX() - radius, centerPt.GetY() - radius, centerPt.GetX() + radius, centerPt.GetY() + radius }, 270,
        angle);
}

void ProgressPaintMethod::PaintMoon(RSCanvas& canvas, const OffsetF& offset, const SizeF& frameSize) const
{
    static int32_t totalDegree = 1;
    PointF centerPt = PointF(frameSize.Width() / 2, frameSize.Height() / 2) + offset;
    double radius = std::min(frameSize.Width() / 2, frameSize.Height() / 2);
    RSBrush brush;
    brush.SetAlpha(true);
    brush.SetColor(ToRSColor(Color::GRAY));
    double angle = (value_ / maxValue_) * totalDegree;
    Rosen::Drawing::Path path;
    canvas.AttachBrush(brush);
    canvas.DrawCircle(ToRSPonit(centerPt), radius);
    brush.SetColor(ToRSColor((color_)));
    canvas.AttachBrush(brush);
    path.AddArc(
        { centerPt.GetX() - radius, centerPt.GetY() - radius, centerPt.GetX() + radius, centerPt.GetY() + radius }, 90,
        180);
    if (angle <= 0.5) {
        double progressOffset = radius - radius * angle / 0.5;
        path.MoveTo(centerPt.GetX(), centerPt.GetY() - radius);
        path.AddArc({ centerPt.GetX() - progressOffset, centerPt.GetY() - radius, centerPt.GetX() + progressOffset,
                        centerPt.GetY() + radius },
            270, -180);
        canvas.DrawPath(path);
    } else {
        double progressOffset = radius * (angle - 0.5) / 0.5;
        path.MoveTo(centerPt.GetX(), centerPt.GetY() - radius);
        path.AddArc({ centerPt.GetX() - progressOffset, centerPt.GetY() - radius, centerPt.GetX() + progressOffset,
                        centerPt.GetY() + radius },
            270, 180);
        canvas.DrawPath(path);
    }
}

void ProgressPaintMethod::PaintCapsule(RSCanvas& canvas, const OffsetF& offset, const SizeF& frameSize) const
{
    static int32_t totalDegree = 1;
    double radius = std::min(frameSize.Width() / 2, frameSize.Height() / 2);
    double offsetX = offset.GetX();
    double offsetY = offset.GetY();
    double length = (value_ / maxValue_) * totalDegree * frameSize.Width();
    RSBrush brush;
    brush.SetAlpha(true);
    brush.SetColor(ToRSColor(Color::GRAY));
    Rosen::Drawing::Path path;
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(
        { { offsetX, offsetY, frameSize.Width() + offsetX, frameSize.Height() + offsetY },
            radius, radius });
    brush.SetColor(ToRSColor((color_)));
    canvas.AttachBrush(brush);
    path.AddArc({ offsetX, offsetY, 2*radius + offsetX, frameSize.Height() + offsetY }, 90, 180);
    if (LessNotEqual(length, radius)) {
        path.AddArc({ offsetX+length, offsetY, 2*radius - length + offsetX, frameSize.Height() + offsetY }, 270, -180);
    } else if (GreatNotEqual(length, frameSize.Width() - radius)) {
        path.AddRect({ offsetX + radius, offsetY, frameSize.Width() + offsetX - radius, frameSize.Height() + offsetY });
        path.AddArc({ offsetX+(frameSize.Width() - radius) * 2.0 - length, offsetY , offsetX+length,
                        frameSize.Height() + offsetY },
            270, 180);
    } else {
        path.AddRect({ radius + offsetX, offsetY, length + offsetX, frameSize.Height() + offsetY });
    }
    canvas.DrawPath(path);
}

} // namespace OHOS::Ace::NG