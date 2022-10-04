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

#include "core/components_ng/pattern/flex/flex_layout_algorithm.h"

#include "base/geometry/axis.h"
#include "base/log/ace_trace.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/flex/flex_layout_property.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {

/**
 * Get the main axis direction based on direction.
 */
FlexDirection FlipAxis(FlexDirection direction)
{
    if (direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE) {
        return FlexDirection::COLUMN;
    }
    return FlexDirection::ROW;
}

/**
 * Determine whether to start the layout from the upper left corner
 */
bool IsStartTopLeft(FlexDirection direction, TextDirection textDirection)
{
    switch (direction) {
        case FlexDirection::ROW:
            return textDirection == TextDirection::LTR;
        case FlexDirection::ROW_REVERSE:
            return textDirection == TextDirection::RTL;
        case FlexDirection::COLUMN:
            return true;
        case FlexDirection::COLUMN_REVERSE:
            return false;
        default:
            return true;
    }
}

float GetCrossAxisSizeHelper(const SizeF& size, FlexDirection direction)
{
    if (direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE) {
        return size.Height();
    }
    return size.Width();
}

float GetMainAxisSizeHelper(const SizeF& size, FlexDirection direction)
{
    if (direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE) {
        return size.Width();
    }
    return size.Height();
}

OptionalSizeF GetCalcSizeHelper(float mainAxisSize, float crossAxisSize, FlexDirection direction)
{
    OptionalSizeF size;
    if (direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE) {
        size.SetWidth(mainAxisSize);
        size.SetHeight(crossAxisSize);
    } else {
        size.SetHeight(mainAxisSize);
        size.SetWidth(crossAxisSize);
    }
    return size;
}

float FlexLayoutAlgorithm::GetChildMainAxisSize(const RefPtr<LayoutWrapper>& layoutWrapper) const
{
    float size = 0.0f;
    if (!layoutWrapper) {
        return size;
    }
    return GetMainAxisSizeHelper(layoutWrapper->GetGeometryNode()->GetMarginFrameSize(), direction_);
}

float FlexLayoutAlgorithm::GetChildCrossAxisSize(const RefPtr<LayoutWrapper>& layoutWrapper) const
{
    if (!layoutWrapper) {
        return 0.0f;
    }
    return GetCrossAxisSizeHelper(layoutWrapper->GetGeometryNode()->GetMarginFrameSize(), direction_);
}

float FlexLayoutAlgorithm::GetSelfCrossAxisSize(const RefPtr<LayoutWrapper>& layoutWrapper) const
{
    if (!layoutWrapper) {
        return 0.0f;
    }
    return GetCrossAxisSizeHelper(layoutWrapper->GetGeometryNode()->GetFrameSize(), direction_);
}

void FlexLayoutAlgorithm::CheckSizeValidity(const RefPtr<LayoutWrapper>& layoutWrapper)
{
    if (!layoutWrapper->GetGeometryNode()->GetMarginFrameSize().IsPositive()) {
        return;
    }
    ++validSizeCount_;
}

/**
 * Check and record baseline distance.
 */
void FlexLayoutAlgorithm::CheckBaselineProperties(const RefPtr<LayoutWrapper>& layoutWrapper)
{
    bool isChildBaselineAlign = false;
    const auto& flexItemProperty = layoutWrapper->GetLayoutProperty()->GetFlexItemProperty();
    isChildBaselineAlign =
        flexItemProperty ? flexItemProperty->GetAlignSelf().value_or(crossAxisAlign_) == FlexAlign::BASELINE : false;
    if (crossAxisAlign_ == FlexAlign::BASELINE || isChildBaselineAlign) {
        float distance = layoutWrapper->GetBaselineDistance();
        baselineProperties_.maxBaselineDistance = std::max(baselineProperties_.maxBaselineDistance, distance);
        baselineProperties_.maxDistanceAboveBaseline = std::max(baselineProperties_.maxDistanceAboveBaseline, distance);
        baselineProperties_.maxDistanceBelowBaseline =
            std::max(baselineProperties_.maxDistanceBelowBaseline, GetSelfCrossAxisSize(layoutWrapper) - distance);
        if (crossAxisAlign_ == FlexAlign::BASELINE) {
            crossAxisSize_ =
                baselineProperties_.maxDistanceAboveBaseline + baselineProperties_.maxDistanceBelowBaseline;
        }
    }
}

