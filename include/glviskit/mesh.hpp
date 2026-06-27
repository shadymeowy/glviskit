#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "render_state.hpp"

namespace glviskit {

class Mesh {
   public:
    explicit Mesh(std::shared_ptr<RenderState> render_state)
        : render_state{std::move(render_state)} {}

    auto Vertex(glm::vec3 position, glm::vec4 color) -> size_t {
        auto &vbo = render_state->mesh_buffer_.VBO();
        const size_t index = vbo.Size();
        vbo.Append({.position = position, .color = color});
        state.vertex_mapping.push_back(static_cast<GLuint>(index));
        return state.vertex_mapping.size() - 1;
    }

    auto Vertex(glm::vec3 position) -> size_t {
        return Vertex(position, state.color);
    }

    void Triangle(size_t i0, size_t i1, size_t i2) {
        auto &ebo = render_state->mesh_buffer_.EBO();
        ebo.Append(state.vertex_mapping.at(i0));
        ebo.Append(state.vertex_mapping.at(i1));
        ebo.Append(state.vertex_mapping.at(i2));
    }

    void Color(const glm::vec4 &c) { state.color = c; }
    [[nodiscard]] auto GetColor() const -> glm::vec4 { return state.color; }
    [[nodiscard]] auto VertexCount() const -> size_t {
        return state.vertex_mapping.size();
    }

   private:
    struct State {
        glm::vec4 color{1.0F};
        std::vector<GLuint> vertex_mapping;
    };

    std::shared_ptr<RenderState> render_state;
    State state{};
    glm::vec4 color_saved_{1.0F};
    size_t saved_size_ = 0;

    void Save() {
        color_saved_ = state.color;
        saved_size_ = state.vertex_mapping.size();
    }
    void Restore() {
        state.color = color_saved_;
        state.vertex_mapping.resize(saved_size_);
    }
    void Clear() {
        state.color = glm::vec4{1.0F};
        state.vertex_mapping.clear();
    }

    friend class RenderList;
};

}  // namespace glviskit
