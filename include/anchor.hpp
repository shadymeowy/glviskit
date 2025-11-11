#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <glm/glm.hpp>

#include "buffer_stack.hpp"
#include "program.hpp"
#include "vao.hpp"

inline constexpr char shader_vertex_anchor[] = R"glsl(
    #version 330 core

    layout(location = 0) in vec3 a_anchor;
    layout(location = 1) in vec3 a_position;
    layout(location = 2) in vec4 a_color;
    layout(location = 3) in mat4 a_transform;
    out vec4 v_color;

    uniform mat4 mvp;
    uniform vec2 screen_size;

    void main()
    {
        mat4 T = mvp * a_transform;
        vec4 p = T * vec4(a_anchor, 1.0);

        vec2 offset = a_position.xy / screen_size;
        
        gl_Position = p;
        gl_Position.xy += offset * p.w;

        v_color = a_color;
    }

)glsl";

inline constexpr char shader_fragment_anchor[] = R"glsl(
    #version 330 core
    in vec4 v_color;
    out vec4 f_color;
    
    void main() {
        f_color = v_color;
    }
)glsl";

using AnchorProgram = Program<shader_vertex_anchor, shader_fragment_anchor>;

class AnchorBuffer {
   public:
    struct Element {
        glm::vec3 anchor;
        glm::vec3 position;
        glm::vec4 color;
    };

    struct Instance {
        glm::mat4 transform;
    };

    AnchorBuffer(GladGLContext &gl)
        : gl{gl}, vbo{gl}, ebo{gl}, vao{gl}, vbo_inst{gl} {
        ConfigureVAO();
    }

    void Render() {
        Sync();

        if (ebo.Size() == 0) {
            return;
        }

        vao.Bind();
        gl.DrawElementsInstanced(GL_TRIANGLES, ebo.Size(), GL_UNSIGNED_INT,
                                 nullptr, vbo_inst.Size());
        vao.Unbind();
    }

    bool Sync() {
        bool re_vbo = vbo.Sync();
        bool re_ebo = ebo.Sync();
        bool re_vbo_inst = vbo_inst.Sync();
        bool reallocated = re_vbo || re_ebo || re_vbo_inst;

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

    void SaveInstances() { vbo_inst.Save(); }
    void RestoreInstances() { vbo_inst.Restore(); }
    void ClearInstances() { vbo_inst.Clear(); }

    VAO vao;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    BufferStack<Instance, GL_ARRAY_BUFFER> vbo_inst;

   private:
    GladGLContext &gl;

    void ConfigureVAO() {
        vao.Bind();
        ebo.Bind();

        vbo.Bind();
        gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, anchor));
        gl.EnableVertexAttribArray(0);
        gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, position));
        gl.EnableVertexAttribArray(1);
        gl.VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, color));
        gl.EnableVertexAttribArray(2);
        vbo.Unbind();

        vbo_inst.Bind();
        std::size_t vec4_size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            gl.VertexAttribPointer(
                3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Instance),
                (void *)(offsetof(Instance, transform) + vec4_size * i));
            gl.EnableVertexAttribArray(3 + i);
            gl.VertexAttribDivisor(3 + i, 1);
        }
        vbo_inst.Unbind();

        vao.Unbind();
    }
};
