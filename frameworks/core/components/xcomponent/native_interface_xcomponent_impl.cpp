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

#include "frameworks/core/components/xcomponent/native_interface_xcomponent_impl.h"

#include "utils/native/base/include/securec.h"

int32_t NativeXComponent::GetXComponentId(char* id, uint64_t* size)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    auto xcomponentId = xcomponentImpl_->GetXComponentId();
    uint64_t idSize = static_cast<uint64_t>(xcomponentId.size());
    if (idSize > (uint64_t)(XCOMPONENT_ID_LEN_MAX)) {
        LOGE("Length of XComponent id should be no more than XCOMPONENT_ID_LEN_MAX");
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    if (strncpy_s(id, (*size), xcomponentId.c_str(), idSize) == 0) {
        (*size) = idSize;
        return XCOMPONENT_RESULT_SUCCESS;
    } else {
        return XCOMPONENT_RESULT_FAILED;
    }
}

int32_t NativeXComponent::GetNativeWindow(void** window)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    auto surface = const_cast<void*>(xcomponentImpl_->GetSurface());
    if (surface) {
        (*window) = surface;
        return XCOMPONENT_RESULT_SUCCESS;
    } else {
        return XCOMPONENT_RESULT_FAILED;
    }
}

int32_t NativeXComponent::GetXComponentSize(const void* window, uint64_t* width, uint64_t* height)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    auto surfaceWindow = const_cast<void*>(xcomponentImpl_->GetSurface());
    if (window != surfaceWindow) {
        return XCOMPONENT_RESULT_FAILED;
    }
    (*width) = static_cast<uint64_t>(xcomponentImpl_->GetXComponentWidth());
    (*height) = static_cast<uint64_t>(xcomponentImpl_->GetXComponentHeight());

    return XCOMPONENT_RESULT_SUCCESS;
}

int32_t NativeXComponent::GetXComponentOffset(const void* window, double* x, double* y)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    auto surfaceWindow =  const_cast<void*>(xcomponentImpl_->GetSurface());
    if (window != surfaceWindow) {
        return XCOMPONENT_RESULT_FAILED;
    }
    (*x) = static_cast<double>(xcomponentImpl_->GetXComponentOffsetX());
    (*y) = static_cast<double>(xcomponentImpl_->GetXComponentOffsetY());

    return XCOMPONENT_RESULT_SUCCESS;
}

int32_t NativeXComponent::GetTouchInfo(const void* window, TouchInfo* touchInfo)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    auto surfaceWindow = const_cast<void*>(xcomponentImpl_->GetSurface());
    if (window != surfaceWindow) {
        return XCOMPONENT_RESULT_FAILED;
    }
    (*touchInfo) = xcomponentImpl_->GetTouchInfo();
    return XCOMPONENT_RESULT_SUCCESS;
}

int32_t NativeXComponent::RegisterCallback(NativeXComponentCallback* callback)
{
    if (xcomponentImpl_ == nullptr) {
        return XCOMPONENT_RESULT_BAD_PARAMETER;
    }
    xcomponentImpl_->SetCallback(callback);
    return XCOMPONENT_RESULT_SUCCESS;
}
