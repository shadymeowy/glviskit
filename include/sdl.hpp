#pragma once
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#ifdef CreateWindow
#undef CreateWindow
#endif

#include <iostream>
#include <map>
#include <memory>

#include "camera.hpp"
#include "gl/glad.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"

namespace glviskit::sdl {

class SDLWindowPtr {
   public:
    SDLWindowPtr(SDL_Window *handle) : ptr{handle} {}
    ~SDLWindowPtr() {
        if (ptr) {
            SDL_DestroyWindow(ptr);
            ptr = nullptr;
        }
    }
    // uncopyable but movable
    SDLWindowPtr(const SDLWindowPtr &) = delete;
    SDLWindowPtr &operator=(const SDLWindowPtr &) = delete;
    SDLWindowPtr(SDLWindowPtr &&other) noexcept : ptr{other.ptr} {
        other.ptr = nullptr;
    }
    SDLWindowPtr &operator=(SDLWindowPtr &&other) noexcept {
        if (this != &other) {
            if (ptr) {
                SDL_DestroyWindow(ptr);
            }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    SDL_Window *ptr;
};

class SDLGLContextPtr {
   public:
    SDLGLContextPtr(SDL_GLContext handle) : ctx{handle} {}
    ~SDLGLContextPtr() {
        if (ctx) {
            SDL_GL_DestroyContext(ctx);
            ctx = nullptr;
        }
    }
    // uncopyable but movable
    SDLGLContextPtr(const SDLGLContextPtr &) = delete;
    SDLGLContextPtr &operator=(const SDLGLContextPtr &) = delete;
    SDLGLContextPtr(SDLGLContextPtr &&other) noexcept : ctx{other.ctx} {
        other.ctx = nullptr;
    }
    SDLGLContextPtr &operator=(SDLGLContextPtr &&other) noexcept {
        if (this != &other) {
            if (ctx) {
                SDL_GL_DestroyContext(ctx);
            }
            ctx = other.ctx;
            other.ctx = nullptr;
        }
        return *this;
    }

    SDL_GLContext ctx;
};

class Window {
   public:
    Window(GladGLContext &gl, const char *title, int w, int h)
        : gl{gl}, renderer(gl), window_{nullptr}, context_{nullptr} {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

        auto handle = SDL_CreateWindow(
            title, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!handle) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        window_ = SDLWindowPtr(handle);
        if (!window_.ptr) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        window_id_ = SDL_GetWindowID(window_.ptr);

        context_ = SDLGLContextPtr(SDL_GL_CreateContext(window_.ptr));

        if (!context_.ctx) {
            std::cerr << "Failed to create SDL GL context: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        MakeCurrent();
    }

    void AddRenderBuffer(std::shared_ptr<RenderBuffer> render_buffer) {
        renderer.AddRenderBuffer(render_buffer);
    }

    std::shared_ptr<Camera> GetCamera() { return renderer.GetCamera(); }
    void SetCamera(std::shared_ptr<Camera> cam) { renderer.SetCamera(cam); }

    void MakeCurrent() { SDL_GL_MakeCurrent(window_.ptr, context_.ctx); }

    void Render() {
        // make context current
        // renderer expects the context to be current
        bool ret = SDL_GL_MakeCurrent(window_.ptr, context_.ctx);
        if (!ret) {
            std::cerr << "Failed to make context current: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        // update screen size
        int width, height;
        SDL_GetWindowSizeInPixels(window_.ptr, &width, &height);

        // do rendering
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.Render(window_id_, width, height);

        // swap buffers
        SDL_GL_SwapWindow(window_.ptr);

        GLenum error2 = gl.GetError();
        if (error2 != GL_NO_ERROR) {
            std::cerr << "OpenGL error in window 2: " << error2 << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void CallbackKeyDown(const SDL_KeyboardEvent &key) {
        std::cout << "Key down in window " << window_id_ << ": "
                  << SDL_GetKeyName(key.key) << std::endl;
    }

    Uint32 GetWindowID() const { return window_id_; }

   private:
    GladGLContext &gl;

    SDLWindowPtr window_;
    SDLGLContextPtr context_;

    Renderer renderer;
    GLuint window_id_;
};

class Manager {
   public:
    Manager() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        if (!SDL_GL_LoadLibrary(nullptr)) {
            std::cerr << "Failed to load SDL GL library: " << SDL_GetError()
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        window_master_ = SDL_CreateWindow(
            "Hidden", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
        if (!window_master_.ptr) {
            std::cerr << "Failed to create SDL master window: "
                      << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }

        context_master_ = SDL_GL_CreateContext(window_master_.ptr);

        if (!context_master_.ctx) {
            std::cerr << "Failed to create SDL master GL context: "
                      << SDL_GetError() << std::endl;
            exit(EXIT_FAILURE);
        }

        int ret = gladLoadGLContext(&gl_, (GLADloadfunc)SDL_GL_GetProcAddress);
        if (!ret) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ~Manager() {
        windows_.clear();

        SDL_Quit();
    }

    std::shared_ptr<Window> CreateWindow(const char *title, int w, int h) {
        // make master context current for context sharing
        SDL_GL_MakeCurrent(window_master_.ptr, context_master_.ctx);

        auto window = std::make_shared<Window>(gl_, title, w, h);
        windows_.insert({window->GetWindowID(), window});
        return window;
    }

    bool Loop() {
        for (auto &[id, window] : windows_) {
            window->Render();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    return false;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        return false;
                    }
                    if (windows_.find(event.key.windowID) != windows_.end()) {
                        windows_[event.key.windowID]->CallbackKeyDown(
                            event.key);
                    }
                    break;
                default:
                    break;
            }
        }
        return true;
    }

    std::shared_ptr<RenderBuffer> CreateRenderBuffer() {
        return std::make_shared<RenderBuffer>(gl_);
    }

   private:
    GladGLContext gl_{};
    SDLWindowPtr window_master_{nullptr};
    SDLGLContextPtr context_master_{nullptr};
    std::map<Uint32, std::shared_ptr<Window>> windows_;
};

}  // namespace glviskit::sdl