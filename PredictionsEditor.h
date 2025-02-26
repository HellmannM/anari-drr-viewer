// Copyright 2022 Matthias Hellmann
// SPDX-License-Identifier: Apache-2.0

#pragma once

// anari
#include "anari_viewer/windows/Window.h"
// std
#include <functional>
#include <string>
#include <vector>
// anari
#include "anari/anari_cpp/ext/linalg.h" // math::float3

#include "prediction.h"

namespace anari_viewer::windows {

using UpdateCameraCallback =
    std::function<void(const anari::math::float3&,
                       const anari::math::float3&,
                       const anari::math::float3&)>;
using ResetCameraCallback = std::function<void(void)>;
using ShowImageCallback = std::function<void(size_t)>;
using SetActiveEstimatorIndexCallback = std::function<void(size_t)>;
using LoadReferenceImageCallback = std::function<void(size_t)>;
using LoadFramebufferAsReferenceImageCallback = std::function<void(void)>;
using MatchCallback = std::function<void(void)>;
using ExportScreenshotCallback = std::function<void(void)>;
using SaveCameraCallback = std::function<void(size_t)>;
using ExportPredictionsCallback = std::function<void(void)>;
using SetMatchThresholdCallback = std::function<void(float)>;

class PredictionsEditor : public anari_viewer::windows::Window
{
 public:
  PredictionsEditor(
        const prediction_container& predictions,
        std::vector<std::string> estimatorNames,
        const char *name = "Predictions Editor");
  ~PredictionsEditor() = default;

  void buildUI() override;

  void setUpdateCameraCallback(UpdateCameraCallback cb);
  void setResetCameraCallback(ResetCameraCallback cb);
  void setShowImageCallback(ShowImageCallback cb);
  void setSetActiveEstimatorIndexCallback(SetActiveEstimatorIndexCallback cb);
  void setLoadReferenceImageCallback(LoadReferenceImageCallback cb);
  void setLoadFramebufferAsReferenceImageCallback(LoadFramebufferAsReferenceImageCallback cb);
  void setMatchCallback(MatchCallback cb);
  void setExportScreenshotCallback(ExportScreenshotCallback cb);
  void setSaveCameraCallback(SaveCameraCallback cb);
  void setExportPredictionsCallback(ExportPredictionsCallback cb);
  void setSetMatchThresholdCallback(SetMatchThresholdCallback cb);
  void triggerUpdateCameraCallback(
      const anari::math::float3& eye,
      const anari::math::float3& center,
      const anari::math::float3& up);
  void triggerResetCameraCallback();
  void triggerShowImageCallback(size_t index);
  void triggerSetActiveEstimatorIndexCallback(size_t index);
  void triggerLoadReferenceImageCallback(size_t index);
  void triggerLoadFramebufferAsReferenceImageCallback();
  void triggerMatchCallback();
  void triggerExportScreenshotCallback();
  void triggerSaveCameraCallback(size_t index);
  void triggerExportPredictionsCallback();
  void triggerSetMatchThresholdCallback(float threshold);

 private:
  // callback called whenever new camera selected
  UpdateCameraCallback m_updateCameraCallback;
  // callback called whenever reset camera selected
  ResetCameraCallback m_resetCameraCallback;
  // callback called whenever an image is selected
  ShowImageCallback m_showImageCallback;
  // callback called whenever a different estimator is selected
  SetActiveEstimatorIndexCallback m_setActiveEstimatorIndexCallback;
  LoadReferenceImageCallback m_loadReferenceImageCallback;
  LoadFramebufferAsReferenceImageCallback m_loadFramebufferAsReferenceImageCallback;
  MatchCallback m_matchCallback;
  ExportScreenshotCallback m_exportScreenshotCallback;
  SaveCameraCallback m_saveCameraCallback;
  ExportPredictionsCallback m_exportPredictionsCallback;
  SetMatchThresholdCallback m_setMatchThresholdCallback;

  const prediction_container* m_predictions;
  size_t m_estimatorIndex;
  std::vector<std::string> m_estimatorNamesStr;
  std::vector<const char*> m_estimatorNames;
  size_t m_selectedImage{static_cast<size_t>(-1)};
  std::string m_filename;
  float m_matchThreshold{50.f};
};

} // namespace anari_viewer::windows
