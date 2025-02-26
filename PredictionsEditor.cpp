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
    std::vector<std::string> estimatorNames,
    const char *name)
    : Window(name, true), m_predictions(&predictions), m_estimatorIndex(0), m_estimatorNamesStr(estimatorNames)
{
  m_estimatorNames = std::vector<const char*>(m_estimatorNamesStr.size(), nullptr);
  std::transform(m_estimatorNamesStr.begin(),
      m_estimatorNamesStr.end(),
      m_estimatorNames.begin(),
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
          m_filename = m_predictions->predictions[m_selectedImage].filename;
          triggerShowImageCallback(m_selectedImage);
          triggerLoadReferenceImageCallback(m_selectedImage);
        }
      }
      ImGui::PopStyleVar();
      ImGui::EndTable();
    }
    if (!m_filename.empty())
      ImGui::Text("%s", m_filename.c_str());

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

  // Combo box for estimator type
  if (!m_estimatorNames.empty())
  {
    int estimatorIndex = static_cast<int>(m_estimatorIndex);
    if (ImGui::Combo("Estimator Type", &estimatorIndex, m_estimatorNames.data(), m_estimatorNames.size()))
    {
      if (estimatorIndex != m_estimatorIndex)
      {
        m_estimatorIndex = estimatorIndex;
        triggerSetActiveEstimatorIndexCallback(static_cast<size_t>(estimatorIndex));
      }
    }
  }

  ImGui::Separator();

  if (ImGui::Button("Set fb as ref")) {
    m_filename.clear();
    triggerLoadFramebufferAsReferenceImageCallback();
  }

  if (ImGui::Button("Match")) {
    triggerMatchCallback();
  }

  if (ImGui::SliderFloat("Match Threshold", &m_matchThreshold, 0.f, 100.f)) {
    triggerSetMatchThresholdCallback(m_matchThreshold);
  }

  ImGui::Separator();
  ImGui::Text("Select pixel: Shift + LMB");
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

void PredictionsEditor::setSetActiveEstimatorIndexCallback(SetActiveEstimatorIndexCallback cb)
{
  m_setActiveEstimatorIndexCallback = cb;
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

void PredictionsEditor::setSetMatchThresholdCallback(SetMatchThresholdCallback cb)
{
  m_setMatchThresholdCallback = cb;
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

void PredictionsEditor::triggerSetActiveEstimatorIndexCallback(size_t index)
{
  if (m_setActiveEstimatorIndexCallback)
    m_setActiveEstimatorIndexCallback(index);
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

void PredictionsEditor::triggerSetMatchThresholdCallback(float threshold)
{
  if (m_setMatchThresholdCallback)
    m_setMatchThresholdCallback(threshold);
}

} // namespace anari_viewer::windows
