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

#include "core/components_ng/pattern/window_scene/container/window_scene_pattern.h"

namespace OHOS::Ace::NG {

WindowScenePattern::WindowScenePattern(
    const sptr<Rosen::ISceneSession>& iSceneSession,
    const std::shared_ptr<Rosen::RSSurfaceNode>& surfaceNode,
    const std::shared_ptr<Rosen::ISessionStateListener>& listener)
    : WindowPattern(surfaceNode)
{
    sessionStage_ = std::make_shared<Rosen::SceneSessionStage>(iSceneSession);
    if (sessionStage_) {
        sessionStage_->Connect();
    }
    sessionStage_->RegisterSessionStateListener(listener);
}

void WindowScenePattern::Foreground()
{
    CHECK_NULL_VOID(sessionStage_);
    sessionStage_->Foreground();
}

void WindowScenePattern::Background()
{
    CHECK_NULL_VOID(sessionStage_);
    sessionStage_->Background();
}

void WindowScenePattern::Disconnect()
{
    CHECK_NULL_VOID(sessionStage_);
    sessionStage_->Disconnect();
}

} // namespace OHOS::Ace::NG
