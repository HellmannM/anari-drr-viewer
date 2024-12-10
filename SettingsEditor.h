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

using SettingsUpdatePhotonEnergyCallback =
    std::function<void(const float &)>;
using SettingsUpdateLacLutCallback =
    std::function<void(const size_t &)>;

class SettingsEditor : public anari_viewer::Window
{
 public:
  SettingsEditor(const char *name = "Settings Editor");
  ~SettingsEditor() = default;

  void buildUI() override;

  void setActiveLacLut(size_t id);
  void setLacLutNames(std::vector<std::pair<size_t, std::string>> names);
  void setLacLut(size_t lacLutIndex);
  void setUpdateLacLutCallback(SettingsUpdateLacLutCallback cb);
  void setUpdatePhotonEnergyCallback(SettingsUpdatePhotonEnergyCallback cb);
  void triggerUpdateLacLutCallback();
  void triggerUpdatePhotonEnergyCallback();

 private:
  // callback called whenever settings are updated
  SettingsUpdatePhotonEnergyCallback m_updatePhotonEnergyCallback;
  SettingsUpdateLacLutCallback m_updateLacLutCallback;

  // flag indicating transfer function has changed in UI
  bool m_settingsChanged{true};

  // LAC LUTs
  std::vector<std::pair<size_t, std::string>> m_names;
  size_t m_lacLutId{0};

  // photon energy
  float m_photonEnergy{120000.f};
  float m_defaultPhotonEnergy{120000.f};
};

} // namespace windows
