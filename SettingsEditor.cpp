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
    triggerUpdateScatterFractionCallback();
    triggerUpdateScatterSigmaCallback();
    triggerUpdateVoxelSpacingCallback();
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
      ImGui::SliderFloat("scatter fraction", &m_scatterFraction, 0.f, 1.f);

  m_settingsChanged |=
      ImGui::SliderFloat("scatter sigma", &m_scatterSigma, 0.f, 1000.f);

  m_settingsChanged |=
      ImGui::InputFloat3("voxel spacing [mm]", m_voxelSpacing, "%.5f");

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

void SettingsEditor::setUpdateScatterFractionCallback(SettingsUpdateScatterFractionCallback cb)
{
  m_updateScatterFractionCallback = cb;
  triggerUpdateScatterFractionCallback();
}

void SettingsEditor::setUpdateScatterSigmaCallback(SettingsUpdateScatterSigmaCallback cb)
{
  m_updateScatterSigmaCallback = cb;
  triggerUpdateScatterSigmaCallback();
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

void SettingsEditor::triggerUpdateScatterFractionCallback()
{
  if (m_updateScatterFractionCallback)
    m_updateScatterFractionCallback(m_scatterFraction);
}

void SettingsEditor::triggerUpdateScatterSigmaCallback()
{
  if (m_updateScatterSigmaCallback)
    m_updateScatterSigmaCallback(m_scatterSigma);
}

void SettingsEditor::triggerUpdateVoxelSpacingCallback()
{
  if (m_updateVoxelSpacingCallback)
    m_updateVoxelSpacingCallback({m_voxelSpacing[0], m_voxelSpacing[1], m_voxelSpacing[2]});
}

} // namespace anari_viewer::windows
