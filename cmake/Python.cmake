# fetch SDL3
include(${CMAKE_CURRENT_LIST_DIR}/SDL3.cmake)

# setup nanobind python module
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# create the python module
nanobind_add_module(glviskit_py
    NB_STATIC LTO "src/bindings.cpp"
)
# include and link
target_include_directories(glviskit_py PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(glviskit_py PRIVATE
    SDL3::SDL3-static
    glm::glm
)
set_target_properties(glviskit_py PROPERTIES OUTPUT_NAME "glviskit")
# set static flags, we dont want any dynamic dependencies
apply_static_flags(glviskit_py)
# needed for python module
install(TARGETS glviskit_py LIBRARY DESTINATION .)