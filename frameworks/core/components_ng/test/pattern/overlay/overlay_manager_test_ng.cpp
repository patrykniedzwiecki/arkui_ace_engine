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
#include <mutex>
#include <optional>
#include <string>

#include "gtest/gtest.h"

#include "base/geometry/ng/rect_t.h"
#include "base/memory/ace_type.h"

#define private public
#define protected public
#include "test/mock/base/mock_task_executor.h"

#include "base/geometry/ng/offset_t.h"
#include "core/components/drag_bar/drag_bar_theme.h"
#include "core/components/select/select_theme.h"
#include "core/components/toast/toast_theme.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/bubble/bubble_event_hub.h"
#include "core/components_ng/pattern/bubble/bubble_pattern.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/menu/menu_pattern.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/pattern/overlay/modal_presentation_pattern.h"
#include "core/components_ng/pattern/overlay/overlay_manager.h"
#include "core/components_ng/pattern/overlay/sheet_drag_bar_paint_method.h"
#include "core/components_ng/pattern/overlay/sheet_drag_bar_pattern.h"
#include "core/components_ng/pattern/overlay/sheet_presentation_pattern.h"
#include "core/components_ng/pattern/overlay/sheet_style.h"
#include "core/components_ng/pattern/root/root_pattern.h"
#include "core/components_ng/pattern/stage/stage_pattern.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
constexpr MenuType TYPE = MenuType::MENU;
const OffsetF MENU_OFFSET(10.0, 10.0);
} // namespace
class OverlayManagerTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    static RefPtr<FrameNode> CreateBubbleNode(const TestProperty& testProperty);
    static RefPtr<FrameNode> CreateTargetNode();
    static void CreateSheetStyle(SheetStyle& sheetStyle);
};

void OverlayManagerTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
    RefPtr<FrameNode> stageNode = AceType::MakeRefPtr<FrameNode>("STAGE", -1, AceType::MakeRefPtr<Pattern>());
    auto stageManager = AceType::MakeRefPtr<StageManager>(stageNode);
    MockPipelineBase::GetCurrent()->stageManager_ = stageManager;
}
void OverlayManagerTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}

RefPtr<FrameNode> OverlayManagerTestNg::CreateTargetNode()
{
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    return frameNode;
}

void OverlayManagerTestNg::CreateSheetStyle(SheetStyle& sheetStyle)
{
    if (!sheetStyle.sheetMode.has_value()) {
        sheetStyle.sheetMode = SheetMode::MEDIUM;
    }
    if (!sheetStyle.showDragBar.has_value()) {
        sheetStyle.showDragBar = true;
    }
}
/**
 * @tc.name: PopupTest001
 * @tc.desc: Test OverlayManager::UpdatePopupNode.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, PopupTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node and popupInfo.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto targetTag = targetNode->GetTag();
    auto popupId = ElementRegister::GetInstance()->MakeUniqueId();
    auto popupNode =
        FrameNode::CreateFrameNode(V2::POPUP_ETS_TAG, popupId, AceType::MakeRefPtr<BubblePattern>(targetId, targetTag));
    PopupInfo popupInfo;
    popupInfo.popupId = popupId;
    popupInfo.popupNode = popupNode;
    popupInfo.target = targetNode;
    popupInfo.markNeedUpdate = true;
    popupInfo.isCurrentOnShow = true;

    /**
     * @tc.steps: step2. create overlayManager and call UpdatePopupNode.
     * @tc.expected: popupMap's data is updated successfully
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    popupNode->MountToParent(rootNode);
    rootNode->MarkDirtyNode();
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->UpdatePopupNode(targetId, popupInfo);
    EXPECT_FALSE(overlayManager->popupMap_[targetId].isCurrentOnShow);

    /**
     * @tc.steps: step3. call HidePopup, ErasePopup, HideAllPopups
     * @tc.expected: popupMap's data is updated successfully
     */
    overlayManager->HidePopup(targetId, popupInfo);
    EXPECT_FALSE(overlayManager->popupMap_[targetId].markNeedUpdate);
    overlayManager->ErasePopup(targetId);
    overlayManager->HideAllPopups();
    EXPECT_TRUE(overlayManager->popupMap_.empty());
}

