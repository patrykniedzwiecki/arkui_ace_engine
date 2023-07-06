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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_JANK_FRAME_REPORT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_JANK_FRAME_REPORT_H

#include "base/utils/macros.h"

#include <string>
#include <vector>

namespace OHOS::Ace {
using JankFrameFlag = uint32_t;

inline constexpr JankFrameFlag JANK_IDLE = 0;
inline constexpr JankFrameFlag JANK_RUNNING_SCROLL = 1;
inline constexpr JankFrameFlag JANK_RUNNING_ANIMATOR = 1 << 1;

class ACE_FORCE_EXPORT JankFrameReport {
public:
    static void JankFrameRecord(int64_t timeStampNanos);
    static void SetFrameJankFlag(JankFrameFlag flag);
    static void ClearFrameJankFlag(JankFrameFlag flag);
    static void StartRecord(const std::string& pageUrl);
    static void FlushRecord();
    static void RecordPreviousEnd();
    static void SetRefreshPeriod(int64_t refreshPeriod);

private:
    static void ClearFrameJankRecord();
    static void ResetFrameJankClock();

    static std::vector<uint16_t> frameJankRecord_;
    static JankFrameFlag recordStatus_;
    static int64_t startTime_;
    static int64_t prevEndTimeStamp_;
    static int64_t refreshPeriod_;
    static std::string pageUrl_;
    static bool needReport_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_JANK_FRAME_REPORT_H