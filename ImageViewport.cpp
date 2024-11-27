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
    : Window(name, true), m_images(images)
{
  glGenTextures(1, &m_framebufferTexture);
  glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      m_viewportSize.x,
      m_viewportSize.y,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      0);

  reshape(m_viewportSize);
}

void ImageViewport::buildUI()
{
  ImVec2 _viewportSize = ImGui::GetContentRegionAvail();
  anari::math::int2 viewportSize(_viewportSize.x, _viewportSize.y);

  if (m_viewportSize != viewportSize)
    reshape(viewportSize);
}

void ImageViewport::clear()
{
  //TODO
}

void ImageViewport::showImage(size_t index)
{
  if ((index >= m_images.size()) || (m_images[index].data.empty()))
    return;
  if (m_images[index].bpp == 4) {
    printf("bad image: unsupported bpp = %lu\n", m_images[index].bpp);
    return;
  }
    
  glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
  glTexSubImage2D(GL_TEXTURE_2D,
      0,
      0,
      0,
      m_images[index].width,
      m_images[index].height,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      m_images[index].data.data());
}

void ImageViewport::reshape(anari::math::int2 newSize)
{
  if (newSize.x <= 0 || newSize.y <= 0)
    return;

  m_viewportSize = newSize;

  glViewport(0, 0, newSize.x, newSize.y);

  glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
  glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      newSize.x,
      newSize.y,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      0);

  clear();
}

} // namespace windows
