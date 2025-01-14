// Copyright 2023-2024 The Khronos Group, Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "Viewport.h"
// Visionaray
#include <common/input/keyboard.h>
#include <common/input/mouse.h>
// std
#include <cstring>
#include <memory>
// stb_image
#include "stb_image_write.h"

namespace anari_viewer::windows {

///////////////////////////////////////////////////////////////////////////////
// DRRViewport definitions ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DRRViewport::DRRViewport(anari::Device device, visionaray::pinhole_camera &camera, const char *name)
    : Window(name, true), m_device(device), m_camera(camera)
{
  m_overlayWindowName = "overlay_";
  m_overlayWindowName += name;

  m_contextMenuName = "vpContextMenu_";
  m_contextMenuName += name;

  // GL //

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

  // ANARI //

  anari::retain(m_device, m_device);

  const char **r_subtypes = anariGetObjectSubtypes(m_device, ANARI_RENDERER);

  if (r_subtypes != nullptr) {
    for (int i = 0; r_subtypes[i] != nullptr; i++) {
      std::string subtype = r_subtypes[i];
      auto parameters =
          anari_viewer::ui::parseParameters(m_device, ANARI_RENDERER, subtype.c_str());
      m_rendererNames.push_back(subtype);
      m_rendererParameters.push_back(parameters);
    }
  } else
    m_rendererNames.emplace_back("default");

  anari::commitParameters(m_device, m_device);

  m_frame = anari::newObject<anari::Frame>(m_device);
  m_perspCamera = anari::newObject<anari::Camera>(m_device, "perspective");

  for (auto &name : m_rendererNames) {
    m_renderers.push_back(
        anari::newObject<anari::Renderer>(m_device, name.c_str()));
  }

  reshape(m_viewportSize);
  setWorld();
  updateFrame();
  updateCamera(true);
  startNewFrame();
  updateImage();
}

DRRViewport::~DRRViewport()
{
  cancelFrame();
  anari::wait(m_device, m_frame);

  anari::release(m_device, m_perspCamera);
  anari::release(m_device, m_world);
  for (auto &r : m_renderers)
    anari::release(m_device, r);
  anari::release(m_device, m_frame);
  anari::release(m_device, m_device);
}

void DRRViewport::buildUI()
{
  ImVec2 _viewportSize = ImGui::GetContentRegionAvail();
  anari::math::int2 viewportSize(_viewportSize.x, _viewportSize.y);

  if (m_viewportSize != viewportSize)
    reshape(viewportSize);

  if (m_viewChanged || (!m_singleShot)) {
    updateCamera();
    updateImage();
  }

  ImGui::Image((void *)(intptr_t)m_framebufferTexture,
      ImGui::GetContentRegionAvail(),
      ImVec2(1, 0),
      ImVec2(0, 1));

  if (m_showOverlay)
    ui_overlay();

  ui_contextMenu();

  if (!m_contextMenuVisible)
    ui_handleInput();
}

void DRRViewport::setWorld(anari::World world, bool resetCameraView)
{
  if (m_world)
    anari::release(m_device, m_world);

  if (!world) {
    world = anari::newObject<anari::World>(m_device);
    resetCameraView = false;
  } else
    anari::retain(m_device, world);

  anari::commitParameters(m_device, world);
  m_world = world;

  if (resetCameraView)
    resetView();

  updateFrame();
}

void DRRViewport::addManipulator(std::shared_ptr<visionaray::camera_manipulator> manip)
{
  m_manipulators.push_back(manip);
}

void DRRViewport::setCamera(visionaray::pinhole_camera &camera)
{
  m_camera = camera;
  m_viewChanged = true;
}

void DRRViewport::resetView()
{
  float aspect = m_viewportSize.x / float(m_viewportSize.y);
  m_camera.perspective(45.0f * visionaray::constants::degrees_to_radians<float>(), aspect, 0.001f, 1000.0f);

  anari::math::float3 bounds[2] = {{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}};

  if (!anariGetProperty(m_device,
          m_world,
          "bounds",
          ANARI_FLOAT32_BOX3,
          &bounds[0],
          sizeof(bounds),
          ANARI_WAIT)) {
    printf("WARNING: bounds not returned by the device! Using unit cube.\n");
  }

  visionaray::vec3f min{bounds[0].x, bounds[0].y, bounds[0].z};
  visionaray::vec3f max{bounds[1].x, bounds[1].y, bounds[1].z};
  m_camera.view_all({min, max}, {0.f, 1.f, 0.f});
  m_viewChanged = true;

  cancelFrame();
  updateCamera(true);
  startNewFrame();
  updateImage();
}

void DRRViewport::setView(anari::math::float3 eye, anari::math::float3 center, anari::math::float3 up)
{
  m_camera.look_at({eye.x, eye.y, eye.z}, {center.x, center.y, center.z}, {up.x, up.y, up.z});

  m_viewChanged = true;
  cancelFrame();
  updateCamera(true);
  startNewFrame();
  updateImage();
}

void DRRViewport::getView(anari::math::float3& eye, anari::math::float3& center, anari::math::float3& up, float& fovy, float& aspect)
{
  auto& e = m_camera.eye();
  auto& c = m_camera.center();
  auto& u = m_camera.up();
  eye    = {e.x, e.y, e.z};
  center = {c.x, c.y, c.z};
  up     = {u.x, u.y, u.z};
  fovy = m_camera.fovy();
  aspect = m_camera.aspect();
}

anari::Device DRRViewport::device() const
{
  return m_device;
}

void DRRViewport::setPhotonEnergy(float energy)
{
  anari::setParameter(m_device, m_renderers[m_currentRenderer], "photonEnergy", ANARI_FLOAT32, &energy);
  anari::commitParameters(m_device, m_renderers[m_currentRenderer]);
  
  m_viewChanged = true;
  cancelFrame();
  updateCamera(true);
  startNewFrame();
  updateImage();
}

bool DRRViewport::getFrame(std::vector<uint8_t>& color, std::vector<float>& depth3d, size_t& width, size_t& height)
{
    auto fb = anari::map<uint32_t>(m_device, m_frame, "channel.color");
    auto db = anari::map<anari::math::float3>(m_device, m_frame, "channel.depth3D");

    if (fb.data && db.data) {
      width = m_viewportSize.x;
      height = m_viewportSize.y;
      const auto fbDataU8 = reinterpret_cast<const uint8_t*>(fb.data);
      const auto dbDataFloat = reinterpret_cast<const float*>(db.data);
      color = std::vector<uint8_t>(fbDataU8, fbDataU8 + width * height * 4);
      depth3d = std::vector<float>(dbDataFloat, dbDataFloat + width * height * 3);
    } else {
      printf("mapped bad frame: %p | %i x %i\n", fb.data, fb.width, fb.height);
      return false;
    }

    anari::unmap(m_device, m_frame, "channel.color");
    anari::unmap(m_device, m_frame, "channel.depth3D");
    return true;
}

void DRRViewport::reshape(anari::math::int2 newSize)
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

