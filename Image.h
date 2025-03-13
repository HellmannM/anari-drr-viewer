#pragma once

#include <cstring>

struct Image
{
    Image() = default;
    Image(size_t w, size_t h, size_t b, const void* d)
        : width(w), height(h), bpp(b)
    {
        auto size = width * height * bpp;
        data.resize(size);
        memcpy(data.data(), d, size);
    }

    size_t width;
    size_t height;
    size_t bpp;
    std::vector<uint8_t> data;
};
