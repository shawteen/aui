﻿/*
 * AUI Framework - Declarative UI toolkit for modern C++20
 * Copyright (C) 2020-2025 Alex2772 and Contributors
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include <utility>
#include <cstring>
#include "AImage.h"
#include "AImageLoaderRegistry.h"
#include <stdexcept>
#include <AUI/Traits/memory.h>


static constexpr auto CACHE_SIZE_THRESHOLD = 1024 * 1024 * 10; // 10 MB


_<AImage> AImage::fromFile(const APath& path) {
    return fromUrl(AUrl("file://" + path));
}

_<AImage> AImage::fromBuffer(AByteBufferView buffer) {
    try {
        if (auto raster = AImageLoaderRegistry::inst().loadRaster(buffer))
            return raster;
    } catch (const AException& e) {
        ALogger::err("Could not load image: " + e.getMessage());
    }
    return nullptr;
}

_<AImage> AImage::fromUrl(const AUrl& url) {
    try {
        auto buffer = AByteBuffer::fromStream(AUrl(url).open(), 0x10000000);

        if (auto raster = AImageLoaderRegistry::inst().loadRaster(buffer))
            return raster;
    } catch (const AException& e) {
        ALogger::err("Could not load image: " + url.full() + ": " + e.getMessage());
    }
    return nullptr;
}


void AImage::mirrorVertically() {
    auto bpp = bytesPerPixel();
    for (std::uint32_t y = 0; y < height() / 2; ++y) {
        auto mirroredY = height() - y - 1;
        auto l1 = modifiableBuffer().begin() + bpp * width() * y;
        auto l2 = modifiableBuffer().begin() + bpp * width() * mirroredY;
        for (std::uint32_t x = 0; x < width() * bpp; ++x, ++l1, ++l2) {
            std::swap(*l1, *l2);
        }
    }
}

AImage AImageView::cropped(glm::uvec2 position, glm::uvec2 size) const {
    AUI_ASSERT(position.x + size.x <= width());
    AUI_ASSERT(position.y + size.y <= height());
    AImage image(size, format());

    for (uint32_t sourceY = 0; sourceY < size.y; ++sourceY) {
        for (uint32_t sourceX = 0; sourceX < size.x; ++sourceX) {
            uint32_t from = ((position.y + sourceY) * width() + (position.x + sourceX)) * bytesPerPixel();
            uint32_t to = (sourceY * size.x + sourceX) * bytesPerPixel();

            for (uint32_t channel = 0; channel < bytesPerPixel(); ++channel)
            {
                image.mOwnedBuffer.at<std::uint8_t>(to + channel) = mData.at<std::uint8_t>(from + channel);
            }
        }
    }

    return image;
}

AImageView::Color AImageView::get(glm::uvec2 position) const noexcept {
    return visit([&](const auto& i) { return AColor(i.get(position)); });
}

AImageView::Color AImageView::averageColor() const noexcept {
    return visit([&](const auto& i) { return AColor(i.averageColor()); });
}


AImage AImageView::mirroredVertically() const {
    AImage copy(*this);
    copy.mirrorVertically();
    return copy;
}

void AImage::set(glm::uvec2 position, AImageView::Color c) noexcept {
    visit([&](auto& i) { i.set(position, AFormattedColorConverter(c)); });
}

AImage AImageView::resizedLinearDownscale(glm::uvec2 newSize) const
{
    if (mSize == newSize) {
        return AImage(*this);
    }

    auto ratio = glm::vec2(size() - 1u) / glm::vec2(newSize);
    AImage n(newSize, format());
    n.fill(0x0_argb);

    for (uint32_t i = 0; i < newSize.y; i++) {
        for (uint32_t j = 0; j < newSize.x; j++) {
            auto x = static_cast<uint32_t>(ratio.x * static_cast<float>(j));
            auto y = static_cast<uint32_t>(ratio.y * static_cast<float>(i));
            float xWeight = (ratio.x * static_cast<float>(j)) - static_cast<float>(x);
            float yWeight = (ratio.y * static_cast<float>(i)) - static_cast<float>(y);

            auto c1 = get(glm::uvec2 {x, y});
            auto c2 = get(glm::uvec2 {x + 1, y});
            auto c3 = get(glm::uvec2 {x, y + 1});
            auto c4 = get(glm::uvec2 {x + 1, y + 1});

            c1 *= (1. - xWeight) * (1. - yWeight);
            c2 *= xWeight * (1. - yWeight);
            c3 *= yWeight * (1. - xWeight);
            c4 *= xWeight * yWeight;

            auto color = c1 + c2 + c3 + c4;
            n.set(glm::uvec2{j, i}, color);
        }
    }
    return n;
}

AImage AImageView::convert(APixelFormat format) const {
    AImage image(size(), format);

    visit([&](const auto& source) {
        image.visit([&](auto& destination) {
            using source_image_t      = std::decay_t<decltype(source)>;
            using destination_image_t = std::decay_t<decltype(destination)>;

            static constexpr auto sourceFormat      = (APixelFormat::Value)source_image_t::FORMAT;
            static constexpr auto destinationFormat = (APixelFormat::Value)destination_image_t::FORMAT;

            std::transform(source.begin(), source.end(), destination.begin(), aui::pixel_format::convert<sourceFormat, destinationFormat>);
        });
    });

    return image;
}

AImageView::AImageView(const AImage& v): AImageView(v.mOwnedBuffer, v.mSize, v.mFormat) {

}

void AImage::insert(glm::uvec2 position, AImageView image) {
    visit([&](auto& destination) {
        image.visit([&](const auto& source) {
            static constexpr auto sourceFormat      = (APixelFormat::Value)std::decay_t<decltype(source)>::FORMAT;
            static constexpr auto destinationFormat = (APixelFormat::Value)std::decay_t<decltype(destination)>::FORMAT;

            for (unsigned y = 0; y < image.height(); ++y) {
                auto targetPosY = position.y + y;
                if (targetPosY >= height()) {
                    break;
                }
                for (unsigned x = 0; x < image.width(); ++x) {
                    auto targetPosX = position.x + x;
                    if (targetPosX >= width()) {
                        break;
                    }

                    destination.set({targetPosX, targetPosY}, aui::pixel_format::convert<sourceFormat, destinationFormat>(source.get({x, y})));
                }
            }
        });
    });
}

void AImage::fill(AImageView::Color color) {
    visit([&](auto& img) {
        typename std::decay_t<decltype(img)>::Color convertedColor = AFormattedColorConverter(color);
        img.fill(convertedColor);
    });
}
