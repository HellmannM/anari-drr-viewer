// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "PredictionsEditor.h"
// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace windows {

PredictionsEditor::PredictionsEditor(
    const prediction_container& predictions,
    std::vector<std::string> matcherNames,
    const char *name)
    : Window(name, true), m_predictions(predictions), m_matcherIndex(0)
{
  m_matcherNames = std::vector<const char*>(matcherNames.size(), nullptr);
  std::transform(matcherNames.begin(),
      matcherNames.end(),
      m_matcherNames.begin(),
      [](const std::string &s) { return s.c_str(); });
}

void PredictionsEditor::buildUI()
{
  if (ImGui::Button("reset view")) {
    triggerResetCameraCallback();
  }

  ImGui::Separator();

  for (size_t i=0; i<m_predictions.predictions.size(); ++i)
  {
    // Button to load the camera
    const auto loadCamera = "Load camera " + std::to_string(i);
    if (ImGui::Button(loadCamera.c_str())) {
      triggerUpdateCameraCallback(
          m_predictions.predictions[i].eye,
          m_predictions.predictions[i].center,
          m_predictions.predictions[i].up);
    }

    // Button to show the image in the ImageViewport
    const auto loadImage = "Load image " + std::to_string(i);
    if (ImGui::Button(loadImage.c_str())) {
      triggerShowImageCallback(i);
      triggerLoadReferenceImageCallback(i);
    }
  }

  ImGui::Separator();

  // Combo box for matcher type
  if (!m_matcherNames.empty())
  {
    int matcherIndex = static_cast<int>(m_matcherIndex);
    if (ImGui::Combo("Matcher Type", &matcherIndex, m_matcherNames.data(), m_matcherNames.size()))
    {
      if (matcherIndex != m_matcherIndex)
      {
        m_matcherIndex = matcherIndex;
        triggerSetActiveMatcherIndexCallback(static_cast<size_t>(matcherIndex));
      }
    }
  }

  ImGui::Separator();

  if (ImGui::Button("Set fb as ref")) {
    triggerLoadFramebufferAsReferenceImageCallback();
  }

  if (ImGui::Button("Match")) {
    triggerMatchCallback();
  }
}

void PredictionsEditor::setUpdateCameraCallback(UpdateCameraCallback cb)
{
  m_updateCameraCallback = cb;
}

void PredictionsEditor::setResetCameraCallback(ResetCameraCallback cb)
{
  m_resetCameraCallback = cb;
}

void PredictionsEditor::setShowImageCallback(ShowImageCallback cb)
{
  m_showImageCallback = cb;
}

void PredictionsEditor::setSetActiveMatcherIndexCallback(SetActiveMatcherIndexCallback cb)
{
  m_setActiveMatcherIndexCallback = cb;
}

void PredictionsEditor::setLoadReferenceImageCallback(LoadReferenceImageCallback cb)
{
  m_loadReferenceImageCallback = cb;
}

void PredictionsEditor::setLoadFramebufferAsReferenceImageCallback(LoadFramebufferAsReferenceImageCallback cb)
{
  m_loadFramebufferAsReferenceImageCallback = cb;
}

void PredictionsEditor::setMatchCallback(MatchCallback cb)
{
  m_matchCallback = cb;
}

void PredictionsEditor::triggerResetCameraCallback()
{
  if (m_resetCameraCallback)
    m_resetCameraCallback();
}

void PredictionsEditor::triggerUpdateCameraCallback(
    const anari::math::float3& eye,
    const anari::math::float3& center,
    const anari::math::float3& up)
{
  if (m_updateCameraCallback)
    m_updateCameraCallback(eye, center, up);
}

void PredictionsEditor::triggerShowImageCallback(size_t index)
{
  if (m_showImageCallback)
    m_showImageCallback(index);
}

void PredictionsEditor::triggerSetActiveMatcherIndexCallback(size_t index)
{
  if (m_setActiveMatcherIndexCallback)
    m_setActiveMatcherIndexCallback(index);
}

void PredictionsEditor::triggerLoadReferenceImageCallback(size_t index)
{
  if (m_loadReferenceImageCallback)
    m_loadReferenceImageCallback(index);
}

void PredictionsEditor::triggerLoadFramebufferAsReferenceImageCallback()
{
  if (m_loadFramebufferAsReferenceImageCallback)
    m_loadFramebufferAsReferenceImageCallback();
}

void PredictionsEditor::triggerMatchCallback()
{
  if (m_matchCallback)
    m_matchCallback();
}

} // namespace windows