/**
 * @tc.name: BindContentCover001
 * @tc.desc: Test OverlayManager::BindContentCover create modal node.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindContentCover001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create target node.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create modal node and get modal node, get pattern.
     * @tc.expected: related function is called.
     */
    int32_t modalTransition = 1;
    bool isShow = true;
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    auto topModalNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_NE(topModalNode, nullptr);
    auto topModalPattern = topModalNode->GetPattern<ModalPresentationPattern>();
    EXPECT_NE(topModalPattern, nullptr);
    auto type = topModalPattern->GetType();
    EXPECT_EQ(type, ModalTransition::NONE);
}

/**
 * @tc.name: BindContentCover002
 * @tc.desc: Test OverlayManager::BindContentCover change ModalTransition dynamically.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindContentCover002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create target node.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create modal node and get modal node, get pattern.
     * @tc.expected: related function is called.
     */
    int32_t modalTransition = 1;
    bool isShow = true;
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    auto topModalNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_NE(topModalNode, nullptr);
    auto topModalPattern = topModalNode->GetPattern<ModalPresentationPattern>();
    EXPECT_NE(topModalPattern, nullptr);
    auto type = topModalPattern->GetType();
    EXPECT_EQ(type, ModalTransition::NONE);

    /**
     * @tc.steps: step4. Change the ModalTransition.
     * @tc.expected: the ModalTransition is updated successfully
     */
    modalTransition = 0;
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    topModalNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_NE(topModalNode, nullptr);
    topModalPattern = topModalNode->GetPattern<ModalPresentationPattern>();
    EXPECT_NE(topModalPattern, nullptr);
    type = topModalPattern->GetType();
    EXPECT_EQ(type, ModalTransition::DEFAULT);
}

/**
 * @tc.name: BindContentCover003
 * @tc.desc: Test OverlayManager::BindContentCover destroy modal node.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindContentCover003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create modal page node.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create modal node.
     */
    int32_t modalTransition = 1;
    bool isShow = true;
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());

    /**
     * @tc.steps: step4. destroy modal page.
     * @tc.expected: destroy successfully
     */
    overlayManager->BindContentCover(!isShow, nullptr, nullptr, modalTransition, targetId);
    EXPECT_TRUE(overlayManager->modalStack_.empty());
}

/**
 * @tc.name: BindSheet001
 * @tc.desc: Test OverlayManager::BindSheet create sheet page.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindSheet001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create builder func.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create sheet node and get sheet node, get pattern.
     * @tc.expected: related function is called.
     */
    SheetStyle sheetStyle;
    CreateSheetStyle(sheetStyle);
    bool isShow = true;
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto dragBarTheme = AceType::MakeRefPtr<DragBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(dragBarTheme));
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindSheet(isShow, nullptr, std::move(builderFunc), sheetStyle, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    auto topSheetNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_FALSE(topSheetNode == nullptr);
    auto topSheetPattern = topSheetNode->GetPattern<SheetPresentationPattern>();
    EXPECT_FALSE(topSheetPattern == nullptr);
    auto sheetLayoutProperty = topSheetNode->GetLayoutProperty<SheetPresentationProperty>();
    EXPECT_FALSE(sheetLayoutProperty == nullptr);
    auto sheetDragBarNode = AceType::DynamicCast<FrameNode>(topSheetNode->GetFirstChild());
    EXPECT_FALSE(sheetDragBarNode == nullptr);
    auto sheetDragBarPattern = sheetDragBarNode->GetPattern<SheetDragBarPattern>();
    EXPECT_FALSE(sheetDragBarPattern == nullptr);
    auto sheetDragBarPaintProperty = sheetDragBarNode->GetPaintProperty<SheetDragBarPaintProperty>();
    EXPECT_FALSE(sheetDragBarPaintProperty == nullptr);
}

