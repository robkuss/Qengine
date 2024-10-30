cmake_minimum_required(VERSION 3.31)
project(.)

set(CMAKE_CXX_STANDARD 23)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.4 ${CMAKE_BINARY_DIR}/glfw-build)

include_directories(libs/glew-2.2.0/include)
include_directories(libs/freetype-2.13.3/include)

# Add the GLEW library path based on architecture (32-bit or 64-bit)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # Detect 64-bit
	set(GLEW_LIB_DIR ${CMAKE_SOURCE_DIR}/libs/glew-2.2.0/lib/Release/x64)
else()  # Assume 32-bit
	set(GLEW_LIB_DIR ${CMAKE_SOURCE_DIR}/libs/glew-2.2.0/lib/Release/Win32)
endif()

link_directories(${GLEW_LIB_DIR})

# Define GLEW_STATIC if linking statically
#add_definitions(-DGLEW_STATIC)

find_package(OpenGL REQUIRED)

# Add the executable target
add_executable(Qengine src/Qengine.cpp)

target_link_libraries(Qengine PRIVATE
	glfw
	OpenGL::GL
	glew32s
)