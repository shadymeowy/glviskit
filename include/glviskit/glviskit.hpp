#pragma once

// NOLINTBEGIN(unused-includes)
#include "camera.hpp"
#include "controller/base_controller.hpp"
#include "controller/first_person_controller.hpp"
#include "controller/null_controller.hpp"
#include "controller/spherical_controller.hpp"
#include "mesh.hpp"
#include "render_list.hpp"
#include "sdl/manager.hpp"
#include "sdl/window.hpp"
#include "window_renderer.hpp"
// NOLINTEND(unused-includes)

namespace glviskit {

// use SDL as the default window manager
using Manager = sdl::Manager;

static auto CreateWindow(const char *title, int w, int h)
    -> std::shared_ptr<sdl::Window> {
    return Manager::GetInstance().CreateWindow(title, w, h);
}

static auto CreateRenderList() -> std::shared_ptr<RenderList> {
    return Manager::GetInstance().CreateRenderList();
}

static auto GetTimeSeconds() -> float { return Manager::GetTimeSeconds(); }

static auto Loop() -> bool { return Manager::GetInstance().Loop(); }
static void Render() { Manager::GetInstance().Render(); }
static void UiBegin() { Manager::GetInstance().UiBegin(); }

}  // namespace glviskit