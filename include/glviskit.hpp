#pragma once

// NOLINTBEGIN(unused-includes)
#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#include "sdl/manager.hpp"
// NOLINTEND(unused-includes)

namespace glviskit {

// use SDL as the default window manager
using Manager = sdl::Manager;

}  // namespace glviskit