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

namespace windows {

using UpdateCameraCallback =
    std::function<void(const anari::math::float3&,
                       const anari::math::float3&,
                       const anari::math::float3&)>;
using ResetCameraCallback = std::function<void(void)>;
using ShowImageCallback = std::function<void(size_t)>;
using SetActiveMatcherIndexCallback = std::function<void(size_t)>;

class PredictionsEditor : public anari_viewer::Window
{
 public:
  PredictionsEditor(
        const prediction_container& predictions,
        std::vector<std::string> matcherNames,
        const char *name = "Predictions Editor");
  ~PredictionsEditor() = default;

  void buildUI() override;

  void setUpdateCameraCallback(UpdateCameraCallback cb);
  void setResetCameraCallback(ResetCameraCallback cb);
  void setShowImageCallback(ShowImageCallback cb);
  void setSetActiveMatcherIndexCallback(SetActiveMatcherIndexCallback cb);
  void triggerUpdateCameraCallback(
      const anari::math::float3& eye,
      const anari::math::float3& center,
      const anari::math::float3& up);
  void triggerResetCameraCallback();
  void triggerShowImageCallback(size_t index);
  void triggerSetActiveMatcherIndexCallback(size_t index);

 private:
  // callback called whenever new camera selected
  UpdateCameraCallback m_updateCameraCallback;
  // callback called whenever reset camera selected
  ResetCameraCallback m_resetCameraCallback;
  // callback called whenever an image is selected
  ShowImageCallback m_showImageCallback;
  // callback called whenever a different matcher is selected
  SetActiveMatcherIndexCallback m_setActiveMatcherIndexCallback;

  prediction_container m_predictions;
  size_t m_matcherIndex;
  std::vector<const char*> m_matcherNames;
};

} // namespace windows
