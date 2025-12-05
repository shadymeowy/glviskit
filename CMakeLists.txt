cmake_minimum_required(VERSION 3.15)

# set cmake options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# define the project
project(glviskit
    VERSION 1.0
    LANGUAGES CXX C
)

# possible versions
set(GLVISKIT_GL_TYPE "AUTO" CACHE STRING "Type of OpenGL backend to use (AUTO, GLAD_GL, GLAD_GLES2, NATIVE_GL, NATIVE_GLES2, NONE)")

# enable clang-tidy if available
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
# if(CLANG_TIDY_EXE)
#     set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
# endif()

# fetch glm, it will be used for all targets
include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)
set_target_properties(glm PROPERTIES CXX_CLANG_TIDY "")

# check for emscripten, python or normal build
if(EMSCRIPTEN)
    message(STATUS "Emscripten build detected")

    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Web.cmake)
elseif(SKBUILD)
    message(STATUS "SKBuild detected")

    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Python.cmake)
else()
    message(STATUS "Normal build detected")

    include(${CMAKE_CURRENT_LIST_DIR}/cmake/Demo.cmake)
endif()