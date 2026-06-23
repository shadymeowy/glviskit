#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "../gl/gl.hpp"
#include "../render_list.hpp"
#include "window.hpp"

namespace glviskit::sdl {

class Manager {
   public:
    // singleton access
    static auto GetInstance() -> Manager & {
        static Manager instance;
        return instance;
    }

    Manager(const Manager &) = delete;
    auto operator=(const Manager &) -> Manager & = delete;
    Manager(Manager &&) = delete;
    auto operator=(Manager &&) -> Manager & = delete;

    ~Manager() {
        windows_.clear();

        SDL_Quit();
    }

    auto CreateWindow(const char *title, int w, int h)
        -> std::shared_ptr<Window> {
        std::shared_ptr<Window> window;

        if (!windows_.empty()) {
            auto any_window = GetAnyWindow();
            any_window->MakeCurrent();
            window = std::make_shared<Window>(title, w, h, true);
        } else {
            window = std::make_shared<Window>(title, w, h, false);
            window->MakeCurrent();
            LoadGLAD();
        }

        windows_.insert({window->GetWindowID(), window});
        return window;
    }

    auto Loop() -> bool {
        Render();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (!ProcessEvent(event)) {
                return false;
            }
        }
        return true;
    }

    void Render() {
        for (auto &[id, window] : windows_) {
            window->Render();
        }
    }

    auto ProcessEvent(const SDL_Event &event) -> bool {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return false;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                if (event.key.key == SDLK_ESCAPE) {
                    return false;
                }
                if (windows_.contains(event.key.windowID)) {
                    windows_[event.key.windowID]->CallbackKey(event.key);
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (windows_.contains(event.button.windowID)) {
                    windows_[event.button.windowID]->CallbackButton(
                        event.button);
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (windows_.contains(event.motion.windowID)) {
                    windows_[event.motion.windowID]->CallbackMotion(
                        event.motion);
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (windows_.contains(event.wheel.windowID)) {
                    windows_[event.wheel.windowID]->CallbackWheel(event.wheel);
                }
            default:
                break;
        }
        return true;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto CreateRenderList() -> std::shared_ptr<RenderList> {
        EnsureContext();
        return std::make_shared<RenderList>();
    }

    static auto GetTimeSeconds() -> float {
        return static_cast<float>(SDL_GetTicks()) / 1000.0F;
    }

   private:
    std::map<Uint32, std::shared_ptr<Window>> windows_;

    Manager() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string{"Failed to initialize SDL: "} +
                                     SDL_GetError());
        }
        if (!SDL_GL_LoadLibrary(nullptr)) {
            throw std::runtime_error(
                std::string{"Failed to load SDL GL library: "} + SDL_GetError());
        }

#if defined(GLVISKIT_GL33)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
#elif defined(GLVISKIT_GLES3)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_ES);
#else
#error "No GL version defined"
#endif
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    }

    // get any active window (for context sharing)
    auto GetAnyWindow() -> std::shared_ptr<Window> {
        EnsureContext();
        return windows_.begin()->second;
    }

    void EnsureContext() {
        if (windows_.empty()) {
            throw std::runtime_error(
                "No context initialized, create a window first");
        }
    }

    // load glad after context creation
    static void LoadGLAD() {
#if defined(GLVISKIT_USE_GLAD_GL)
        int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
        if (version == 0) {
            throw std::runtime_error("Failed to load OpenGL via GLAD");
        }
        if (GLAD_VERSION_MAJOR(version) < 3 ||
            (GLAD_VERSION_MAJOR(version) == 3 &&
             GLAD_VERSION_MINOR(version) < 3)) {
            throw std::runtime_error(
                "OpenGL 3.3 or newer is required, but the driver provided " +
                std::to_string(GLAD_VERSION_MAJOR(version)) + "." +
                std::to_string(GLAD_VERSION_MINOR(version)));
        }
#elif defined(GLVISKIT_USE_GLAD_GLES2)
        int version = gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
        if (version == 0) {
            throw std::runtime_error("Failed to load OpenGL ES via GLAD");
        }
        if (GLAD_VERSION_MAJOR(version) < 3) {
            throw std::runtime_error(
                "OpenGL ES 3.0 or newer is required, but the driver provided " +
                std::to_string(GLAD_VERSION_MAJOR(version)) + "." +
                std::to_string(GLAD_VERSION_MINOR(version)));
        }
#endif
    }
};

}  // namespace glviskit::sdl