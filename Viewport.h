// Copyright 2023-2024 The Khronos Group, Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Orbit.h"
#include "../ui_anari.h"
// glad
#include "glad/glad.h"
// glfw
#include <GLFW/glfw3.h>
// anari
#include <anari/anari_cpp/ext/linalg.h>
#include <anari/anari_cpp.hpp>
// anari_viewer
#include "anari_viewer/windows/Window.h"
// visionaray
#include <common/input/mouse_event.h>
#include <common/manip/camera_manipulator.h>
#include <visionaray/pinhole_camera.h>
// std
#include <array>
#include <limits>
#include <memory>

namespace anari_viewer::windows {

struct DRRViewport : public anari_viewer::windows::Window
{
  DRRViewport(anari::Device device, visionaray::pinhole_camera& camera, const char *name = "Viewport");
  ~DRRViewport();

  void buildUI() override;

  void setWorld(anari::World world = nullptr, bool resetCameraView = true);

  void addManipulator(std::shared_ptr<visionaray::camera_manipulator> manip);
  void setCamera(visionaray::pinhole_camera &camera);

  void resetView();
  void setView(anari::math::float3 eye, anari::math::float3 center, anari::math::float3 up);
  void getView(anari::math::float3& eye, anari::math::float3& center, anari::math::float3& up, float& fovy, float& aspect);
  void setPhotonEnergy(float energy);
  bool getFrame(std::vector<uint8_t>& color, std::vector<float>& depth3d, size_t& width, size_t& height);

  anari::Device device() const;

 private:
  void reshape(anari::math::int2 newWindowSize);

  void startNewFrame();
  void updateFrame();
  void updateCamera(bool force = false);
  void updateImage();
  void cancelFrame();

  void ui_handleInput();
  void ui_contextMenu();
  void ui_overlay();

  void handleMouseDownEvent(visionaray::mouse_event const& event);
  void handleMouseUpEvent(visionaray::mouse_event const& event);
  void handleMouseMoveEvent(visionaray::mouse_event const& event);

  // Data /////////////////////////////////////////////////////////////////////

  bool m_currentlyRendering{true};
  bool m_contextMenuVisible{false};
  bool m_frameCancelled{false};
  bool m_saveNextFrame{false};
  int m_screenshotIndex{0};

  bool m_dolly{false};
  bool m_pan{false};
  bool m_orbit{false};
  bool m_viewChanged{false};
  visionaray::mouse::button m_button{visionaray::mouse::button::NoButton};
  visionaray::keyboard::key m_modifier{visionaray::keyboard::key::NoKey};
  visionaray::pinhole_camera &m_camera;
  std::vector<std::shared_ptr<visionaray::camera_manipulator>> m_manipulators;

  bool m_showOverlay{true};
  int m_frameSamples{0};
  bool m_singleShot{true};

  float m_fov{40.f};

  // ANARI objects //

  anari::DataType m_format{ANARI_UFIXED8_RGBA_SRGB};

  anari::Device m_device{nullptr};
  anari::Frame m_frame{nullptr};
  anari::World m_world{nullptr};

  anari::Camera m_perspCamera{nullptr};

  std::vector<std::string> m_rendererNames;
  std::vector<anari_viewer::ui::ParameterList> m_rendererParameters;
  std::vector<anari::Renderer> m_renderers;
  int m_currentRenderer{0};

  // OpenGL + display

  GLuint m_framebufferTexture{0};
  anari::math::int2 m_viewportSize{1920, 1080};
  anari::math::int2 m_renderSize{1920, 1080};

  float m_latestFL{1.f};
  float m_minFL{std::numeric_limits<float>::max()};
  float m_maxFL{-std::numeric_limits<float>::max()};

  std::string m_overlayWindowName;
  std::string m_contextMenuName;
};

} // namespace anari_viewer::windows
