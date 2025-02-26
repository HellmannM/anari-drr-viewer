#include "ImageTransformEstimatorWrapper.h"

ImageTransformEstimatorWrapper::~ImageTransformEstimatorWrapper() noexcept
{
  try {
    destroy();
  } catch(...) {}
}

void ImageTransformEstimatorWrapper::init(std::vector<std::string> &estimatorLibraryNames)
{
  for (auto &lib : estimatorLibraryNames) {
    void *handle = dlopen(lib.c_str(), RTLD_NOW);
    if (!handle) {
      fprintf(stderr, "Error: %s\n", dlerror());
      fprintf(stderr, "Could not load %s\n", lib.c_str());
      continue;
    }

    // Load symbols
    image_transform_estimator* (*create_estimator)() = (image_transform_estimator* (*)()) dlsym(handle, "create_estimator");
    const char* (*get_name)() = (const char* (*)())dlsym(handle, "get_estimator_type");
    const char* (*get_desc)() = (const char* (*)())dlsym(handle, "get_estimator_description");

    if (!create_estimator || !get_name || !get_desc) {
      fprintf(stderr, "Error: %s\n", dlerror());
      fprintf(stderr, "Could not load symbols from file: %s\n", lib.c_str());
      dlclose(handle);
      continue;
    }

    image_transform_estimator* estimator = create_estimator();
    if (!estimator) {
      fprintf(stderr, "Could not create instance of estimator\n");
      continue;
    }
    m_estimators.push_back(estimator);
    m_estimatorLibraryHandles.push_back(handle);
    m_estimatorNames.emplace_back(get_name());
    m_estimatorDescriptions.emplace_back(get_desc());

    fprintf(stdout, "Loaded estimator %s: %s\n", m_estimatorNames.back().c_str(), m_estimatorDescriptions.back().c_str());
  }

  if (!m_estimators.empty())
    setActiveEstimatorIndex(0);
}

void ImageTransformEstimatorWrapper::destroy()
{
  for (size_t i = 0; i < m_estimatorLibraryHandles.size(); ++i) {
    auto handle = m_estimatorLibraryHandles[i];
    void (*destroy_estimator)(image_transform_estimator*) =
        (void (*)(image_transform_estimator*))dlsym(handle, "destroy_estimator");

    if (destroy_estimator) {
      destroy_estimator(m_estimators[i]);
    }
    dlclose(handle);
  }
  m_estimators.clear();
  m_estimatorLibraryHandles.clear();
  m_estimatorNames.clear();
  m_estimatorDescriptions.clear();
}

void ImageTransformEstimatorWrapper::setActiveEstimatorIndex(size_t index)
{
  if (index >= m_estimators.size())
    return;
  m_activeEstimatorIndex = index;
  m_activeEstimator = m_estimators[index];
}

image_transform_estimator* ImageTransformEstimatorWrapper::getActiveEstimator()
{
  return m_activeEstimator;
}