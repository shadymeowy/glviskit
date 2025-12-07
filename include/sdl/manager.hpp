#pragma once

#include <iostream>
#include <memory>

#include "../gl/gl.hpp"
#include "../render_buffer.hpp"
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
                if (event.key.key == SDLK_ESCAPE) {
                    return false;
                }
                if (windows_.contains(event.key.windowID)) {
                    windows_[event.key.windowID]->CallbackKeyDown(event.key);
                }
                break;
            default:
                break;
        }
        return true;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    auto CreateRenderBuffer() -> std::shared_ptr<RenderBuffer> {
        EnsureContext();
        return std::make_shared<RenderBuffer>();
    }

    static auto GetTimeSeconds() -> float {
        return static_cast<float>(SDL_GetTicks()) / 1000.0F;
    }

   private:
    std::map<Uint32, std::shared_ptr<Window>> windows_;

    Manager() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
            exit(EXIT_FAILURE);
        }
        if (!SDL_GL_LoadLibrary(nullptr)) {
            std::cerr << "Failed to load SDL GL library: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
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
        int ret = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
#elif defined(GLVISKIT_USE_GLAD_GLES2)
        int ret = gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
#else
        int ret = 1;
#endif
        if (ret == 0) {
            std::cerr << "Failed to initialize GLAD" << '\n';
            exit(EXIT_FAILURE);
        }

        std::cerr << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
        std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << '\n';
    }
};

}  // namespace glviskit::sdl