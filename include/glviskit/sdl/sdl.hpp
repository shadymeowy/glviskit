#pragma once

// if not emscripten, prevent SDL main redefinition
#ifndef EMSCRIPTEN
#define SDL_MAIN_HANDLED
#endif
// prevent min/max macros on Windows
#define NOMINMAX
#include <SDL3/SDL.h>
// undefine CreateWindow macro if defined from Windows.h
#ifdef CreateWindow
#undef CreateWindow
#endif

#include "sdl_ptr.hpp"

namespace glviskit::sdl {

// define common types
using SDLWindowPtr = SDLPtr<SDL_Window *, void, SDL_DestroyWindow>;
using SDLGLContextPtr = SDLPtr<SDL_GLContext, bool, SDL_GL_DestroyContext>;
using SDLRendererPtr = SDLPtr<SDL_Renderer *, void, SDL_DestroyRenderer>;
using SDLTexturePtr = SDLPtr<SDL_Texture *, void, SDL_DestroyTexture>;

}  // namespace glviskit::sdl