cmake_minimum_required(VERSION 3.15)

project(glviskit_python LANGUAGES CXX)

# set cpp standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable position independent code for everything
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# fetch glviskit
include(FetchContent)
FetchContent_Declare(
    glviskit
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
FetchContent_MakeAvailable(glviskit)

# setup nanobind python module
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# create the python module
nanobind_add_module(glviskit_python
    NB_STATIC LTO
    NB_DOMAIN "glviskit"
    STABLE_ABI
    "src/bindings.cpp"
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/py.typed" "")

# link glviskit
target_link_libraries(glviskit_python PRIVATE
    glviskit::glviskit
)

# we want to hide all symbols by default
set_target_properties(glviskit_python PROPERTIES 
    OUTPUT_NAME "glviskit"
    CXX_VISIBILITY_PRESET hidden
)

# needed for python module
install(TARGETS glviskit_python LIBRARY DESTINATION .)

# install the stub file too
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glviskit.pyi"
    "${CMAKE_CURRENT_BINARY_DIR}/py.typed"
    DESTINATION .
)