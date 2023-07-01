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

#include "core/components_ng/pattern/rich_editor/rich_editor_pattern.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/text/span_node.h"

#if not defined(ACE_UNITTEST)
#if defined(ENABLE_STANDARD_INPUT)
#include "commonlibrary/c_utils/base/include/refbase.h"

#include "core/components_ng/pattern/rich_editor/on_rich_editor_changed_listener_impl.h"
#endif
#endif

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t IMAGE_SPAN_LENGTH = 1;
constexpr int32_t RICH_EDITOR_TWINKLING_INTERVAL_MS = 500;
} // namespace
RichEditorPattern::RichEditorPattern() {}

RichEditorPattern::~RichEditorPattern() {}

void RichEditorPattern::OnModifyDone()
{
    TextPattern::OnModifyDone();

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    instanceId_ = context->GetInstanceId();
    InitMouseEvent();
    auto focusHub = host->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    InitFocusEvent(focusHub);
    auto gestureEventHub = host->GetOrCreateGestureEventHub();
    InitClickEvent(gestureEventHub);
    InitLongPressEvent(gestureEventHub);
    InitTouchEvent();
}

void RichEditorPattern::BeforeCreateLayoutWrapper()
{
    TextPattern::BeforeCreateLayoutWrapper();
}

bool RichEditorPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    bool ret = TextPattern::OnDirtyLayoutWrapperSwap(dirty, config);
    if (!isRichEditorInit_) {
        auto eventHub = GetEventHub<RichEditorEventHub>();
        CHECK_NULL_RETURN(eventHub, ret);
        eventHub->FireOnReady();
        isRichEditorInit_ = true;
    }
    MoveCaretAfterTextChange();
    return ret;
}

int32_t RichEditorPattern::GetInstanceId() const
{
    return instanceId_;
}

std::function<ImageSourceInfo()> RichEditorPattern::CreateImageSourceInfo(const ImageSpanOptions& options)
{
    std::string src;
    RefPtr<PixelMap> pixMap = nullptr;
    std::string bundleName;
    std::string moduleName;
    if (options.image.has_value()) {
        src = options.image.value();
    }
    if (options.imagePixelMap.has_value()) {
        pixMap = options.imagePixelMap.value();
    }
    if (options.bundleName.has_value()) {
        bundleName = options.bundleName.value();
    }
    if (options.moduleName.has_value()) {
        moduleName = options.moduleName.value();
    }
    auto createSourceInfoFunc = [&src, noPixMap = !options.imagePixelMap.has_value(), &pixMap, &bundleName,
                                    &moduleName]() -> ImageSourceInfo {
#if defined(PIXEL_MAP_SUPPORTED)
        if (noPixMap) {
            return ImageSourceInfo(src, bundleName, moduleName);
        }
        return ImageSourceInfo(pixMap);
#else
        return ImageSourceInfo(src, bundleName, moduleName);
#endif
    };
    return std::move(createSourceInfoFunc);
}

int32_t RichEditorPattern::AddImageSpan(const ImageSpanOptions& options)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, -1);

    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto imageNode = FrameNode::GetOrCreateFrameNode(
        V2::IMAGE_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ImagePattern>(); });
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();

    int32_t spanIndex = 0;
    if (options.offset.has_value()) {
        int32_t offset = TextSpanSplit(options.offset.value());
        if (offset == -1) {
            spanIndex = host->GetChildren().size();
        } else {
            spanIndex = offset;
        }
        imageNode->MountToParent(host, offset);
    } else {
        spanIndex = host->GetChildren().size();
        imageNode->MountToParent(host);
    }
    std::function<ImageSourceInfo()> createSourceInfoFunc = CreateImageSourceInfo(options);
    imageLayoutProperty->UpdateImageSourceInfo(createSourceInfoFunc());
    if (options.imageAttribute.has_value()) {
        if (options.imageAttribute.value().size.has_value()) {
            imageLayoutProperty->UpdateUserDefinedIdealSize(
                CalcSize(CalcLength(options.imageAttribute.value().size.value().width.Value()),
                    CalcLength(options.imageAttribute.value().size.value().height.Value())));
        }
        if (options.imageAttribute.value().verticalAlign.has_value()) {
            imageLayoutProperty->UpdateVerticalAlign(options.imageAttribute.value().verticalAlign.value());
        }
        if (options.imageAttribute.value().objectFit.has_value()) {
            imageLayoutProperty->UpdateImageFit(options.imageAttribute.value().objectFit.value());
        }
    }
    imageNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    imageNode->MarkModifyDone();
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    host->MarkModifyDone();

    return spanIndex;
}

