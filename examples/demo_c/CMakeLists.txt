cmake_minimum_required(VERSION 3.15)

# define the project
project(glviskit_demo_c
    VERSION 1.0
    LANGUAGES CXX C
)

# enable compile commands export
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# c standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# fetch glviskit
include(FetchContent)
FetchContent_Declare(
    glviskit
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.."
)
FetchContent_MakeAvailable(glviskit)

# create the demo executable
add_executable(glviskit_demo_c main.c)

target_link_libraries(glviskit_demo_c
    PRIVATE glviskit::c_api
)

if(UNIX)
    target_link_libraries(glviskit_demo_c PRIVATE m)
endif()
