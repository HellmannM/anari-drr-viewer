// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// glad
#include "glad/glad.h"
// glfw
#include <GLFW/glfw3.h>
// anari
#include <anari/anari_cpp/ext/linalg.h>
// std
#include <functional>
#include <string>
#include <vector>
// ours
#include "Image.h"
#include "Window.h"

namespace anari_viewer::windows {

class ImageViewport : public anari_viewer::windows::Window
{
 public:
  ImageViewport(const std::vector<Image>& images, const char *name = "Image Viewport");
  ~ImageViewport() = default;

  void buildUI() override;

  void showImage(size_t index);

 private:
  void reshape(anari::math::int2 newWindowSize);

  std::vector<Image> m_images;
  ssize_t m_imageIndex{-1};

  GLuint m_framebufferTexture{0};
  anari::math::int2 m_viewportSize{1920, 1080};
};

} // namespace anari_viewer::windows