/**
 * @tc.name: BindSheet002
 * @tc.desc: Test OverlayManager::BindSheet change sheetStyle dynamically.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindSheet002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create builder.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create sheet node and get sheet node, get pattern.
     * @tc.expected: related function is called.
     */
    SheetStyle sheetStyle;
    CreateSheetStyle(sheetStyle);
    bool isShow = true;
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto dragBarTheme = AceType::MakeRefPtr<DragBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(dragBarTheme));
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindSheet(isShow, nullptr, std::move(builderFunc), sheetStyle, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    auto topSheetNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_FALSE(topSheetNode == nullptr);
    auto sheetNodeLayoutProperty = topSheetNode->GetLayoutProperty<SheetPresentationProperty>();
    auto style = sheetNodeLayoutProperty->GetSheetStyle();
    EXPECT_EQ(style->sheetMode.value(), SheetMode::MEDIUM);
    EXPECT_EQ(style->showDragBar.value(), true);

    /**
     * @tc.steps: step4. Change the sheetStyle.
     * @tc.expected: the sheetStyle is updated successfully
     */
    sheetStyle.sheetMode = SheetMode::LARGE;
    sheetStyle.showDragBar = false;
    overlayManager->BindSheet(isShow, nullptr, std::move(builderFunc), sheetStyle, targetId);
    auto sheetNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_FALSE(topSheetNode == nullptr);
    auto sheetPattern = sheetNode->GetPattern<SheetPresentationPattern>();
    EXPECT_EQ(sheetPattern->GetTargetId(), topSheetNode->GetPattern<SheetPresentationPattern>()->GetTargetId());
    sheetNodeLayoutProperty = sheetNode->GetLayoutProperty<SheetPresentationProperty>();
    style = sheetNodeLayoutProperty->GetSheetStyle();
    EXPECT_EQ(style->sheetMode.value(), SheetMode::LARGE);
    EXPECT_EQ(style->showDragBar.value(), false);
}