int32_t RichEditorPattern::AddTextSpan(const TextSpanOptions& options)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, -1);

    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto spanNode = SpanNode::GetOrCreateSpanNode(nodeId);

    int32_t spanIndex = 0;
    if (options.offset.has_value()) {
        int32_t offset = TextSpanSplit(options.offset.value());
        if (offset == -1) {
            spanIndex = host->GetChildren().size();
        } else {
            spanIndex = offset;
        }
        spanNode->MountToParent(host, offset);
    } else {
        spanIndex = host->GetChildren().size();
        spanNode->MountToParent(host);
    }
    spanNode->UpdateContent(options.value);
    spanNode->AddPropertyInfo(PropertyInfo::NONE);
    if (options.style.has_value()) {
        spanNode->UpdateTextColor(options.style.value().GetTextColor());
        spanNode->AddPropertyInfo(PropertyInfo::FONTCOLOR);
        spanNode->UpdateFontSize(options.style.value().GetFontSize());
        spanNode->AddPropertyInfo(PropertyInfo::FONTSIZE);
        spanNode->UpdateItalicFontStyle(options.style.value().GetFontStyle());
        spanNode->AddPropertyInfo(PropertyInfo::FONTSTYLE);
        spanNode->UpdateFontWeight(options.style.value().GetFontWeight());
        spanNode->AddPropertyInfo(PropertyInfo::FONTWEIGHT);
        spanNode->UpdateFontFamily(options.style.value().GetFontFamilies());
        spanNode->AddPropertyInfo(PropertyInfo::FONTFAMILY);
        spanNode->UpdateTextDecoration(options.style.value().GetTextDecoration());
        spanNode->AddPropertyInfo(PropertyInfo::TEXTDECORATION);
        spanNode->UpdateTextDecorationColor(options.style.value().GetTextDecorationColor());
        spanNode->AddPropertyInfo(PropertyInfo::NONE);
    }

    return spanIndex;
}

void RichEditorPattern::DeleteSpans(const RangeOptions& options)
{
    int32_t start = 0;
    int32_t end = 0;
    auto length = GetTextContentLength();
    start = (!options.start.has_value()) ? 0 : options.start.value();
    end = (!options.end.has_value()) ? length : options.end.value();
    if (start > end) {
        auto value = start;
        start = end;
        end = value;
    }
    start = std::max(0, start);
    end = std::min(length, end);
    if (start > length || end < 0 || start == end) {
        return;
    }

    auto startInfo = GetSpanPositionInfo(start);
    auto endInfo = GetSpanPositionInfo(end - 1);
    if (startInfo.spanIndex_ == endInfo.spanIndex_) {
        DeleteSpanByRange(start, end, startInfo);
    } else {
        DeleteSpansByRange(start, end, startInfo, endInfo);
    }
}

void RichEditorPattern::DeleteSpanByRange(int32_t start, int32_t end, SpanPositionInfo info)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto childrens = host->GetChildren();
    auto it = childrens.begin();
    std::advance(it, info.spanIndex_);
    if (start == info.spanStart_ && end == info.spanEnd_) {
        ClearContent(*it);
        host->RemoveChild(*it);
    } else {
        auto spanNode = DynamicCast<SpanNode>(*it);
        CHECK_NULL_VOID(spanNode);
        auto spanItem = spanNode->GetSpanItem();
        auto beforStr = StringUtils::ToWstring(spanItem->content).substr(0, start - info.spanStart_);
        auto endStr = StringUtils::ToWstring(spanItem->content).substr(end - info.spanStart_);
        std::wstring result = beforStr + endStr;
        auto str = StringUtils::ToString(result);
        spanNode->UpdateContent(str);
    }
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    host->MarkModifyDone();
}

void RichEditorPattern::DeleteSpansByRange(
    int32_t start, int32_t end, SpanPositionInfo startInfo, SpanPositionInfo endInfo)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto childrens = host->GetChildren();
    auto itStart = childrens.begin();
    std::advance(itStart, startInfo.spanIndex_);
    auto saveStartSpan = (start == startInfo.spanStart_) ? 0 : 1;
    if (saveStartSpan) {
        auto spanNodeStart = DynamicCast<SpanNode>(*itStart);
        CHECK_NULL_VOID(spanNodeStart);
        auto spanItemStart = spanNodeStart->GetSpanItem();
        auto beforStr = StringUtils::ToWstring(spanItemStart->content).substr(0, start - startInfo.spanStart_);
        auto strStart = StringUtils::ToString(beforStr);
        spanNodeStart->UpdateContent(strStart);
    }
    auto itEnd = childrens.begin();
    std::advance(itEnd, endInfo.spanIndex_);
    auto delEndSpan = (end == endInfo.spanEnd_) ? 1 : 0;
    if (!delEndSpan) {
        auto spanNodeEnd = DynamicCast<SpanNode>(*itEnd);
        CHECK_NULL_VOID(spanNodeEnd);
        auto spanItemEnd = spanNodeEnd->GetSpanItem();
        auto endStr =
            StringUtils::ToWstring(spanItemEnd->content).substr(end - endInfo.spanStart_, endInfo.spanEnd_ - end);
        auto strEnd = StringUtils::ToString(endStr);
        spanNodeEnd->UpdateContent(strEnd);
    }
    auto startIter = childrens.begin();
    std::advance(startIter, startInfo.spanIndex_ + saveStartSpan);
    auto endIter = childrens.begin();
    std::advance(endIter, endInfo.spanIndex_);
    for (auto iter = startIter; iter != endIter; ++iter) {
        ClearContent(*iter);
        host->RemoveChild(*iter);
    }
    if (delEndSpan) {
        ClearContent(*endIter);
        host->RemoveChild(*endIter);
    }
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    host->MarkModifyDone();
}

