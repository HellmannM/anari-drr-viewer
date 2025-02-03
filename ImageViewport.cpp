// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "ImageViewport.h"
// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace anari_viewer::windows {

ImageViewport::ImageViewport(const std::vector<Image>& images, const char *name)
    : Window(name, true), m_images(images)
{
  glGenTextures(1, &m_framebufferTexture);
  glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

  ImGui::Image((void *)(intptr_t)m_framebufferTexture,
      ImGui::GetContentRegionAvail(),
      ImVec2(1, 0),
      ImVec2(0, 1));
}

void ImageViewport::showImage(size_t index)
{
  if ((index >= m_images.size()) || (m_images[index].data.empty()))
    return;

  const uint8_t* imageData{nullptr};
  std::vector<uint8_t> swizzledImageData;
  if (m_images[index].bpp == 3) {
    // convert to rgba first
    swizzledImageData.resize(m_images[index].width * m_images[index].height * 4);
    for (int i = 0; i < m_images[index].width * m_images[index].height; ++i) {
        swizzledImageData[i * 4 + 0] = m_images[index].data.data()[i * 3 + 0];
        swizzledImageData[i * 4 + 1] = m_images[index].data.data()[i * 3 + 1];
        swizzledImageData[i * 4 + 2] = m_images[index].data.data()[i * 3 + 2];
        swizzledImageData[i * 4 + 3] = 255;
    }
    imageData = swizzledImageData.data();
  }
  else if (m_images[index].bpp == 4) {
    imageData = m_images[index].data.data();
  }
  else {
    printf("bad image: unsupported bpp = %lu\n", m_images[index].bpp);
    return;
  }
  m_imageIndex = static_cast<ssize_t>(index);

  // copy into temporary texture
  GLuint tempTexture{0};
  glGenTextures(1, &tempTexture);
  glBindTexture(GL_TEXTURE_2D, tempTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      m_images[index].width,
      m_images[index].height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      0);
  glTexSubImage2D(GL_TEXTURE_2D,
      0,
      0,
      0,
      m_images[index].width,
      m_images[index].height,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      imageData);

  // setup fb to render into m_framebufferTexture
  GLint oldFbo{0};
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebufferTexture, 0);

  // calc scaling and offset
  int imgWidth = m_images[index].width;
  int imgHeight = m_images[index].height;
  GLfloat offsetX = 0;
  GLfloat offsetY = 0;
  GLfloat renderWidth = m_viewportSize.x;
  GLfloat renderHeight = m_viewportSize.y;
  float ratioImg = float(imgWidth) / imgHeight;
  float ratioScreen = renderWidth / renderHeight;
  if (ratioImg > ratioScreen)
      renderHeight = renderWidth / ratioImg;
  else
      renderWidth = renderHeight * ratioImg;
  offsetX = 0 + (m_viewportSize.x - renderWidth) * .5f;
  offsetY = 0 + (m_viewportSize.y - renderHeight) * .5f;

  // render to fbo (and flip x+y axes)
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, tempTexture);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1, -1, 0);
  glScalef(2.0f / m_viewportSize.x, 2.0f / m_viewportSize.y, 1.0);
  glBegin(GL_QUADS);
  glTexCoord2f(1, 1);
  glVertex2f(offsetX, offsetY);
  glTexCoord2f(0, 1);
  glVertex2f(offsetX + renderWidth, offsetY);
  glTexCoord2f(0, 0);
  glVertex2f(offsetX + renderWidth, offsetY + renderHeight);
  glTexCoord2f(1, 0);
  glVertex2f(offsetX, offsetY + renderHeight);
  glEnd();
  glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
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

  if (m_imageIndex >= 0)
    showImage(m_imageIndex);
}

} // namespace anari_viewer::windows
