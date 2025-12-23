#pragma once

// NOLINTBEGIN

// check which GL header to include
#if defined(GLVISKIT_USE_GLAD_GL)
#include "../glad/gl.h"
#define GLVISKIT_GL33
#elif defined(GLVISKIT_USE_GLAD_GLES2)
#include "../glad/gles2.h"
#define GLVISKIT_GLES3
#elif defined(GLVISKIT_USE_GL_NATIVE)
#include <GL/gl.h>
#define GLVISKIT_GL33
#elif defined(GLVISKIT_USE_GLES_NATIVE)
#include <GLES3/gl3.h>
#define GLVISKIT_GLES3
#else
#include "../glad/gl.h"
#define GLVISKIT_GL33
#endif

// NOLINTEND