void RichEditorPattern::ClearContent(const RefPtr<UINode>& child)
{
    CHECK_NULL_VOID(child);
    if (child->GetTag() == V2::SPAN_ETS_TAG) {
        auto spanNode = DynamicCast<SpanNode>(child);
        if (spanNode) {
            spanNode->UpdateContent("");
            spanNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
    }
}

SpanPositionInfo RichEditorPattern::GetSpanPositionInfo(int32_t position)
{
    SpanPositionInfo spanPositionInfo(-1, -1, -1, -1);
    if (!spanItemChildren_.empty()) {
        position = std::clamp(position, 0, GetTextContentLength());
        // find the spanItem where the position is
        auto it = std::find_if(
            spanItemChildren_.begin(), spanItemChildren_.end(), [position](const RefPtr<SpanItem>& spanItem) {
                return (spanItem->position - static_cast<int32_t>(StringUtils::ToWstring(spanItem->content).length()) <=
                           position) &&
                       (position < spanItem->position);
            });
        // the position is at the end
        if (it == spanItemChildren_.end()) {
            return spanPositionInfo;
        }

        spanPositionInfo.spanIndex_ = std::distance(spanItemChildren_.begin(), it);
        auto contentLen = StringUtils::ToWstring((*it)->content).length();
        spanPositionInfo.spanStart_ = (*it)->position - contentLen;
        spanPositionInfo.spanEnd_ = (*it)->position;
        spanPositionInfo.spanOffset_ = position - spanPositionInfo.spanStart_;
    }
    return spanPositionInfo;
}

void RichEditorPattern::CopyTextSpanStyle(RefPtr<SpanNode>& source, RefPtr<SpanNode>& target)
{
    CHECK_NULL_VOID(source);
    CHECK_NULL_VOID(target);

    if (source->HasFontSize()) {
        target->UpdateFontSize(source->GetFontSizeValue(Dimension()));
        target->AddPropertyInfo(PropertyInfo::FONTSIZE);
    }

    if (source->HasTextColor()) {
        target->UpdateTextColor(source->GetTextColorValue(Color::BLACK));
        target->AddPropertyInfo(PropertyInfo::FONTCOLOR);
    }

    if (source->HasItalicFontStyle()) {
        target->UpdateItalicFontStyle(source->GetItalicFontStyleValue(OHOS::Ace::FontStyle::NORMAL));
        target->AddPropertyInfo(PropertyInfo::FONTSTYLE);
    }

    if (source->HasFontWeight()) {
        target->UpdateFontWeight(source->GetFontWeightValue(FontWeight::NORMAL));
        target->AddPropertyInfo(PropertyInfo::FONTWEIGHT);
    }

    if (source->HasFontFamily()) {
        target->UpdateFontFamily(source->GetFontFamilyValue({ "HarmonyOS Sans" }));
        target->AddPropertyInfo(PropertyInfo::FONTFAMILY);
    }

    if (source->HasTextDecoration()) {
        target->UpdateTextDecoration(source->GetTextDecorationValue(TextDecoration::NONE));
        target->AddPropertyInfo(PropertyInfo::TEXTDECORATION);
    }

    if (source->HasTextDecorationColor()) {
        target->UpdateTextDecorationColor(source->GetTextDecorationColorValue(Color::BLACK));
        target->AddPropertyInfo(PropertyInfo::NONE);
    }

    if (source->HasTextCase()) {
        target->UpdateTextCase(source->GetTextCaseValue(TextCase::NORMAL));
        target->AddPropertyInfo(PropertyInfo::TEXTCASE);
    }

    if (source->HasLetterSpacing()) {
        target->UpdateLetterSpacing(source->GetLetterSpacingValue(Dimension()));
        target->AddPropertyInfo(PropertyInfo::LETTERSPACE);
    }

    if (source->HasLineHeight()) {
        target->UpdateLineHeight(source->GetLineHeightValue(Dimension()));
        target->AddPropertyInfo(PropertyInfo::LINEHEIGHT);
    }
}

int32_t RichEditorPattern::TextSpanSplit(int32_t position)
{
    int32_t spanIndex = 0;
    int32_t spanStart = 0;
    int32_t spanOffset = 0;

    if (spanItemChildren_.empty()) {
        return -1;
    }

    auto positionInfo = GetSpanPositionInfo(position);
    spanIndex = positionInfo.spanIndex_;
    spanStart = positionInfo.spanStart_;
    spanOffset = positionInfo.spanOffset_;

    if (spanOffset == 0 || spanOffset == -1) {
        return spanIndex;
    }

    auto host = GetHost();
    CHECK_NULL_RETURN(host, -1);
    auto it = host->GetChildren().begin();
    std::advance(it, spanIndex);

    auto spanNode = DynamicCast<SpanNode>(*it);
    CHECK_NULL_RETURN(spanNode, -1);
    auto spanItem = spanNode->GetSpanItem();
    auto newContent = StringUtils::ToWstring(spanItem->content).substr(spanOffset);
    auto deleteContent = StringUtils::ToWstring(spanItem->content).substr(0, spanOffset);

    auto* stack = ViewStackProcessor::GetInstance();
    CHECK_NULL_RETURN(stack, -1);
    auto nodeId = stack->ClaimNodeId();
    auto newSpanNode = SpanNode::GetOrCreateSpanNode(nodeId);
    CHECK_NULL_RETURN(newSpanNode, -1);

    auto newSpanItem = newSpanNode->GetSpanItem();
    newSpanItem->position = spanStart + spanOffset;
    auto spanIter = spanItemChildren_.begin();
    std::advance(spanIter, spanIndex);
    spanItemChildren_.insert(spanIter, newSpanItem);

    spanNode->UpdateContent(StringUtils::ToString(newContent));
    newSpanNode->UpdateContent(StringUtils::ToString(deleteContent));

    CopyTextSpanStyle(spanNode, newSpanNode);
    newSpanNode->MountToParent(host, spanIndex);

    return spanIndex + 1;
}

int32_t RichEditorPattern::GetTextContentLength()
{
    if (!spanItemChildren_.empty()) {
        auto it = spanItemChildren_.rbegin();
        return (*it)->position;
    }
    return 0;
}

int32_t RichEditorPattern::GetCaretPosition()
{
    return caretPosition_;
}

bool RichEditorPattern::SetCaretOffset(int32_t caretPosition)
{
    bool success = false;
    success = SetCaretPosition(caretPosition);
    StartTwinkling();
    return success;
}

OffsetF RichEditorPattern::CalcCursorOffsetByPosition(int32_t position, float& selectLineHeight)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, OffsetF(0, 0));
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, OffsetF(0, 0));
    auto rootOffset = pipeline->GetRootRect().GetOffset();
    auto textPaintOffset = GetTextRect().GetOffset() - OffsetF(0.0f, std::min(baselineOffset_, 0.0f));
    auto startOffset = TextPattern::CalcCursorOffsetByPosition(position, selectLineHeight);
    return startOffset + textPaintOffset - rootOffset;
}

