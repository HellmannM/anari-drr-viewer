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
  if (ImGui::Button("reset view")) {
    triggerResetCameraCallback(/*azel*/ true);
  }

  ImGui::Separator();

  for (size_t i=0; i<m_predictions.predictions.size(); ++i)
  {
    const auto buttonName = "Load " + std::to_string(i);
    if (ImGui::Button(buttonName.c_str())) {
      triggerUpdateCameraCallback(
          m_predictions.predictions[i].eye,
          m_predictions.predictions[i].center,
          m_predictions.predictions[i].up);
    }
  }

  ImGui::Separator();
}

void PredictionsEditor::setUpdateCameraCallback(UpdateCameraCallback cb)
{
  m_updateCameraCallback = cb;
}

void PredictionsEditor::setResetCameraCallback(ResetCameraCallback cb)
{
  m_resetCameraCallback = cb;
}

void PredictionsEditor::triggerResetCameraCallback(bool resetAzel)
{
  if (m_resetCameraCallback)
    m_resetCameraCallback(resetAzel);
}

void PredictionsEditor::triggerUpdateCameraCallback(
    const anari::math::float3& eye,
    const anari::math::float3& center,
    const anari::math::float3& up)
{
  if (m_updateCameraCallback)
    m_updateCameraCallback(eye, center, up);
}

} // namespace windows
