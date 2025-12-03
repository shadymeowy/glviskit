cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(glviskittest
    VERSION 1.0
    LANGUAGES CXX C
)

include(FetchContent)
FetchContent_Declare(SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.32.10
)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL2)

include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

add_library(glviskit_core INTERFACE)
target_include_directories(glviskit_core INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(glviskit_core INTERFACE
    SDL2::SDL2-static
    glm::glm
)

add_executable(glviskit_test_app
    "src/main.cpp"
)
target_link_libraries(glviskit_test_app PRIVATE
    glviskit_core
)

macro(apply_static_flags TARGET_NAME)
    if(WIN32 AND MINGW)
        target_link_libraries(${TARGET_NAME} PRIVATE
            "-static-libgcc"
            "-static-libstdc++"
            "-Wl,-Bstatic"
            "-lwinpthread"
            "-Wl,-Bdynamic"
        )
    endif()

    if(UNIX AND NOT APPLE)
        target_link_options(${TARGET_NAME} PRIVATE
            -static-libgcc
            -static-libstdc++
        )
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
endmacro()

apply_static_flags(glviskit_test_app)