#pragma once

#include <array>
#include <dlfcn.h>
#include <cstdint>
#include <string>
#include <vector>

struct feature_matcher
{
  enum PIXEL_TYPE
  {
    PIXEL_TYPE_UNKNOWN = 0,
    RGBA = 1
  };

  enum DEPTH_TYPE
  {
    DEPTH_TYPE_UNKNOWN = 0,
    FLOAT = 1,
    FLOAT3 = 2
  };

  virtual void set_reference_image(const std::vector<uint8_t> &data, size_t width, size_t height, PIXEL_TYPE pixel_type);
  virtual void match(const std::vector<uint8_t> &data, size_t width, size_t height, PIXEL_TYPE pixel_type);
  virtual void calibrate(size_t width, size_t height, float fovy, float aspect);
  virtual bool update_camera(const std::vector<float>& depth,
                             const std::vector<float>& depth3D,
                             size_t width,
                             size_t height,
                             DEPTH_TYPE depth_type,
                             DEPTH_TYPE depth3D_type,
                             std::array<float, 3>& eye,
                             std::array<float, 3>& center,
                             std::array<float, 3>& up);
};

struct MatchersWrapper
{
  MatchersWrapper() = default;
  ~MatchersWrapper() noexcept;

  void init(std::vector<std::string> &matcherLibraryNames);
  void destroy();
  void setActiveMatcherIndex(size_t index);

  std::vector<void*> m_matcherLibraryHandles;
  std::vector<feature_matcher*> m_matchers;
  std::vector<std::string> m_matcherNames;
  std::vector<std::string> m_matcherDescriptions;
  size_t m_activeMatcherIndex{0};
  feature_matcher* m_activeMatcher{nullptr};
};
