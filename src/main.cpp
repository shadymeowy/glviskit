#include <SDL2/SDL.h>
#include <glad/gl.h>

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "renderer.hpp"

float randFloat() { return static_cast<float>(rand()) / RAND_MAX; }

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

    Camera &GetCamera() { return renderer.GetCamera(); }

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

   private:
    GladGLContext &gl;

    SDLWindowPtr window_;
    SDLGLContextPtr context_;

    Renderer renderer;
    GLuint window_id_;
};

int main() {
    GladGLContext _gl;
    GladGLContext &gl = _gl;

    // init SDL and create hidden window for context
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_Window *window_master = SDL_CreateWindow(
        "Hidden", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    auto context_master = SDL_GL_CreateContext(window_master);
    gladLoadGLContext(&gl, (GLADloadfunc)SDL_GL_GetProcAddress);

    // create a window 1
    SDL_GL_MakeCurrent(window_master, context_master);
    SDLWindow window1(gl, "Window1", 800, 600);

    // create a window 2
    SDL_GL_MakeCurrent(window_master, context_master);
    SDLWindow window2(gl, "Window2", 800, 600);

    auto render_buffer = std::make_shared<RenderBuffer>(gl);
    window1.AddRenderBuffer(render_buffer);
    window2.AddRenderBuffer(render_buffer);

    render_buffer->ClearInstances();
    for (int i = 1; i < 5; i++) {
        int s = (i % 2 == 0) ? 1 : -1;
        render_buffer->AddInstance(
            glm::rotate(glm::mat4(1.0f), 0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(3.0f * (i - 0.5), 0, 0)));
        render_buffer->AddInstance(
            glm::rotate(glm::mat4(1.0f), -0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(-3.0f * (i - 0.5), 0, 0)));
    }
    auto render_buffer_sine = std::make_shared<RenderBuffer>(gl);
    window1.AddRenderBuffer(render_buffer_sine);
    window2.AddRenderBuffer(render_buffer_sine);

    auto render_buffer_axes = std::make_shared<RenderBuffer>(gl);
    window1.AddRenderBuffer(render_buffer_axes);
    window2.AddRenderBuffer(render_buffer_axes);

    render_buffer_axes->Size(5.0f);
    render_buffer_axes->Color({1.0f, 0.0f, 0.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    render_buffer_axes->Color({0.0f, 1.0f, 0.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    render_buffer_axes->Color({0.0f, 0.0f, 1.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});

    render_buffer->Color({1.0f, 1.0f, 1.0f, 1.0f});
    render_buffer->Size(3.0f);
    for (int i = 0; i < 10; i++) {
        render_buffer->Point({randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f});
    }

    auto &camera = window1.GetCamera();
    camera.SetPerspectiveFov(glm::radians(60.0f), glm::radians(60.0f));
    camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.SetRotation(glm::vec3(0.0, 0.0f, 0.0f));
    camera.SetPreserveAspectRatio(true);
    camera.SetDistance(15.0f);

    auto &camera2 = window2.GetCamera();
    camera2.SetPerspectiveFov(glm::radians(60.0f), glm::radians(60.0f));
    camera2.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera2.SetRotation(glm::vec3(0.0, 0.0f, 0.0f));
    camera2.SetPreserveAspectRatio(true);
    camera2.SetDistance(15.0f);

    float angle = 0.0f;
    int fps_counter = 0;
    int frame_index = 0;

    bool running = true;
    while (running) {
        float curr_time = SDL_GetTicks() / 1000.0f;

        frame_index++;
        angle += 0.005f;
        camera.SetRotation({-0.5f, angle, 0.0f});
        camera2.SetRotation({-0.5f, -angle, 0.0f});

        for (int i = 0; i < 10; i++) {
            render_buffer->Size(randFloat() * 1.0f + 1.0f);
            render_buffer->Color(
                {randFloat(), randFloat(), randFloat(), randFloat()});
            render_buffer->Point({randFloat() * 2.0f - 1.0f,
                                  randFloat() * 2.0f - 1.0f,
                                  randFloat() * 2.0f - 1.0f});
        }

        if (frame_index % 10 == 0) {
            render_buffer->Color(
                {randFloat(), randFloat(), randFloat(), randFloat()});
            render_buffer->Size(randFloat() * 4.0f);
            render_buffer->Line(
                {randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f,
                 randFloat() * 2.0f - 1.0f},
                {randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f,
                 randFloat() * 2.0f - 1.0f});
        }

        render_buffer_sine->Restore();
        render_buffer_sine->Color({1.0f, 0.0f, 0.0f, 1.0f});
        render_buffer_sine->Size(4.0f);
        for (float x = -1.0f; x <= 1.0f; x += 0.001f) {
            float y = sinf(50.0f * x + 10 * static_cast<float>(curr_time));
            float z = cosf(50.0f * x + 10 * static_cast<float>(curr_time));

            render_buffer_sine->Color(
                {x * 0.5f + 0.5f, y * 0.5f + 0.5f, 0.5f, 1.0f});
            render_buffer_sine->LineTo({20.0f * x, 1.5 * y, 1.5 * z});
        }
        render_buffer_sine->LineEnd();

        window1.Render();
        window2.Render();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    std::cout
                        << "Key down: " << SDL_GetKeyName(event.key.keysym.sym)
                        << "Window ID: " << event.key.windowID << std::endl;
                    break;
                default:
                    break;
            }
        }
    }

    SDL_Quit();
    return 0;
}