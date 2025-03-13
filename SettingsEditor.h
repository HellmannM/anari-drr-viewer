// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// std
#include <functional>
#include <string>
#include <vector>
// ours
#include "Window.h"

namespace anari_viewer::windows {

using SettingsUpdateScatterFractionCallback =
    std::function<void(const float &)>;
using SettingsUpdateScatterSigmaCallback =
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
  void setUpdateScatterFractionCallback(SettingsUpdateScatterFractionCallback cb);
  void setUpdateScatterSigmaCallback(SettingsUpdateScatterSigmaCallback cb);
  void setUpdateVoxelSpacingCallback(SettingsUpdateVoxelSpacingCallback cb);
  void triggerUpdateLacLutCallback();
  void triggerUpdateScatterFractionCallback();
  void triggerUpdateScatterSigmaCallback();
  void triggerUpdateVoxelSpacingCallback();

 private:
  // callback called whenever settings are updated
  SettingsUpdateScatterFractionCallback m_updateScatterFractionCallback;
  SettingsUpdateScatterSigmaCallback m_updateScatterSigmaCallback;
  SettingsUpdateLacLutCallback m_updateLacLutCallback;
  SettingsUpdateVoxelSpacingCallback m_updateVoxelSpacingCallback;

  // flag indicating transfer function has changed in UI
  bool m_settingsChanged{true};

  // LAC LUTs
  std::vector<std::pair<size_t, std::string>> m_names;
  size_t m_lacLutId{0};
  // scatter
  float m_scatterFraction{0.5f};
  float m_scatterSigma{50.f};
  // voxel spacing
  float m_voxelSpacing[3]{1.f, 1.f, 1.f};
};

} // namespace anari_viewer::windows
