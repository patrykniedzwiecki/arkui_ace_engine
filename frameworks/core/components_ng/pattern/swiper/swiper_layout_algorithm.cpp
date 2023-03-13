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

#include "core/components_ng/pattern/swiper/swiper_layout_algorithm.h"

#include <algorithm>
#include <cstdint>

#include "base/geometry/axis.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/log/ace_trace.h"
#include "base/utils/utils.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/pattern/swiper/swiper_layout_property.h"
#include "core/components_ng/pattern/swiper/swiper_utils.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {

namespace {
constexpr Dimension INDICATOR_PADDING = 8.0_vp;
} // namespace

void SwiperLayoutAlgorithm::AddToItemRange(int32_t index)
{
    if (index != currentIndex_) {
        index = ClampIndex(index);
        itemRange_.insert(index);
    }
}

int32_t SwiperLayoutAlgorithm::ClampIndex(int32_t index)
{
    return isLoop_ ? (index + totalCount_) % totalCount_ : std::clamp(index, 0, totalCount_ - 1);
}

void SwiperLayoutAlgorithm::LoadItemWithDrag(float translateLength)
{
    if (NonPositive(translateLength) || NearZero(currentOffset_)) {
        return;
    }

    int32_t nextIndex = currentIndex_;
    auto loadItems = std::abs(static_cast<int32_t>(floorf(currentOffset_ / translateLength)));
    do {
        nextIndex = Positive(currentOffset_) ? (nextIndex - 1) : (nextIndex + 1);
        AddToItemRange(nextIndex);
        loadItems--;
    } while (loadItems >= 0);
}

void SwiperLayoutAlgorithm::InitInActiveItems(float translateLength)
{
    // inActiveItems collect items which exist in preItemRange_, but not exist in itemRange_,
    // need remove these items from wrapper.
    inActiveItems_.clear();

    // Collect the same item in itemRange_ and preItemRange_.
    std::vector<int32_t> intersection;
    set_intersection(itemRange_.begin(), itemRange_.end(), preItemRange_.begin(), preItemRange_.end(),
        inserter(intersection, intersection.begin()));

    // Collect difference items between preItemRange_ and intersection.
    set_difference(preItemRange_.begin(), preItemRange_.end(), intersection.begin(), intersection.end(),
        inserter(inActiveItems_, inActiveItems_.begin()));

    std::set<int32_t> activeItems;
    auto firstIndex = currentIndex_;
    if (!NearZero(currentOffset_) && Positive(translateLength)) {
        auto loadItems = std::abs(static_cast<int32_t>(floorf(currentOffset_ / translateLength)));
        firstIndex = Positive(currentOffset_) ? (firstIndex - loadItems - 1) : (firstIndex + loadItems - 1);
        firstIndex = ClampIndex(firstIndex);
    }

    auto displayCount = NearZero(currentOffset_) ? displayCount_ : displayCount_ + 1;
    for (int32_t i = 0; i <= displayCount; i++) {
        activeItems.insert(ClampIndex(firstIndex + i));
    }

    if (activeItems.empty()) {
        return;
    }

    set_difference(itemRange_.begin(), itemRange_.end(), activeItems.begin(), activeItems.end(),
        inserter(inActiveItems_, inActiveItems_.begin()));
}

void SwiperLayoutAlgorithm::InitItemRange(LayoutWrapper* layoutWrapper)
{
    itemRange_.clear();

    if (currentIndex_ < 0 || currentIndex_ > totalCount_ - 1) {
        currentIndex_ = 0;
    }

    CHECK_NULL_VOID(layoutWrapper);
    auto layoutProperty = AceType::DynamicCast<SwiperLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(layoutProperty);

    auto axis = layoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    auto itemSpace = SwiperUtils::GetItemSpace(layoutProperty);
    auto translateLength = axis == Axis::HORIZONTAL ? maxChildSize_.Width() : maxChildSize_.Height();
    translateLength += itemSpace;

    /* Load next index while swiping */
    LoadItemWithDrag(translateLength);

    if (startIndex_ <= endIndex_) {
        for (auto index = startIndex_; index <= endIndex_; ++index) {
            itemRange_.insert(index);
        }
    } else {
        for (auto index = startIndex_; index < totalCount_; ++index) {
            itemRange_.insert(index);
        }
        for (auto index = 0; index <= endIndex_; ++index) {
            itemRange_.insert(index);
        }
    }

    if (targetIndex_.has_value() && (targetIndex_.value() < startIndex_ || targetIndex_.value() > endIndex_)) {
        itemRange_.insert(targetIndex_.value());
    }

    InitInActiveItems(translateLength);
}

void SwiperLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto swiperLayoutProperty = AceType::DynamicCast<SwiperLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(swiperLayoutProperty);

    auto axis = swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    const auto& constraint = swiperLayoutProperty->GetLayoutConstraint();
    if (!constraint) {
        LOGE("fail to measure swiper due to layoutConstraint is nullptr");
        return;
    }

    auto isSingleCase =
        (swiperLayoutProperty->GetDisplayCount().has_value() && swiperLayoutProperty->GetDisplayCountValue() == 1) ||
        (!swiperLayoutProperty->GetDisplayCount().has_value() && SwiperUtils::IsStretch(swiperLayoutProperty));

    OptionalSizeF idealSize;
    auto measureType = isSingleCase ? MeasureType::MATCH_CONTENT : MeasureType::MATCH_PARENT_MAIN_AXIS;
    idealSize = CreateIdealSize(constraint.value(), axis, swiperLayoutProperty->GetMeasureType(measureType));

    auto padding = swiperLayoutProperty->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, idealSize);

    InitItemRange(layoutWrapper);

    // Measure children.
    auto layoutConstraint = SwiperUtils::CreateChildConstraint(swiperLayoutProperty, idealSize);
    auto crossSize = 0.0f;
    auto mainSize = 0.0f;
    for (const auto& index : itemRange_) {
        auto wrapper = layoutWrapper->GetOrCreateChildByIndex(index);
        if (!wrapper) {
            break;
        }
        wrapper->Measure(layoutConstraint);
        crossSize = std::max(crossSize, GetCrossAxisSize(wrapper->GetGeometryNode()->GetMarginFrameSize(), axis));
        mainSize = std::max(mainSize, GetMainAxisSize(wrapper->GetGeometryNode()->GetMarginFrameSize(), axis));
    }

    maxChildSize_ = axis == Axis::HORIZONTAL ? SizeF(mainSize, crossSize) : SizeF(crossSize, mainSize);

    if (isSingleCase) {
        // single case.
        idealSize.UpdateIllegalSizeWithCheck(maxChildSize_);
    } else {
        // multi case, update cross size.
        if (axis == Axis::HORIZONTAL) {
            if (!idealSize.Height()) {
                idealSize.SetHeight(crossSize);
            }
        } else {
            if (!idealSize.Width()) {
                idealSize.SetWidth(crossSize);
            }
        }
    }

    AddPaddingToSize(padding, idealSize);

    auto geometryNode = layoutWrapper->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    geometryNode->SetFrameSize(idealSize.ConvertToSizeT());

    // Measure swiper indicator
    if (swiperLayoutProperty->GetShowIndicatorValue(true)) {
        auto indicatorWrapper = layoutWrapper->GetOrCreateChildByIndex(totalCount_);
        if (indicatorWrapper) {
            auto indicatorLayoutConstraint = swiperLayoutProperty->CreateChildConstraint();
            indicatorLayoutConstraint.parentIdealSize = idealSize;
            indicatorWrapper->Measure(indicatorLayoutConstraint);
        }
    }
}

void SwiperLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto swiperLayoutProperty = AceType::DynamicCast<SwiperLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(swiperLayoutProperty);
    auto axis = swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    auto displayCount = swiperLayoutProperty->GetDisplayCount().value_or(1);
    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    auto itemWidth = (axis == Axis::HORIZONTAL ? (size.Width() / displayCount) : (size.Height() / displayCount));
    auto itemSpace = SwiperUtils::GetItemSpace(swiperLayoutProperty);
    auto padding = swiperLayoutProperty->CreatePaddingAndBorder();
    auto rightPadding = padding.right.value_or(0.0f);
    auto bottomPadding = padding.bottom.value_or(0.0f);
    OffsetF paddingOffset = { padding.left.value_or(0.0f), padding.top.value_or(0.0f) };

    // Effect when difference between current index and target index is greater than 1.
    // e.g. Current index is 0, call swipeTo method to jump to index 2,
    // change item's position only 0 and 2, ignore others.
    if (SwiperUtils::IsStretch(swiperLayoutProperty) && targetIndex_.has_value() &&
        std::abs(targetIndex_.value() - currentIndex_) > 1) {
        auto currentOffset = axis == Axis::HORIZONTAL ? OffsetF(currentOffset_, 0.0f) : OffsetF(0.0f, currentOffset_);
        auto itemSpaceOffset = (axis == Axis::HORIZONTAL ? OffsetF(itemSpace / 2, 0) : OffsetF(0, itemSpace / 2));
        // Layout current item.
        auto currentWrapper = layoutWrapper->GetOrCreateChildByIndex(currentIndex_);
        if (currentWrapper && currentWrapper->GetGeometryNode()) {
            currentWrapper->GetGeometryNode()->SetMarginFrameOffset(currentOffset + itemSpaceOffset + paddingOffset);
            currentWrapper->Layout();
        }

        // Layout target item.
        auto targetMainOffset = targetIndex_.value() > currentIndex_ ? itemWidth : -itemWidth;
        auto targetOffset =
            axis == Axis::HORIZONTAL ? OffsetF(targetMainOffset, 0.0f) : OffsetF(0.0f, targetMainOffset);
        auto targetWrapper = layoutWrapper->GetOrCreateChildByIndex(targetIndex_.value());
        if (targetWrapper && targetWrapper->GetGeometryNode()) {
            targetWrapper->GetGeometryNode()->SetMarginFrameOffset(
                currentOffset + targetOffset + itemSpaceOffset + paddingOffset);
            targetWrapper->Layout();
        }
        return;
    }

    // Layout children.
    // Split item range by current index.
    std::set<int32_t, std::greater<>> preItems;
    std::set<int32_t> nextItems;
    for (const auto& index : itemRange_) {
        if (index < currentIndex_) {
            preItems.insert(index);
        } else {
            nextItems.insert(index);
        }
    }

    // Layout items behind current item.
    OffsetF preOffset =
        (axis == Axis::HORIZONTAL ? OffsetF(-itemSpace + currentOffset_, 0) : OffsetF(0, -itemSpace + currentOffset_));
    for (const auto& index : preItems) {
        auto wrapper = layoutWrapper->GetOrCreateChildByIndex(index);
        if (!wrapper) {
            continue;
        }
        auto geometryNode = wrapper->GetGeometryNode();
        preOffset -=
            (axis == Axis::HORIZONTAL ? OffsetF(maxChildSize_.Width(), 0) : OffsetF(0, maxChildSize_.Height()));
        geometryNode->SetMarginFrameOffset(preOffset + paddingOffset);
        wrapper->Layout();
        preOffset -= (axis == Axis::HORIZONTAL ? OffsetF(itemSpace, 0) : OffsetF(0, itemSpace));
        preOffset += (axis == Axis::HORIZONTAL ? OffsetF(rightPadding, 0) : OffsetF(0, bottomPadding));
    }

    // Layout items after current item.
    OffsetF nextOffset = (axis == Axis::HORIZONTAL ? OffsetF(currentOffset_, 0) : OffsetF(0, currentOffset_));
    for (const auto& index : nextItems) {
        auto wrapper = layoutWrapper->GetOrCreateChildByIndex(index);
        if (!wrapper) {
            continue;
        }
        auto geometryNode = wrapper->GetGeometryNode();
        geometryNode->SetMarginFrameOffset(nextOffset + paddingOffset);
        wrapper->Layout();
        nextOffset += (axis == Axis::HORIZONTAL ? OffsetF(maxChildSize_.Width() + itemSpace, 0)
                                                : OffsetF(0, maxChildSize_.Height() + itemSpace));
        nextOffset += (axis == Axis::HORIZONTAL ? OffsetF(rightPadding, 0) : OffsetF(0, bottomPadding));
    }

    // Layout swiper indicator
    if (swiperLayoutProperty->GetShowIndicatorValue(true)) {
        auto indicatorWrapper = layoutWrapper->GetOrCreateChildByIndex(totalCount_);
        if (indicatorWrapper) {
            if (swiperLayoutProperty->GetIndicatorTypeValue(SwiperIndicatorType::DOT) == SwiperIndicatorType::DIGIT) {
                PlaceDigitChild(indicatorWrapper, swiperLayoutProperty);
            }
            indicatorWrapper->Layout();
        }
    }

    // Mark inactive in wrapper.
    for (const auto& index : inActiveItems_) {
        if (!NearZero(currentOffset_)) {
            continue;
        }

        if (swiperLayoutProperty->GetShowIndicatorValue(true) && index == totalCount_) {
            continue;
        }
        layoutWrapper->RemoveChildInRenderTree(index);
    }

    // Adjust offset when looped.
    if (!isLoop_ || (totalCount_ <= displayCount)) {
        return;
    }
    LOGD("current index is %{public}d, and current offset is %{public}f", currentIndex_, currentOffset_);
    if (currentIndex_ <= (displayCount_ - 1) && GreatOrEqual(currentOffset_, 0.0)) {
        for (int32_t index = 0; index < displayCount_; ++index) {
            if ((totalCount_ - index - 1) == currentIndex_) {
                break;
            }
            auto lastWrapper = layoutWrapper->GetOrCreateChildByIndex(totalCount_ - index - 1);
            if (lastWrapper) {
                auto geometryNode = lastWrapper->GetGeometryNode();
                preOffset -=
                    (axis == Axis::HORIZONTAL ? OffsetF(maxChildSize_.Width(), 0) : OffsetF(0, maxChildSize_.Height()));
                geometryNode->SetMarginFrameOffset(preOffset + paddingOffset);
                lastWrapper->Layout();
                preOffset -= (axis == Axis::HORIZONTAL ? OffsetF(itemSpace, 0) : OffsetF(0, itemSpace));
                preOffset += (axis == Axis::HORIZONTAL ? OffsetF(rightPadding, 0) : OffsetF(0, bottomPadding));
            }
        }
    } else if (currentIndex_ >= (totalCount_ - displayCount_) && LessOrEqual(currentOffset_, 0.0)) {
        for (int32_t index = 0; index < displayCount_; ++index) {
            if (index == currentIndex_) {
                break;
            }
            auto firstWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
            if (firstWrapper) {
                auto geometryNode = firstWrapper->GetGeometryNode();
                geometryNode->SetMarginFrameOffset(nextOffset + paddingOffset);
                firstWrapper->Layout();
                nextOffset += (axis == Axis::HORIZONTAL ? OffsetF(maxChildSize_.Width() + itemSpace, 0)
                                                        : OffsetF(0, maxChildSize_.Height() + itemSpace));
                nextOffset += (axis == Axis::HORIZONTAL ? OffsetF(rightPadding, 0) : OffsetF(0, bottomPadding));
            }
        }
    }
}

