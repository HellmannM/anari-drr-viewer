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
using ResetCameraCallback = std::function<void(bool)>;

class PredictionsEditor : public anari_viewer::Window
{
 public:
  PredictionsEditor(const prediction_container& predictions, const char *name = "Predictions Editor");
  ~PredictionsEditor() = default;

  void buildUI() override;

  void setUpdateCameraCallback(UpdateCameraCallback cb);
  void setResetCameraCallback(ResetCameraCallback cb);
  void triggerUpdateCameraCallback(
      const anari::math::float3& eye,
      const anari::math::float3& center,
      const anari::math::float3& up);
  void triggerResetCameraCallback(bool resetAzel);

 private:
  // callback called whenever new camera selected
  UpdateCameraCallback m_updateCameraCallback;
  // callback called whenever reset camera selected
  ResetCameraCallback m_resetCameraCallback;

  prediction_container m_predictions;
};

} // namespace windows
