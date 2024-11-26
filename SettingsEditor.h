// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// anari
#include "anari_viewer/windows/Window.h"
// std
#include <functional>
#include <string>
#include <vector>

namespace windows {

using SettingsUpdateCallback =
    std::function<void(const float &)>;

class SettingsEditor : public anari_viewer::Window
{
 public:
  SettingsEditor(const char *name = "Settings Editor");
  ~SettingsEditor() = default;

  void buildUI() override;

  void setUpdateCallback(SettingsUpdateCallback cb);
  void triggerUpdateCallback();

 private:
  // callback called whenever settings are updated
  SettingsUpdateCallback m_updateCallback;

  // flag indicating transfer function has changed in UI
  bool m_settingsChanged{true};

  // photon energy
  float m_photonEnergy{120000.f};
  float m_defaultPhotonEnergy{120000.f};
};

} // namespace windows
