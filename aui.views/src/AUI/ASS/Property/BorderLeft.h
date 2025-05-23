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
// Created by alex2 on 08.01.2021.
//

#pragma once

#include <AUI/ASS/Property/IProperty.h>
#include <AUI/Util/AMetric.h>

namespace ass {

    /**
     * @brief Represents left border.
     * @ingroup ass_properties
     */
    struct BorderLeft {
        AMetric width;
        AColor color;

        BorderLeft(std::nullptr_t): width(0) {}
        BorderLeft(const AMetric &width, const AColor &color) : width(width), color(color) {}
    };

    namespace prop {

        template<>
        struct API_AUI_VIEWS Property<BorderLeft> : IPropertyBase {
        private:
            BorderLeft mInfo;

        public:
            Property(const BorderLeft& info) : mInfo(info) {

            }

            void renderFor(AView* view, const ARenderContext& ctx) override;

            bool isNone() override;

            PropertySlot getPropertySlot() const override;

            [[nodiscard]]
            const auto& value() const noexcept {
                return mInfo;
            }
        };
    }
}