  m_camera.set_viewport(0, 0, newSize.x, newSize.y);
  float fovy = m_camera.fovy();
  float aspect = newSize.x / static_cast<float>(newSize.y);
  float z_near = std::max(m_camera.z_near(), 1.f);
  float z_far = std::min(m_camera.z_far(), FLT_MAX);
  m_camera.perspective(fovy, aspect, z_near, z_far);

  updateFrame();
  cancelFrame();
  updateCamera(true);
  startNewFrame();
  updateImage();
}

void DRRViewport::startNewFrame()
{
  anari::getProperty(
      m_device, m_frame, "numSamples", m_frameSamples, ANARI_NO_WAIT);
  anari::render(m_device, m_frame);
  m_renderSize = m_viewportSize;
  m_currentlyRendering = true;
  m_frameCancelled = false;
}

void DRRViewport::updateFrame()
{
  anari::setParameter(
      m_device, m_frame, "size", anari::math::uint2(m_viewportSize));
  anari::setParameter(
      m_device, m_frame, "channel.color", ANARI_UFIXED8_RGBA_SRGB);
  anari::setParameter(m_device, m_frame, "accumulation", true);
  anari::setParameter(m_device, m_frame, "world", m_world);
  anari::setParameter(m_device, m_frame, "camera", m_perspCamera);
  anari::setParameter(
      m_device, m_frame, "renderer", m_renderers[m_currentRenderer]);

  anari::commitParameters(m_device, m_frame);
}

