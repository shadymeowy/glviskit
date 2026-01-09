#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "render_state.hpp"

namespace glviskit {

// for drawing paths of connected lines
// associated with a RenderList
class Path {
   public:
    explicit Path(std::shared_ptr<RenderState> render_state)
        : render_state{std::move(render_state)} {}

    // Efficient way to draw connected lines
    void LineTo(glm::vec3 position) {
        auto &vbo = render_state->line_buffer.VBO();
        auto &ebo = render_state->line_buffer.EBO();

        size_t base_index = vbo.Size();

        if (state.line_counter == 0) {
            // for first point just store and return
            state.line_prev = position;
            state.color_prev = state.color;
            state.size_prev = state.size;
            state.line_counter++;
            return;
        }

        auto direction = position - state.line_prev;
        // vertices for new line segment
        vbo.Append({.position = state.line_prev,
                    .velocity = direction,
                    .color = state.color_prev,
                    .size = state.size_prev});
        vbo.Append({.position = state.line_prev,
                    .velocity = direction,
                    .color = state.color_prev,
                    .size = -state.size_prev});
        vbo.Append({.position = position,
                    .velocity = direction,
                    .color = state.color,
                    .size = state.size});
        vbo.Append({.position = position,
                    .velocity = direction,
                    .color = state.color,
                    .size = -state.size});

        // new line segment
        ebo.Append(base_index + 0);
        ebo.Append(base_index + 2);
        ebo.Append(base_index + 1);
        ebo.Append(base_index + 1);
        ebo.Append(base_index + 2);
        ebo.Append(base_index + 3);

        if (state.line_counter > 1) {
            // connect previous segment
            // we have +0, +1 from new segment and -2, -1 from previous segment
            ebo.Append(base_index - 2);
            ebo.Append(base_index + 0);
            ebo.Append(base_index - 1);
            ebo.Append(base_index - 1);
            ebo.Append(base_index + 0);
            ebo.Append(base_index + 1);
        }

        // update previous points
        state.line_prev = position;
        state.color_prev = state.color;
        state.size_prev = state.size;
        state.line_counter++;
    }

    void LineEnd() {
        // reset line drawing state
        state.line_counter = 0;
    }

    void Color(const glm::vec4 &c) { state.color = c; }
    void Size(float s) { state.size = s; }

   private:
    struct State {
        // attributes for rendering
        glm::vec4 color{1.0F};
        float size = 1.0F;

        // line drawing state
        size_t line_counter = 0;
        glm::vec3 line_prev{0.0F};
        glm::vec4 color_prev{1.0F};
        float size_prev = 1.0F;
    };

    // associated render state
    std::shared_ptr<RenderState> render_state;

    // current drawing state
    State state{};
    State state_saved{};

    // save/restore/clear state
    void Save() { state_saved = state; }
    void Restore() { state = state_saved; }
    void Clear() { state = State{}; }

    friend class RenderList;
};

}  // namespace glviskit