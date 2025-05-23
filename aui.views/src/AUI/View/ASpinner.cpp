/*
 * AUI Framework - Declarative UI toolkit for modern C++20
 * Copyright (C) 2020-2025 Alex2772 and Contributors
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

//
// Created by Alex2772 on 12/27/2021.
//

#include "ASpinner.h"

#include <AUI/Render/IRenderer.h>

void ASpinner::render(ARenderContext ctx) {
    using namespace std::chrono;
    ctx.render.translate(glm::vec2(getSize()) / 2.f);
    ctx.render.rotate(AAngleRadians(float(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() % 1000) / 1000.f * 2 * glm::pi<float>()));
    ctx.render.translate(-glm::vec2(getSize()) / 2.f);
    AView::render(ctx);
    redraw();
}
