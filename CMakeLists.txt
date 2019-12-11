project(emptyOpengGL)

cmake_minimum_required(VERSION 3.2.0)

add_subdirectory(ext/glfw)

include_directories(ext/glfw/include)
include_directories(ext/glm)
include_directories(ext)
include_directories(include)

if (APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- Mitsuba requires C++11 support!")
  endif()
  # Enable link time optimization and set the default symbol
  # visibility to hidden (very important to obtain small binaries)
  if (NOT ${U_CMAKE_BUILD_TYPE} MATCHES DEBUG)
    # Default symbol visibility
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
  endif()
endif()

set(SRC_FILES 
	#include
	include/opengl.h
	include/renderer.h
	include/glew.c
	include/shader.h
	include/camera.h
	include/trackball.h
	include/mesh.h
	include/cube.h
	
		
	#src
	src/main.cpp
	src/renderer.cpp
	src/shader.cpp
	src/camera.cpp
	src/trackball.cpp
	src/mesh.cpp
	src/cube.cpp
)

add_definitions(-DGLEW_STATIC)
add_definitions(-DDATA_DIR="${PROJECT_SOURCE_DIR}/data")

add_library(glew STATIC include/glew.c)

add_executable(emptyGL ${SRC_FILES})

target_link_libraries(
				emptyGL
				glew
				glfw
				${GLFW_LIBRARIES})

#file(GLOB SHADERS "data/shaders/*.vert" "data/shaders/*.frag")
#IndicateExternalFile("native-ogl" ${SHADERS})