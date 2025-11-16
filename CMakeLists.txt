cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(drawing3dtest
    VERSION 1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
    "src/main.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glm::glm
)