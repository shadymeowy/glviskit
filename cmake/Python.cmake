# fetch SDL3
include(${CMAKE_CURRENT_LIST_DIR}/SDL3.cmake)

# handle GL source files
include(${CMAKE_CURRENT_LIST_DIR}/GL.cmake)

# setup nanobind python module
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
find_package(nanobind CONFIG REQUIRED)

# create the python module
nanobind_add_module(glviskit
    NB_STATIC LTO
    NB_DOMAIN "glviskit"
    STABLE_ABI
    "src/bindings.cpp"
    "src/gl.c"
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/py.typed" "")

# include and link
target_include_directories(glviskit PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(glviskit PRIVATE
    SDL3::SDL3-static
    glm::glm
)
set_target_properties(glviskit PROPERTIES OUTPUT_NAME "glviskit")

# set static flags, we dont want any dynamic dependencies
# include(${CMAKE_CURRENT_LIST_DIR}/Static.cmake)
# apply_static_flags(glviskit)

# needed for python module
install(TARGETS glviskit LIBRARY DESTINATION .)
# install the stub file too
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glviskit.pyi"
    "${CMAKE_CURRENT_BINARY_DIR}/py.typed"
    DESTINATION .
)