/**
 * @tc.name: BindSheet003
 * @tc.desc: Test OverlayManager::BindSheet destroy sheet node.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, BindSheet003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. create builder.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create sheet node.
     * @tc.expected: Make sure the modalStack holds the sheetNode.
     */
    SheetStyle sheetStyle;
    CreateSheetStyle(sheetStyle);
    bool isShow = true;
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto dragBarTheme = AceType::MakeRefPtr<DragBarTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(dragBarTheme));
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindSheet(isShow, nullptr, std::move(builderFunc), sheetStyle, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    auto sheetNode = overlayManager->modalStack_.top().Upgrade();
    EXPECT_EQ(sheetNode->GetTag(), "SheetPage");

    /**
     * @tc.steps: step4. destroy modal page.
     * @tc.expected: destroy modal successfully.
     */
    overlayManager->BindSheet(!isShow, nullptr, nullptr, sheetStyle, targetId);
    EXPECT_TRUE(overlayManager->modalStack_.empty());
}
/**
 * @tc.name: PopupTest002
 * @tc.desc: Test OverlayManager::PopupEvent functions.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, PopupTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node and popupInfo.
     */
    std::vector<RefPtr<FrameNode>> targetNodes;
    std::vector<PopupInfo> popups;
    for (int i = 0; i < 2; i++) {
        auto targetNode = CreateTargetNode();
        ASSERT_NE(targetNode, nullptr);
        targetNodes.emplace_back(targetNode);
        auto targetId = targetNodes[i]->GetId();
        auto targetTag = targetNodes[i]->GetTag();

        auto popupId = ElementRegister::GetInstance()->MakeUniqueId();
        auto popupNode = FrameNode::CreateFrameNode(
            V2::POPUP_ETS_TAG, popupId, AceType::MakeRefPtr<BubblePattern>(targetId, targetTag));
        PopupInfo popupInfo;
        popupInfo.popupId = popupId;
        popupInfo.popupNode = popupNode;
        popupInfo.target = targetNode;
        popupInfo.markNeedUpdate = true;
        popupInfo.isBlockEvent = false;
        popups.emplace_back(popupInfo);
    }
    /**
     * @tc.steps: step2. create overlayManager and call UpdatePopupNode.
     * @tc.expected: Push popup successfully
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    auto targetId1 = targetNodes[0]->GetId();
    auto targetId2 = targetNodes[1]->GetId();
    overlayManager->UpdatePopupNode(targetId1, popups[0]);
    EXPECT_TRUE(overlayManager->popupMap_[targetId1].isCurrentOnShow);
    overlayManager->UpdatePopupNode(targetId2, popups[1]);
    EXPECT_TRUE(overlayManager->popupMap_[targetId2].isCurrentOnShow);
    /**
     * @tc.steps: step3. call HidePopup when childCount is 2
     * @tc.expected: popupMap's data is updated successfully
     */
    overlayManager->HideAllPopups();
    EXPECT_FALSE(overlayManager->popupMap_.empty());
    /**
     * @tc.steps: step4. call RemoveOverlay when childCount is 2
     * @tc.expected: remove successfully
     */
    EXPECT_TRUE(overlayManager->RemoveOverlay(false));
}
/**
 * @tc.name: PopupTest003
 * @tc.desc: Test OverlayManager::HidePopup.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, PopupTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node and popupInfo.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto targetTag = targetNode->GetTag();
    auto popupId = ElementRegister::GetInstance()->MakeUniqueId();
    auto popupNode =
        FrameNode::CreateFrameNode(V2::POPUP_ETS_TAG, popupId, AceType::MakeRefPtr<BubblePattern>(targetId, targetTag));
    PopupInfo popupInfo;
    popupInfo.popupId = popupId;
    popupInfo.popupNode = popupNode;
    popupInfo.target = targetNode;
    popupInfo.markNeedUpdate = true;
    popupInfo.isCurrentOnShow = true;

    /**
     * @tc.steps: step2. create overlayManager and call HidePopup.
     * @tc.expected: popupMap's data is updated successfully
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    popupNode->MountToParent(rootNode);
    rootNode->MarkDirtyNode();
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->HidePopup(targetId, popupInfo);
    EXPECT_FALSE(overlayManager->popupMap_[targetId].markNeedUpdate);
    auto rootChildren = rootNode->GetChildren();
    auto iter = std::find(rootChildren.begin(), rootChildren.end(), popupInfo.popupNode);
    EXPECT_TRUE(iter == rootChildren.end());
}
/**
 * @tc.name: MenuTest001
 * @tc.desc: Test OverlayManager::ShowMenu.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, MenuTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create menu node and root node.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto selectTheme = AceType::MakeRefPtr<SelectTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(selectTheme));
    auto menuId = ElementRegister::GetInstance()->MakeUniqueId();
    auto menuNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, menuId, AceType::MakeRefPtr<MenuPattern>(1, "Test", TYPE));
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    menuNode->MountToParent(rootNode);
    rootNode->MarkDirtyNode();

    /**
     * @tc.steps: step2. call showMenu when menuNode already appended.
     * @tc.expected: function exits rightly
     */
    auto targetId = rootNode->GetId();
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->ShowMenu(targetId, MENU_OFFSET, menuNode);
    overlayManager->HideMenu(targetId);
    EXPECT_FALSE(overlayManager->menuMap_.empty());
    overlayManager->ShowMenuInSubWindow(rootNode->GetId(), MENU_OFFSET, menuNode);
    overlayManager->HideMenuInSubWindow(rootNode->GetId());
    overlayManager->HideMenuInSubWindow();
    EXPECT_FALSE(overlayManager->menuMap_.empty());

    /**
     * @tc.steps: step3. call HideMenu related functions after menuNode already erased.
     * @tc.expected: return normally
     */
    overlayManager->HideAllMenus();
    overlayManager->DeleteMenu(targetId);
    overlayManager->HideMenu(targetId);
    overlayManager->HideMenuInSubWindow(targetId);
    overlayManager->HideMenuInSubWindow();
    EXPECT_TRUE(overlayManager->menuMap_.empty());

    /**
     * @tc.steps: step4. call DeleteMenu again after menuNode already erased.
     * @tc.expected: return normally
     */
    overlayManager->DeleteMenu(targetId);
    EXPECT_TRUE(overlayManager->menuMap_.empty());
}
/**
 * @tc.name: PopupTest004
 * @tc.desc: Test OverlayManager::HideAllPopups when useCustom is true.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, PopupTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node and popupInfo.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto targetTag = targetNode->GetTag();
    auto popupId = ElementRegister::GetInstance()->MakeUniqueId();
    auto popupNode =
        FrameNode::CreateFrameNode(V2::POPUP_ETS_TAG, popupId, AceType::MakeRefPtr<BubblePattern>(targetId, targetTag));
    PopupInfo popupInfo;
    popupInfo.popupId = popupId;
    popupInfo.popupNode = popupNode;
    popupInfo.target = targetNode;
    popupInfo.markNeedUpdate = true;
    auto layoutProp = popupNode->GetLayoutProperty<BubbleLayoutProperty>();
    ASSERT_NE(layoutProp, nullptr);
    layoutProp->UpdateUseCustom(true);

    /**
     * @tc.steps: step2. create overlayManager and call HideAllPopups when ShowInSubwindow is false.
     * @tc.expected: popupMap's data is updated successfully
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->UpdatePopupNode(targetId, popupInfo);
    overlayManager->HideAllPopups();
    EXPECT_FALSE(overlayManager->popupMap_[targetId].markNeedUpdate);

    /**
     * @tc.steps: step3. update ShowInSubwindow and call HideAllPopups again.
     * @tc.expected: popupMap's data is updated successfully
     */
    layoutProp->UpdateShowInSubWindow(true);
    overlayManager->UpdatePopupNode(targetId, popupInfo);
    overlayManager->HideAllPopups();
    EXPECT_FALSE(overlayManager->popupMap_[targetId].markNeedUpdate);

    /**
     * @tc.steps: step4. call ShowIndexerPopup and RemoveIndexerPopup.
     * @tc.expected: mount and remove successfully,Repeatedly calling the function exits normally
     */
    EXPECT_TRUE(overlayManager->customPopupMap_.empty());
    overlayManager->ShowIndexerPopup(targetId, popupNode);
    EXPECT_FALSE(overlayManager->customPopupMap_.empty());
    overlayManager->ShowIndexerPopup(targetId, popupNode);
    EXPECT_EQ(overlayManager->customPopupMap_[targetId], popupNode);
    overlayManager->RemoveIndexerPopup();
    EXPECT_TRUE(overlayManager->customPopupMap_.empty());
    overlayManager->RemoveIndexerPopup();
    EXPECT_TRUE(overlayManager->customPopupMap_.empty());
}
/**
 * @tc.name: MenuTest002
 * @tc.desc: Test OverlayManager::ShowMenu.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, MenuTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create menu node and root node.
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto selectTheme = AceType::MakeRefPtr<SelectTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(selectTheme));
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    auto targetId = rootNode->GetId();
    auto menuId = ElementRegister::GetInstance()->MakeUniqueId();
    auto menuNode =
        FrameNode::CreateFrameNode(V2::MENU_WRAPPER_ETS_TAG, menuId, AceType::MakeRefPtr<MenuWrapperPattern>(targetId));
    auto subMenuId = ElementRegister::GetInstance()->MakeUniqueId();
    auto subMenuNode =
        FrameNode::CreateFrameNode(V2::MENU_ETS_TAG, subMenuId, AceType::MakeRefPtr<MenuPattern>(1, "Test", TYPE));
    menuNode->AddChild(subMenuNode);
    /**
     * @tc.steps: step2. call showMenu when menuNode is nullptr and menuMap is empty.
     * @tc.expected: function exits normally
     */
    auto taskExecutor = AceType::MakeRefPtr<MockTaskExecutor>();
    ASSERT_NE(taskExecutor, nullptr);
    auto pipeline = PipelineBase::GetCurrentContext();
    pipeline->taskExecutor_ = taskExecutor;
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->ShowMenu(targetId, MENU_OFFSET, nullptr);
    overlayManager->ShowMenuInSubWindow(targetId, MENU_OFFSET, nullptr);
    EXPECT_TRUE(overlayManager->menuMap_.empty());
    /**
     * @tc.steps: step3. call showMenu when menuNode is not appended.
     * @tc.expected: menuNode mounted successfully
     */
    overlayManager->ShowMenu(targetId, MENU_OFFSET, menuNode);
    EXPECT_FALSE(overlayManager->menuMap_.empty());
    /**
     * @tc.steps: step4. call showMenu when menuNode is nullptr and menuMap is not empty.
     * @tc.expected: function exits normally
     */
    overlayManager->ShowMenu(targetId, MENU_OFFSET, nullptr);
    EXPECT_FALSE(overlayManager->menuMap_.empty());
    /**
     * @tc.steps: step5. call HideAllMenus.
     * @tc.expected: function exits normally
     */
    overlayManager->HideAllMenus();
    overlayManager->CleanMenuInSubWindow();
    EXPECT_FALSE(overlayManager->menuMap_.empty());
}
/**
 * @tc.name: RemoveOverlayTest001
 * @tc.desc: Test OverlayManager::RemoveOverlay.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, RemoveOverlayTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node and popupInfo.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto targetTag = targetNode->GetTag();
    auto popupId = ElementRegister::GetInstance()->MakeUniqueId();
    auto popupNode =
        FrameNode::CreateFrameNode(V2::POPUP_ETS_TAG, popupId, AceType::MakeRefPtr<BubblePattern>(targetId, targetTag));
    PopupInfo popupInfo;
    popupInfo.popupId = popupId;
    popupInfo.popupNode = popupNode;
    popupInfo.target = targetNode;
    popupInfo.markNeedUpdate = true;

    /**
     * @tc.steps: step2. create overlayManager and call removeOverlay when has one child.
     * @tc.expected: removing overlay failed
     */
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->UpdatePopupNode(targetId, popupInfo);
    EXPECT_FALSE(overlayManager->popupMap_[targetId].markNeedUpdate);
    auto res = overlayManager->RemoveOverlay(false);
    EXPECT_FALSE(res);
    EXPECT_TRUE(overlayManager->RemoveOverlayInSubwindow());
    EXPECT_FALSE(overlayManager->RemoveOverlayInSubwindow());
}
/**
 * @tc.name: RemoveOverlayTest002
 * @tc.desc:  Test OverlayManager::RemoveOverlay related functions.
 * @tc.type: FUNC
 */