/**
 * Initialize the FlexLayoutAlgorithm property.
 */
void FlexLayoutAlgorithm::InitFlexProperties(LayoutWrapper* layoutWrapper)
{
    mainAxisSize_ = 0.0f;
    crossAxisSize_ = 0.0f;
    allocatedSize_ = 0.0f;
    totalFlexWeight_ = 0.0f;
    maxDisplayPriority_ = 0;
    layoutWrapper_ = layoutWrapper;
    auto layoutProperty = AceType::DynamicCast<FlexLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(layoutProperty);
    direction_ = layoutProperty->GetFlexDirection().value_or(FlexDirection::ROW);
    mainAxisAlign_ = layoutProperty->GetMainAxisAlignValue(FlexAlign::FLEX_START);
    secondaryMeasureList_.clear();
    crossAxisAlign_ = layoutProperty->GetCrossAxisAlignValue(FlexAlign::FLEX_START);
    baselineProperties_.Reset();
    TravelChildrenFlexProps(layoutWrapper);
}

void FlexLayoutAlgorithm::TravelChildrenFlexProps(LayoutWrapper* layoutWrapper)
{
    const auto& layoutProperty = layoutWrapper->GetLayoutProperty();
    const auto& children = layoutWrapper->GetAllChildrenWithBuild();
    for (const auto& child : children) {
        const auto& childLayoutProperty = child->GetLayoutProperty();
        const auto& childMagicItemProperty = childLayoutProperty->GetMagicItemProperty();
        const auto& childFlexItemProperty = childLayoutProperty->GetFlexItemProperty();
        MagicLayoutNode node;
        auto childLayoutConstraint = layoutProperty->CreateChildConstraint();
        node.layoutWrapper = child;
        node.layoutConstraint = childLayoutConstraint;
        int32_t childDisplayPriority = 1;
        float childLayoutWeight = 0.0f;
        if (childMagicItemProperty) {
            childLayoutWeight = childMagicItemProperty->GetLayoutWeight().value_or(0.0f);
        }
        if (childFlexItemProperty) {
            childDisplayPriority = childFlexItemProperty->GetDisplayIndex().value_or(1);
        }
        if (!magicNodes_.count(childDisplayPriority)) {
            magicNodes_.insert(
                std::map<int32_t, std::list<MagicLayoutNode>>::value_type(childDisplayPriority, { node }));
            if (GreatNotEqual(childLayoutWeight, 0.0f)) {
                magicNodeWeights_.insert(std::map<int32_t, float>::value_type(childDisplayPriority, childLayoutWeight));
            }
        } else {
            magicNodes_[childDisplayPriority].emplace_back(node);
            if (GreatNotEqual(childLayoutWeight, 0.0f)) {
                magicNodeWeights_[childDisplayPriority] += childLayoutWeight;
            }
        }
        totalFlexWeight_ += GreatNotEqual(childLayoutWeight, 0.0f) ? childLayoutWeight : 0.0f;
        maxDisplayPriority_ = std::max(childDisplayPriority, maxDisplayPriority_);
    }
}

void FlexLayoutAlgorithm::ResizeFlexSizeByItem(const RefPtr<LayoutWrapper>& childLayoutWrapper)
{
    float mainAxisSize = GetChildMainAxisSize(childLayoutWrapper);
    if (GreatOrEqual(mainAxisSize, Infinity<float>())) {
        mainAxisSize = 0.0f;
    }
    crossAxisSize_ = std::max(crossAxisSize_, GetChildCrossAxisSize(childLayoutWrapper));
    allocatedSize_ += mainAxisSize;
    allocatedSize_ += space_;
}

