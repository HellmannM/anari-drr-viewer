// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// anari
#include "anari_viewer/windows/Window.h"
// std
#include <functional>
#include <string>
#include <vector>
// ours
#include "Image.h"

namespace windows {

class ImageViewport : public anari_viewer::Window
{
 public:
  ImageViewport(const std::vector<Image>& images, const char *name = "Image Viewport");
  ~ImageViewport() = default;

  void buildUI() override;

  void clear();
  void showImage(size_t index);

 private:
  std::vector<Image> m_images;
};

} // namespace windows
