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
    RGBA8 = 0,
    RGB8 = 1,
    R8 = 2,
    FLOAT3 = 3
  };

  enum IMAGE_TYPE {
      REFERENCE = 0,
      QUERY = 1,
      DEPTH3D = 2
  };

  feature_matcher();
  virtual void calibrate(size_t width, size_t height, float fovy, float aspect);
  virtual void match();
  virtual void set_image(const void* data,
                         size_t width,
                         size_t height,
                         PIXEL_TYPE pixel_type,
                         IMAGE_TYPE image_type,
                         bool swizzle);
  virtual bool update_camera(std::array<float, 3>& eye,
                             std::array<float, 3>& center,
                             std::array<float, 3>& up);
  virtual void set_good_match_threshold(float threshold);
};

struct MatchersWrapper
{
  MatchersWrapper() = default;
  ~MatchersWrapper() noexcept;

  void init(std::vector<std::string> &matcherLibraryNames);
  void destroy();
  void setActiveMatcherIndex(size_t index);
  feature_matcher* getActiveMatcher();

  std::vector<void*> m_matcherLibraryHandles;
  std::vector<feature_matcher*> m_matchers;
  std::vector<std::string> m_matcherNames;
  std::vector<std::string> m_matcherDescriptions;
  size_t m_activeMatcherIndex{0};
  feature_matcher* m_activeMatcher{nullptr};
};
