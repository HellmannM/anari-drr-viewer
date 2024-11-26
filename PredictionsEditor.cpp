// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "PredictionsEditor.h"
// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace windows {

PredictionsEditor::PredictionsEditor(const prediction_container& predictions, const char *name)
    : Window(name, true), m_predictions(predictions){}

void PredictionsEditor::buildUI()
{
  if (m_predictionsChanged) {
    triggerUpdateCameraCallback();
    m_predictionsChanged = false;
  }

//  //combo box
//  std::vector<const char *> names(m_tfnsNames.size(), nullptr);
//  std::transform(m_tfnsNames.begin(),
//      m_tfnsNames.end(),
//      names.begin(),
//      [](const std::string &t) { return t.c_str(); });
//
//  int newMap = m_currentMap;
//  if (ImGui::Combo("color map", &newMap, names.data(), names.size()))
//    setMap(newMap);
//
//  ImGui::Separator();

//  drawEditor();
//
//  ImGui::Separator();


  if (ImGui::Button("reset##view")) {
    //TODO
    m_predictionsChanged = true;
  }

  ImGui::Separator();

  for (size_t i=0; i<m_predictions.predictions.size(); ++i)
  {
    const auto buttonName = "Load " + std::to_string(i);
    if (ImGui::Button(buttonName.c_str())) {
      m_eye = m_predictions.predictions[i].eye;
      m_center = m_predictions.predictions[i].center;
      m_up = m_predictions.predictions[i].up;
      m_predictionsChanged = true;
    }
  }

  ImGui::Separator();
}

void PredictionsEditor::setUpdateCameraCallback(PredictionsUpdateCameraCallback cb)
{
  m_updateCameraCallback = cb;
}

void PredictionsEditor::triggerUpdateCameraCallback()
{
  if (m_updateCameraCallback)
    m_updateCameraCallback(m_eye, m_center, m_up);
}

} // namespace windows
