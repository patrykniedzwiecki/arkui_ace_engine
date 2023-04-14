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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_CUSTOM_NODE_BASE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_CUSTOM_NODE_BASE_H

#include <functional>
#include <string>

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/custom/custom_node_pattern.h"
#include "core/pipeline/base/element_register.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT CustomNodeBase : public virtual AceType {
    DECLARE_ACE_TYPE(CustomNodeBase, AceType);

public:
    CustomNodeBase() = default;
    ~CustomNodeBase() override;

    void FireOnAppear() const
    {
        if (appearFunc_) {
            appearFunc_();
        }
    }

    virtual void SetRenderFunction(const RenderFunction& renderFunction) {}

    void SetAppearFunction(std::function<void()>&& appearFunc)
    {
        appearFunc_ = std::move(appearFunc);
    }

    void SetUpdateFunction(std::function<void()>&& updateFunc)
    {
        updateFunc_ = std::move(updateFunc);
    }

    void SetDestroyFunction(std::function<void()>&& destroyFunc)
    {
        destroyFunc_ = std::move(destroyFunc);
    }

    void SetReloadFunction(std::function<void(bool)>&& reloadFunc)
    {
        reloadFunc_ = std::move(reloadFunc);
    }

    void FireReloadFunction(bool deep)
    {
        if (reloadFunc_) {
            reloadFunc_(deep);
        }
    }

    virtual void SetCompleteReloadFunc(RenderFunction&& func) = 0;

    void SetPageTransitionFunction(std::function<void()>&& pageTransitionFunc)
    {
        pageTransitionFunc_ = std::move(pageTransitionFunc);
    }

    void CallPageTransitionFunction() const
    {
        if (pageTransitionFunc_) {
            pageTransitionFunc_();
        }
    }

    void SetForceUpdateNodeFunc(std::function<void(int32_t)>&& forceNodeUpdateFunc)
    {
        forceNodeUpdateFunc_ = std::move(forceNodeUpdateFunc);
    }

    void FireNodeUpdateFunc(ElementIdType id)
    {
        if (forceNodeUpdateFunc_) {
            forceNodeUpdateFunc_(id);
        } else {
            LOGE("fail to find node update func to execute %{public}d node update", id);
        }
    }

    void FireRecycleSelf()
    {
        if (recycleCustomNodeFunc_) {
            recycleCustomNodeFunc_(AceType::Claim<CustomNodeBase>(this));
        }
    }

    void SetRecycleFunction(std::function<void(RefPtr<CustomNodeBase>)>&& recycleCustomNode)
    {
        recycleCustomNodeFunc_ = std::move(recycleCustomNode);
    }

    void SetRecycleRenderFunc(std::function<void()>&& func)
    {
        recycleRenderFunc_ = std::move(func);
    }

    void FireRecycleRenderFunc()
    {
        if (recycleRenderFunc_) {
            ACE_SCOPED_TRACE("CustomNode:BuildRecycle");
            recycleRenderFunc_();
            recycleRenderFunc_ = nullptr;
        }
    }

    bool HasRecycleRenderFunc() {
        return recycleRenderFunc_ != nullptr;
    }

    void ResetRecycle()
    {
        recycleRenderFunc_ = nullptr;
    }

    void Reset()
    {
        updateFunc_ = nullptr;
        appearFunc_ = nullptr;
        destroyFunc_ = nullptr;
    }

    // called by view in js thread
    void MarkNeedUpdate();

    // called by pipeline in js thread of update.
    void Update();

    void SetJSViewName(std::string&& name)
    {
        jsViewName_ = name;
    }

    const std::string& GetJSViewName() const
    {
        return jsViewName_;
    }

private:
    std::function<void()> updateFunc_;
    std::function<void()> appearFunc_;
    std::function<void()> destroyFunc_;
    std::function<void()> pageTransitionFunc_;
    std::function<void(bool)> reloadFunc_;
    std::function<void()> completeReloadFunc_;
    std::function<void(int32_t)> forceNodeUpdateFunc_;
    std::function<void(RefPtr<CustomNodeBase>)> recycleCustomNodeFunc_;
    std::function<void()> recycleRenderFunc_;
    bool needRebuild_ = false;

    std::string jsViewName_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_CUSTOM_NODE_BASE_H
