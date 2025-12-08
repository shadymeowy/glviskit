#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "gl/buffer_stack.hpp"
#include "gl/instance.hpp"
#include "primitive/anchor.hpp"
#include "primitive/line.hpp"
#include "primitive/point.hpp"

namespace glviskit {

class RenderBuffer {
   public:
    RenderBuffer()
        : line_buffer{vbo_inst},
          point_buffer{vbo_inst},
          anchor_buffer{vbo_inst} {
        // create identity instance
        AddInstance(glm::mat4{1.0F});
    }

    void Line(glm::vec3 start, glm::vec3 end) {
        // if there is an ongoing line, end it first
        // otherwise, this is noop
        LineEnd();
        LineTo(start);
        LineTo(end);
        LineEnd();
    }

    void Point(glm::vec3 position) {
        auto &vbo = point_buffer.VBO();
        auto &ebo = point_buffer.EBO();

        size_t index = vbo.Size();
        vbo.Append({.position = position, .color = color, .size = size});
        ebo.Append(index);
    }

    // Efficient way to draw connected lines
    void LineTo(glm::vec3 position) {
        auto &vbo = line_buffer.VBO();
        auto &ebo = line_buffer.EBO();

        size_t base_index = vbo.Size();

        if (line_counter == 0) {
            // for first point just store and return
            line_prev = position;
            color_prev = color;
            size_prev = size;
            line_counter++;
            return;
        }
        if (line_counter == 1) {
            // for second point append initial vertices with direction
            auto direction = position - line_prev;
            vbo.Append({.position = line_prev,
                        .velocity = direction,
                        .color = color_prev,
                        .size = size_prev});
            vbo.Append({.position = line_prev,
                        .velocity = -direction,
                        .color = color_prev,
                        .size = size_prev});

            // nothing add to ebo yet

            // update previous points
            line_prev_prev = line_prev;
            line_prev = position;
            color_prev = color;
            size_prev = size;
            line_counter++;
            return;
        }

        // for subsequent points, we will calculate the bisector direction
        auto dir1 = glm::normalize(line_prev - line_prev_prev);
        auto dir2 = glm::normalize(position - line_prev);
        auto bisector = glm::normalize(dir1 + dir2);

        // append two vertices at the previous point with bisector direction
        vbo.Append({.position = line_prev,
                    .velocity = bisector,
                    .color = color_prev,
                    .size = size_prev});
        vbo.Append({.position = line_prev,
                    .velocity = -bisector,
                    .color = color_prev,
                    .size = size_prev});

        // add two triangles to connect previous segment
        ebo.Append(base_index - 2);
        ebo.Append(base_index + 0);
        ebo.Append(base_index - 1);
        ebo.Append(base_index - 1);
        ebo.Append(base_index + 0);
        ebo.Append(base_index + 1);

        // update previous points
        line_prev_prev = line_prev;
        line_prev = position;
        color_prev = color;
        size_prev = size;
        line_counter++;
    }

    void LineEnd() {
        if (line_counter >= 2) {
            auto &vbo = line_buffer.VBO();
            auto &ebo = line_buffer.EBO();

            // if we have at least two points, finish the last segment
            size_t base_index = vbo.Size();
            auto direction = line_prev - line_prev_prev;
            vbo.Append({.position = line_prev,
                        .velocity = direction,
                        .color = color_prev,
                        .size = size_prev});
            vbo.Append({.position = line_prev,
                        .velocity = -direction,
                        .color = color_prev,
                        .size = size_prev});

            ebo.Append(base_index - 2);
            ebo.Append(base_index + 0);
            ebo.Append(base_index - 1);
            ebo.Append(base_index - 1);
            ebo.Append(base_index + 0);
            ebo.Append(base_index + 1);
        }

        // reset line drawing state
        line_counter = 0;
        line_prev_prev = glm::vec3{0.0F};
        line_prev = glm::vec3{0.0F};
    }

    void AnchoredSquare(glm::vec3 anchor) {
        auto &vbo = anchor_buffer.VBO();
        auto &ebo = anchor_buffer.EBO();
        size_t index = vbo.Size();
        auto s = size * 0.5F;
        // four vertices
        vbo.Append({.anchor = anchor, .position = {-s, -s, 0}, .color = color});
        vbo.Append({.anchor = anchor, .position = {s, -s, 0}, .color = color});
        vbo.Append({.anchor = anchor, .position = {s, s, 0}, .color = color});
        vbo.Append({.anchor = anchor, .position = {-s, s, 0}, .color = color});
        // two triangles
        ebo.Append(index + 0);
        ebo.Append(index + 1);
        ebo.Append(index + 2);
        ebo.Append(index + 2);
        ebo.Append(index + 3);
        ebo.Append(index + 0);
    }

    // attributes for subsequent drawing
    void Color(const glm::vec4 &c) { color = c; }
    void Size(float s) { size = s; }

    // instancing
    void AddInstance(const glm::mat4 &transform) {
        vbo_inst.Append({transform});
    }

    void AddInstance(const glm::vec3 &position,
                     const glm::vec3 &rotation = glm::vec3{0.0F},
                     const glm::vec3 &scale = glm::vec3{1.0F}) {
        // translation
        auto t = glm::translate(glm::mat4{1.0F}, position);

        // rotation
        auto angle = glm::length(rotation);
        glm::mat4 r{1.0F};
        if (angle > 1e-6F) {
            auto axis = rotation / angle;
            r = glm::rotate(glm::mat4{1.0F}, angle, axis);
        }

        // scale
        auto s = glm::scale(glm::mat4{1.0F}, scale);
        AddInstance(t * r * s);
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
    // instance transform buffer
    InstanceBuffer vbo_inst;

    // buffers to render
    line::Buffer line_buffer;
    point::Buffer point_buffer;
    anchor::Buffer anchor_buffer;

    // attributes for rendering
    glm::vec4 color{1.0F};
    float size = 1.0F;

    // line drawing state
    size_t line_counter = 0;
    glm::vec3 line_prev_prev{0.0F};
    glm::vec3 line_prev{0.0F};
    glm::vec4 color_prev{1.0F};
    float size_prev = 1.0F;

    friend class Renderer;
};

}  // namespace glviskit