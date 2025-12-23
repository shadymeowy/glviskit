#pragma once

#include <iostream>

#include "../gl/gl.hpp"
#include "../renderer.hpp"
#include "sdl.hpp"

namespace glviskit::sdl {

class Window {
   public:
    Window(const char *title, int w, int h, bool share_context)
        : window_{nullptr}, context_{nullptr} {
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
                            share_context ? 1 : 0);

        auto *handle =
            SDL_CreateWindow(title, w, h,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                 SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (handle == nullptr) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        window_ = SDLWindowPtr(handle);
        if (window_.Get() == nullptr) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
        }
        window_id_ = SDL_GetWindowID(window_.Get());

        context_ = SDLGLContextPtr(SDL_GL_CreateContext(window_.Get()));

        if (context_.Get() == nullptr) {
            std::cerr << "Failed to create SDL GL context: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        MakeCurrent();
    }

    void AddRenderBuffer(const std::shared_ptr<RenderBuffer> &render_buffer) {
        renderer.AddRenderBuffer(render_buffer);
    }

    auto GetCamera() -> std::shared_ptr<Camera> { return renderer.GetCamera(); }
    void SetCamera(std::shared_ptr<Camera> cam) {
        renderer.SetCamera(std::move(cam));
    }

    void MakeCurrent() { SDL_GL_MakeCurrent(window_.Get(), context_.Get()); }

    void Render() {
        // make context current
        // renderer expects the context to be current
        bool ret = SDL_GL_MakeCurrent(window_.Get(), context_.Get());
        if (!ret) {
            std::cerr << "Failed to make context current: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        // update screen size
        int width;
        int height;
        SDL_GetWindowSizeInPixels(window_.Get(), &width, &height);

        // do rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.Render(window_id_, width, height);

        // swap buffers
        SDL_GL_SwapWindow(window_.Get());

        GLenum error2 = glGetError();
        if (error2 != GL_NO_ERROR) {
            std::cerr << "OpenGL error in window 2: " << error2 << '\n';
            exit(EXIT_FAILURE);
        }
    }

    void CallbackKeyDown(const SDL_KeyboardEvent &key) const {
        std::cout << "Key down in window " << window_id_ << ": "
                  << SDL_GetKeyName(key.key) << '\n';
    }

    [[nodiscard]] auto GetWindowID() const -> Uint32 { return window_id_; }

   private:
    SDLWindowPtr window_;
    SDLGLContextPtr context_;

    Renderer renderer;
    GLuint window_id_;

    friend class Manager;
};

}  // namespace glviskit::sdl