void SwiperLayoutAlgorithm::PlaceDigitChild(
    const RefPtr<LayoutWrapper>& indicatorWrapper, const RefPtr<LayoutProperty>& layoutProperty)
{
    auto swiperLayoutProperty = AceType::DynamicCast<SwiperLayoutProperty>(layoutProperty);
    CHECK_NULL_VOID(swiperLayoutProperty);
    auto indicatorGeometryNode = indicatorWrapper->GetGeometryNode();
    CHECK_NULL_VOID(indicatorGeometryNode);
    auto indicatorWidth = INDICATOR_PADDING.ConvertToPx() * 2.0;
    auto indicatorHeight = 0.0f;
    for (auto&& child : indicatorWrapper->GetAllChildrenWithBuild()) {
        auto textGeometryNode = child->GetGeometryNode();
        CHECK_NULL_VOID(textGeometryNode);
        auto textFrameSize = textGeometryNode->GetFrameSize();
        indicatorWidth += textFrameSize.Width();
        if (indicatorHeight < textFrameSize.Height()) {
            indicatorHeight = textFrameSize.Height();
        }
    }
    auto layoutPropertyConstraint = indicatorWrapper->GetLayoutProperty();
    CHECK_NULL_VOID(layoutPropertyConstraint);
    const auto& layoutConstraint = layoutPropertyConstraint->GetLayoutConstraint();
    auto swiperWidth = layoutConstraint->parentIdealSize.Width().value();
    auto swiperHeight = layoutConstraint->parentIdealSize.Height().value();
    auto left = swiperLayoutProperty->GetLeft();
    auto right = swiperLayoutProperty->GetRight();
    auto top = swiperLayoutProperty->GetTop();
    auto bottom = swiperLayoutProperty->GetBottom();
    auto axis = swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    Offset position;
    Dimension indicatorPositionDefault = 0.0_vp;
    if (left.has_value() && !NearEqual(left->ConvertToPx(), indicatorPositionDefault.ConvertToPx())) {
        auto leftValue = GetValidEdgeLength(swiperWidth, indicatorWidth, Dimension(left->Value()));
        position.SetX(leftValue);
    } else if (right.has_value() && !NearEqual(right->ConvertToPx(), indicatorPositionDefault.ConvertToPx())) {
        auto rightValue = GetValidEdgeLength(swiperWidth, indicatorWidth, Dimension(right->Value()));
        position.SetX(swiperWidth - indicatorWidth - rightValue);
    } else {
        position.SetX(axis == Axis::HORIZONTAL ? (swiperWidth - indicatorWidth) * 0.5 : swiperWidth - indicatorWidth);
    }
    if (top.has_value() && !NearEqual(top->ConvertToPx(), indicatorPositionDefault.ConvertToPx())) {
        auto topValue = GetValidEdgeLength(swiperHeight, indicatorHeight, Dimension(top->Value()));
        position.SetY(topValue);
    } else if (bottom.has_value() && !NearEqual(bottom->ConvertToPx(), indicatorPositionDefault.ConvertToPx())) {
        auto bottomValue = GetValidEdgeLength(swiperHeight, indicatorHeight, Dimension(bottom->Value()));
        position.SetY(swiperHeight - indicatorHeight - bottomValue);
    } else {
        if (axis == Axis::HORIZONTAL) {
            position.SetY(swiperHeight - indicatorHeight);
        } else {
            position.SetY((swiperHeight - indicatorHeight) * 0.5);
        }
    }
    auto currentOffset = OffsetF{ static_cast<float>(position.GetX()), static_cast<float>(position.GetY()) };
    indicatorGeometryNode->SetMarginFrameOffset(currentOffset);
}

double SwiperLayoutAlgorithm::GetValidEdgeLength(float swiperLength, float indicatorLength, const Dimension& edge)
{
    double edgeLength = edge.Unit() == DimensionUnit::PERCENT ? swiperLength * edge.Value() : edge.ConvertToPx();
    if (!NearZero(edgeLength) && edgeLength > swiperLength - indicatorLength) {
        edgeLength = swiperLength - indicatorLength;
    }
    if (edgeLength < 0.0) {
        edgeLength = 0.0;
    }
    return edgeLength;
}

} // namespace OHOS::Ace::NG
