#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>

#include "../controller/base_controller.hpp"
#include "../controller/spherical_controller.hpp"
#include "../gl/gl.hpp"
#include "../window_renderer.hpp"
#include "sdl.hpp"

namespace glviskit::sdl {

class Window {
   public:
    Window(const char *title, int w, int h, bool share_context)
        : window_{nullptr},
          context_{nullptr},
          camera_controller_(std::make_shared<SphericalController>()),
          time_prev_(static_cast<float>(SDL_GetTicks()) / 1000.0F) {
        // set GL attributes
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT,
                            share_context ? 1 : 0);

        auto *handle =
            SDL_CreateWindow(title, w, h,
                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                 SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (handle == nullptr) {
            throw std::runtime_error(
                std::string{"Failed to create SDL window: "} + SDL_GetError());
        }

        window_ = SDLWindowPtr(handle);
        window_id_ = SDL_GetWindowID(window_.Get());

        context_ = SDLGLContextPtr(SDL_GL_CreateContext(window_.Get()));
        if (context_.Get() == nullptr) {
            throw std::runtime_error(
                std::string{"Failed to create SDL GL context: "} +
                SDL_GetError());
        }

        MakeCurrent();
    }

    void AddRenderList(const std::shared_ptr<RenderList> &render_list) {
        window_renderer_.AddRenderList(render_list);
    }

    auto GetCamera() -> std::shared_ptr<Camera> {
        return window_renderer_.GetCamera();
    }

    void SetCamera(const std::shared_ptr<Camera> &cam) {
        window_renderer_.SetCamera(cam);
    }

    void SetBackgroundColor(const glm::vec4 &color) {
        window_renderer_.SetBackgroundColor(color);
    }

    [[nodiscard]] auto GetBackgroundColor() const -> glm::vec4 {
        return window_renderer_.GetBackgroundColor();
    }

    [[nodiscard]] auto GetSizeInPixels() const -> std::array<int, 2> {
        int width;
        int height;
        SDL_GetWindowSizeInPixels(window_.Get(), &width, &height);
        return {width, height};
    }

    void CaptureRGBA(std::span<unsigned char> pixels) {
        MakeCurrent();
        auto [width, height] = GetSizeInPixels();
        window_renderer_.CaptureRGBA(width, height, pixels);
    }

    auto GetController() -> std::shared_ptr<BaseController> {
        return camera_controller_;
    }

    void SetController(const std::shared_ptr<BaseController> &controller) {
        camera_controller_ = controller;
    }

    void MakeCurrent() { SDL_GL_MakeCurrent(window_.Get(), context_.Get()); }

    void Render() {
        // update camera controller
        float time_now = static_cast<float>(SDL_GetTicks()) / 1000.0F;
        float delta_time = time_now - time_prev_;
        time_prev_ = time_now;
        camera_controller_->Update(delta_time, *GetCamera());

        // make context current
        // renderer expects the context to be current
        bool ret = SDL_GL_MakeCurrent(window_.Get(), context_.Get());
        if (!ret) {
            throw std::runtime_error(
                std::string{"Failed to make context current: "} +
                SDL_GetError());
        }

        // update screen size
        int width;
        int height;
        SDL_GetWindowSizeInPixels(window_.Get(), &width, &height);

        // do rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window_renderer_.Render(window_id_, width, height);

        // swap buffers
        SDL_GL_SwapWindow(window_.Get());

        GLenum error2 = glGetError();
        if (error2 != GL_NO_ERROR) {
            throw std::runtime_error("OpenGL error in window 2: " +
                                     std::to_string(error2));
        }
    }

    void CallbackKey(const SDL_KeyboardEvent &event) const {
        camera_controller_->KeyEvent(event.key, event.down);
    }

    void CallbackButton(const SDL_MouseButtonEvent &event) const {
        camera_controller_->ButtonEvent(event.button, event.down);
    }

    void CallbackMotion(const SDL_MouseMotionEvent &event) const {
        camera_controller_->MouseMotionEvent(event.xrel, event.yrel);
    }

    void CallbackWheel(const SDL_MouseWheelEvent &event) const {
        camera_controller_->MouseWheelEvent(event.y);
    }

    [[nodiscard]] auto GetWindowID() const -> Uint32 { return window_id_; }

   private:
    SDLWindowPtr window_;
    SDLGLContextPtr context_;

    std::shared_ptr<BaseController> camera_controller_;
    WindowRenderer window_renderer_;
    GLuint window_id_;

    float time_prev_;

    friend class Manager;
};

}  // namespace glviskit::sdl