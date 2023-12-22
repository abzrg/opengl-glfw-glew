cmake_minimum_required(VERSION 3.16)

project(
  OpenGL-GLFW-GLEW
  LANGUAGES CXX C
  DESCRIPTION "OpenGL + GLFW + GLEW (TheCherno Series)"
  VERSION 0.1.0
)

if(CMAKE_GENERATOR MATCHES "Ninja")
  add_compile_options("-fdiagnostics-color=always")
endif()


# Language setting
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)


# ---< Dependencies >----------------------------------------------------------


include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)


# -- GLFW
FetchContent_Declare(
  glfw3
  GIT_REPOSITORY  https://github.com/glfw/glfw.git
  GIT_TAG         "3.3.8"
  GIT_CONFIG      "advice.detachedhead=false"
  GIT_SHALLOW     TRUE
  GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(glfw3)
if(NOT glfw3_POPULATED)
  FetchContent_Populate(glfw3)
  set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs" FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation" FORCE)
  set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target" FORCE)
  add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR} EXCLUDE_FROM_ALL)
  add_library(GLFW::GLFW ALIAS glfw)
endif()


# -- GLEW
FetchContent_Declare(
  glew
  # URL            https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
  GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake
  GIT_TAG         "glew-cmake-2.2.0" # f456deace7b408655109aaeff71421ef2d3858c6
  GIT_CONFIG      "advice.detachedhead=false"
  GIT_SHALLOW     TRUE
  GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
  FetchContent_Populate(glew)
  option(ONLY_LIBS "Do not build executables" ON)
  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
  add_library(GLEW::GLEW_STATIC ALIAS libglew_static)
  add_library(GLEW::GLEW_SAHARED ALIAS libglew_shared)
endif()


# -- OpenGL
find_package(OpenGL REQUIRED) # Some platforms need OpenGL.
if(NOT OpenGL_FOUND)
  if(WIN32)
    message(FATAL_ERROR "OpenGL development files not found. Please install some version of the Windows SDK.")
  elseif(Linux)
    message(FATAL_ERROR "OpenGL development files not found. Please install them using your system package manager, for eg. libgl-dev")
  else(Apple)
    message(FATAL_ERROR "OpenGL development files not found.")
  endif()
endif()
if(NOT TARGET OpenGL::GLU)
  # GLU is a dependency of GLEW but it's not advertized as an OpenGL COMPONENT
  message(FATAL_ERROR "GLU is a dependency of GLEW but was not found.")
endif()


# -- DEPS: OpenGL + GLFW + GLEW
add_library(deps INTERFACE)
target_link_libraries(deps
  INTERFACE OpenGL::GL # On some platforms we still need to link directly.
  INTERFACE GLEW::GLEW_STATIC
  INTERFACE GLFW::GLFW
)
target_include_directories(deps INTERFACE ${OPENGL_INCLUDE_DIRS})


# ---< App >-------------------------------------------------------------------

add_subdirectory(src)
