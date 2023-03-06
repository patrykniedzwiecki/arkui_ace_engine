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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_PAINT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_PAINT_PROPERTY_H

#include "base/geometry/dimension.h"
#include "core/components/progress/progress_theme.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_owner.h"
#include "core/components_ng/pattern/loading_progress/loading_progress_style.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/paint_property.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT LoadingProgressPaintProperty : public PaintProperty {
    DECLARE_ACE_TYPE(LoadingProgressPaintProperty, PaintProperty);

public:
    LoadingProgressPaintProperty() = default;

    ~LoadingProgressPaintProperty() override = default;

    RefPtr<PaintProperty> Clone() const override
    {
        auto paintProperty = MakeRefPtr<LoadingProgressPaintProperty>();
        paintProperty->propColor_ = CloneColor();
        paintProperty->propLoadingProgressOwner_ = CloneLoadingProgressOwner();
        paintProperty->propRefreshAnimationState_ = CloneRefreshAnimationState();
        paintProperty->propRefreshFollowRatio_ = CloneRefreshFollowRatio();
        paintProperty->propRefreshTransitionRatio_ = CloneRefreshTransitionRatio();
        paintProperty->propRefreshFadeAwayRatio_ = CloneRefreshFadeAwayRatio();
        return paintProperty;
    }

    void Reset() override
    {
        PaintProperty::Reset();
        ResetColor();
        ResetLoadingProgressOwner();
        ResetRefreshAnimationState();
        ResetRefreshFollowRatio();
        ResetRefreshTransitionRatio();
        ResetRefreshFadeAwayRatio();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        PaintProperty::ToJsonValue(json);

        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto progressTheme = pipeline->GetTheme<ProgressTheme>();
        CHECK_NULL_VOID(progressTheme);

        json->Put("color", propColor_.value_or(progressTheme->GetLoadingColor()).ColorToString().c_str());
    }

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Color, Color, PROPERTY_UPDATE_MEASURE);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(LoadingProgressOwner, LoadingProgressOwner, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(RefreshAnimationState, int32_t, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(RefreshFollowRatio, float, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(RefreshTransitionRatio, float, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(RefreshFadeAwayRatio, float, PROPERTY_UPDATE_RENDER);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_PAINT_PROPERTY_H
