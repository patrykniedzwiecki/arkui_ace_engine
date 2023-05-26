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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCREEN_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCREEN_PATTERN_H

#include "session/screen/include/screen_session.h"

#include "core/components_ng/pattern/stack/stack_pattern.h"

namespace OHOS::Ace::NG {
class ScreenPattern : public StackPattern {
    DECLARE_ACE_TYPE(ScreenPattern, StackPattern);

public:
    ScreenPattern(const sptr<Rosen::ScreenSession>& screenSession) : screenSession_(screenSession) {}
    ~ScreenPattern() override = default;

    std::optional<RenderContext::ContextParam> GetContextParam() const override
    {
        return RenderContext::ContextParam { RenderContext::ContextType::EXTERNAL };
    }

protected:
    void OnAttachToFrameNode() override;

private:
    void UpdateDisplayInfo();

    sptr<Rosen::ScreenSession> screenSession_;

    ACE_DISALLOW_COPY_AND_MOVE(ScreenPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_SCREEN_PATTERN_H