bool RichEditorPattern::SetCaretPosition(int32_t pos)
{
    auto lastCaretPosition = caretPosition_;
    caretPosition_ = std::clamp(pos, 0, GetTextContentLength());
    if (caretPosition_ == pos) {
        return true;
    } else {
        caretPosition_ = lastCaretPosition;
        return false;
    }
}

bool RichEditorPattern::GetCaretVisible() const
{
    return caretVisible_;
}

void RichEditorPattern::SetUpdateSpanStyle(struct UpdateSpanStyle updateSpanStyle)
{
    updateSpanStyle_ = updateSpanStyle;
}

void RichEditorPattern::UpdateTextStyle(RefPtr<SpanNode>& spanNode, TextStyle textStyle)
{
    if (updateSpanStyle_.updateTextColor.has_value()) {
        spanNode->UpdateTextColor(textStyle.GetTextColor());
    }
    if (updateSpanStyle_.updateFontSize.has_value()) {
        spanNode->UpdateFontSize(textStyle.GetFontSize());
    }
    if (updateSpanStyle_.updateItalicFontStyle.has_value()) {
        spanNode->UpdateItalicFontStyle(textStyle.GetFontStyle());
    }
    if (updateSpanStyle_.updateFontWeight.has_value()) {
        spanNode->UpdateFontWeight(textStyle.GetFontWeight());
    }
    if (updateSpanStyle_.updateFontFamily.has_value()) {
        spanNode->UpdateFontFamily(textStyle.GetFontFamilies());
    }
    if (updateSpanStyle_.updateTextDecoration.has_value()) {
        spanNode->UpdateTextDecoration(textStyle.GetTextDecoration());
    }
    if (updateSpanStyle_.updateTextDecorationColor.has_value()) {
        spanNode->UpdateTextDecorationColor(textStyle.GetTextDecorationColor());
    }
}

void RichEditorPattern::UpdateImageStyle(RefPtr<FrameNode>& imageNode, ImageSpanAttribute imageStyle)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    if (updateSpanStyle_.updateImageWidth.has_value() || updateSpanStyle_.updateImageHeight.has_value()) {
        imageLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(
            CalcLength(imageStyle.size.value().width.Value()), CalcLength(imageStyle.size.value().height.Value())));
    }
    if (updateSpanStyle_.updateImageFit.has_value()) {
        imageLayoutProperty->UpdateVerticalAlign(imageStyle.verticalAlign.value());
    }
    if (updateSpanStyle_.updateImageVerticalAlign.has_value()) {
        imageLayoutProperty->UpdateImageFit(imageStyle.objectFit.value());
    }
    imageNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    imageNode->MarkModifyDone();
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    host->MarkModifyDone();
}

