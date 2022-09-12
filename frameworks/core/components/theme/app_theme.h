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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_APP_THEME_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_APP_THEME_H

#include <unordered_map>

#include "core/components/common/properties/color.h"
#include "core/components/theme/theme.h"
#include "core/components/theme/theme_constants.h"

namespace OHOS::Ace {

/**
 * AppTheme defines color and styles of whole app. AppTheme should be built
 * using AppTheme::Builder.
 */
class AppTheme : public virtual Theme {
    DECLARE_ACE_TYPE(AppTheme, Theme);

public:
    class Builder {
    public:
        Builder() = default;
        ~Builder() = default;

        RefPtr<AppTheme> Build(const RefPtr<ThemeConstants>& themeConstants) const;
    };

    ~AppTheme() override = default;

    const Color& GetBackgroundColor() const
    {
        return backgroundColor_;
    }

    void SetBackgroundColor(const Color& color)
    {
        backgroundColor_ = color;
    }

    float GetHoverScaleStart() const
    {
        return hoverScaleStart_;
    }

    void SetHoverScaleStart(float scale)
    {
        hoverScaleStart_ = scale;
    }

    float GetHoverScaleEnd() const
    {
        return hoverScaleEnd_;
    }

    void SetHoverScaleEnd(float scale)
    {
        hoverScaleEnd_ = scale;
    }

    const Color& GetHoverHighlightStart() const
    {
        return hoverHighlightStart_;
    }

    void SetHoverHighlightStart(const Color& color)
    {
        hoverHighlightStart_ = color;
    }

    const Color& GetHoverHighlightEnd() const
    {
        return hoverHighlightEnd_;
    }

    void SetHoverHighlightEnd(const Color& color)
    {
        hoverHighlightEnd_ = color;
    }

    int32_t GetHoverDuration() const
    {
        return hoverDuration_;
    }

    void SetHoverDuration(int32_t duration)
    {
        hoverDuration_ = duration;
    }

protected:
    AppTheme() = default;

private:
    Color backgroundColor_;
    
    // HoverEffect parameters
    float hoverScaleStart_ = 1.0f;
    float hoverScaleEnd_ = 1.05f;                               // HoverEffect.Scale change from scale rate 1 to 1.05
    Color hoverHighlightStart_ = Color::TRANSPARENT;
    Color hoverHighlightEnd_ = Color::FromRGBO(0, 0, 0, 0.05);  // HoverEffect.HighLight change transparency from 100% to 5%
    int32_t hoverDuration_ = 250;                               // HoverEffect animation duration
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_THEME_APP_THEME_H
