// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#include "PredictionsEditor.h"
// std
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace anari_viewer::windows {

PredictionsEditor::PredictionsEditor(
    const prediction_container& predictions,
    std::vector<std::string> matcherNames,
    const char *name)
    : Window(name, true), m_predictions(&predictions), m_matcherIndex(0), m_matcherNamesStr(matcherNames)
{
  m_matcherNames = std::vector<const char*>(m_matcherNamesStr.size(), nullptr);
  std::transform(m_matcherNamesStr.begin(),
      m_matcherNamesStr.end(),
      m_matcherNames.begin(),
      [](const std::string &s) { return s.c_str(); });
}

void PredictionsEditor::buildUI()
{
  if (ImGui::Button("reset view"))
    triggerResetCameraCallback();

  if (ImGui::Button("export screenshot"))
    triggerExportScreenshotCallback();

  auto numPredictions = m_predictions->predictions.size();
  if (numPredictions > 0)
  {
    if (ImGui::Button("export predictions"))
      triggerExportPredictionsCallback();

    ImGui::Separator();

    ImGui::Text("Select image:");
    if (ImGui::BeginTable("Images", numPredictions))
    {
      ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
      for (size_t i=0; i<numPredictions; ++i)
      {
        ImGui::TableNextColumn();
        std::string itemid = std::to_string(i+1);
        if (ImGui::Selectable(itemid.c_str(), i == m_selectedImage))
        {
          m_selectedImage = i;
          triggerShowImageCallback(m_selectedImage);
          triggerLoadReferenceImageCallback(m_selectedImage);
        }
      }
      ImGui::PopStyleVar();
      ImGui::EndTable();
    }

    if (m_selectedImage < numPredictions)
    {
      if (ImGui::Button("Load camera from json"))
      {
        triggerUpdateCameraCallback(
            m_predictions->predictions[m_selectedImage].initial_camera.eye,
            m_predictions->predictions[m_selectedImage].initial_camera.center,
            m_predictions->predictions[m_selectedImage].initial_camera.up);
      }

      if (ImGui::Button("Save current camera"))
        triggerSaveCameraCallback(m_selectedImage);

      if (m_predictions->predictions[m_selectedImage].refined_camera.initialized)
      {
        if (ImGui::Button("Load refined camera"))
        {
          triggerUpdateCameraCallback(
              m_predictions->predictions[m_selectedImage].refined_camera.eye,
              m_predictions->predictions[m_selectedImage].refined_camera.center,
              m_predictions->predictions[m_selectedImage].refined_camera.up);
        }
      }
    }

    ImGui::Separator();
  }

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

void PredictionsEditor::setExportScreenshotCallback(ExportScreenshotCallback cb)
{
  m_exportScreenshotCallback = cb;
}

void PredictionsEditor::setSaveCameraCallback(SaveCameraCallback cb)
{
  m_saveCameraCallback = cb;
}

void PredictionsEditor::setExportPredictionsCallback(ExportPredictionsCallback cb)
{
  m_exportPredictionsCallback = cb;
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

void PredictionsEditor::triggerExportScreenshotCallback()
{
  if (m_exportScreenshotCallback)
    m_exportScreenshotCallback();
}

void PredictionsEditor::triggerSaveCameraCallback(size_t index)
{
  if (m_saveCameraCallback)
    m_saveCameraCallback(index);
}

void PredictionsEditor::triggerExportPredictionsCallback()
{
  if (m_exportPredictionsCallback)
    m_exportPredictionsCallback();
}

} // namespace anari_viewer::windows
