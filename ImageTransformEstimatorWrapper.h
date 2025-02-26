#pragma once

#include <array>
#include <dlfcn.h>
#include <cstdint>
#include <string>
#include <vector>

#include <image_transform_estimator.h>

struct ImageTransformEstimatorWrapper
{
  ImageTransformEstimatorWrapper() = default;
  ~ImageTransformEstimatorWrapper() noexcept;

  void init(std::vector<std::string> &estimatorLibraryNames);
  void destroy();
  void setActiveEstimatorIndex(size_t index);
  image_transform_estimator* getActiveEstimator();

  std::vector<void*> m_estimatorLibraryHandles;
  std::vector<image_transform_estimator*> m_estimators;
  std::vector<std::string> m_estimatorNames;
  std::vector<std::string> m_estimatorDescriptions;
  size_t m_activeEstimatorIndex{0};
  image_transform_estimator* m_activeEstimator{nullptr};
};
