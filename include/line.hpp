#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <map>

#include "buffer_stack.hpp"
#include "instance.hpp"
#include "program.hpp"
#include "vao.hpp"

inline constexpr char shader_vertex_line[] = R"glsl(
    #version 330 core

    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec3 a_velocity;
    layout(location = 2) in vec4 a_color;
    layout(location = 3) in float a_size;
    layout(location = 4) in mat4 a_transform;
    out vec4 v_color;

    uniform mat4 mvp;
    uniform vec2 screen_size;

    void main()
    {
        mat4 T = mvp * a_transform;
        vec4 p = T * vec4(a_position, 1.0);
        vec4 v = T * vec4(a_velocity, 0.0);

        vec2 v_screen = (v.xy * p.w - p.xy * v.w) * screen_size;
        vec2 v2 = normalize(v_screen);

        vec2 normal = vec2(v2.y, -v2.x);
        vec2 offset = normal * a_size / screen_size;
        
        gl_Position = p;
        gl_Position.xy += offset * p.w;

        v_color = a_color;
    }

)glsl";

inline constexpr char shader_fragment_line[] = R"glsl(
    #version 330 core
    in vec4 v_color;
    out vec4 f_color;
    
    void main() {
        f_color = v_color;
    }
)glsl";

using LineProgram = Program<shader_vertex_line, shader_fragment_line>;

class LineBuffer {
   public:
    struct Element {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec4 color;
        float size;
    };

    explicit LineBuffer(GladGLContext &gl, InstanceBuffer &vbo_inst)
        : gl{gl},
          vbo{gl},
          ebo{gl},
          vaos{},
          vbo_inst{vbo_inst},
          vao_configured{} {}

    void Render(GLuint ctx_id) {
        if (ebo.Size() == 0 || vbo_inst.Size() == 0) {
            return;
        }

        EnsureVAO(ctx_id);

        bool reallocated = Sync();
        if (reallocated) {
            InvalidateVAOs();
        }

        if (!vao_configured.at(ctx_id)) {
            ConfigureVAO(ctx_id);
            vao_configured.at(ctx_id) = true;
        }

        auto &vao = vaos.at(ctx_id);
        vao.Bind();
        gl.DrawElementsInstanced(GL_TRIANGLES, ebo.Size(), GL_UNSIGNED_INT,
                                 nullptr, vbo_inst.Size());
        vao.Unbind();
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

    std::map<GLuint, VAO> vaos;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    InstanceBuffer &vbo_inst;

   private:
    GladGLContext &gl;
    std::map<GLuint, bool> vao_configured;

    void ConfigureVAO(GLuint ctx_id) {
        VAO &vao = vaos.at(ctx_id);
        vao.Bind();
        ebo.Bind();

        vbo.Bind();
        gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, position));
        gl.EnableVertexAttribArray(0);
        gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, velocity));
        gl.EnableVertexAttribArray(1);
        gl.VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, color));
        gl.EnableVertexAttribArray(2);
        gl.VertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Element),
                               (void *)offsetof(Element, size));
        gl.EnableVertexAttribArray(3);
        vbo.Unbind();

        vbo_inst.Bind();
        std::size_t vec4_size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            gl.VertexAttribPointer(
                4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Instance),
                (void *)(offsetof(Instance, transform) + vec4_size * i));
            gl.EnableVertexAttribArray(4 + i);
            gl.VertexAttribDivisor(4 + i, 1);
        }
        vbo_inst.Unbind();

        vao.Unbind();
    }

    bool Sync() {
        bool re_vbo = vbo.Sync();
        bool re_ebo = ebo.Sync();
        bool re_vbo_inst = vbo_inst.Sync();
        bool reallocated = re_vbo || re_ebo || re_vbo_inst;
        return reallocated;
    }

    void EnsureVAO(GLuint ctx_id) {
        // create VAO for the context if it does not exist
        if (vaos.find(ctx_id) == vaos.end()) {
            vaos.emplace(ctx_id, VAO{gl});
            vao_configured.emplace(ctx_id, false);
        }
    }

    void InvalidateVAOs() {
        // mark all VAOs as needing reconfiguration
        for (auto &entry : vao_configured) {
            entry.second = false;
        }
    }
};
