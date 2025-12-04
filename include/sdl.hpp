#pragma once
#define SDL_MAIN_HANDLED
#define NOMINMAX
#include <SDL3/SDL.h>
#ifdef CreateWindow
#undef CreateWindow
#endif

#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "camera.hpp"
#include "gl/glad.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"

namespace glviskit::sdl {

class SDLWindowPtr {
   public:
    explicit SDLWindowPtr(SDL_Window *handle) : ptr{handle} {}
    ~SDLWindowPtr() {
        if (ptr != nullptr) {
            SDL_DestroyWindow(ptr);
            ptr = nullptr;
        }
    }
    // uncopyable but movable
    SDLWindowPtr(const SDLWindowPtr &) = delete;
    auto operator=(const SDLWindowPtr &) -> SDLWindowPtr & = delete;
    SDLWindowPtr(SDLWindowPtr &&other) noexcept : ptr{other.ptr} {
        other.ptr = nullptr;
    }
    auto operator=(SDLWindowPtr &&other) noexcept -> SDLWindowPtr & {
        if (this != &other) {
            if (ptr != nullptr) {
                SDL_DestroyWindow(ptr);
            }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> SDL_Window * { return ptr; }

   private:
    SDL_Window *ptr;
};

class SDLGLContextPtr {
   public:
    explicit SDLGLContextPtr(SDL_GLContext handle) : ctx{handle} {}
    ~SDLGLContextPtr() {
        if (ctx != nullptr) {
            SDL_GL_DestroyContext(ctx);
            ctx = nullptr;
        }
    }
    // uncopyable but movable
    SDLGLContextPtr(const SDLGLContextPtr &) = delete;
    auto operator=(const SDLGLContextPtr &) -> SDLGLContextPtr & = delete;
    SDLGLContextPtr(SDLGLContextPtr &&other) noexcept : ctx{other.ctx} {
        other.ctx = nullptr;
    }
    auto operator=(SDLGLContextPtr &&other) noexcept -> SDLGLContextPtr & {
        if (this != &other) {
            if (ctx != nullptr) {
                SDL_GL_DestroyContext(ctx);
            }
            ctx = other.ctx;
            other.ctx = nullptr;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> SDL_GLContext { return ctx; }

   private:
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

        auto *handle = SDL_CreateWindow(
            title, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.Render(window_id_, width, height);

        // swap buffers
        SDL_GL_SwapWindow(window_.Get());

        GLenum error2 = gl.GetError();
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
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
            exit(EXIT_FAILURE);
        }
        if (!SDL_GL_LoadLibrary(nullptr)) {
            std::cerr << "Failed to load SDL GL library: " << SDL_GetError()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        window_master_ = SDLWindowPtr(SDL_CreateWindow(
            "Hidden", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN));
        if (window_master_.Get() == nullptr) {
            std::cerr << "Failed to create SDL master window: "
                      << SDL_GetError() << '\n';
            exit(EXIT_FAILURE);
        }

        context_master_ =
            SDLGLContextPtr(SDL_GL_CreateContext(window_master_.Get()));

        if (context_master_.Get() == nullptr) {
            std::cerr << "Failed to create SDL master GL context: "
                      << SDL_GetError() << '\n';
            exit(EXIT_FAILURE);
        }

        int ret = gladLoadGLContext(&gl_, (GLADloadfunc)SDL_GL_GetProcAddress);
        if (ret == 0) {
            std::cerr << "Failed to initialize GLAD" << '\n';
            exit(EXIT_FAILURE);
        }
    }

    // movable but not copyable
    Manager(const Manager &) = delete;
    auto operator=(const Manager &) -> Manager & = delete;
    Manager(Manager &&) = default;
    auto operator=(Manager &&) -> Manager & = default;

    ~Manager() {
        windows_.clear();

        SDL_Quit();
    }

    auto CreateWindow(const char *title, int w, int h)
        -> std::shared_ptr<Window> {
        // make master context current for context sharing
        SDL_GL_MakeCurrent(window_master_.Get(), context_master_.Get());

        auto window = std::make_shared<Window>(gl_, title, w, h);
        windows_.insert({window->GetWindowID(), window});
        return window;
    }

    auto Loop() -> bool {
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
                    if (windows_.contains(event.key.windowID)) {
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

    auto CreateRenderBuffer() -> std::shared_ptr<RenderBuffer> {
        return std::make_shared<RenderBuffer>(gl_);
    }

    static auto GetTimeSeconds() -> float {
        return static_cast<float>(SDL_GetTicks()) / 1000.0F;
    }

   private:
    GladGLContext gl_{};
    SDLWindowPtr window_master_{nullptr};
    SDLGLContextPtr context_master_{nullptr};
    std::map<Uint32, std::shared_ptr<Window>> windows_;
};

}  // namespace glviskit::sdl