void FlexLayoutAlgorithm::MeasureAndCleanMagicNodes(FlexItemProperties& flexItemProperties)
{
    if (GreatNotEqual(totalFlexWeight_, 0.0f)) {
        /**
         * The child elements with layoutWeight=0 are measured first.
         * Then, measure the sub elements of layoutWeight>1 based on the remaining space.
         * If the total main axis size of the element is larger than the main axis size of Flex, the lower priority
         * element will be deleted.
         */
        auto firstLoopIter = magicNodes_.rbegin();
        while (firstLoopIter != magicNodes_.rend()) {
            auto& childList = firstLoopIter->second;
            for (auto& child : childList) {
                auto childLayoutWrapper = child.layoutWrapper;
                auto childConstraint = child.layoutConstraint;
                float childLayoutWeight = 0.0f;
                const auto& childMagicItemProperty = childLayoutWrapper->GetLayoutProperty()->GetMagicItemProperty();
                if (childMagicItemProperty) {
                    childLayoutWeight = childMagicItemProperty->GetLayoutWeightValue();
                }
                if (LessOrEqual(childLayoutWeight, 0.0f)) {
                    childLayoutWrapper->Measure(childConstraint);
                    ResizeFlexSizeByItem(childLayoutWrapper);
                    CheckSizeValidity(childLayoutWrapper);
                    CheckBaselineProperties(childLayoutWrapper);
                }
            }
            /**
             * The main axis size of the element with layoutWeight of 0 is larger than the Flex main axis size
             */
            if (allocatedSize_ - space_ > mainAxisSize_) {
                for (const auto& child : childList) {
                    allocatedSize_ -= GetChildCrossAxisSize(child.layoutWrapper);
                    allocatedSize_ -= space_;
                    child.layoutWrapper->SetActive(false);
                    child.layoutWrapper->GetGeometryNode()->SetFrameSize(SizeF());
                }
                break;
            }
            firstLoopIter++;
        }
        auto remainedMainAxisSize = mainAxisSize_ - allocatedSize_;
        auto spacePerWeight = remainedMainAxisSize / totalFlexWeight_;
        auto secondIterLoop = magicNodes_.rbegin();
        while (secondIterLoop != firstLoopIter) {
            auto& childList = secondIterLoop->second;
            bool isExceed = false;
            for (auto& child : childList) {
                auto childLayoutWrapper = child.layoutWrapper;
                auto& childConstraint = child.layoutConstraint;
                float childLayoutWeight = 0.0f;
                const auto& childMagicItemProperty = childLayoutWrapper->GetLayoutProperty()->GetMagicItemProperty();
                if (childMagicItemProperty) {
                    childLayoutWeight = childMagicItemProperty->GetLayoutWeightValue();
                }
                if (LessOrEqual(childLayoutWeight, 0.0)) {
                    continue;
                }
                float childCalcSize = spacePerWeight * childLayoutWeight;
                if (GetMainAxisSizeHelper(childConstraint.minSize, direction_) > childCalcSize) {
                    isExceed = true;
                }
                UpdateLayoutConstraintOnMainAxis(childConstraint, childCalcSize);
            }
            if (isExceed) {
                if (magicNodes_.size() <= 1) {
                    break;
                }
                isExceed = true;
                auto& lowPriorityChildList = magicNodes_.begin()->second;
                for (const auto& child : lowPriorityChildList) {
                    allocatedSize_ -= GetChildCrossAxisSize(child.layoutWrapper);
                    allocatedSize_ -= space_;
                    child.layoutWrapper->SetActive(false);
                    child.layoutWrapper->GetGeometryNode()->SetFrameSize(SizeF());
                }
                totalFlexWeight_ -= magicNodeWeights_[magicNodes_.begin()->first];
                remainedMainAxisSize = mainAxisSize_ - allocatedSize_;
                spacePerWeight = remainedMainAxisSize / totalFlexWeight_;
                isExceed = false;
                magicNodes_.erase(magicNodes_.begin());
                secondIterLoop = magicNodes_.rbegin();
            } else {
                secondIterLoop++;
            }
        }
        auto iter = magicNodes_.rbegin();
        while (iter != magicNodes_.rend()) {
            auto& childList = iter->second;
            for (auto& child : childList) {
                auto childLayoutWrapper = child.layoutWrapper;
                auto childLayoutConstraint = child.layoutConstraint;
                float childLayoutWeight = 0.0f;
                const auto& childMagicItemProperty = childLayoutWrapper->GetLayoutProperty()->GetMagicItemProperty();
                if (childMagicItemProperty) {
                    childLayoutWeight = childMagicItemProperty->GetLayoutWeightValue();
                }
                secondaryMeasureList_.emplace_back(child);
                if (LessOrEqual(childLayoutWeight, 0.0)) {
                    continue;
                }
                childLayoutWrapper->Measure(childLayoutConstraint);
            }
            iter++;
        }
    } else if (GreatNotEqual(maxDisplayPriority_, 1)) {
        auto iter = magicNodes_.rbegin();
        while (iter != magicNodes_.rend()) {
            auto childList = iter->second;
            for (auto& child : childList) {
                auto childLayoutWrapper = child.layoutWrapper;
                auto childLayoutConstraint = child.layoutConstraint;
                childLayoutWrapper->Measure(childLayoutConstraint);
                ResizeFlexSizeByItem(childLayoutWrapper);
                CheckSizeValidity(childLayoutWrapper);
                CheckBaselineProperties(childLayoutWrapper);
                const auto& flexItemProperty = childLayoutWrapper->GetLayoutProperty()->GetFlexItemProperty();
                if (flexItemProperty && GreatNotEqual(flexItemProperty->GetFlexGrow().value_or(0.0f), 0.0f)) {
                    flexItemProperties.totalGrow += flexItemProperty->GetFlexGrowValue();
                }
                secondaryMeasureList_.emplace_back(child);
            }
            if (allocatedSize_ - space_ > mainAxisSize_) {
                for (auto& child : childList) {
                    allocatedSize_ -= GetChildCrossAxisSize(child.layoutWrapper);
                    allocatedSize_ -= space_;
                    child.layoutWrapper->SetActive(false);
                    child.layoutWrapper->GetGeometryNode()->SetFrameSize(SizeF());
                    auto childLayoutWrapper = child.layoutWrapper;
                    const auto& flexItemProperty = childLayoutWrapper->GetLayoutProperty()->GetFlexItemProperty();
                    if (flexItemProperty && GreatNotEqual(flexItemProperty->GetFlexGrow().value_or(0.0f), 0.0f)) {
                        flexItemProperties.totalGrow -= flexItemProperty->GetFlexGrowValue();
                    }
                    secondaryMeasureList_.pop_back();
                }
                break;
            }
            iter++;
        }
    } else {
        auto iter = magicNodes_.rbegin();
        while (iter != magicNodes_.rend()) {
            auto childList = iter->second;
            for (auto& child : childList) {
                auto childLayoutWrapper = child.layoutWrapper;
                auto childLayoutConstraint = child.layoutConstraint;
                childLayoutWrapper->Measure(childLayoutConstraint);
                ResizeFlexSizeByItem(childLayoutWrapper);
                CheckSizeValidity(childLayoutWrapper);
                CheckBaselineProperties(childLayoutWrapper);
                const auto& flexItemProperty = childLayoutWrapper->GetLayoutProperty()->GetFlexItemProperty();
                if (flexItemProperty) {
                    auto flexShrink = flexItemProperty->GetFlexShrink().value_or(1.0f);
                    auto flexGrow = flexItemProperty->GetFlexGrow().value_or(0.0f);
                    flexItemProperties.totalShrink += flexShrink * GetChildMainAxisSize(childLayoutWrapper);
                    flexItemProperties.totalGrow += flexGrow;
                }
                secondaryMeasureList_.emplace_back(child);
            }
            iter++;
        }
    }
}

