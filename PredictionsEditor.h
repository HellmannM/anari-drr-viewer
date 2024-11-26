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

using PredictionsUpdateCameraCallback =
    std::function<void(const anari::math::float3&, const anari::math::float3&, const anari::math::float3&)>;

class PredictionsEditor : public anari_viewer::Window
{
 public:
  PredictionsEditor(const prediction_container& predictions, const char *name = "Predictions Editor");
  ~PredictionsEditor() = default;

  void buildUI() override;

  void setUpdateCameraCallback(PredictionsUpdateCameraCallback cb);
  void triggerUpdateCameraCallback();

 private:
  // callback called whenever new camera selected
  PredictionsUpdateCameraCallback m_updateCameraCallback;

  // flag indicating transfer function has changed in UI
  bool m_predictionsChanged{false};

  prediction_container m_predictions;
  anari::math::float3 m_eye;
  anari::math::float3 m_center;
  anari::math::float3 m_up;
};

} // namespace windows
