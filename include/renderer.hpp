#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "anchor.hpp"
#include "buffer_stack.hpp"
#include "instance.hpp"
#include "line.hpp"
#include "point.hpp"

// Renderer class manages multiple primitive buffer instances
// and handles their rendering in efficient order.
// Different than Buffer classes, it also holds programs themselves.
// Also it keeps an instance buffer shared among all primitive buffers.

class RenderBuffer {
   public:
    RenderBuffer(GladGLContext &gl)
        : gl{gl},
          vbo_inst{gl},
          line_buffer{gl, vbo_inst},
          point_buffer{gl, vbo_inst},
          anchor_buffer{gl, vbo_inst} {
        // create identity instance
        RenderInstance(glm::mat4{1.0f});
    }

    RenderBuffer(const RenderBuffer &) = delete;
    RenderBuffer &operator=(const RenderBuffer &) = delete;

    void Line(glm::vec3 start, glm::vec3 end) {
        // if there is an ongoing line, end it first
        // otherwise, this is noop
        LineEnd();
        LineTo(start);
        LineTo(end);
        LineEnd();
    }

    void Point(glm::vec3 position) {
        size_t index = point_buffer.vbo.Size();
        point_buffer.vbo.Append({position, color, size});
        point_buffer.ebo.Append(index);
    }

    // Efficient way to draw connected lines
    void LineTo(glm::vec3 position) {
        size_t base_index = line_buffer.vbo.Size();

        if (line_counter == 0) {
            // for first point just store and return
            line_prev = position;
            color_prev = color;
            size_prev = size;
            line_counter++;
            return;
        } else if (line_counter == 1) {
            // for second point append initial vertices with direction
            auto direction = position - line_prev;
            line_buffer.vbo.Append(
                {line_prev, direction, color_prev, size_prev});
            line_buffer.vbo.Append(
                {line_prev, -direction, color_prev, size_prev});

            // nothing add to ebo yet

            // update previous points
            line_prev_prev = line_prev;
            line_prev = position;
            color_prev = color;
            size_prev = size;
            line_counter++;
            return;
        } else {
            // for subsequent points, we will calculate the bisector direction
            auto dir1 = glm::normalize(line_prev - line_prev_prev);
            auto dir2 = glm::normalize(position - line_prev);
            auto bisector = glm::normalize(dir1 + dir2);

            // append two vertices at the previous point with bisector direction
            line_buffer.vbo.Append(
                {line_prev, bisector, color_prev, size_prev});
            line_buffer.vbo.Append(
                {line_prev, -bisector, color_prev, size_prev});

            // add two triangles to connect previous segment
            line_buffer.ebo.Append(base_index - 2);
            line_buffer.ebo.Append(base_index + 0);
            line_buffer.ebo.Append(base_index - 1);
            line_buffer.ebo.Append(base_index - 1);
            line_buffer.ebo.Append(base_index + 0);
            line_buffer.ebo.Append(base_index + 1);

            // update previous points
            line_prev_prev = line_prev;
            line_prev = position;
            color_prev = color;
            size_prev = size;
            line_counter++;
        }
    }

    void LineEnd() {
        if (line_counter >= 2) {
            // if we have at least two points, finish the last segment
            size_t base_index = line_buffer.vbo.Size();
            auto direction = line_prev - line_prev_prev;
            line_buffer.vbo.Append(
                {line_prev, direction, color_prev, size_prev});
            line_buffer.vbo.Append(
                {line_prev, -direction, color_prev, size_prev});

            line_buffer.ebo.Append(base_index - 2);
            line_buffer.ebo.Append(base_index + 0);
            line_buffer.ebo.Append(base_index - 1);
            line_buffer.ebo.Append(base_index - 1);
            line_buffer.ebo.Append(base_index + 0);
            line_buffer.ebo.Append(base_index + 1);
        }

        // reset line drawing state
        line_counter = 0;
        line_prev_prev = glm::vec3{0.0f};
        line_prev = glm::vec3{0.0f};
    }

    void AnchoredSquare(glm::vec3 anchor) {
        size_t index = anchor_buffer.vbo.Size();
        auto s = size * 0.5f;
        // four vertices
        anchor_buffer.vbo.Append({anchor, {-s, -s, 0}, color});
        anchor_buffer.vbo.Append({anchor, {s, -s, 0}, color});
        anchor_buffer.vbo.Append({anchor, {s, s, 0}, color});
        anchor_buffer.vbo.Append({anchor, {-s, s, 0}, color});
        // two triangles
        anchor_buffer.ebo.Append(index + 0);
        anchor_buffer.ebo.Append(index + 1);
        anchor_buffer.ebo.Append(index + 2);
        anchor_buffer.ebo.Append(index + 2);
        anchor_buffer.ebo.Append(index + 3);
        anchor_buffer.ebo.Append(index + 0);
    }

    // attributes for subsequent drawing
    void Color(const glm::vec4 &c) { color = c; }
    void Size(float s) { size = s; }

    // instancing
    void RenderInstance(const glm::mat4 &transform) {
        vbo_inst.Append({transform});
    }

    // save and restore buffers
    void Save() {
        line_buffer.Save();
        point_buffer.Save();
        anchor_buffer.Save();
    }

    void Restore() {
        line_buffer.Restore();
        point_buffer.Restore();
        anchor_buffer.Restore();
    }

    void Clear() {
        line_buffer.Clear();
        point_buffer.Clear();
        anchor_buffer.Clear();
    }

    void SaveInstances() { vbo_inst.Save(); }

    void RestoreInstances() { vbo_inst.Restore(); }

    void ClearInstances() { vbo_inst.Clear(); }

   private:
    GladGLContext &gl;

    // instance transform buffer
    InstanceBuffer vbo_inst;

    // buffers to render
    LineBuffer line_buffer;
    PointBuffer point_buffer;
    AnchorBuffer anchor_buffer;

    // attributes for rendering
    glm::vec4 color{1.0f};
    float size = 1.0f;

    // line drawing state
    size_t line_counter = 0;
    glm::vec3 line_prev_prev{0.0f};
    glm::vec3 line_prev{0.0f};
    glm::vec4 color_prev{1.0f};
    float size_prev = 1.0f;

    friend class Renderer;
};

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

    void Render() {
        // Render all line buffers
        program_line.Use();
        for (auto &line_buf : buffers) {
            line_buf->line_buffer.Render();
        }

        // Render all point buffers
        program_point.Use();
        for (auto &point_buf : buffers) {
            point_buf->point_buffer.Render();
        }

        // Render all anchor buffers
        program_anchor.Use();
        for (auto &anchor_buf : buffers) {
            anchor_buf->anchor_buffer.Render();
        }
    }

    void SetMVP(const glm::mat4 &mvp) {
        program_line.SetMVP(mvp);
        program_point.SetMVP(mvp);
        program_anchor.SetMVP(mvp);
    }

    void SetScreenSize(const glm::vec2 &screen_size) {
        program_line.SetScreenSize(screen_size);
        program_point.SetScreenSize(screen_size);
        program_anchor.SetScreenSize(screen_size);
    }

   private:
    GladGLContext &gl;

    LineProgram program_line;
    PointProgram program_point;
    AnchorProgram program_anchor;

    std::vector<std::shared_ptr<RenderBuffer>> buffers;
};
