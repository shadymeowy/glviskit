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

#include <iostream>
#include <map>
#include <memory>
#include <utility>

#include "camera.hpp"
#include "gl/gl.hpp"
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
#elif defined(GLVISKIT_GLES2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
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