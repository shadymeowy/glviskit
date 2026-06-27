cmake_minimum_required(VERSION 3.15)

# define the project
project(glviskit
    VERSION 1.0
    LANGUAGES CXX C
)

# set cmake options if this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# need position independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# possible gl backends
set(GLVISKIT_GL_TYPE "AUTO" CACHE STRING "Type of OpenGL backend to use (AUTO, GLAD_GL, GLAD_GLES2, NATIVE_GL, NATIVE_GLES2, NONE)")

# statically link the C/C++ runtime into glviskit_c for redistributable binaries
option(GLVISKIT_STATIC_RUNTIME "Statically link the C/C++ runtime into glviskit_c" OFF)
if(GLVISKIT_STATIC_RUNTIME AND MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

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
# if this is the top-level project, set the C++ standard for the build
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

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

# handle Dear ImGui and the UI sources
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ImGui.cmake)

# create the C ABI shared library target
add_library(glviskit_c SHARED
    src/c_api.cpp
)
add_library(glviskit::c_api ALIAS glviskit_c)

target_link_libraries(glviskit_c PRIVATE
    glviskit::glviskit
)

target_compile_features(glviskit_c PRIVATE cxx_std_20)
target_compile_definitions(glviskit_c PRIVATE GLVISKIT_C_BUILD=1)
target_include_directories(glviskit_c PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(glviskit_c PROPERTIES
    OUTPUT_NAME "glviskit_c"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# export only the glv_* C API; localize statically linked SDL3/glviskit_lib
# symbols so they do not leak from the shared library
if(APPLE)
    target_link_options(glviskit_c PRIVATE
        "LINKER:-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/cmake/glviskit_c.exports")
elseif(UNIX)
    target_link_options(glviskit_c PRIVATE "LINKER:--exclude-libs,ALL")
endif()

# optionally fold the C/C++ runtime into the library for redistribution
if(GLVISKIT_STATIC_RUNTIME AND NOT MSVC AND NOT APPLE)
    target_link_options(glviskit_c PRIVATE -static-libstdc++ -static-libgcc)
endif()

# if skbuild is available, build python bindings
if(SKBUILD)
    add_subdirectory(python)
endif()
