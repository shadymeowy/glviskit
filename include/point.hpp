#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <glm/glm.hpp>

#include "buffer_stack.hpp"
#include "instance.hpp"
#include "program.hpp"
#include "vao.hpp"

// This file defines a PointBuffer class.
// While quite simple, it is the reference for defining reusable
// and generic components for rendering.

// First, we define the shader sources
inline constexpr char shader_vertex_point[] = R"glsl(
    #version 330 core
    
    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec4 a_color;
    layout(location = 2) in float a_size;
    layout(location = 3) in mat4 a_transform;
    out vec4 v_color;

    uniform mat4 mvp;
    uniform vec2 screen_size;
    
    void main() {
        gl_Position = mvp * a_transform * vec4(a_position, 1.0);
        v_color = a_color;
        gl_PointSize = a_size;
    }
)glsl";

inline constexpr char shader_fragment_point[] = R"glsl(
    #version 330 core
    in vec4 v_color;
    out vec4 f_color;
    
    void main() {
        f_color = v_color;
    }
)glsl";

// Define PointProgram which holds the shaders.
// In theory, multiple point programs with different shaders could be defined
// for same PointBuffer.
using PointProgram = Program<shader_vertex_point, shader_fragment_point>;

// PointBuffer is responsible for storing and rendering points
// with position, color, and size attributes.
// By itself, it only provides a render call and attributes setup for VAO,
// nothing more.

// Any VBO template class can be used to store its elements.
// We generally opt for StackVBO for dynamic data.
class PointBuffer {
   public:
    struct Element {
        glm::vec3 position;
        glm::vec4 color;
        float size;
    };

    PointBuffer(GladGLContext &gl, InstanceBuffer &vbo_inst)
        : gl{gl}, vbo{gl}, ebo{gl}, vao{gl}, vbo_inst{vbo_inst} {
        ConfigureVAO();
    }

    void Render() {
        // ensure VBO is synced before rendering
        // it is noop if already synced
        Sync();

        // if there is nothing to draw, return
        if (ebo.Size() == 0) {
            return;
        }

        // bind VAO using RAII binder,
        // which will unbind it at the end of the scope
        vao.Bind();
        // draw call
        gl.DrawElementsInstanced(GL_POINTS, ebo.Size(), GL_UNSIGNED_INT,
                                 nullptr, vbo_inst.Size());
        vao.Unbind();
    }

    bool Sync() {
        // sync all buffers
        bool re_vbo = vbo.Sync();
        bool re_ebo = ebo.Sync();
        bool re_vbo_inst = vbo_inst.Sync();
        bool reallocated = re_vbo || re_ebo || re_vbo_inst;

        // reconfigure VAO if VBO was reallocated
        if (reallocated) {
            ConfigureVAO();
        }

        return reallocated;
    }

    void Save() {
        vbo.Save();
        ebo.Save();
    }

    void Restore() {
        vbo.Restore();
        ebo.Restore();
    }

    void Clear() {
        vbo.Clear();
        ebo.Clear();
    }

    // In this case, we don't really need an EBO
    // but can be useful for other primitives.
    VAO vao;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    // we are using instancing for MVP matrices
    // so multiple copies can be rendered with different transforms
    // note that this is a reference since we are generally
    // sharing it with other primitive buffer classes
    InstanceBuffer &vbo_inst;

   private:
    GladGLContext &gl;

    void ConfigureVAO() {
        vao.Bind();
        ebo.Bind();

        // attribute pointers for position, color, size
        vbo.Bind();
        gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, position));
        gl.EnableVertexAttribArray(0);
        gl.VertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, color));
        gl.EnableVertexAttribArray(1);
        gl.VertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, size));
        gl.EnableVertexAttribArray(2);
        vbo.Unbind();

        // attribute for transform matrix used in instancing
        vbo_inst.Bind();
        for (int i = 0; i < 4; i++) {
            gl.VertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                   sizeof(Instance),
                                   (void *)(offsetof(Instance, transform) +
                                            sizeof(glm::vec4) * i));
            gl.EnableVertexAttribArray(3 + i);
            // set attribute divisor for instancing
            gl.VertexAttribDivisor(3 + i, 1);
        }
        vbo_inst.Unbind();

        vao.Unbind();
    }
};
