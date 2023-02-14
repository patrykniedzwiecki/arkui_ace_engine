/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ROSEN_ROSEN_WINDOW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ROSEN_ROSEN_WINDOW_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "base/utils/noncopyable.h"
#include "core/common/platform_window.h"

namespace OHOS::Ace::Platform {

template<class T>
class SemQueue {
public:
    void PopFront(T &t)
    {
        std::unique_lock lock(mutex_);
        if (queue_.empty()) {
            const auto &func = [this]() {
                return !queue_.empty();
            };
            cv_.wait(lock, func);
        }
        t = queue_.front();
        queue_.pop();
    }

    void Push(const T &t)
    {
        std::unique_lock lock(mutex_);
        queue_.push(std::move(t));
        cv_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

class RSWindow final : public PlatformWindow {
public:
    void Destroy() override;
    void RequestFrame() override;
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override;
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override;

private:
    void VsyncThreadMain();

    std::vector<AceVsyncCallback> vsyncCallbacks_;
    std::unique_ptr<std::thread> vsyncThread_ = nullptr;
    SemQueue<bool> vsyncRequests_;
};

} // namespace OHOS::Ace::Platform

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ROSEN_ROSEN_WINDOW_H
