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

#include "core/components_ng/pattern/window_scene/scene/host/host_window_extension.h"

namespace OHOS::Ace::NG {
HostWindowExtension::HostWindowExtension(const std::string& /* bundleName */, const std::string& /* abilityName */) {}

HostWindowExtension::~HostWindowExtension() = default;

void HostWindowExtension::OnWindowShow() {}

void HostWindowExtension::OnWindowHide() {}

void HostWindowExtension::OnConnect() {}

void HostWindowExtension::RequestExtensionSessionActivation() {}

void HostWindowExtension::RequestExtensionSessionBackground() {}

void HostWindowExtension::RequestExtensionSessionDestruction() {}
} // namespace OHOS::Ace::NG