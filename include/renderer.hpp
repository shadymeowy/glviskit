#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "camera.hpp"
#include "gl/gl.hpp"
#include "primitive/anchor.hpp"
#include "primitive/line.hpp"
#include "primitive/point.hpp"
#include "render_buffer.hpp"

namespace glviskit {

class Renderer {
   public:
    Renderer() : camera{std::make_shared<Camera>()} {}

    void Render(GLuint ctx_id, int _width, int _height) {
        // if gl context not initialized, do it now
        if (!initialized_) {
            InitializeContext();
        }

        const auto width = static_cast<float>(_width);
        const auto height = static_cast<float>(_height);

        // update camera viewport size
        camera->SetViewportSize({width, height});
        // set viewport
        glViewport(0, 0, _width, _height);
        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // get camera transform matrix
        auto mvp = camera->CalculateTransform();

        // Render all line buffers
        program_line->Use();
        program_line->SetScreenSize({width, height});
        program_line->SetMVP(mvp);
        for (auto &line_buf : buffers) {
            line_buf->line_buffer.Render(ctx_id);
        }

        // Render all point buffers
        program_point->Use();
        program_point->SetScreenSize({width, height});
        program_point->SetMVP(mvp);
        for (auto &point_buf : buffers) {
            point_buf->point_buffer.Render(ctx_id);
        }

        // Render all anchor buffers
        program_anchor->Use();
        program_anchor->SetScreenSize({width, height});
        program_anchor->SetMVP(mvp);
        for (auto &anchor_buf : buffers) {
            anchor_buf->anchor_buffer.Render(ctx_id);
        }
    }

    void AddRenderBuffer(const std::shared_ptr<RenderBuffer> &render_buffer) {
        buffers.push_back(render_buffer);
    }

    auto GetCamera() -> std::shared_ptr<Camera> { return camera; }
    void SetCamera(std::shared_ptr<Camera> cam) { camera = std::move(cam); }

   private:
    void InitializeContext() {
        program_line = std::make_unique<line::Program>();
        program_point = std::make_unique<point::Program>();
        program_anchor = std::make_unique<anchor::Program>();

        glClearColor(0.0F, 0.0F, 0.0F, 0.0F);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);

        initialized_ = true;
    }

    // TODO: share programs across multiple renderers?
    std::unique_ptr<line::Program> program_line{nullptr};
    std::unique_ptr<point::Program> program_point{nullptr};
    std::unique_ptr<anchor::Program> program_anchor{nullptr};

    // make camera shareable across windows
    std::shared_ptr<Camera> camera;
    bool initialized_{false};

    std::vector<std::shared_ptr<RenderBuffer>> buffers;
};

}  // namespace glviskit