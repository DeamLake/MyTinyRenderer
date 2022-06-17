# CMakeList.txt: 顶层 CMageoke 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.15)

set(curr_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(platform_dir ${curr_dir}/src/Platform)
set(core_dir ${curr_dir}/src/Core)
set(utils_dir ${curr_dir}/src/Utils)
set(shader_dir ${curr_dir}/src/Shader)

project(SoftRenderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Debug和Release output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/out)

# external library
include_directories(${CMAKE_SOURCE_DIR}/3rdParty)

# Add header file include directories
include_directories(
    ${platform_dir}
    ${core_dir}
    ${utils_dir}
    ${shader_dir}
)

# Add block directories
add_subdirectory(src/Platform)
add_subdirectory(src/Core)
add_subdirectory(src/Utils)
add_subdirectory(src/Shader)

# Target
add_executable(Sandbox "src/SandBox.cpp")
set_target_properties(Sandbox PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")
target_link_libraries(Sandbox platform core utils shader)

