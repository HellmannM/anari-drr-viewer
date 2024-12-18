#pragma once

#include <dlfcn.h>
#include <cstdint>
#include <string>
#include <vector>

struct feature_matcher
{
  enum PIXEL_TYPE
  {
    UNKNOWN = 0,
    RGBA = 1
  };

  virtual void set_reference_image(const std::vector<uint8_t> &data, int width, int height, PIXEL_TYPE pixel_type);
  virtual void match(const std::vector<uint8_t> &data, int width, int height, PIXEL_TYPE pixel_type);
};

struct MatchersWrapper
{
  MatchersWrapper() = default;
  ~MatchersWrapper() noexcept;

  void init(std::vector<std::string> &matcherLibraryNames);
  void destroy();

  std::vector<void*> m_matcherLibraryHandles;
  std::vector<feature_matcher*> m_matchers;
  std::vector<std::string> m_matcherNames;
  std::vector<std::string> m_matcherDescriptions;
};
