cmake_minimum_required(VERSION 3.15)

# define the project
project(glviskit
    VERSION 1.0
    LANGUAGES CXX C
)

# set cmake options
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# possible gl backends
set(GLVISKIT_GL_TYPE "AUTO" CACHE STRING "Type of OpenGL backend to use (AUTO, GLAD_GL, GLAD_GLES2, NATIVE_GL, NATIVE_GLES2, NONE)")

# enable clang-tidy if available
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# if(CLANG_TIDY_EXE)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()

# create the main library target
add_library(glviskit_lib STATIC)
add_library(glviskit::glviskit ALIAS glviskit_lib)

# set position independent code
set_target_properties(glviskit_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

# set cpp version
target_compile_features(glviskit_lib PUBLIC cxx_std_20)

# include and link
target_include_directories(glviskit_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# handle glm
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GLM.cmake)

# handle SDL3
include(${CMAKE_CURRENT_LIST_DIR}/cmake/SDL3.cmake)

# handle GL source files and autodetect GL type
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GL.cmake)
