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

#include "core/components_ng/pattern/list/list_paint_method.h"

#include "core/components_ng/pattern/scroll/inner/scroll_bar_painter.h"
#include "core/components_ng/render/divider_painter.h"

namespace OHOS::Ace::NG {
void ListPaintMethod::PaintDivider(const DividerInfo& dividerInfo, const PositionMap& itemPosition, RSCanvas& canvas)
{
    float laneLen = dividerInfo.crossSize / dividerInfo.lanes - dividerInfo.startMargin - dividerInfo.endMargin;
    float crossLen = dividerInfo.crossSize - dividerInfo.startMargin - dividerInfo.endMargin;
    DividerPainter dividerPainter(dividerInfo.constrainStrokeWidth, crossLen,
        dividerInfo.isVertical, dividerInfo.color, LineCap::SQUARE);

    int32_t lanes = dividerInfo.lanes;
    int32_t laneIdx = 0;
    bool lastIsItemGroup = false;
    bool isFirstItem = (itemPosition.begin()->first == 0);
    std::list<int32_t> lastLineIndex;

    for (const auto& child : itemPosition) {
        if (!isFirstItem) {
            float mainPos = child.second.startPos - dividerInfo.halfSpaceWidth + dividerInfo.mainPadding;
            float crossPos = dividerInfo.startMargin + dividerInfo.crossPadding;
            if (lanes > 1 && !lastIsItemGroup && !child.second.isGroup) {
                crossPos += laneIdx * dividerInfo.crossSize / dividerInfo.lanes;
                dividerPainter.SetDividerLength(laneLen);
            } else {
                dividerPainter.SetDividerLength(crossLen);
            }
            OffsetF offset = dividerInfo.isVertical ? OffsetF(mainPos, crossPos) : OffsetF(crossPos, mainPos);
            dividerPainter.DrawLine(canvas, offset);
        }
        if (laneIdx == 0) {
            lastLineIndex.clear();
        }
        lastLineIndex.emplace_back(child.first);
        lastIsItemGroup = child.second.isGroup;
        laneIdx = (lanes <= 1 || (laneIdx + 1) >= lanes || child.second.isGroup) ? 0 : laneIdx + 1;
        isFirstItem = isFirstItem ? laneIdx > 0 : false;
    }
    if (!lastLineIndex.empty() && *lastLineIndex.rbegin() < dividerInfo.totalItemCount - 1) {
        int32_t laneIdx = 0;
        for (auto index : lastLineIndex) {
            float mainPos = itemPosition.at(index).endPos + dividerInfo.halfSpaceWidth + dividerInfo.mainPadding;
            float crossPos = dividerInfo.startMargin + dividerInfo.crossPadding;
            if (lanes > 1 && !itemPosition.at(index).isGroup) {
                crossPos += laneIdx * dividerInfo.crossSize / dividerInfo.lanes;
                dividerPainter.SetDividerLength(laneLen);
            } else {
                dividerPainter.SetDividerLength(crossLen);
            }
            OffsetF offset = dividerInfo.isVertical ? OffsetF(mainPos, crossPos) : OffsetF(crossPos, mainPos);
            dividerPainter.DrawLine(canvas, offset);
            laneIdx++;
        }
    }
}

void ListPaintMethod::PaintDivider(PaintWrapper* paintWrapper, RSCanvas& canvas)
{
    if (!divider_.strokeWidth.IsValid()) {
        return;
    }
    const auto& geometryNode = paintWrapper->GetGeometryNode();
    auto frameSize = geometryNode->GetPaddingSize();
    OffsetF paddingOffset = geometryNode->GetPaddingOffset() - geometryNode->GetFrameOffset();
    Axis axis = vertical_ ? Axis::VERTICAL : Axis::HORIZONTAL;
    DividerInfo dividerInfo = {
        .constrainStrokeWidth = divider_.strokeWidth.ConvertToPx(),
        .crossSize = vertical_ ? frameSize.Height() : frameSize.Width(),
        .startMargin = divider_.startMargin.ConvertToPx(),
        .endMargin = divider_.endMargin.ConvertToPx(),
        .halfSpaceWidth = (space_ + divider_.strokeWidth.ConvertToPx()) / 2.0f, /* 2.0f half */
        .mainPadding = paddingOffset.GetMainOffset(axis),
        .crossPadding = paddingOffset.GetCrossOffset(axis),
        .isVertical = vertical_,
        .lanes = lanes_ > 1 ? lanes_ : 1,
        .totalItemCount = totalItemCount_,
        .color = divider_.color
    };
    PaintDivider(dividerInfo, itemPosition_, canvas);
}

void ListPaintMethod::PaintScrollBar(RSCanvas& canvas)
{
    auto scrollBar = scrollBar_.Upgrade();
    CHECK_NULL_VOID(scrollBar);
    ScrollBarPainter::PaintRectBar(canvas, scrollBar, UINT8_MAX);
}

void ListPaintMethod::PaintEdgeEffect(PaintWrapper* paintWrapper, RSCanvas& canvas)
{
    CHECK_NULL_VOID(paintWrapper);
    auto edgeEffect = edgeEffect_.Upgrade();
    CHECK_NULL_VOID(edgeEffect);
    auto frameSize = paintWrapper->GetGeometryNode()->GetFrameSize();
    edgeEffect->Paint(canvas, frameSize, { 0.0f, 0.0f });
}

CanvasDrawFunction ListPaintMethod::GetForegroundDrawFunction(PaintWrapper* paintWrapper)
{
    auto paintFunc = [weak = WeakClaim(this), paintWrapper](RSCanvas& canvas) {
        auto painter = weak.Upgrade();
        CHECK_NULL_VOID(painter);
        painter->PaintDivider(paintWrapper, canvas);
        painter->PaintScrollBar(canvas);
        painter->PaintEdgeEffect(paintWrapper, canvas);
    };
    return paintFunc;
}
} // namespace OHOS::Ace::NG