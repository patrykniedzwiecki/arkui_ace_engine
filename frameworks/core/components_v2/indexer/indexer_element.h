/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ELEMENT_H

#include "core/focus/focus_node.h"
#include "core/pipeline/base/component_group_element.h"
#include "core/components_v2/indexer/render_indexer.h"
#include "core/components_v2/indexer/render_indexer_item.h"


namespace OHOS::Ace::V2 {
class IndexerElement : public ComponentGroupElement, public FocusNode {
    DECLARE_ACE_TYPE(IndexerElement, ComponentGroupElement, FocusNode);

public:
    void PerformBuild() override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    bool IndexMoveUp();
    bool IndexMoveDown();

private:
    WeakPtr<RenderIndexerItem> prevFocusedItem_;
};
} // namespace OHOS::Ace::V2
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_INDEXER_ELEMENT_H