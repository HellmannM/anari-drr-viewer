set(glm_DIR ${CMAKE_CURRENT_LIST_DIR}/external/glm/lib/cmake/glm)
find_package(glm REQUIRED)
mark_as_advanced(glm_DIR)

find_package(anari 0.8.0 REQUIRED COMPONENTS viewer)

set(SUBPROJECT_NAME anariDRRViewer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

add_executable(${SUBPROJECT_NAME}
    ImageViewport.cpp
    LacTransform.cpp
    PredictionsEditor.cpp
    SettingsEditor.cpp
    Viewport.cpp
    viewer.cpp
)
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

if (IS_DIRECTORY "${NLOHMANN_JSON_INCLUDE_DIR}/nlohmann")
    option(USE_JSON "Support loading JSON files" ON)
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
    #target_sources(${SUBPROJECT_NAME} PRIVATE prediction.cpp)
    target_compile_definitions(${SUBPROJECT_NAME} PRIVATE -DHAVE_JSON)
else()
    option(USE_JSON "Support loading JSON files" OFF)
    message(WARNING "Could not find nlohmann json. Generating without JSON support")
endif()

if (IS_DIRECTORY "${VISIONARAY_INCLUDE_DIR}/visionaray")
    option(USE_VISIONARAY "Support loading PNG files" ON)
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build all libraries as shared libraries instead of static" FORCE)
    set(VSNRAY_ENABLE_CUDA TRUE CACHE BOOL "CUDA for Visionaray" FORCE)
    set(VSNRAY_ENABLE_3DCONNEXIONCLIENT FALSE CACHE BOOL "Use 3DconnexionClient, if available" FORCE)
    set(VSNRAY_ENABLE_COCOA FALSE CACHE BOOL "Use Cocoa, if available" FORCE)
    set(VSNRAY_ENABLE_COMMON TRUE CACHE BOOL "Build the common library with several utils" FORCE)
    set(VSNRAY_ENABLE_COMPILE_FAILURE_TESTS FALSE CACHE BOOL "Build compile failure tests" FORCE)
    set(VSNRAY_ENABLE_CUDA_STYLE_THREAD_INTROSPECTION FALSE CACHE BOOL "Define CUDA-style thread introspection variables in CPU scheduler headers" FORCE)
    set(VSNRAY_ENABLE_EXAMPLES FALSE CACHE BOOL "Build the programming examples" FORCE)
    set(VSNRAY_ENABLE_PBRT_PARSER FALSE CACHE BOOL "Build with pbrtParser" FORCE)
    set(VSNRAY_ENABLE_PEDANTIC FALSE CACHE BOOL "Compile with pedantic enabled (Ignored if warnings are disabled)" FORCE)
    set(VSNRAY_ENABLE_PTEX FALSE CACHE BOOL "Use Ptex, if available" FORCE)
    set(VSNRAY_ENABLE_QT5 FALSE CACHE BOOL "Use Qt5, if available" FORCE)
    set(VSNRAY_ENABLE_SDL2 FALSE CACHE BOOL "Use SDL2, if available" FORCE)
    set(VSNRAY_ENABLE_TBB TRUE CACHE BOOL "Use TBB, if available" FORCE)
    set(VSNRAY_ENABLE_UNITTESTS FALSE CACHE BOOL "Build unit tests" FORCE)
    set(VSNRAY_ENABLE_VIEWER FALSE CACHE BOOL "Build the vsnray-viewer program" FORCE)
    set(VSNRAY_ENABLE_WARNINGS FALSE CACHE BOOL "Enable all warnings" FORCE)

    add_subdirectory("${VISIONARAY_DIR}" "${VISIONARAY_BUILD_DIR}")
    set_property(TARGET visionaray PROPERTY POSITION_INDEPENDENT_CODE ON)
    set_property(TARGET visionaray_common PROPERTY POSITION_INDEPENDENT_CODE ON)

    include_directories(${VISIONARAY_INCLUDE_DIR})
    include_directories(${VISIONARAY_SRC_DIR})
    target_link_libraries(${SUBPROJECT_NAME} visionaray)
    target_link_libraries(${SUBPROJECT_NAME} visionaray_common)
    target_compile_definitions(${SUBPROJECT_NAME} PRIVATE -DHAVE_VISIONARAY)
else()
    option(USE_VISIONARAY "Support loading PNG files" OFF)
    message(WARNING "Could not find visionaray. Generating without PNG support")
endif()

