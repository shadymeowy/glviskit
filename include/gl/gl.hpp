#pragma once

// NOLINTBEGIN

// check which GL header to include
#if defined(GLVISKIT_USE_GLAD_GL)
#include "../glad/gl.h"
#define GLVISKIT_GL33 1
#define GLVISKIT_GLES3 0
#elif defined(GLVISKIT_USE_GLAD_GLES2)
#include "../glad/gles2.h"
#define GLVISKIT_GL33 0
#define GLVISKIT_GLES3 1
#elif defined(GLVISKIT_USE_GL_NATIVE)
#include <GL/gl.h>
#define GLVISKIT_GL33 1
#define GLVISKIT_GLES3 0
#elif defined(GLVISKIT_USE_GLES_NATIVE)
#include <GLES3/gl3.h>
#define GLVISKIT_GL33 0
#define GLVISKIT_GLES3 1
#else
#include "../glad/gl.h"
#define GLVISKIT_GL33 1
#define GLVISKIT_GLES3 0
#endif

// NOLINTEND