#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <map>

#include "../gl/buffer_stack.hpp"
#include "../gl/gl.hpp"
#include "../gl/instance.hpp"
#include "../gl/program.hpp"
#include "../gl/vao.hpp"

namespace glviskit::symbol {

// billboarded quad anchored at a 3D point, sampled from an msdf atlas
// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_vertex[] = GLVISKIT_VERT_HEADER R"glsl(
    layout(location = 0) in vec3 a_anchor;
    layout(location = 1) in vec2 a_offset;
    layout(location = 2) in vec2 a_corner;
    layout(location = 3) in float a_idx;
    layout(location = 4) in vec4 a_color;
    layout(location = 5) in float a_overlay;
    layout(location = 6) in mat4 a_transform;
    out vec2 v_uv;
    out vec4 v_color;

    uniform mat4 mvp;
    uniform vec2 screen_size;
    uniform vec2 u_grid;

    void main()
    {
        mat4 T = mvp * a_transform;
        vec4 p = T * vec4(a_anchor, 1.0);

        vec2 offset = a_offset / screen_size;

        gl_Position = p;
        gl_Position.xy += offset * p.w;

        if (a_overlay > 0.0) {
            gl_Position.z = (-1.0 + a_overlay * 1e-3) * gl_Position.w;
        }

        float ix = mod(a_idx, u_grid.x);
        float iy = floor(a_idx / u_grid.x);

        v_uv = vec2((ix + a_corner.x) / u_grid.x,
                    (iy + 1.0 - a_corner.y) / u_grid.y);
        v_color = a_color;
    }
)glsl";

// NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays)
inline constexpr char shader_fragment[] = GLVISKIT_FRAG_HEADER R"glsl(
    in vec2 v_uv;
    in vec4 v_color;
    out vec4 f_color;

    uniform sampler2D u_atlas;
    uniform float u_px_range;
    uniform int alpha_test;

    float median(float r, float g, float b) {
        return max(min(r, g), min(max(r, g), b));
    }

    void main() {
        vec3 s = texture(u_atlas, v_uv).rgb;
        float sd = median(s.r, s.g, s.b);

        // screen-space distance range from atlas derivatives
        vec2 unit_range = vec2(u_px_range) / vec2(textureSize(u_atlas, 0));
        vec2 screen_tex_size = vec2(1.0) / fwidth(v_uv);
        float px_range = max(0.5 * dot(unit_range, screen_tex_size), 1.0);

        float screen_dist = px_range * (sd - 0.5);
        float alpha = clamp(screen_dist + 0.5, 0.0, 1.0) * v_color.a;

        if (alpha_test == 1 && alpha < 0.99) {
            discard;
        }
        if (alpha_test == 0 && alpha >= 0.99) {
            discard;
        }

        f_color = vec4(v_color.rgb, alpha);
    }
)glsl";

// NOLINTNEXTLINE(hicpp-no-array-decay)
using Program = Program<shader_vertex, shader_fragment>;

class Buffer {
   public:
    struct Element {
        glm::vec3 anchor;
        glm::vec2 offset;
        glm::vec2 corner;
        float idx;
        glm::vec4 color;
        float overlay;
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
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, offset));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, corner));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, idx));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, color));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Element),
                              (void *)offsetof(Element, overlay));
        glEnableVertexAttribArray(5);
        vbo.Unbind();

        vbo_inst.Bind();
        std::size_t vec4_size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            glVertexAttribPointer(
                6 + i, 4, GL_FLOAT, GL_FALSE, sizeof(Instance),
                (void *)(offsetof(Instance, transform) + (vec4_size * i)));
            glEnableVertexAttribArray(6 + i);
            glVertexAttribDivisor(6 + i, 1);
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

}  // namespace glviskit::symbol