void RichEditorPattern::UpdateSpanStyle(int32_t start, int32_t end, TextStyle textStyle, ImageSpanAttribute imageStyle)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    int32_t spanStart = 0;
    int32_t spanEnd = 0;

    for (auto it = host->GetChildren().begin(); it != host->GetChildren().end(); ++it) {
        auto spanNode = DynamicCast<SpanNode>(*it);
        auto imageNode = DynamicCast<FrameNode>(*it);
        if (!spanNode) {
            if (spanEnd != 0) {
                spanStart = spanEnd;
            }
            spanEnd = spanStart + 1;
        } else {
            auto spanItem = spanNode->GetSpanItem();
            auto contentLen = StringUtils::ToWstring(spanItem->content).length();
            spanStart = spanItem->position - contentLen;
            spanEnd = spanItem->position;
        }
        if (spanEnd < start) {
            continue;
        }

        if (spanStart >= start && spanEnd <= end) {
            if (spanNode) {
                UpdateTextStyle(spanNode, textStyle);
            } else {
                UpdateImageStyle(imageNode, imageStyle);
            }
            if (spanEnd == end) {
                break;
            }
            continue;
        }
        if (spanStart < start && start < spanEnd) {
            TextSpanSplit(start);
            --it;
            continue;
        }
        if (spanStart < end && end < spanEnd) {
            TextSpanSplit(end);
            --(--it);
            continue;
        }
        if (spanStart >= end) {
            break;
        }
    }
}

void RichEditorPattern::ScheduleCaretTwinkling()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);

    if (!context->GetTaskExecutor()) {
        LOGW("context has no task executor.");
        return;
    }

    auto weak = WeakClaim(this);
    caretTwinklingTask_.Reset([weak] {
        auto client = weak.Upgrade();
        CHECK_NULL_VOID(client);
        client->OnCaretTwinkling();
    });
    auto taskExecutor = context->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostDelayedTask(caretTwinklingTask_, TaskExecutor::TaskType::UI, RICH_EDITOR_TWINKLING_INTERVAL_MS);
}

void RichEditorPattern::StartTwinkling()
{
    caretTwinklingTask_.Cancel();
    caretVisible_ = true;
    GetHost()->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    ScheduleCaretTwinkling();
}

void RichEditorPattern::OnCaretTwinkling()
{
    caretTwinklingTask_.Cancel();
    caretVisible_ = !caretVisible_;
    GetHost()->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    ScheduleCaretTwinkling();
}

void RichEditorPattern::StopTwinkling()
{
    caretTwinklingTask_.Cancel();
    if (caretVisible_) {
        caretVisible_ = false;
        GetHost()->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    }
}

void RichEditorPattern::HandleClickEvent(GestureEvent& info)
{
    if (textSelector_.IsValid()) {
        CloseSelectOverlay();
        ResetSelection();
    }
    auto contentRect = GetTextRect();
    contentRect.SetTop(contentRect.GetY() - std::min(baselineOffset_, 0.0f));
    contentRect.SetHeight(contentRect.Height() - std::max(baselineOffset_, 0.0f));
    if (contentRect.IsInRegion(PointF(info.GetLocalLocation().GetX(), info.GetLocalLocation().GetY()))) {
        CHECK_NULL_VOID_NOLOG(!spanItemChildren_.empty());
        Offset textOffset = { info.GetLocalLocation().GetX() - contentRect.GetX(),
            info.GetLocalLocation().GetY() - contentRect.GetY() };
        CHECK_NULL_VOID(paragraph_);
        auto position = paragraph_->GetHandlePositionForClick(textOffset);
        auto focusHub = GetHost()->GetOrCreateFocusHub();
        if (focusHub) {
            if (focusHub->RequestFocusImmediately()) {
                float caretHeight = 0.0f;
                SetCaretPosition(position);
                OffsetF caretOffset = CalcCursorOffsetByPosition(GetCaretPosition(), caretHeight);
                CHECK_NULL_VOID(richEditorOverlayModifier_);
                richEditorOverlayModifier_->SetCaretOffsetAndHeight(caretOffset, caretHeight);
                StartTwinkling();
            } else {
                LOGE("request focus fail");
            }
        } else {
            LOGE("focusHub is NULL");
        }
    }
}

void RichEditorPattern::InitClickEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID_NOLOG(!clickEventInitialized_);
    auto clickCallback = [weak = WeakClaim(this)](GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleClickEvent(info);
    };
    auto clickListener = MakeRefPtr<ClickEvent>(std::move(clickCallback));
    gestureHub->AddClickEvent(clickListener);
    clickEventInitialized_ = true;
}

void RichEditorPattern::InitFocusEvent(const RefPtr<FocusHub>& focusHub)
{
    CHECK_NULL_VOID_NOLOG(!focusEventInitialized_);
    auto focusTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleFocusEvent();
    };
    focusHub->SetOnFocusInternal(focusTask);
    auto blurTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleBlurEvent();
    };
    focusHub->SetOnBlurInternal(blurTask);
    focusEventInitialized_ = true;
}

void RichEditorPattern::HandleBlurEvent()
{
    StopTwinkling();
}

void RichEditorPattern::HandleFocusEvent() {}

void RichEditorPattern::OnVisibleChange(bool isVisible)
{
    TextPattern::OnVisibleChange(isVisible);
}

bool RichEditorPattern::CloseKeyboard(bool forceClose)
{
    return false;
}

