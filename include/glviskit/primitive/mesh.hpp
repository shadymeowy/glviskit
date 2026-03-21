#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>

#include "../gl/buffer_stack.hpp"
#include "../gl/gl.hpp"
#include "../gl/instance.hpp"
#include "../gl/program.hpp"
#include "../gl/vao.hpp"

namespace glviskit::mesh {

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_vertex[] = GLVISKIT_VERT_HEADER R"glsl(
    layout(location = 0) in vec3 a_position;
    layout(location = 1) in vec4 a_color;
    layout(location = 2) in mat4 a_transform;
    out vec4 v_color;

    uniform mat4 mvp;

    void main()
    {
        gl_Position = mvp * a_transform * vec4(a_position, 1.0);
        v_color = a_color;
    }
)glsl";

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_fragment[] = GLVISKIT_FRAG_HEADER R"glsl(
    in vec4 v_color;
    out vec4 f_color;

    uniform int alpha_test;

    void main() {
        if (alpha_test == 1 && v_color.a < 0.99) {
            discard;
        }

        if (alpha_test == 0 && v_color.a >= 0.99) {
            discard;
        }

        f_color = v_color;
    }
)glsl";

// NOLINTNEXTLINE(hicpp-no-array-decay)
using Program = Program<shader_vertex, shader_fragment>;

class Buffer {
   public:
    struct Element {
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
    std::map<GLuint, VAO> vaos;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    InstanceBuffer &vbo_inst;
    std::map<GLuint, bool> vao_configured;

    void ConfigureVAO(GLuint ctx_id) {
        VAO &vao = vaos.at(ctx_id);
        vao.Bind();
        ebo.Bind();

        vbo.Bind();
        // NOLINTBEGIN(performance-no-int-to-ptr)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, color));
        glEnableVertexAttribArray(1);
        vbo.Unbind();

        vbo_inst.Bind();
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(Instance),
                                  (void *)(offsetof(Instance, transform) +
                                           (sizeof(glm::vec4) * i)));
            glEnableVertexAttribArray(2 + i);
            glVertexAttribDivisor(2 + i, 1);
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
        if (!vaos.contains(ctx_id)) {
            vaos.emplace(ctx_id, VAO{});
            vao_configured.emplace(ctx_id, false);
        }
    }

    void InvalidateVAOs() {
        for (auto &entry : vao_configured) {
            entry.second = false;
        }
    }
};

}  // namespace glviskit::mesh
