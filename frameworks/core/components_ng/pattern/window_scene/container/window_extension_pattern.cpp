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

#include "core/components_ng/pattern/window_scene/container/window_extension_pattern.h"

namespace OHOS::Ace::NG {

WindowExtensionPattern::WindowExtensionPattern(
/*     const sptr<Rosen::IExtensionSession>& iExtensionSession,
    const std::shared_ptr<Rosen::RSSurfaceNode>& surfaceNode,
    const std::shared_ptr<AbilityRuntime::Context>& runtimeContext */)
    // : WindowPattern(surfaceNode, runtimeContext)
    : WindowPattern(nullptr)
{
    // sessionStage_ = std::shared_ptr<Rosen::ExtensionSessionStage>(new Rosen::ExtensionSessionStage(iExtensionSession));
}

} // namespace OHOS::Ace::NG