void RichEditorPattern::HandleLongPress(GestureEvent& info)
{
    if (isMousePressed_) {
        return;
    }
    TextPattern::HandleLongPress(info);
}

void RichEditorPattern::InitLongPressEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID_NOLOG(!longPressEvent_);
    auto longPressCallback = [weak = WeakClaim(this)](GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleLongPress(info);
    };
    longPressEvent_ = MakeRefPtr<LongPressEvent>(std::move(longPressCallback));
    gestureHub->SetLongPressEvent(longPressEvent_);

    auto onTextSelectorChange = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto frameNode = pattern->GetHost();
        CHECK_NULL_VOID(frameNode);
        frameNode->OnAccessibilityEvent(AccessibilityEventType::TEXT_SELECTION_UPDATE);
    };
    textSelector_.SetOnAccessibility(std::move(onTextSelectorChange));
}

bool RichEditorPattern::SelectOverlayIsOn()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, false);
    CHECK_NULL_RETURN_NOLOG(selectOverlayProxy_, false);
    auto overlayId = selectOverlayProxy_->GetSelectOverlayId();
    return pipeline->GetSelectOverlayManager()->HasSelectOverlay(overlayId);
}

void RichEditorPattern::UpdateEditingValue(const std::shared_ptr<TextEditingValue>& value, bool needFireChangeEvent) {}

void RichEditorPattern::PerformAction(TextInputAction action, bool forceCloseKeyboard) {}

void RichEditorPattern::InitMouseEvent()
{
    CHECK_NULL_VOID_NOLOG(!mouseEventInitialized_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(eventHub);
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(inputHub);

    auto mouseTask = [weak = WeakClaim(this)](MouseInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        pattern->HandleMouseEvent(info);
    };
    auto mouseEvent = MakeRefPtr<InputEvent>(std::move(mouseTask));
    inputHub->AddOnMouseEvent(mouseEvent);
    mouseEventInitialized_ = true;
}

void RichEditorPattern::OnHover(bool isHover) {}

bool RichEditorPattern::RequestKeyboard(bool isFocusViewChanged, bool needStartTwinkling, bool needShowSoftKeyboard)
{
    return true;
}

#if defined(ENABLE_STANDARD_INPUT)
bool RichEditorPattern::EnableStandardInput(bool needShowSoftKeyboard)
{
    return true;
}
#else
bool RichEditorPattern::UnableStandardInput(bool isFocusViewChanged)
{
    return true;
}
#endif

bool RichEditorPattern::HasConnection() const
{
    return true;
}

void RichEditorPattern::InsertValue(const std::string& insertValue) {}

void RichEditorPattern::CreateTextSpanNode(RefPtr<SpanNode>& spanNode, const TextInsertValueInfo& info,
    const std::string& insertValue, RichEditorAbstractSpanResult& retInfo)
{}

bool RichEditorPattern::BeforeIMEInsertValue(const std::string& insertValue)
{
    return true;
}

void RichEditorPattern::AfterIMEInsertValue(
    const RefPtr<SpanNode>& spanNode, int32_t insertValueLength, RichEditorAbstractSpanResult& retInfo)
{}

void RichEditorPattern::DeleteBackward(int32_t length) {}

void RichEditorPattern::DeleteForward(int32_t length) {}

void RichEditorPattern::SetInputMethodStatus(bool keyboardShown) {}

bool RichEditorPattern::CursorMoveLeft()
{
    return true;
}

bool RichEditorPattern::CursorMoveRight()
{
    return true;
}

bool RichEditorPattern::CursorMoveUp()
{
    return true;
}

bool RichEditorPattern::CursorMoveDown()
{
    return true;
}

void RichEditorPattern::CalcInsertValueObj(TextInsertValueInfo& info) {}

void RichEditorPattern::CalcDeleteValueObj(int32_t currentPosition, int32_t length, RichEditorDeleteValue& info) {}

int32_t RichEditorPattern::DeleteValueSetImageSpan(
    const RefPtr<SpanItem> spanItem, RichEditorAbstractSpanResult& spanResult)
{
    return IMAGE_SPAN_LENGTH;
}

int32_t RichEditorPattern::DeleteValueSetTextSpan(
    const RefPtr<SpanItem> spanItem, int32_t currentPosition, int32_t length, RichEditorAbstractSpanResult& spanResult)
{
    return 0;
}

void RichEditorPattern::DeleteByDeleteValueInfo(const RichEditorDeleteValue& info) {}

bool RichEditorPattern::OnKeyEvent(const KeyEvent& keyEvent)
{
    return true;
}

void RichEditorPattern::MoveCaretAfterTextChange() {}

void RichEditorPattern::InitTouchEvent()
{
    CHECK_NULL_VOID_NOLOG(!touchListener_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto gesture = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gesture);
    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        pattern->HandleTouchEvent(info);
    };
    touchListener_ = MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    gesture->AddTouchEvent(touchListener_);
}

void RichEditorPattern::HandleTouchEvent(const TouchEventInfo& info)
{
    if (SelectOverlayIsOn()) {
        return;
    }
    auto touchType = info.GetTouches().front().GetTouchType();
    if (touchType == TouchType::DOWN) {
    } else if (touchType == TouchType::UP) {
        isMousePressed_ = false;
    }
}

