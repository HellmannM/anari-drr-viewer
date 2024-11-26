set(glm_DIR ${CMAKE_CURRENT_LIST_DIR}/external/glm/lib/cmake/glm)
find_package(glm REQUIRED)
mark_as_advanced(glm_DIR)

find_package(anari 0.8.0 REQUIRED COMPONENTS viewer)

set(SUBPROJECT_NAME anariDRRViewer)

add_executable(${SUBPROJECT_NAME}
    PredictionsEditor.cpp SettingsEditor.cpp Viewport.cpp viewer.cpp)
target_link_libraries(${SUBPROJECT_NAME} glm::glm anari::anari_viewer)

option(USE_ITK "Support loading Nifti files from ITK" ON)
if (USE_ITK)
  find_package(ITK CONFIG REQUIRED)
  include(${ITK_USE_FILE})
  include_directories(${ITK_INCLUDE_DIRS})
  target_sources(${SUBPROJECT_NAME} PRIVATE readNifti.cpp)
  target_compile_definitions(${SUBPROJECT_NAME} PRIVATE -DHAVE_ITK)
  target_link_libraries(${SUBPROJECT_NAME} ${ITK_LIBRARIES})
endif()

option(USE_JSON "Support loading JSON files" ON)
if (USE_JSON)
  if (EXISTS "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann/json.hpp")
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    #target_sources(${SUBPROJECT_NAME} PRIVATE prediction.cpp)
    target_compile_definitions(${SUBPROJECT_NAME} PRIVATE -DHAVE_JSON)
  else()
    message(WARNING "Could not find nlohmann json. Generating without JSON support")
  endif()
endif()

