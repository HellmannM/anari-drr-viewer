set(glm_DIR ${CMAKE_CURRENT_LIST_DIR}/external/glm/lib/cmake/glm)
find_package(glm REQUIRED)
mark_as_advanced(glm_DIR)

find_package(anari 0.11.1 REQUIRED COMPONENTS viewer)
find_package(visionaray 0.4.2 REQUIRED)

set(SUBPROJECT_NAME anariDRRViewer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

add_executable(${SUBPROJECT_NAME}
    ImageViewport.cpp
    LacTransform.cpp
    Matcher.cpp
    PredictionsEditor.cpp
    SettingsEditor.cpp
    Viewport.cpp
    viewer.cpp
)
target_link_libraries(${SUBPROJECT_NAME} glm::glm anari::anari_viewer)
target_link_libraries(${SUBPROJECT_NAME} visionaray::visionaray_common)

# ITK (nifti loader)
option(USE_ITK "Support loading Nifti files from ITK" ON)
if (USE_ITK)
  find_package(ITK CONFIG REQUIRED)
  include(${ITK_USE_FILE})
  include_directories(${ITK_INCLUDE_DIRS})
  target_sources(${SUBPROJECT_NAME} PRIVATE readNifti.cpp)
  target_compile_definitions(${SUBPROJECT_NAME} PRIVATE -DHAVE_ITK)
  target_link_libraries(${SUBPROJECT_NAME} ${ITK_LIBRARIES})
endif()

# nlohmann JSON (JSON loader)
include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

# copy LacLuts.json file to bin dir
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LacLuts.json" "${CMAKE_BINARY_DIR}/LacLuts.json" COPYONLY)

