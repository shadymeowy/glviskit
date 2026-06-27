#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "../gl/gl.hpp"
#include "sdl.hpp"

namespace glviskit::sdl {

class WindowUiRenderer {
   public:
    WindowUiRenderer() = default;

    ~WindowUiRenderer() {
        if (ctx_ == nullptr) {
            return;
        }

        // tear down imgui while the gl context is still alive
        SDL_GL_MakeCurrent(window_, context_);
        ImGui::SetCurrentContext(ctx_);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(ctx_);
        ctx_ = nullptr;
    }

    WindowUiRenderer(const WindowUiRenderer &) = delete;
    auto operator=(const WindowUiRenderer &) -> WindowUiRenderer & = delete;
    WindowUiRenderer(WindowUiRenderer &&) = delete;
    auto operator=(WindowUiRenderer &&) -> WindowUiRenderer & = delete;

    // begin a new frame, creating the context on first use
    void NewFrame(SDL_Window *window, SDL_GLContext context) {
        if (ctx_ == nullptr) {
            Initialize(window, context);
        }
        ImGui::SetCurrentContext(ctx_);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        frame_active_ = true;
    }

    // finalize the frame and draw it
    void Render() {
        if (ctx_ == nullptr || !frame_active_) {
            return;
        }
        ImGui::SetCurrentContext(ctx_);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        frame_active_ = false;
    }

    void ProcessEvent(const SDL_Event &event) {
        if (ctx_ == nullptr) {
            return;
        }
        ImGui::SetCurrentContext(ctx_);
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    auto WantCaptureMouse() -> bool {
        if (ctx_ == nullptr) {
            return false;
        }
        ImGui::SetCurrentContext(ctx_);
        return ImGui::GetIO().WantCaptureMouse;
    }

    auto WantCaptureKeyboard() -> bool {
        if (ctx_ == nullptr) {
            return false;
        }
        ImGui::SetCurrentContext(ctx_);
        return ImGui::GetIO().WantCaptureKeyboard;
    }

    // widgets
    auto Begin(const char *title) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::Begin(title);
    }

    void End() {
        ImGui::SetCurrentContext(ctx_);
        ImGui::End();
    }

    void Text(const char *text) {
        ImGui::SetCurrentContext(ctx_);
        ImGui::TextUnformatted(text);
    }

    void Separator() {
        ImGui::SetCurrentContext(ctx_);
        ImGui::Separator();
    }

    auto Button(const char *label) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::Button(label);
    }

    auto Checkbox(const char *label, bool &value) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::Checkbox(label, &value);
    }

    auto SliderFloat(const char *label, float &value, float min, float max)
        -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::SliderFloat(label, &value, min, max);
    }

    auto SliderInt(const char *label, int &value, int min, int max) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::SliderInt(label, &value, min, max);
    }

    auto DragFloat(const char *label, float &value, float speed, float min,
                   float max) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::DragFloat(label, &value, speed, min, max);
    }

    auto ColorEdit3(const char *label, float *rgb) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::ColorEdit3(label, rgb);
    }

    auto ColorEdit4(const char *label, float *rgba) -> bool {
        ImGui::SetCurrentContext(ctx_);
        return ImGui::ColorEdit4(label, rgba);
    }

   private:
    void Initialize(SDL_Window *window, SDL_GLContext context) {
        window_ = window;
        context_ = context;

        IMGUI_CHECKVERSION();
        ctx_ = ImGui::CreateContext();
        ImGui::SetCurrentContext(ctx_);
        ImGui::StyleColorsDark();
        ImGui_ImplSDL3_InitForOpenGL(window, context);
#if defined(GLVISKIT_GLES3)
        ImGui_ImplOpenGL3_Init("#version 300 es");
#else
        ImGui_ImplOpenGL3_Init("#version 330");
#endif
    }

    ImGuiContext *ctx_{nullptr};
    bool frame_active_{false};
    SDL_Window *window_{nullptr};
    SDL_GLContext context_{nullptr};
};

}  // namespace glviskit::sdl
