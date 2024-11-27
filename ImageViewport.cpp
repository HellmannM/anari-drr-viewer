// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "ImageViewport.h"
// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace windows {

ImageViewport::ImageViewport(const std::vector<Image>& images, const char *name)
    : Window(name, true), m_images(images){}

void ImageViewport::buildUI()
{
  //TODO
}

void ImageViewport::clear()
{
  //TODO
}

void ImageViewport::showImage(size_t index)
{
  //TODO
}


} // namespace windows
