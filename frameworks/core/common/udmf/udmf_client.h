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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_UDMF_CLIENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_UDMF_CLIENT_H

#include <map>

#include "base/memory/ace_type.h"
#include "core/common/frontend.h"
#include "core/common/udmf/unified_data.h"
#include "core/components/form/resource/form_request_data.h"

namespace OHOS::Ace {
class UdmfClient : public AceType {
    DECLARE_ACE_TYPE(UdmfClient, AceType);

public:
    static UdmfClient* GetInstance();
    virtual RefPtr<UnifiedData> TransformUnifiedData(NativeValue* nativeValue) = 0;
    virtual NativeValue* TransformUdmfUnifiedData(RefPtr<UnifiedData>& UnifiedData) = 0;
    virtual NativeValue* TransformSummary(std::map<std::string, int64_t>& summary) = 0;
    virtual RefPtr<UnifiedData> CreateUnifiedData() = 0;
    virtual int32_t SetData(const RefPtr<UnifiedData>& unifiedData, std::string& key) = 0;
    virtual int32_t GetData(const RefPtr<UnifiedData>& unifiedData, const std::string& key) = 0;
    virtual int32_t GetSummary(std::string& key, std::map<std::string, int64_t>& summaryMap) = 0;
    virtual void AddFormRecord(
        const RefPtr<UnifiedData>& unifiedData, int32_t formId, const RequestFormInfo& cardInfo) = 0;
    virtual void AddLinkRecord(
        const RefPtr<UnifiedData>& unifiedData, const std::string& url, const std::string& description) = 0;
    virtual void AddPixelMapRecord(const RefPtr<UnifiedData>& unifiedData, std::vector<uint8_t>& data) = 0;
    virtual void AddImageRecord(const RefPtr<UnifiedData>& unifiedData, const std::string& uri) = 0;
    virtual void AddTextRecord(const RefPtr<UnifiedData>& unifiedData, const std::string& selectedStr) = 0;
    virtual std::string GetSingleTextRecord(const RefPtr<UnifiedData>& unifiedData) = 0;
    virtual int32_t GetVideoRecordUri(const RefPtr<UnifiedData>& unifiedData, std::string& uri) = 0;
};
} // namespace OHOS::Ace
#endif