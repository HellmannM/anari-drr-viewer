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
}

void PredictionsEditor::setUpdateCameraCallback(PredictionsUpdateCameraCallback cb)
{
  m_updateCameraCallback = cb;
  triggerUpdateCameraCallback();
}

void PredictionsEditor::triggerUpdateCameraCallback()
{
//  if (m_updateCameraCallback)
//    m_updateCameraCallback(/*TODO*/);
}

} // namespace windows
