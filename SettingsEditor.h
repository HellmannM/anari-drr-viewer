// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// anari
#include "anari_viewer/windows/Window.h"
// std
#include <functional>
#include <string>
#include <vector>

namespace anari_viewer::windows {

using SettingsUpdatePhotonEnergyCallback =
    std::function<void(const float &)>;
using SettingsUpdateLacLutCallback =
    std::function<void(const size_t &)>;
using SettingsUpdateVoxelSpacingCallback =
    std::function<void(const std::array<float, 3> &)>;

class SettingsEditor : public anari_viewer::windows::Window
{
 public:
  SettingsEditor(const char *name = "Settings Editor");
  ~SettingsEditor() = default;

  void buildUI() override;

  void setActiveLacLut(size_t id);
  void setVoxelSpacing(const std::array<float, 3> &voxelSpacing);
  void setLacLutNames(std::vector<std::pair<size_t, std::string>> names);
  void setLacLut(size_t lacLutIndex);
  void setUpdateLacLutCallback(SettingsUpdateLacLutCallback cb);
  void setUpdatePhotonEnergyCallback(SettingsUpdatePhotonEnergyCallback cb);
  void setUpdateVoxelSpacingCallback(SettingsUpdateVoxelSpacingCallback cb);
  void triggerUpdateLacLutCallback();
  void triggerUpdatePhotonEnergyCallback();
  void triggerUpdateVoxelSpacingCallback();

 private:
  // callback called whenever settings are updated
  SettingsUpdatePhotonEnergyCallback m_updatePhotonEnergyCallback;
  SettingsUpdateLacLutCallback m_updateLacLutCallback;
  SettingsUpdateVoxelSpacingCallback m_updateVoxelSpacingCallback;

  // flag indicating transfer function has changed in UI
  bool m_settingsChanged{true};

  // LAC LUTs
  std::vector<std::pair<size_t, std::string>> m_names;
  size_t m_lacLutId{0};
  // photon energy
  float m_photonEnergy{120000.f};
  float m_defaultPhotonEnergy{120000.f};
  // voxel spacing
  float m_voxelSpacing[3]{1.f, 1.f, 1.f};
};

} // namespace anari_viewer::windows
