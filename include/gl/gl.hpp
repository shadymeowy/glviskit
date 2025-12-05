#pragma once

// NOLINTBEGIN

// check which GL header to include
#if defined(GLVISKIT_USE_GLAD_GL)
#include "glad/gl.h"
#elif defined(GLVISKIT_USE_GLAD_GLES2)
#include "glad/gles2.h"
#elif defined(GLVISKIT_USE_GL_NATIVE)
#include <GL/gl.h>
#elif defined(GLVISKIT_USE_GLES_NATIVE)
#include <GLES3/gl3.h>
#else
#error \
    "No GL header defined. Define one of GLVISKIT_USE_GLAD_GL, GLVISKIT_USE_GLAD_GLES2, GLVISKIT_USE_GL_NATIVE, GLVISKIT_USE_GLES_NATIVE."
#endif

// NOLINTEND