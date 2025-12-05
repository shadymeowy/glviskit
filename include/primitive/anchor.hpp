#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>

#include "../gl/buffer_stack.hpp"
#include "../gl/gl.hpp"
#include "../gl/instance.hpp"
#include "../gl/program.hpp"
#include "../gl/vao.hpp"

namespace glviskit::anchor {

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_vertex[] = GLVISKIT_VERT_HEADER R"glsl(
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

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_fragment[] = GLVISKIT_FRAG_HEADER R"glsl(
    in vec4 v_color;
    out vec4 f_color;
    
    void main() {
        f_color = v_color;
    }
)glsl";

// NOLINTNEXTLINE(hicpp-no-array-decay)
using Program = Program<shader_vertex, shader_fragment>;

class Buffer {
   public:
    struct Element {
        glm::vec3 anchor;
        glm::vec3 position;
        glm::vec4 color;
    };

    explicit Buffer(InstanceBuffer &vbo_inst) : vbo_inst{vbo_inst} {}

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
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(ebo.Size()),
                                GL_UNSIGNED_INT, nullptr,
                                static_cast<GLsizei>(vbo_inst.Size()));
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

    auto VBO() -> auto & { return vbo; }
    auto EBO() -> auto & { return ebo; }

   private:
    std::map<GLuint, bool> vao_configured;
    std::map<GLuint, VAO> vaos;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    InstanceBuffer &vbo_inst;

    void ConfigureVAO(GLuint ctx_id) {
        VAO &vao = vaos.at(ctx_id);
        vao.Bind();
        ebo.Bind();

        vbo.Bind();
        // NOLINTBEGIN(performance-no-int-to-ptr)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, anchor));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, color));
        glEnableVertexAttribArray(2);
        vbo.Unbind();

        vbo_inst.Bind();
        std::size_t vec4_size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(
                3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Instance),
                (void *)(offsetof(Instance, transform) + (vec4_size * i)));
            glEnableVertexAttribArray(3 + i);
            glVertexAttribDivisor(3 + i, 1);
        }
        // NOLINTEND(performance-no-int-to-ptr)
        vbo_inst.Unbind();

        vao.Unbind();
    }

    auto Sync() -> bool {
        bool re_vbo = vbo.Sync();
        bool re_ebo = ebo.Sync();
        bool re_vbo_inst = vbo_inst.Sync();
        bool reallocated = re_vbo || re_ebo || re_vbo_inst;
        return reallocated;
    }

    void EnsureVAO(GLuint ctx_id) {
        // create VAO for the context if it does not exist
        if (!vaos.contains(ctx_id)) {
            vaos.emplace(ctx_id, VAO{});
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

}  // namespace glviskit::anchor