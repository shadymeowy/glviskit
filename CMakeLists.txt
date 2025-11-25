cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)

project(drawing3dtest
    VERSION 1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
    "src/main.cpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2-static
    glm::glm
)

if(WIN32 AND MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-static-libgcc"
        "-static-libstdc++"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-Wl,-Bstatic"
        "-lwinpthread"
        "-Wl,-Bdynamic"
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++
    )
endif()