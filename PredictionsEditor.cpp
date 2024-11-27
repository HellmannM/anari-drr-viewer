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
    }
  }

  ImGui::Separator();

//  if (ImGui::Button("manual reset +x")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{575.175049f, -622.799988f, 95.999924f},
//        anari::math::float3{-0.342020f, 0.939693f, 0.f},
//        anari::math::float3{-0.939693f, -0.342020f, 0.f});
//  }
//  if (ImGui::Button("manual reset +y")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{255.499924f, 575.175049f, -782.299988f},
//        anari::math::float3{0.f, -0.342020f, 0.939693f},
//        anari::math::float3{0.f, -0.939693f, -0.342020f});
//  }
//  if (ImGui::Button("manual reset +z")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{255.499924f, -622.799988f, 415.675079f},
//        anari::math::float3{0.f, 0.939693f, -0.342020f},
//        anari::math::float3{0.f, -0.342020f, -0.939693f});
//  }
//  if (ImGui::Button("manual reset -x")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{-64.175079f, 1133.800049f, 94.000076f},
//        anari::math::float3{0.342020f, -0.939693f, 0.f},
//        anari::math::float3{0.939693f, 0.342020f, 0.f});
//  }
//  if (ImGui::Button("manual reset -y")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{255.500076f, -64.175079f, 974.299988f},
//        anari::math::float3{0.f, 0.342020f, -0.939693f},
//        anari::math::float3{0.f, 0.939693f, 0.342020f});
//  }
//  if (ImGui::Button("manual reset -z")) {
//    triggerUpdateCameraCallback(
//        anari::math::float3{255.500076f, 1133.800049f, -223.675079f},
//        anari::math::float3{0.f, -0.939693f, 0.342020f},
//        anari::math::float3{0.f, 0.342020f, 0.939693f});
//  }
}

void PredictionsEditor::setUpdateCameraCallback(UpdateCameraCallback cb)
{
  m_updateCameraCallback = cb;
}

void PredictionsEditor::setResetCameraCallback(ResetCameraCallback cb)
{
  m_resetCameraCallback = cb;
}

void PredictionsEditor::setShowImageCallback(ResetCameraCallback cb)
{
  m_showImageCallback = cb;
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

void PredictionsEditor::triggerShowImageCallback(size_t index)
{
  if (m_showImageCallback)
    m_showImageCallback(index);
}

} // namespace windows