void DRRViewport::updateCamera(bool force)
{
  if (!force && !m_viewChanged)
    return;

  const auto& vEye = m_camera.eye();
  auto vDir = visionaray::normalize(m_camera.center() - vEye);
  const auto& vUp  = m_camera.up();
  anari::math::float3 eye{vEye.x, vEye.y, vEye.z};
  anari::math::float3 dir{vDir.x, vDir.y, vDir.z};
  anari::math::float3 up{vUp.x, vUp.y, vUp.z};

  anari::setParameter(m_device, m_perspCamera, "aspect", m_viewportSize.x / float(m_viewportSize.y));
  anari::setParameter(m_device, m_perspCamera, "position", eye);
  anari::setParameter(m_device, m_perspCamera, "direction", dir);
  anari::setParameter(m_device, m_perspCamera, "up", up);

  auto radians = [](float degrees) -> float { return degrees * M_PI / 180.f; };
  anari::setParameter(m_device, m_perspCamera, "fovy", radians(m_fov));

  anari::commitParameters(m_device, m_perspCamera);

  m_viewChanged = false;
  return;
}

void DRRViewport::updateImage()
{
  if (m_frameCancelled)
    anari::wait(m_device, m_frame);
  else if (m_saveNextFrame
      || (m_currentlyRendering && anari::isReady(m_device, m_frame))) {
    m_currentlyRendering = false;

    float duration = 0.f;
    anari::getProperty(m_device, m_frame, "duration", duration);

    m_latestFL = duration * 1000;
    m_minFL = std::min(m_minFL, m_latestFL);
    m_maxFL = std::max(m_maxFL, m_latestFL);

    auto fb = anari::map<uint32_t>(m_device, m_frame, "channel.color");

    if (fb.data) {
      glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
      glTexSubImage2D(GL_TEXTURE_2D,
          0,
          0,
          0,
          fb.width,
          fb.height,
          GL_RGBA,
          GL_UNSIGNED_BYTE,
          fb.data);
    } else {
      printf("mapped bad frame: %p | %i x %i\n", fb.data, fb.width, fb.height);
    }

    if (m_saveNextFrame) {
      std::string filename =
          "screenshot" + std::to_string(m_screenshotIndex++) + ".png";
      stbi_write_png(
          filename.c_str(), fb.width, fb.height, 4, fb.data, 4 * fb.width);
      printf("frame saved to '%s'\n", filename.c_str());
      m_saveNextFrame = false;
    }

    anari::unmap(m_device, m_frame, "channel.color");
  }

  if (!m_currentlyRendering || m_frameCancelled)
    startNewFrame();
}

void DRRViewport::cancelFrame()
{
  m_frameCancelled = true;
  anari::discard(m_device, m_frame);
}

void DRRViewport::handleMouseDownEvent(visionaray::mouse_event const& event)
{
  for (auto& manip : m_manipulators)
    manip->handle_mouse_down(event);
}

void DRRViewport::handleMouseUpEvent(visionaray::mouse_event const& event)
{
  for (auto& manip : m_manipulators)
    manip->handle_mouse_up(event);
}

void DRRViewport::handleMouseMoveEvent(visionaray::mouse_event const& event)
{
  for (auto& manip : m_manipulators)
    manip->handle_mouse_move(event);
}

