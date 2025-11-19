#pragma once
#include <SDL2/SDL.h>
#include <glad/gl.h>

#include <iostream>
#include <map>
#include <memory>

#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"

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
            SDL_GL_DeleteContext(ctx);
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
                SDL_GL_DeleteContext(ctx);
            }
            ctx = other.ctx;
            other.ctx = nullptr;
        }
        return *this;
    }

    SDL_GLContext ctx;
};

class SDLWindow {
   public:
    SDLWindow(GladGLContext &gl, const char *title, int w, int h)
        : gl{gl}, renderer(gl), window_{nullptr}, context_{nullptr} {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

        window_ = SDLWindowPtr(SDL_CreateWindow(
            title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));

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
        SDL_GL_MakeCurrent(window_.ptr, context_.ctx);

        // update screen size
        int width, height;
        SDL_GetWindowSize(window_.ptr, &width, &height);

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

    Uint32 GetWindowID() const { return window_id_; }

   private:
    GladGLContext &gl;

    SDLWindowPtr window_;
    SDLGLContextPtr context_;

    Renderer renderer;
    GLuint window_id_;
};

class SDLManager {
   public:
    SDLManager() {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        window_master_ = SDL_CreateWindow(
            "Hidden", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
            SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

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
        if (ret == 0) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    ~SDLManager() {
        windows_.clear();

        SDL_Quit();
    }

    std::shared_ptr<SDLWindow> CreateWindow(const char *title, int w, int h) {
        // make master context current for context sharing
        SDL_GL_MakeCurrent(window_master_.ptr, context_master_.ctx);

        auto window = std::make_shared<SDLWindow>(gl_, title, w, h);
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
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        return false;
                    }
                    std::cout
                        << "Key down: " << SDL_GetKeyName(event.key.keysym.sym)
                        << "Window ID: " << event.key.windowID << std::endl;
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
    std::map<Uint32, std::shared_ptr<SDLWindow>> windows_;
};