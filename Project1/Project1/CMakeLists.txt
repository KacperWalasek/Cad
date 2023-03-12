cmake_minimum_required(VERSION 3.23)

project(CAD CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(vendor)
include_directories(vendor/glfw/include)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(src)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/imgui)
