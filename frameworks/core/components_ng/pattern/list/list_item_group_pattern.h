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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_GROUP_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_GROUP_PATTERN_H

#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "base/utils/utils.h"
#include "core/components_ng/pattern/list/list_item_group_layout_property.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/syntax/shallow_builder.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT ListItemGroupPattern : public Pattern {
    DECLARE_ACE_TYPE(ListItemGroupPattern, Pattern);

public:
    explicit ListItemGroupPattern(const RefPtr<ShallowBuilder>& shallowBuilder) : shallowBuilder_(shallowBuilder) {}
    ~ListItemGroupPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<ListItemGroupLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override;

    void AddHeader(const RefPtr<NG::UINode>& header)
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        if (headerIndex_ < 0) {
            headerIndex_ = itemStartIndex_;
            host->AddChild(header);
            itemStartIndex_++;
        } else {
            host->ReplaceChild(host->GetChildAtIndex(headerIndex_), header);
        }
    }

    void AddFooter(const RefPtr<NG::UINode>& footer)
    {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        if (footerIndex_ < 0) {
            footerIndex_ = itemStartIndex_;
            host->AddChild(footer);
            itemStartIndex_++;
        } else {
            host->ReplaceChild(host->GetChildAtIndex(footerIndex_), footer);
        }
    }

private:
    RefPtr<ShallowBuilder> shallowBuilder_;

    int32_t headerIndex_ = -1;
    int32_t footerIndex_ = -1;
    int32_t itemStartIndex_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ListItemGroupPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LIST_LIST_ITEM_PATTERN_H
