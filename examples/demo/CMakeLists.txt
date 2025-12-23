cmake_minimum_required(VERSION 3.15)

# define the project
project(glviskit_demo
    VERSION 1.0
    LANGUAGES CXX C
)

# enable compile commands export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# cpp standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# fetch glviskit
include(FetchContent)
FetchContent_Declare(
    glviskit
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)
FetchContent_MakeAvailable(glviskit)

# create the demo executable
add_executable(glviskit_demo main.cpp)

target_link_libraries(glviskit_demo
    PRIVATE glviskit::glviskit
)