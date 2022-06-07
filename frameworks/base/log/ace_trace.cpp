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

#include "base/log/ace_trace.h"

#ifndef WINDOWS_PLATFORM
#include "securec.h"
#endif

namespace OHOS::Ace {
namespace {

const size_t MAX_STRING_SIZE = 128;

}

bool AceTraceBeginWithArgv(const char* format, va_list args)
{
    char name[MAX_STRING_SIZE] = { 0 };
    if (vsnprintf_s(name, sizeof(name), sizeof(name) - 1, format, args) < 0) {
        return false;
    }
    AceTraceBegin(name);
    return true;
}

bool AceTraceBeginWithArgs(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    bool retVal = AceTraceBeginWithArgv(format, args);
    va_end(args);
    return retVal;
}

AceScopedTrace::AceScopedTrace(const char* format, ...) : traceEnabled_(AceTraceEnabled())
{
    if (traceEnabled_) {
        va_list args;
        va_start(args, format);
        traceEnabled_ = AceTraceBeginWithArgv(format, args);
        va_end(args);
    }
}

AceScopedTrace::~AceScopedTrace()
{
    if (traceEnabled_) {
        AceTraceEnd();
    }
}

AceScopedTraceFlag::AceScopedTraceFlag(bool flag, const char* format, ...)
{
    if (flag && AceTraceEnabled()) {
        va_list args;
        va_start(args, format);
        flagTraceEnabled_ = AceTraceBeginWithArgv(format, args);
        va_end(args);
    }
}

AceScopedTraceFlag::~AceScopedTraceFlag()
{
    if (flagTraceEnabled_) {
        AceTraceEnd();
    }
}

std::string ACE_EXPORT AceAsyncTraceBeginWithArgv(int32_t taskId, const char* format, va_list args)
{
    char name[MAX_STRING_SIZE] = { 0 };
    if (vsnprintf_s(name, sizeof(name), sizeof(name) - 1, format, args) < 0) {
        return "";
    }
    AceAsyncTraceBegin(taskId, name);
    return name;
}

std::string ACE_EXPORT AceAsyncTraceBeginWithArgs(int32_t taskId, char* format, ...)
{
    va_list args;
    va_start(args, format);
    std::string name = AceAsyncTraceBeginWithArgv(taskId, format, args);
    va_end(args);
    return name;
}

std::atomic<std::int32_t> AceAsyncScopedTrace::id_ = 0;

AceAsyncScopedTrace::AceAsyncScopedTrace(const char* format, ...) : asyncTraceEnabled_(AceTraceEnabled())
{
    taskId_ = id_++;
    if (asyncTraceEnabled_) {
        va_list args;
        va_start(args, format);
        name_ = AceAsyncTraceBeginWithArgv(taskId_, format, args);
    }
}

AceAsyncScopedTrace::~AceAsyncScopedTrace()
{
    if (!name_.empty()) {
        AceAsyncTraceEnd(taskId_, name_.c_str());
    }
}
} // namespace OHOS::Ace
