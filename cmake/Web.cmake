# handle GL source files
include(${CMAKE_CURRENT_LIST_DIR}/GL.cmake)

# create the demo wasm target
add_executable(glviskit_wasm
    "src/main_wasm.cpp"
    "${GLVISKIT_GL_SOURCES}"
)

# output html by default
set_target_properties(glviskit_wasm PROPERTIES SUFFIX ".html")

# include directories and compile/link options
target_include_directories(glviskit_wasm PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_options(glviskit_wasm PRIVATE
    "-sUSE_SDL=3"
)
target_link_options(glviskit_wasm PRIVATE
    "-sUSE_SDL=3"
    "-sMIN_WEBGL_VERSION=2"
    "-sMAX_WEBGL_VERSION=2"
    "-sWASM=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/shell_minimal.html"
)

# link libraries
target_link_libraries(glviskit_wasm PRIVATE
    glm::glm
)