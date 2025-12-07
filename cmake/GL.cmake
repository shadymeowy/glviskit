# determine which OpenGL backend to use
if(NOT DEFINED GLVISKIT_GL_TYPE)
    set(GLVISKIT_GL_TYPE "AUTO")
endif()

message(STATUS "GLVISKIT_GL_TYPE is set to ${GLVISKIT_GL_TYPE}")

# auto mode
if(GLVISKIT_GL_TYPE STREQUAL "AUTO")
    if(EMSCRIPTEN)
        set(GLVISKIT_GL_TYPE "NATIVE_GLES2")
    else()
        set(GLVISKIT_GL_TYPE "GLAD_GL")
    endif()
endif()

message(STATUS "Using GLVISKIT_GL_TYPE: ${GLVISKIT_GL_TYPE}")

# set flags accordingly
if(GLVISKIT_GL_TYPE STREQUAL "GLAD_GL")
    add_compile_definitions(GLVISKIT_USE_GLAD_GL=1)
elseif(GLVISKIT_GL_TYPE STREQUAL "GLAD_GLES2")
    add_compile_definitions(GLVISKIT_USE_GLAD_GLES2=1)
elseif(GLVISKIT_GL_TYPE STREQUAL "NATIVE_GL")
    add_compile_definitions(GLVISKIT_USE_GL_NATIVE=1)
elseif(GLVISKIT_GL_TYPE STREQUAL "NATIVE_GLES2")
    add_compile_definitions(GLVISKIT_USE_GLES_NATIVE=1)
else()
    message(FATAL_ERROR "Unknown GLVISKIT_GL_TYPE: ${GLVISKIT_GL_TYPE}")
endif()

# set source file variable
if(GLVISKIT_GL_TYPE STREQUAL "GLAD_GL")
    list(APPEND GLVISKIT_GL_SOURCES "src/gl.c")
elseif(GLVISKIT_GL_TYPE STREQUAL "GLAD_GLES2")
    list(APPEND GLVISKIT_GL_SOURCES "src/gles2.c")
endif()