#pragma once

#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#include "sdl.hpp"

namespace glviskit {

// use SDL as the default window manager
using Manager = sdl::Manager;

}  // namespace glviskit