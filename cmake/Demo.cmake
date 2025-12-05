# fetch SDL3
include(${CMAKE_CURRENT_LIST_DIR}/SDL3.cmake)

# create the demo executable
add_executable(glviskit_demo
    "src/main.cpp"
)
# include and link
target_include_directories(glviskit_demo PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(glviskit_demo PRIVATE
    SDL3::SDL3-static
    glm::glm
)
# set static flags
# include(${CMAKE_CURRENT_LIST_DIR}/Static.cmake)
# apply_static_flags(glviskit_demo)