HWTEST_F(OverlayManagerTestNg, RemoveOverlayTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create target node.
     */
    auto targetNode = CreateTargetNode();
    auto targetId = targetNode->GetId();
    auto stageNode = FrameNode::CreateFrameNode(
        V2::STAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<StagePattern>());
    auto rootNode = FrameNode::CreateFrameNode(V2::ROOT_ETS_TAG, 1, AceType::MakeRefPtr<RootPattern>());
    stageNode->MountToParent(rootNode);
    targetNode->MountToParent(stageNode);
    rootNode->MarkDirtyNode();
    auto stageManager = AceType::MakeRefPtr<StageManager>(stageNode);
    MockPipelineBase::GetCurrent()->stageManager_ = stageManager;

    /**
     * @tc.steps: step2. create modal page node.
     */
    auto builderFunc = []() -> RefPtr<UINode> {
        auto frameNode =
            FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
        auto childFrameNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
        frameNode->AddChild(childFrameNode);
        return frameNode;
    };

    /**
     * @tc.steps: step3. create modal node and call removeOverlay when modalStack is not empty.
     * @tc.expected: remove successfully.
     */
    int32_t modalTransition = 0;
    bool isShow = true;
    auto overlayManager = AceType::MakeRefPtr<OverlayManager>(rootNode);
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    EXPECT_FALSE(overlayManager->modalStack_.empty());
    EXPECT_TRUE(overlayManager->RemoveOverlay(false));

    /**
     * @tc.steps: step4. Change the ModalTransition and Call RemoveModalInOverlay.
     * @tc.expected: remove successfully.
     */
    modalTransition = 2;
    overlayManager->BindContentCover(isShow, nullptr, std::move(builderFunc), modalTransition, targetId);
    EXPECT_TRUE(overlayManager->RemoveModalInOverlay());
}
} // namespace OHOS::Ace::NG