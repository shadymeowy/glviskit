#pragma once

// NOLINTBEGIN(unused-includes)
#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#include "sdl/manager.hpp"
#include "sdl/window.hpp"
// NOLINTEND(unused-includes)

namespace glviskit {

// use SDL as the default window manager
using Manager = sdl::Manager;

auto CreateWindow(const char *title, int w, int h)
    -> std::shared_ptr<sdl::Window> {
    return Manager::GetInstance().CreateWindow(title, w, h);
}

auto CreateRenderBuffer() -> std::shared_ptr<RenderBuffer> {
    return Manager::GetInstance().CreateRenderBuffer();
}

auto GetTimeSeconds() -> float {
    return Manager::GetInstance().GetTimeSeconds();
}

auto Loop() -> bool { return Manager::GetInstance().Loop(); }

void Render() { Manager::GetInstance().Render(); }

}  // namespace glviskit