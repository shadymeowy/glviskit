#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "camera.hpp"
#include "gl/gl.hpp"
#include "primitive/circle.hpp"
#include "primitive/line.hpp"
#include "primitive/point.hpp"
#include "render_list.hpp"

namespace glviskit {

class WindowRenderer {
   public:
    WindowRenderer() : camera{std::make_shared<Camera>()} {}

    void Render(GLuint ctx_id, int _width, int _height) {
        // if gl context not initialized, do it now
        if (!initialized_) {
            InitializeContext();
        }

        const auto width = static_cast<float>(_width);
        const auto height = static_cast<float>(_height);

        // update camera viewport size
        camera->SetViewportSize({width, height});

        // get camera transform matrix
        auto mvp = camera->CalculateTransform();

        // set viewport
        glViewport(0, 0, _width, _height);

        // opaque objects first
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw everything with alpha test 1
        RenderLists(ctx_id, width, height, mvp, 1);

        // render transparent objects
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        // draw everything with alpha test 0
        RenderLists(ctx_id, width, height, mvp, 0);
    }

    void AddRenderList(const std::shared_ptr<RenderList> &render_list) {
        buffers.push_back(render_list);
    }

    auto GetCamera() -> std::shared_ptr<Camera> { return camera; }
    void SetCamera(std::shared_ptr<Camera> cam) { camera = std::move(cam); }

   private:
    void InitializeContext() {
        program_line = std::make_unique<line::Program>();
        program_point = std::make_unique<point::Program>();
        program_circle = std::make_unique<circle::Program>();

        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glDisable(GL_CULL_FACE);
#ifdef GLVISKIT_GL33
        glEnable(GL_MULTISAMPLE);
#endif
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LESS);

        initialized_ = true;
    }

    void RenderLists(GLuint ctx_id, float width, float height,
                       const glm::mat4 &mvp, int alpha_test) {
        // Render all line buffers
        program_line->Use();
        program_line->SetScreenSize({width, height});
        program_line->SetMVP(mvp);
        program_line->SetAlphaTest(alpha_test);
        for (auto &line_buf : buffers) {
            line_buf->line_buffer.Render(ctx_id);
        }

        // Render all point buffers
        program_point->Use();
        program_point->SetScreenSize({width, height});
        program_point->SetMVP(mvp);
        program_point->SetAlphaTest(alpha_test);
        for (auto &point_buf : buffers) {
            point_buf->point_buffer.Render(ctx_id);
        }

        // Render all circle buffers
        program_circle->Use();
        program_circle->SetScreenSize({width, height});
        program_circle->SetMVP(mvp);
        program_circle->SetAlphaTest(alpha_test);
        for (auto &circle_buf : buffers) {
            circle_buf->circle_buffer.Render(ctx_id);
        }
    }

    // TODO: share programs across multiple renderers?
    std::unique_ptr<line::Program> program_line{nullptr};
    std::unique_ptr<point::Program> program_point{nullptr};
    std::unique_ptr<circle::Program> program_circle{nullptr};

    // make camera shareable across windows
    std::shared_ptr<Camera> camera;
    bool initialized_{false};

    std::vector<std::shared_ptr<RenderList>> buffers;
};

}  // namespace glviskit