void RichEditorPattern::HandleMouseEvent(const MouseInfo& info)
{
    if (info.GetButton() == MouseButton::LEFT_BUTTON && info.GetAction() == MouseAction::MOVE) {
        auto textPaintOffset = contentRect_.GetOffset() - OffsetF(0.0, std::min(baselineOffset_, 0.0f));
        Offset textOffset = { info.GetLocalLocation().GetX() - textPaintOffset.GetX(),
            info.GetLocalLocation().GetY() - textPaintOffset.GetY() };
        CHECK_NULL_VOID(paragraph_);

        if (!isFirstmouseSelect_) {
            int32_t extend = paragraph_->GetHandlePositionForClick(textOffset);
            int32_t extendEnd = extend + GetGraphemeClusterLength(extend);
            textSelector_.Update(extend, extendEnd);
            isFirstmouseSelect_ = true;
        } else {
            int32_t extend = paragraph_->GetHandlePositionForClick(textOffset);
            textSelector_.Update(textSelector_.baseOffset, extend);
        }

        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
    } else if (info.GetButton() == MouseButton::LEFT_BUTTON && info.GetAction() == MouseAction::PRESS) {
        isMousePressed_ = true;
    } else if (info.GetButton() == MouseButton::LEFT_BUTTON && info.GetAction() == MouseAction::RELEASE) {
        isMousePressed_ = false;
        isFirstmouseSelect_ = false;

        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto eventHub = host->GetEventHub<RichEditorEventHub>();
        CHECK_NULL_VOID(eventHub);
        auto selectStart = std::min(textSelector_.baseOffset, textSelector_.destinationOffset);
        auto selectEnd = std::max(textSelector_.baseOffset, textSelector_.destinationOffset);
        auto textSelectInfo = GetSpansInfo(selectStart, selectEnd);
        if (textSelectInfo.GetSelection().resultObjects.size() > 0) {
            eventHub->FireOnSelect(&textSelectInfo);
        }
    }
}

void RichEditorPattern::OnHandleMoveDone(const RectF& handleRect, bool isFirstHandle)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<RichEditorEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto selectStart = std::min(textSelector_.baseOffset, textSelector_.destinationOffset);
    auto selectEnd = std::max(textSelector_.baseOffset, textSelector_.destinationOffset);
    auto textSelectInfo = GetSpansInfo(selectStart, selectEnd);
    if (textSelectInfo.GetSelection().resultObjects.size() > 0) {
        eventHub->FireOnSelect(&textSelectInfo);
    }
    TextPattern::OnHandleMoveDone(handleRect, isFirstHandle);
}

RefPtr<UINode> RichEditorPattern::GetChildByIndex(int32_t index) const
{
    auto host = GetHost();
    const auto& children = host->GetChildren();
    int32_t size = static_cast<int32_t>(children.size());
    if (index < 0 || index >= size) {
        return nullptr;
    }
    auto pos = children.begin();
    std::advance(pos, index);
    return *pos;
}

std::string RichEditorPattern::GetSelectedSpanText(std::wstring value, int32_t start, int32_t end) const
{
    if (start < 0 || end > static_cast<int32_t>(value.length()) || start >= end) {
        LOGI("Get selected boundary is invalid");
        return "";
    }
    auto min = std::min(start, end);
    auto max = std::max(start, end);

    return StringUtils::ToString(value.substr(min, max - min));
}

TextStyleResult RichEditorPattern::GetTextStyleObject(RefPtr<SpanNode> node)
{
    TextStyleResult textStyle;
    if (node->HasTextColor()) {
        textStyle.fontColor = node->GetTextColorValue(Color::BLACK).ColorToString();
    }
    if (node->HasFontSize()) {
        textStyle.fontSize = node->GetFontSizeValue(Dimension()).ConvertToVp();
    }
    if (node->HasItalicFontStyle()) {
        textStyle.fontStyle = static_cast<int32_t>(node->GetItalicFontStyleValue(OHOS::Ace::FontStyle::NORMAL));
    }
    if (node->HasFontWeight()) {
        textStyle.fontWeight = static_cast<int32_t>(node->GetFontWeightValue(FontWeight::NORMAL));
    }
    if (node->HasFontFamily()) {
        std::string fontFamilyValue = "";
        const std::vector<std::string> defaultFontFamily = { "HarmonyOS Sans" };
        auto fontFamily = node->GetFontFamilyValue(defaultFontFamily);
        for (auto str : fontFamily) {
            fontFamilyValue += str;
        }
        textStyle.fontFamily = fontFamilyValue;
    }
    if (node->HasTextDecoration()) {
        textStyle.decorationType = static_cast<int32_t>(node->GetTextDecorationValue(TextDecoration::NONE));
        textStyle.decorationColor = node->GetTextDecorationColorValue(Color::BLACK).ColorToString();
    }
    return textStyle;
}

