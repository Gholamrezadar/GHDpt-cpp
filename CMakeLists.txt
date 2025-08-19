cmake_minimum_required(VERSION 3.15)

# Project name and version
project(GHDgui VERSION 1.0)

# O3 optimization
if(MSVC)
    # MSVC-specific release flags
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    # Flags for GCC/Clang
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)

# Header Files
file(GLOB PROJECT_HEADERS src/*.h
                        src/*.hpp)
# Source Files
file(GLOB PROJECT_SOURCES src/*.cpp)

# ImGui source files
file(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)

# Add the include directories
include_directories(
        src
        vendor
        vendor/imgui
        vendor/glfw/include
        vendor/glad/include
)

# Add GLFW library
add_subdirectory(vendor/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # don't build docs
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE) # don't build tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build examples

# Add glad as a library
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# Add the main executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glad
    glfw
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
)