void FlexLayoutAlgorithm::SecondaryMeasureByProperty(FlexItemProperties& flexItemProperties)
{
    float remainSpace = mainAxisSize_ - allocatedSize_;
    float spacePerFlex = 0;
    float allocatedFlexSpace = 0;
    std::function<float(const RefPtr<LayoutWrapper>&)> getFlex;
    RefPtr<LayoutWrapper> lastChild;
    if (GreatOrEqual(remainSpace, 0.0f) || GreatNotEqual(maxDisplayPriority_, 1)) {
        getFlex = [](const RefPtr<LayoutWrapper>& item) -> float {
            const auto& flexItemProperty = item->GetLayoutProperty()->GetFlexItemProperty();
            float ret = 0.0f;
            if (flexItemProperty) {
                ret = flexItemProperty->GetFlexGrow().value_or(0.0f);
            }
            return ret;
        };
        spacePerFlex = NearZero(flexItemProperties.totalGrow) ? 0.0f : remainSpace / flexItemProperties.totalGrow;
        lastChild = flexItemProperties.lastGrowChild;
    } else {
        getFlex = [](const RefPtr<LayoutWrapper>& item) -> float {
            const auto& flexItemProperty = item->GetLayoutProperty()->GetFlexItemProperty();
            float ret = 0.0f;
            if (flexItemProperty) {
                ret = flexItemProperty->GetFlexShrink().value_or(1.0f);
            }
            return ret;
        };
        spacePerFlex = NearZero(flexItemProperties.totalShrink) ? 0.0f : remainSpace / flexItemProperties.totalShrink;
        lastChild = flexItemProperties.lastShrinkChild;
    }
    auto iter = secondaryMeasureList_.rbegin();
    /**
     * get the real cross axis size.
     */
    auto padding = layoutWrapper_->GetLayoutProperty()->CreatePaddingAndBorder();
    auto paddingLeft = padding.left.value_or(0.0f);
    auto paddingRight = padding.right.value_or(0.0f);
    auto paddingTop = padding.top.value_or(0.0f);
    auto paddingBottom = padding.bottom.value_or(0.0f);
    auto crossAxisSize = crossAxisSize_;
    if (!NearEqual(selfIdealCrossAxisSize_, -1.0f)) {
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            crossAxisSize = selfIdealCrossAxisSize_ - paddingTop - paddingBottom;
        } else {
            crossAxisSize = selfIdealCrossAxisSize_ - paddingLeft - paddingRight;
        }
    }
    while (iter != secondaryMeasureList_.rend()) {
        auto child = *iter;
        bool needSecondaryLayout = false;
        auto childLayoutWrapper = child.layoutWrapper;
        if (GetSelfAlign(childLayoutWrapper) == FlexAlign::STRETCH) {
            UpdateLayoutConstraintOnCrossAxis(child.layoutConstraint, crossAxisSize);
            needSecondaryLayout = true;
        }
        if (LessOrEqual(totalFlexWeight_, 0.0f)) {
            float itemFlex = getFlex(child.layoutWrapper);
            float flexSize = child.layoutWrapper == lastChild ? (remainSpace - allocatedFlexSpace)
                             : GreatOrEqual(remainSpace, 0.0f) || GreatNotEqual(maxDisplayPriority_, 1)
                                 ? spacePerFlex * itemFlex
                                 : spacePerFlex * itemFlex * GetChildMainAxisSize(child.layoutWrapper);
            if (!NearZero(flexSize)) {
                flexSize += GetChildMainAxisSize(childLayoutWrapper);
                UpdateLayoutConstraintOnMainAxis(child.layoutConstraint, flexSize);
                needSecondaryLayout = true;
            }
        }
        if (needSecondaryLayout) {
            childLayoutWrapper->Measure(child.layoutConstraint);
            CheckBaselineProperties(child.layoutWrapper);
        }
        iter++;
    }
}

