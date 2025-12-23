#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>

#include "../gl/buffer_stack.hpp"
#include "../gl/gl.hpp"
#include "../gl/instance.hpp"
#include "../gl/program.hpp"
#include "../gl/vao.hpp"

namespace glviskit::point {

// This file defines a PointBuffer class.
// While quite simple, it is the reference for defining reusable
// and generic components for rendering.

// First, we define the shader sources
// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_vertex[] = GLVISKIT_VERT_HEADER R"glsl(
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

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_fragment[] = GLVISKIT_FRAG_HEADER R"glsl(
    in vec4 v_color;
    out vec4 f_color;
    
    void main() {
        f_color = v_color;
    }
)glsl";

// Define PointProgram which holds the shaders.
// In theory, multiple point programs with different shaders could be defined
// for same PointBuffer.
// NOLINTNEXTLINE(hicpp-no-array-decay)
using Program = Program<shader_vertex, shader_fragment>;

// PointBuffer is responsible for storing and rendering points
// with position, color, and size attributes.
// By itself, it only provides a render call and attributes setup for VAO,
// nothing more.

// Any VBO template class can be used to store its elements.
// We generally opt for StackVBO for dynamic data.
class Buffer {
   public:
    struct Element {
        glm::vec3 position;
        glm::vec4 color;
        float size;
    };

    explicit Buffer(InstanceBuffer &vbo_inst) : vbo_inst{vbo_inst} {}

    void Render(GLuint ctx_id) {
        // if there is nothing to draw, return
        if (ebo.Size() == 0 || vbo_inst.Size() == 0) {
            return;
        }

        // ensure VAO is created and configured
        EnsureVAO(ctx_id);

        // check if buffers were reallocated
        bool reallocated = Sync();
        if (reallocated) {
            // Invalidate all VAOs if buffers were reallocated
            InvalidateVAOs();
        }

        // configure VAO if not yet configured
        if (!vao_configured.at(ctx_id)) {
            ConfigureVAO(ctx_id);
            vao_configured.at(ctx_id) = true;
        }

        // bind VAO using RAII binder,
        // which will unbind it at the end of the scope
        auto &vao = vaos.at(ctx_id);
        vao.Bind();
        // draw call
        glDrawElementsInstanced(GL_POINTS, static_cast<GLsizei>(ebo.Size()),
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
    // In this case, we don't really need an EBO
    // but can be useful for other primitives.
    std::map<GLuint, VAO> vaos;
    BufferStack<Element, GL_ARRAY_BUFFER> vbo;
    BufferStack<GLuint, GL_ELEMENT_ARRAY_BUFFER> ebo;
    // we are using instancing for MVP matrices
    // so multiple copies can be rendered with different transforms
    // note that this is a reference since we are generally
    // sharing it with other primitive buffer classes
    InstanceBuffer &vbo_inst;

    void ConfigureVAO(GLuint ctx_id) {
        VAO &vao = vaos.at(ctx_id);
        vao.Bind();
        ebo.Bind();

        // attribute pointers for position, color, size
        vbo.Bind();
        // NOLINTBEGIN(performance-no-int-to-ptr)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, position));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, color));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, size));
        glEnableVertexAttribArray(2);
        vbo.Unbind();

        // attribute for transform matrix used in instancing
        vbo_inst.Bind();
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE,
                                  sizeof(Instance),
                                  (void *)(offsetof(Instance, transform) +
                                           (sizeof(glm::vec4) * i)));
            glEnableVertexAttribArray(3 + i);
            // set attribute divisor for instancing
            glVertexAttribDivisor(3 + i, 1);
        }
        // NOLINTEND(performance-no-int-to-ptr)
        vbo_inst.Unbind();

        vao.Unbind();
    }

    auto Sync() -> bool {
        // sync all buffers
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

}  // namespace glviskit::point