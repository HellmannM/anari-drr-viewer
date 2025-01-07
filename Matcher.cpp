#include "Matcher.h"

MatchersWrapper::~MatchersWrapper() noexcept
{
  try {
    destroy();
  } catch(...) {}
}

void MatchersWrapper::init(std::vector<std::string> &matcherLibraryNames)
{
  for (auto &lib : matcherLibraryNames) {
    void *handle = dlopen(lib.c_str(), RTLD_NOW);
    if (!handle) {
      fprintf(stderr, "Error: %s\n", dlerror());
      fprintf(stderr, "Could not load %s\n", lib.c_str());
      continue;
    }

    // Load symbols
    feature_matcher* (*create_matcher)() = (feature_matcher* (*)()) dlsym(handle, "create_matcher");
    const char* (*get_name)() = (const char* (*)())dlsym(handle, "get_matcher_type");
    const char* (*get_desc)() = (const char* (*)())dlsym(handle, "get_matcher_description");

    if (!create_matcher || !get_name || !get_desc) {
      fprintf(stderr, "Error: %s\n", dlerror());
      fprintf(stderr, "Could not load symbols from file: %s\n", lib.c_str());
      dlclose(handle);
      continue;
    }

    feature_matcher* matcher = create_matcher();
    if (!matcher) {
      fprintf(stderr, "Could not create instance of matcher\n");
      continue;
    }
    m_matchers.push_back(matcher);
    m_matcherLibraryHandles.push_back(handle);
    m_matcherNames.emplace_back(get_name());
    m_matcherDescriptions.emplace_back(get_desc());

    fprintf(stdout, "Loaded Matcher %s: %s\n", m_matcherNames.back().c_str(), m_matcherDescriptions.back().c_str());
  }

  if (!m_matchers.empty())
    setActiveMatcherIndex(0);
}

void MatchersWrapper::destroy()
{
  for (size_t i = 0; i < m_matcherLibraryHandles.size(); ++i) {
    auto handle = m_matcherLibraryHandles[i];
    void (*destroy_matcher)(feature_matcher*) =
        (void (*)(feature_matcher*))dlsym(handle, "destroy_matcher");

    if (destroy_matcher) {
      destroy_matcher(m_matchers[i]);
    }
    dlclose(handle);
  }
  m_matchers.clear();
  m_matcherLibraryHandles.clear();
  m_matcherNames.clear();
  m_matcherDescriptions.clear();
}

void MatchersWrapper::setActiveMatcherIndex(size_t index)
{
  if (index >= m_matchers.size())
    return;
  m_activeMatcherIndex = index;
  m_activeMatcher = m_matchers[index];
}

feature_matcher* MatchersWrapper::getActiveMatcher()
{
  return m_activeMatcher;
}