void FlexLayoutAlgorithm::UpdateLayoutConstraintOnMainAxis(LayoutConstraintF& layoutConstraint, float size)
{
    OptionalSizeF& selfIdealSize = layoutConstraint.selfIdealSize;
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        selfIdealSize.SetWidth(size);
    } else {
        selfIdealSize.SetHeight(size);
    }
}

void FlexLayoutAlgorithm::UpdateLayoutConstraintOnCrossAxis(LayoutConstraintF& layoutConstraint, float size)
{
    OptionalSizeF& selfIdealSize = layoutConstraint.selfIdealSize;
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        selfIdealSize.SetHeight(size);
    } else {
        selfIdealSize.SetWidth(size);
    }
}

void FlexLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto children = layoutWrapper->GetAllChildrenWithBuild();
    /**
     * Obtain the main axis size and cross axis size based on user setting.
     */
    const auto& layoutConstraint = layoutWrapper->GetLayoutProperty()->GetLayoutConstraint();
    const auto& measureType = layoutWrapper->GetLayoutProperty()->GetMeasureType();
    auto realSize = CreateIdealSize(layoutConstraint.value(),
        direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE ? Axis::HORIZONTAL
                                                                                     : Axis::VERTICAL,
        measureType)
                        .ConvertToSizeT();
    if (children.empty()) {
        layoutWrapper->GetGeometryNode()->SetFrameSize(realSize);
        return;
    }
    InitFlexProperties(layoutWrapper);
    mainAxisSize_ = GetMainAxisSizeHelper(realSize, direction_);
    /**
     * The user has not set the main axis size
     */
    if (NearEqual(mainAxisSize_, -1.0f)) {
        mainAxisSize_ = direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE
                            ? layoutConstraint->maxSize.Width()
                            : layoutConstraint->maxSize.Height();
    }
    auto padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    auto paddingLeft = padding.left.value_or(0.0f);
    auto paddingRight = padding.right.value_or(0.0f);
    auto paddingTop = padding.top.value_or(0.0f);
    auto paddingBottom = padding.bottom.value_or(0.0f);
    auto horizontalPadding = paddingLeft + paddingRight;
    auto verticalPadding = paddingTop + paddingBottom;
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        mainAxisSize_ -= horizontalPadding;
    } else {
        mainAxisSize_ -= verticalPadding;
    }
    if (NearEqual(mainAxisSize_, Infinity<float>())) {
        LOGW("The main axis size does not support infinite");
        return;
    }
    selfIdealCrossAxisSize_ = GetCrossAxisSizeHelper(realSize, direction_);
    FlexItemProperties flexItemProperties;
    /**
     * first measure
     */
    MeasureAndCleanMagicNodes(flexItemProperties);
    /**
     * secondary measure
     */
    SecondaryMeasureByProperty(flexItemProperties);
    if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
        crossAxisSize_ += paddingTop + paddingBottom;
    } else {
        crossAxisSize_ += paddingLeft + paddingRight;
    }
    realSize.UpdateIllegalSizeWithCheck(
        GetCalcSizeHelper(mainAxisSize_ + horizontalPadding, crossAxisSize_ + verticalPadding, direction_)
            .ConvertToSizeT());
    layoutWrapper->GetGeometryNode()->SetFrameSize(realSize);
}

