# setup cython python module
find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

set(CYTHON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/glviskit.pyx")
set(CYTHON_PXD "${CMAKE_CURRENT_SOURCE_DIR}/src/glviskit_c_api.pxd")
set(CYTHON_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/glviskit.cpp")

add_custom_command(
    OUTPUT "${CYTHON_OUTPUT}"
    COMMAND "${Python_EXECUTABLE}" -m cython --cplus --3str
            -I "${CMAKE_CURRENT_SOURCE_DIR}/src"
            -o "${CYTHON_OUTPUT}" "${CYTHON_SOURCE}"
    DEPENDS "${CYTHON_SOURCE}" "${CYTHON_PXD}"
    COMMENT "Cythonizing glviskit.pyx"
    VERBATIM
)

# create the python module
Python_add_library(glviskit_python MODULE WITH_SOABI
    "${CYTHON_OUTPUT}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/c_api.cpp"
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/py.typed" "")

# link glviskit
target_link_libraries(glviskit_python PRIVATE
    glviskit::glviskit
)
target_compile_features(glviskit_python PRIVATE cxx_std_20)
target_compile_definitions(glviskit_python PRIVATE GLVISKIT_C_STATIC=1)
target_include_directories(glviskit_python PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# we want to hide all symbols by default
set_target_properties(glviskit_python PROPERTIES
    OUTPUT_NAME "glviskit"
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

# needed for python module
install(TARGETS glviskit_python LIBRARY DESTINATION .)

# install the stub file too
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glviskit.pyi"
    "${CMAKE_CURRENT_BINARY_DIR}/py.typed"
    DESTINATION .
)
