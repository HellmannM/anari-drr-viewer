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
    RGBA = 1,
    FLOAT3 = 2
  };

  enum IMAGE_TYPE {
      REFERENCE = 0,
      QUERY = 1,
      DEPTH3D = 2
  };

  virtual void set_image(const void* data,
                         size_t width,
                         size_t height,
                         PIXEL_TYPE pixel_type,
                         IMAGE_TYPE image_type,
                         bool swizzle);
  virtual void calibrate(size_t width, size_t height, float fovy, float aspect);
  virtual bool update_camera(std::array<float, 3>& eye,
                             std::array<float, 3>& center,
                             std::array<float, 3>& up);
  virtual void match();
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
