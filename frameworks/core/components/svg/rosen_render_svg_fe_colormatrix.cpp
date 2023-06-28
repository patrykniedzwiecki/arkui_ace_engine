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

#include "frameworks/core/components/svg/rosen_render_svg_fe_colormatrix.h"

#include "include/effects/SkImageFilters.h"
#ifndef NEW_SKIA
#include "include/effects/SkColorFilterImageFilter.h"
#else
#include "include/core/SkColorFilter.h"
#endif

namespace OHOS::Ace {

#ifndef USE_ROSEN_DRAWING
void RosenRenderSvgFeColorMatrix::OnAsImageFilter(sk_sp<SkImageFilter>& imageFilter) const
{
#ifdef USE_SYSTEM_SKIA
    auto colorFilter = SkColorFilter::MakeMatrixFilterRowMajor255(matrix_);
#else
    auto colorFilter = SkColorFilters::Matrix(matrix_);
#endif
#ifndef NEW_SKIA
    imageFilter = SkColorFilterImageFilter::Make(colorFilter, imageFilter);
#else
    imageFilter = SkImageFilters::ColorFilter(colorFilter, imageFilter);
#endif
}
#else
void RosenRenderSvgFeColorMatrix::OnAsImageFilter(std::shared_ptr<RSImageFilter>& imageFilter) const
{
    RSColorMatrix colorMatrix;
    colorMatrix.SetArray(matrix_);
    auto colorFilter = RSColorFilter::CreateMatrixColorFilter(colorMatrix);
    if (colorFilter) {
        imageFilter = RSImageFilter::CreateColorFilterImageFilter(*colorFilter, imageFilter);
    }
}
#endif

} // namespace OHOS::Ace
