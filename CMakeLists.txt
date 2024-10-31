cmake_minimum_required(VERSION 3.31)
project(Qengine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLEW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/glew-2.2.0/include)
set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/freetype-2.13.3/include)
set(FREETYPE_LIBRARY ${CMAKE_SOURCE_DIR}/libs/freetype-2.13.3/lib/Release/freetype.lib)

# Detect architecture for GLEW
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(GLEW_LIB_DIR ${CMAKE_SOURCE_DIR}/libs/glew-2.2.0/lib/Release/x64)
else()  # Assume 32-bit
	set(GLEW_LIB_DIR ${CMAKE_SOURCE_DIR}/libs/glew-2.2.0/lib/Release/Win32)
endif()

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(libs/glfw-3.4 ${CMAKE_BINARY_DIR}/glfw-build)

# Add the executable target
add_executable(Qengine
		src/Qengine.cpp
		src/scene/graphics/Viewport.cpp
)

# Define static linking
target_compile_definitions(Qengine PRIVATE GLEW_STATIC FREETYPE_STATIC)

target_include_directories(Qengine PRIVATE
	${GLEW_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
)

target_link_directories(Qengine PRIVATE
	${GLEW_LIB_DIR}
	${FREETYPE_LIBRARY}
)

target_link_libraries(Qengine PRIVATE
	glfw
	OpenGL::GL
	glew32s
	Freetype::Freetype
)
