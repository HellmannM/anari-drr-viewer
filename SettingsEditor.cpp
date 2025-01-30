// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "SettingsEditor.h"
// std
#include <algorithm>
#include <vector>

namespace anari_viewer::windows {

SettingsEditor::SettingsEditor(const char *name)
    : Window(name, true) {}

void SettingsEditor::buildUI()
{
  if (m_settingsChanged) {
    triggerUpdatePhotonEnergyCallback();
    m_settingsChanged = false;
  }

  //combo box
  std::vector<const char *> names(m_names.size(), nullptr);
  std::transform(m_names.begin(),
      m_names.end(),
      names.begin(),
      [](const std::pair<size_t, std::string> &t) { return t.second.c_str(); });
  int lacLutIdIndex = static_cast<int>(m_lacLutId);
  if (ImGui::Combo("LAC LUT", &lacLutIdIndex, names.data(), names.size()))
    setLacLut(static_cast<size_t>(lacLutIdIndex));

  ImGui::Separator();

  m_settingsChanged |=
      ImGui::SliderFloat("energy [eV]", &m_photonEnergy, 0.f, 150000.f);

  m_settingsChanged |=
      ImGui::InputFloat3("voxel spacing [mm]", m_voxelSpacing, "%.5f");

  if (ImGui::Button("reset##energy")) {
    m_photonEnergy = m_defaultPhotonEnergy;
    m_settingsChanged = true;
  }

  ImGui::Separator();

//  //DragFloatRange2
//  m_tfnChanged |= ImGui::DragFloatRange2("value range",
//      &m_valueRange.x,
//      &m_valueRange.y,
//      0.1f,
//      -10000.f,
//      10000.0f,
//      "Min: %.7f",
//      "Max: %.7f");
//
//  if (ImGui::Button("reset##valueRange")) {
//    m_valueRange = m_defaultValueRange;
//    m_tfnChanged = true;
//  }
}

void SettingsEditor::setLacLut(size_t lacLutIndex)
{
  auto lacLutId = m_names[lacLutIndex].first;
  if (lacLutId == m_lacLutId)
    return;
  m_lacLutId = lacLutId;
  triggerUpdateLacLutCallback();
  //TODO set photonEnergy
}

void SettingsEditor::setActiveLacLut(size_t id)
{
  m_lacLutId = id;
}

void SettingsEditor::setVoxelSpacing(const std::array<float, 3> &voxelSpacing)
{
  std::copy(voxelSpacing.begin(), voxelSpacing.end(), m_voxelSpacing);
}

void SettingsEditor::setLacLutNames(std::vector<std::pair<size_t, std::string>> names)
{
  m_names = names;
}

void SettingsEditor::setUpdateLacLutCallback(SettingsUpdateLacLutCallback cb)
{
  m_updateLacLutCallback = cb;
  triggerUpdateLacLutCallback();
}

void SettingsEditor::setUpdatePhotonEnergyCallback(SettingsUpdatePhotonEnergyCallback cb)
{
  m_updatePhotonEnergyCallback = cb;
  triggerUpdatePhotonEnergyCallback();
}

void SettingsEditor::setUpdateVoxelSpacingCallback(SettingsUpdateVoxelSpacingCallback cb)
{
  m_updateVoxelSpacingCallback = cb;
  triggerUpdateVoxelSpacingCallback();
}

void SettingsEditor::triggerUpdateLacLutCallback()
{
  if (m_updateLacLutCallback)
    m_updateLacLutCallback(m_lacLutId);
}

void SettingsEditor::triggerUpdatePhotonEnergyCallback()
{
  if (m_updatePhotonEnergyCallback)
    m_updatePhotonEnergyCallback(m_photonEnergy);
}

void SettingsEditor::triggerUpdateVoxelSpacingCallback()
{
  if (m_updateVoxelSpacingCallback)
  {
    m_updateVoxelSpacingCallback({m_voxelSpacing[0], m_voxelSpacing[1], m_voxelSpacing[2]});
  }
}

} // namespace anari_viewer::windows
