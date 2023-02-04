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

#include "frameworks/bridge/card_frontend/form_frontend_declarative.h"

#include "base/log/event_report.h"
#include "base/utils/utils.h"
#include "core/common/thread_checker.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace {
namespace {
const char MANIFEST_JSON[] = "manifest.json";
const char FILE_TYPE_BIN[] = ".abc";
} // namespace

std::string FormFrontendDeclarative::GetFormSrcPath(const std::string& uri, const std::string& suffix) const
{
    if (uri.empty()) {
        LOGE("page uri is empty");
        return "";
    }
    // the case uri is starts with "/" and "/" is the mainPage
    if (uri.size() != 0) {
        if (uri.compare(0, 1, "/") == 0) {
            return uri.substr(1) + suffix;
        }
        if (uri.compare(0, 2, "./") == 0) {
            return uri.substr(2) + suffix;
        }
    }

    LOGE("can't find this page %{private}s path", uri.c_str());
    return "";
}

void FormFrontendDeclarative::ParseManifest() const
{
    std::call_once(onceFlag_, [this]() {
        std::string jsonContent;
        if (!Framework::GetAssetContentImpl(assetManager_, MANIFEST_JSON, jsonContent)) {
            LOGE("Kee FormFrontendDeclarative::ParseManifest RunPage parse manifest.json failed");
            EventReport::SendFormException(FormExcepType::RUN_PAGE_ERR);
            return;
        }
        manifestParser_->Parse(jsonContent);
    });
}

void FormFrontendDeclarative::RunPage(int32_t pageId, const std::string& url, const std::string& params)
{
    LOGI("Kee FormFrontendDeclarative::RunPage url = %{public}s", url.c_str());
    std::string urlPath = GetFormSrcPath(url, FILE_TYPE_BIN);

    if (urlPath.empty()) {
        LOGE("fail to eTS Card run page due to path url is empty");
        EventReport::SendFormException(FormExcepType::RUN_PAGE_ERR);
        return;
    }
    LOGI("Kee FormFrontendDeclarative::RunPage urlPath = %{public}s", urlPath.c_str());
    if (delegate_) {
        auto container = Container::Current();
        if (!container) {
            LOGE("RunPage host container null");
            EventReport::SendFormException(FormExcepType::RUN_PAGE_ERR);
            return;
        }
        container->SetCardFrontend(AceType::WeakClaim(this), cardId_);
        auto delegate = AceType::DynamicCast<Framework::FormFrontendDelegateDeclarative>(delegate_);
        if (delegate) {
            delegate->RunCard(urlPath, params, "", cardId_);
        } else {
            LOGE("FormFrontendDeclarative::RunPage delegate nullptr");
        }
    }
}

void FormFrontendDeclarative::UpdateData(const std::string& dataList)
{
    LOGI("Kee FormFrontendDeclarative::UpdateData dataList = %{public}s", dataList.c_str());
    CHECK_NULL_VOID(taskExecutor_);
    taskExecutor_->PostTask(
        [weak = AceType::WeakClaim(this), dataList] {
            auto frontend = weak.Upgrade();
            if (frontend) {
                frontend->UpdatePageData(dataList);
            }
        },
        TaskExecutor::TaskType::UI); // eTSCard UI == Main JS/UI/PLATFORM
}

void FormFrontendDeclarative::UpdatePageData(const std::string& dataList)
{
    CHECK_RUN_ON(UI); // eTSCard UI == Main JS/UI/PLATFORM
    auto delegate = GetDelegate();
    if (!delegate) {
        LOGE("the delegate is null");
        EventReport::SendFormException(FormExcepType::UPDATE_PAGE_ERR);
        return;
    }
    delegate->UpdatePageData(dataList);
}

void FormFrontendDeclarative::SetColorMode(ColorMode colorMode) {}
void FormFrontendDeclarative::OnSurfaceChanged(int32_t width, int32_t height) {}

void FormFrontendDeclarative::HandleSurfaceChanged(int32_t width, int32_t height)
{
    CHECK_RUN_ON(JS);
    OnMediaFeatureUpdate();
}

void FormFrontendDeclarative::OnMediaFeatureUpdate()
{
    CHECK_RUN_ON(JS);
}
} // namespace OHOS::Ace
