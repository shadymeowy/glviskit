#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "anchor.hpp"
#include "camera.hpp"
#include "line.hpp"
#include "point.hpp"
#include "render_buffer.hpp"

class Renderer {
   public:
    Renderer(GladGLContext &gl)
        : gl{gl},
          program_line{gl},
          program_point{gl},
          program_anchor{gl},
          buffers{} {}

    void AddRenderBuffer(std::shared_ptr<RenderBuffer> render_buffer) {
        buffers.push_back(render_buffer);
    }

    std::shared_ptr<RenderBuffer> CreateRenderBuffer() {
        auto render_buffer = std::make_shared<RenderBuffer>(gl);
        AddRenderBuffer(render_buffer);
        return render_buffer;
    }

    void Render(GLuint ctx_id, const Camera &camera) {
        SetMVP(camera.GetTransformMatrix());

        // Render all line buffers
        program_line.Use();
        for (auto &line_buf : buffers) {
            line_buf->line_buffer.Render(ctx_id);
        }

        // Render all point buffers
        program_point.Use();
        for (auto &point_buf : buffers) {
            point_buf->point_buffer.Render(ctx_id);
        }

        // Render all anchor buffers
        program_anchor.Use();
        for (auto &anchor_buf : buffers) {
            anchor_buf->anchor_buffer.Render(ctx_id);
        }
    }

    void SetScreenSize(const glm::vec2 &screen_size) {
        program_line.SetScreenSize(screen_size);
        program_point.SetScreenSize(screen_size);
        program_anchor.SetScreenSize(screen_size);
    }

   private:
    void SetMVP(const glm::mat4 &mvp) {
        program_line.SetMVP(mvp);
        program_point.SetMVP(mvp);
        program_anchor.SetMVP(mvp);
    }

    GladGLContext &gl;

    LineProgram program_line;
    PointProgram program_point;
    AnchorProgram program_anchor;

    std::vector<std::shared_ptr<RenderBuffer>> buffers;
};