void DRRViewport::ui_handleInput()
{
  ImGuiIO &io = ImGui::GetIO();

  const bool dolly = ImGui::IsMouseDown(ImGuiMouseButton_Right)
      || (ImGui::IsMouseDown(ImGuiMouseButton_Left)
          && io.KeysDown[GLFW_KEY_LEFT_SHIFT]);
  const bool pan = ImGui::IsMouseDown(ImGuiMouseButton_Middle)
      || (ImGui::IsMouseDown(ImGuiMouseButton_Left)
          && io.KeysDown[GLFW_KEY_LEFT_ALT]);
  const bool orbit = ImGui::IsMouseDown(ImGuiMouseButton_Left);

  if (!m_dolly && !m_pan && !m_orbit)
    if (!ImGui::IsItemHovered() || !(dolly || pan || orbit))
      return;

  anari::math::float2 position;
  std::memcpy(&position, &io.MousePos, sizeof(position));
  auto windowOffset = ImGui::GetCursorScreenPos();
  const visionaray::mouse::pos mousePos = {
      static_cast<int>(m_viewportSize.x - position.x + windowOffset.x),
      static_cast<int>(-position.y + windowOffset.y)};

  // map buttons/keys
  auto button = visionaray::mouse::button::NoButton;
  auto modifier = visionaray::keyboard::key::NoKey;
  if (dolly){
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)){      
      button = visionaray::mouse::button::Right;
      modifier = visionaray::keyboard::key::NoKey;
    }
    else if ((ImGui::IsMouseDown(ImGuiMouseButton_Left)
                 && io.KeysDown[GLFW_KEY_LEFT_SHIFT])) {
      button = visionaray::mouse::button::Left;
      modifier = visionaray::keyboard::key::Shift;
    }
  }
  else if (pan) {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)){      
      button = visionaray::mouse::button::Middle;
      modifier = visionaray::keyboard::key::NoKey;
    }
    else if ((ImGui::IsMouseDown(ImGuiMouseButton_Left)
                 && io.KeysDown[GLFW_KEY_LEFT_ALT])) {
      button = visionaray::mouse::button::Left;
      modifier = visionaray::keyboard::key::Alt;
    }
  }
  else if (orbit) {
    button = visionaray::mouse::button::Left;
    modifier = visionaray::keyboard::key::NoKey;
  }

  if (dolly) {
    if (m_dolly) {
      //mouse move
      handleMouseMoveEvent(visionaray::mouse_event(visionaray::mouse::Move, mousePos, m_button, visionaray::keyboard::NoKey));
      m_viewChanged = true;
    } else {
      //onMouseDown
      handleMouseDownEvent(visionaray::mouse_event(visionaray::mouse::ButtonDown, mousePos, button, modifier));
      m_button = button;
      m_modifier = modifier;
      m_dolly = true;
    }
  } else {
    if (m_dolly) {
      //onMouseUp
      handleMouseUpEvent(visionaray::mouse_event(visionaray::mouse::ButtonUp, mousePos, m_button, m_modifier));
      m_button = visionaray::mouse::button::NoButton;
      m_modifier = visionaray::keyboard::key::NoKey;
      m_dolly = false;
    }
  }

  if (pan) {
    if (m_pan) {
      //mouse move
      handleMouseMoveEvent(visionaray::mouse_event(visionaray::mouse::Move, mousePos, m_button, visionaray::keyboard::NoKey));
      m_viewChanged = true;
    } else {
      //onMouseDown
      handleMouseDownEvent(visionaray::mouse_event(visionaray::mouse::ButtonDown, mousePos, button, modifier));
      m_button = button;
      m_modifier = modifier;
      m_pan = true;
    }
  } else {
    if (m_pan) {
      //onMouseUp
      handleMouseUpEvent(visionaray::mouse_event(visionaray::mouse::ButtonUp, mousePos, m_button, m_modifier));
      m_button = visionaray::mouse::button::NoButton;
      m_modifier = visionaray::keyboard::key::NoKey;
      m_pan = false;
    }
  }

  if (orbit) {
    if (m_orbit) {
      //mouse move
      handleMouseMoveEvent(visionaray::mouse_event(visionaray::mouse::Move, mousePos, m_button, visionaray::keyboard::NoKey));
      m_viewChanged = true;
    } else {
      //onMouseDown
      handleMouseDownEvent(visionaray::mouse_event(visionaray::mouse::ButtonDown, mousePos, button, modifier));
      m_button = button;
      m_modifier = modifier;
      m_orbit = true;
    }
  } else {
    if (m_orbit) {
      //onMouseUp
      handleMouseUpEvent(visionaray::mouse_event(visionaray::mouse::ButtonUp, mousePos, m_button, m_modifier));
      m_button = visionaray::mouse::button::NoButton;
      m_modifier = visionaray::keyboard::key::NoKey;
      m_orbit = false;
    }
  }
}