/**
 * Get cross axis size in stretch.
 * At this time, the cross axis size has been determined.
 */
float FlexLayoutAlgorithm::GetStretchCrossAxisLimit() const
{
    float crossAxisLimit = GreatNotEqual(selfIdealCrossAxisSize_, -1.0f) ? selfIdealCrossAxisSize_ : crossAxisSize_;
    return crossAxisLimit;
}

void FlexLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    auto children = layoutWrapper->GetAllChildrenWithBuild();
    if (children.empty()) {
        LOGE("FlexLayoutAlgorithm::Layout, children is empty");
        return;
    }
    float remainSpace = std::max(mainAxisSize_ - allocatedSize_, 0.0f);
    float frontSpace = 0.0f;
    float betweenSpace = 0.0f;
    CalculateSpace(remainSpace, frontSpace, betweenSpace);
    PlaceChildren(layoutWrapper, frontSpace, betweenSpace);

    for (auto&& child : children) {
        child->Layout();
    }
}

void FlexLayoutAlgorithm::CalculateSpace(float remainSpace, float& frontSpace, float& betweenSpace) const
{
    if (NearZero(remainSpace) && mainAxisAlign_ != FlexAlign::SPACE_CUSTOMIZATION) {
        return;
    }
    switch (mainAxisAlign_) {
        case FlexAlign::FLEX_START:
            frontSpace = 0.0f;
            betweenSpace = 0.0f;
            break;
        case FlexAlign::FLEX_END:
            frontSpace = remainSpace;
            betweenSpace = 0.0f;
            break;
        case FlexAlign::CENTER:
            frontSpace = remainSpace / 2;
            betweenSpace = 0.0f;
            break;
        case FlexAlign::SPACE_BETWEEN:
            frontSpace = 0.0f;
            betweenSpace = validSizeCount_ > 1 ? remainSpace / static_cast<float>(validSizeCount_ - 1) : 0.0f;
            break;
        case FlexAlign::SPACE_AROUND:
            betweenSpace = validSizeCount_ > 0 ? remainSpace / static_cast<float>(validSizeCount_) : 0.0f;
            frontSpace = betweenSpace / 2;
            break;
        case FlexAlign::SPACE_EVENLY:
            betweenSpace = validSizeCount_ > 0 ? remainSpace / static_cast<float>(validSizeCount_ + 1) : 0.0f;
            frontSpace = betweenSpace;
            break;
        case FlexAlign::SPACE_CUSTOMIZATION:
            betweenSpace = space_;
            frontSpace = 0.0f;
            break;
        default:
            break;
    }
    LOGD("CalculateSpace end front space is %{public}f, between space is %{public}f, remain space is %{public}f",
        frontSpace, betweenSpace, remainSpace);
}