ResultObject RichEditorPattern::GetTextResultObject(RefPtr<SpanItem> item, int32_t index, int32_t start, int32_t end)
{
    bool selectFlag = false;
    ResultObject resultObject;
    int32_t endPosition = item->position;
    int32_t itemLength = StringUtils::ToWstring(item->content).length();
    int32_t startPosition = endPosition - itemLength;

    if (startPosition >= start && endPosition <= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
        resultObject.valueString = GetSelectedSpanText(StringUtils::ToWstring(item->content), 0, itemLength);
    } else if (startPosition < start && endPosition <= end && endPosition > start) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
        resultObject.valueString =
            GetSelectedSpanText(StringUtils::ToWstring(item->content), start - startPosition, itemLength);
    } else if (startPosition >= start && startPosition < end && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
        resultObject.valueString = GetSelectedSpanText(StringUtils::ToWstring(item->content), 0, end - startPosition);
    } else if (startPosition <= start && endPosition >= end) {
        selectFlag = true;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = start - startPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = end - startPosition;
        resultObject.valueString =
            GetSelectedSpanText(StringUtils::ToWstring(item->content), start - startPosition, end - startPosition);
    }
    if (selectFlag == true) {
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.type = RichEditorSpanType::TYPESPAN;

        auto node = DynamicCast<SpanNode>(GetChildByIndex(index));
        resultObject.textStyle = GetTextStyleObject(node);
    }
    return resultObject;
}

ResultObject RichEditorPattern::GetImageResultObject(RefPtr<SpanItem> item, int32_t index, int32_t start, int32_t end)
{
    int32_t itemLength = 1;
    ResultObject resultObject;
    int32_t endPosition = item->position;
    int32_t startPosition = endPosition - itemLength;
    if ((start <= startPosition) && (end >= endPosition)) {
        auto imageNode = DynamicCast<FrameNode>(GetChildByIndex(index));
        auto imageLayoutProperty = DynamicCast<ImageLayoutProperty>(imageNode->GetLayoutProperty());
        resultObject.spanPosition.spanIndex = index;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGESTART] = startPosition;
        resultObject.spanPosition.spanRange[RichEditorSpanRange::RANGEEND] = endPosition;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGESTART] = 0;
        resultObject.offsetInSpan[RichEditorSpanRange::RANGEEND] = itemLength;
        resultObject.type = RichEditorSpanType::TYPEIMAGE;
        if (!imageLayoutProperty->GetImageSourceInfo()->GetPixmap()) {
            resultObject.valueString = imageLayoutProperty->GetImageSourceInfo()->GetSrc().c_str();
        } else {
            resultObject.valuePixelMap = imageLayoutProperty->GetImageSourceInfo()->GetPixmap();
        }
        auto geometryNode = imageNode->GetGeometryNode();
        resultObject.imageStyle.size[RichEditorImageSize::SIZEWIDTH] = geometryNode->GetMarginFrameSize().Width();
        resultObject.imageStyle.size[RichEditorImageSize::SIZEHEIGHT] = geometryNode->GetMarginFrameSize().Height();
        if (imageLayoutProperty->HasImageFit()) {
            resultObject.imageStyle.verticalAlign = static_cast<int32_t>(imageLayoutProperty->GetImageFitValue());
        }
        if (imageLayoutProperty->HasVerticalAlign()) {
            resultObject.imageStyle.objectFit = static_cast<int32_t>(imageLayoutProperty->GetVerticalAlignValue());
        }
    }
    return resultObject;
}

RichEditorSelection RichEditorPattern::GetSpansInfo(int32_t start, int32_t end)
{
    int32_t index = 0;
    std::int32_t realEnd = 0;
    std::int32_t realStart = 0;
    RichEditorSelection selection;
    std::list<ResultObject> resultObjects;
    auto length = GetTextContentLength();
    realStart = (start == -1) ? 0 : start;
    realEnd = (end == -1) ? length : end;
    if (realStart > realEnd) {
        auto temp = realStart;
        realStart = realEnd;
        realEnd = temp;
    }
    realStart = std::max(0, realStart);
    realEnd = std::min(length, realEnd);
    selection.SetSelectionEnd(realEnd);
    selection.SetSelectionStart(realStart);
    if (realStart > length || realEnd < 0) {
        selection.SetResultObjectList(resultObjects);
        return selection;
    }
    if (spanItemChildren_.empty()) {
        selection.SetResultObjectList(resultObjects);
        return selection;
    }
    for (const auto& item : spanItemChildren_) {
        if (GetChildByIndex(index) == nullptr) {
            index++;
            continue;
        }
        if (DynamicCast<ImageSpanItem>(item)) {
            ResultObject resultObject = GetImageResultObject(item, index, realStart, realEnd);
            if (!resultObject.valueString.empty()) {
                resultObjects.emplace_back(resultObject);
            }
        } else {
            ResultObject resultObject = GetTextResultObject(item, index, realStart, realEnd);
            if (!resultObject.valueString.empty() || resultObject.valuePixelMap) {
                resultObjects.emplace_back(resultObject);
            }
        }
        index++;
    }
    selection.SetResultObjectList(resultObjects);
    return selection;
}
} // namespace OHOS::Ace::NG