void DRRViewport::ui_contextMenu()
{
  constexpr float INDENT_AMOUNT = 25.f;

  const bool rightClicked = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Right);

  if (rightClicked && ImGui::IsWindowHovered()) {
    m_contextMenuVisible = true;
    ImGui::OpenPopup(m_contextMenuName.c_str());
  }

  if (ImGui::BeginPopup(m_contextMenuName.c_str())) {
    ImGui::Text("Renderer:");
    ImGui::Indent(INDENT_AMOUNT);

    if (m_renderers.size() > 1 && ImGui::BeginMenu("subtype")) {
      for (int i = 0; i < m_rendererNames.size(); i++) {
        const auto& rendererName = m_rendererNames[i];
        if (ImGui::MenuItem(rendererName.c_str())) {
          if ((rendererName == "DRR") || (rendererName == "drr") || (rendererName == "default"))
            m_singleShot = true;
          else
            m_singleShot = false;
          m_currentRenderer = i;
          updateFrame();
          cancelFrame();
          startNewFrame();
          updateImage();
        }
      }
      ImGui::EndMenu();
    }

    if (!m_rendererParameters.empty() && ImGui::BeginMenu("parameters")) {
      auto &parameters = m_rendererParameters[m_currentRenderer];
      auto renderer = m_renderers[m_currentRenderer];
      for (auto &p : parameters)
        anari_viewer::ui::buildUI(m_device, renderer, p);
      ImGui::EndMenu();
    }

    ImGui::Unindent(INDENT_AMOUNT);
    ImGui::Separator();

    ImGui::Text("Camera:");
    ImGui::Indent(INDENT_AMOUNT);

    if (ImGui::SliderFloat("fov", &m_fov, 0.1f, 180.f)) {
      updateCamera(true);
      cancelFrame();
      startNewFrame();
      updateImage();
    }

    ImGui::EndDisabled();

    // if (ImGui::Combo("up", &m_arcballUp, "+x\0+y\0+z\0-x\0-y\0-z\0\0")) {
    //   m_arcball->setAxis(static_cast<manipulators::OrbitAxis>(m_arcballUp));
    //   resetView();
    // }

    if (ImGui::MenuItem("reset view"))
      resetView();

    ImGui::Unindent(INDENT_AMOUNT);
    ImGui::Separator();

    ImGui::Text("DRRViewport:");
    ImGui::Indent(INDENT_AMOUNT);

    ImGui::Checkbox("show stats", &m_showOverlay);
    if (ImGui::MenuItem("reset stats")) {
      m_minFL = m_latestFL;
      m_maxFL = m_latestFL;
    }

    if (ImGui::MenuItem("take screenshot"))
      m_saveNextFrame = true;

    ImGui::Unindent(INDENT_AMOUNT);
    ImGui::Separator();

    ImGui::Text("World:");
    ImGui::Indent(INDENT_AMOUNT);

    if (ImGui::MenuItem("print bounds")) {
      anari::math::float3 bounds[2];

      anariGetProperty(m_device,
          m_world,
          "bounds",
          ANARI_FLOAT32_BOX3,
          &bounds[0],
          sizeof(bounds),
          ANARI_WAIT);

      printf("current world bounds {%f, %f, %f} x {%f, %f, %f}\n",
          bounds[0].x,
          bounds[0].y,
          bounds[0].z,
          bounds[1].x,
          bounds[1].y,
          bounds[1].z);
    }

    ImGui::Unindent(INDENT_AMOUNT);

    if (!ImGui::IsPopupOpen(m_contextMenuName.c_str()))
      m_contextMenuVisible = false;

    ImGui::EndPopup();
  }
}

void DRRViewport::ui_overlay()
{
  ImGuiIO &io = ImGui::GetIO();
  ImVec2 windowPos = ImGui::GetWindowPos();
  windowPos.x += 10;
  windowPos.y += 25 * io.FontGlobalScale;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
      | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing
      | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);

  ImGui::Begin(m_overlayWindowName.c_str(), nullptr, window_flags);

  ImGui::Text("viewport: %i x %i", m_viewportSize.x, m_viewportSize.y);
  ImGui::Text(" samples: %i", m_frameSamples);

  if (m_currentlyRendering)
    ImGui::Text(" latency: %.2fms", m_latestFL);
  else
    ImGui::Text(" latency: --");

  ImGui::Text("   (min): %.2fms", m_minFL);
  ImGui::Text("   (max): %.2fms", m_maxFL);

  ImGui::Separator();

  static bool showCameraInfo = false;

  ImGui::Checkbox("camera info", &showCameraInfo);

  if (showCameraInfo) {
    const auto eye = m_camera.eye();
    const auto center = m_camera.center();
    const auto dir = visionaray::normalize(center-eye);
    const auto up = m_camera.up();
    ImGui::Text("   eye: (%f, %f, %f)", eye.x, eye.y, eye.z);
    ImGui::Text("center: (%f, %f, %f)", center.x, center.y, center.z);
    ImGui::Text("   dir: (%f, %f, %f)", dir.x, dir.y, dir.z);
    ImGui::Text("    up: (%f, %f, %f)", up.x, up.y, up.z);
  }

  ImGui::End();
}

} // namespace anari_viewer::windows