void FlexLayoutAlgorithm::PlaceChildren(LayoutWrapper* layoutWrapper, float frontSpace, float betweenSpace)
{
    LOGD("Place children, direction %{public}d, frontSpace %{public}f, betweenSpace %{public}f", direction_, frontSpace,
        betweenSpace);
    float childMainPos = IsStartTopLeft(direction_, TextDirection::LTR) ? frontSpace : mainAxisSize_ - frontSpace;
    float crossAxisSize = NearEqual(selfIdealCrossAxisSize_, -1.0f) ? crossAxisSize_ : selfIdealCrossAxisSize_;
    float childCrossPos = 0.0f;
    auto children = layoutWrapper->GetAllChildrenWithBuild();
    const auto padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    auto paddingLeft = padding.left.value_or(0.0f);
    auto paddingTop = padding.top.value_or(0.0f);
    auto paddingRight = padding.right.value_or(0.0f);
    auto paddingBottom = padding.bottom.value_or(0.0f);
    for (const auto& child : children) {
        auto alignItem = GetSelfAlign(child);
        auto textDirection = AdjustTextDirectionByDir();
        switch (alignItem) {
            case FlexAlign::FLEX_START:
            case FlexAlign::FLEX_END:
                childCrossPos =
                    (IsStartTopLeft(FlipAxis(direction_), textDirection) == (alignItem == FlexAlign::FLEX_START))
                        ? direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE ? paddingLeft
                                                                                                       : paddingTop
                        : (crossAxisSize - GetChildCrossAxisSize(child) -
                              (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE
                                      ? paddingRight
                                      : paddingBottom));
                break;
            case FlexAlign::CENTER:
                childCrossPos = (crossAxisSize / 2) - (GetChildCrossAxisSize(child) / 2);
                break;
            case FlexAlign::STRETCH:
                childCrossPos = IsStartTopLeft(FlipAxis(direction_), textDirection)
                                    ? direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE
                                          ? paddingLeft
                                          : paddingTop
                                    : (crossAxisSize - GetChildCrossAxisSize(child) -
                                          (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE
                                                  ? paddingRight
                                                  : paddingBottom));
                break;
            case FlexAlign::BASELINE:
                childCrossPos = 0.0;
                if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
                    float distance = child->GetBaselineDistance();
                    childCrossPos = baselineProperties_.maxBaselineDistance - distance;
                }
                break;
            default:
                break;
        }
        OffsetF offset;
        if (direction_ == FlexDirection::ROW || direction_ == FlexDirection::ROW_REVERSE) {
            offset = OffsetF(childMainPos + paddingLeft, childCrossPos);
        } else {
            offset = OffsetF(childCrossPos, childMainPos + paddingTop);
        }

        if (!IsStartTopLeft(direction_, TextDirection::LTR)) {
            if (direction_ != FlexDirection::COLUMN_REVERSE) {
                offset.SetX(offset.GetX() - GetChildMainAxisSize(child));
            } else {
                offset.SetY(offset.GetY() - GetChildMainAxisSize(child));
            }
            child->GetGeometryNode()->SetMarginFrameOffset(offset);
            childMainPos -= GetChildMainAxisSize(child) + betweenSpace;
        } else {
            child->GetGeometryNode()->SetMarginFrameOffset(offset);
            childMainPos += GetChildMainAxisSize(child) + betweenSpace;
        }
    }
}

FlexAlign FlexLayoutAlgorithm::GetSelfAlign(const RefPtr<LayoutWrapper>& layoutWrapper) const
{
    const auto& flexItemProperty = layoutWrapper->GetLayoutProperty()->GetFlexItemProperty();
    return flexItemProperty ? flexItemProperty->GetAlignSelf().value_or(crossAxisAlign_) : crossAxisAlign_;
}

TextDirection FlexLayoutAlgorithm::AdjustTextDirectionByDir() const
{
    auto textDir = TextDirection::LTR;
    if (direction_ == FlexDirection::ROW_REVERSE) {
        textDir = textDir == TextDirection::RTL ? TextDirection::LTR : TextDirection::RTL;
    }
    return textDir;
}

} // namespace OHOS::Ace::NG
