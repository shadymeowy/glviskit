#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "line.hpp"
#include "point.hpp"

// Renderer class manages multiple LineBuffer and PointBuffer instances
// and handles their rendering in efficient order.
// Different than Buffer classes, it also holds programs themselves.

// RenderBuffer class holds references to LineBuffer and PointBuffer
// instances to be rendered together.
// It is purely for convenience and does not perform rendering itself.

class RenderBuffer {
   public:
    RenderBuffer(GladGLContext &gl, std::shared_ptr<LineBuffer> line_buffer,
                 std::shared_ptr<PointBuffer> point_buffer)
        : gl{gl}, line_buffer{line_buffer}, point_buffer{point_buffer} {
        // create identity instance
        RenderInstance(glm::mat4{1.0f});
    }

    void Line(glm::vec3 start, glm::vec3 end) {
        // if there is an ongoing line, end it first
        if (line_counter != 0) {
            LineEnd();
        }

        size_t base_index = line_buffer->vbo.Size();

        auto direction = end - start;
        line_buffer->vbo.Append({start, direction, color, size});
        line_buffer->vbo.Append({start, -direction, color, size});
        line_buffer->vbo.Append({end, direction, color, size});
        line_buffer->vbo.Append({end, -direction, color, size});

        line_buffer->ebo.Append(base_index + 1);
        line_buffer->ebo.Append(base_index + 0);
        line_buffer->ebo.Append(base_index + 2);
        line_buffer->ebo.Append(base_index + 1);
        line_buffer->ebo.Append(base_index + 2);
        line_buffer->ebo.Append(base_index + 3);
    }

    void Point(glm::vec3 position) {
        size_t index = point_buffer->vbo.Size();
        point_buffer->vbo.Append({position, color, size});
        point_buffer->ebo.Append(index);
    }

    // Efficient way to draw connected lines
    void LineTo(glm::vec3 position) {
        size_t base_index = line_buffer->vbo.Size();

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
            line_buffer->vbo.Append(
                {line_prev, direction, color_prev, size_prev});
            line_buffer->vbo.Append(
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
            line_buffer->vbo.Append(
                {line_prev, bisector, color_prev, size_prev});
            line_buffer->vbo.Append(
                {line_prev, -bisector, color_prev, size_prev});

            // add two triangles to connect previous segment
            line_buffer->ebo.Append(base_index - 2);
            line_buffer->ebo.Append(base_index + 0);
            line_buffer->ebo.Append(base_index - 1);
            line_buffer->ebo.Append(base_index - 1);
            line_buffer->ebo.Append(base_index + 0);
            line_buffer->ebo.Append(base_index + 1);

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
            size_t base_index = line_buffer->vbo.Size();
            auto direction = line_prev - line_prev_prev;
            line_buffer->vbo.Append(
                {line_prev, direction, color_prev, size_prev});
            line_buffer->vbo.Append(
                {line_prev, -direction, color_prev, size_prev});

            line_buffer->ebo.Append(base_index - 2);
            line_buffer->ebo.Append(base_index + 0);
            line_buffer->ebo.Append(base_index - 1);
            line_buffer->ebo.Append(base_index - 1);
            line_buffer->ebo.Append(base_index + 0);
            line_buffer->ebo.Append(base_index + 1);
        }

        // reset line drawing state
        line_counter = 0;
        line_prev_prev = glm::vec3{0.0f};
        line_prev = glm::vec3{0.0f};
    }

    // attributes for subsequent drawing
    void Color(const glm::vec4 &c) { color = c; }
    void Size(float s) { size = s; }

    // instancing
    void RenderInstance(const glm::mat4 &transform) {
        point_buffer->vbo_inst.Append({transform});
        line_buffer->vbo_inst.Append({transform});
    }

    // save and restore buffers
    void Save() {
        line_buffer->Save();
        point_buffer->Save();
    }

    void Restore() {
        line_buffer->Restore();
        point_buffer->Restore();
    }

    void Clear() {
        line_buffer->Clear();
        point_buffer->Clear();
    }

    void SaveInstances() {
        line_buffer->SaveInstances();
        point_buffer->SaveInstances();
    }

    void RestoreInstances() {
        line_buffer->RestoreInstances();
        point_buffer->RestoreInstances();
    }

    void ClearInstances() {
        line_buffer->ClearInstances();
        point_buffer->ClearInstances();
    }

   private:
    GladGLContext &gl;

    // buffers to render
    std::shared_ptr<LineBuffer> line_buffer;
    std::shared_ptr<PointBuffer> point_buffer;

    // attributes for rendering
    glm::vec4 color;
    float size;

    // line drawing state
    size_t line_counter;
    glm::vec3 line_prev_prev;
    glm::vec3 line_prev;
    glm::vec4 color_prev;
    float size_prev;
};

class Renderer {
   public:
    Renderer(GladGLContext &gl) : gl{gl}, program_line{gl}, program_point{gl} {}

    std::shared_ptr<LineBuffer> CreateLineBuffer() {
        auto line_buf = std::make_shared<LineBuffer>(gl);
        line_buffer.push_back(line_buf);
        return line_buf;
    }

    std::shared_ptr<PointBuffer> CreatePointBuffer() {
        auto point_buf = std::make_shared<PointBuffer>(gl);
        point_buffer.push_back(point_buf);
        return point_buf;
    }

    RenderBuffer CreateRenderBuffer() {
        return RenderBuffer{gl, CreateLineBuffer(), CreatePointBuffer()};
    }

    void Render() {
        // Render all line buffers
        program_line.Use();
        for (auto &line_buf : line_buffer) {
            line_buf->Render();
        }

        // Render all point buffers
        program_point.Use();
        for (auto &point_buf : point_buffer) {
            point_buf->Render();
        }
    }

    void SetMVP(const glm::mat4 &mvp) {
        program_line.SetMVP(mvp);
        program_point.SetMVP(mvp);
    }

    void SetScreenSize(const glm::vec2 &screen_size) {
        program_line.SetScreenSize(screen_size);
        program_point.SetScreenSize(screen_size);
    }

   private:
    GladGLContext &gl;

    LineProgram program_line;
    std::vector<std::shared_ptr<LineBuffer>> line_buffer;

    PointProgram program_point;
    std::vector<std::shared_ptr<PointBuffer>> point_buffer;
};
