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
        auto &vbo = render_state->line_buffer_.VBO();
        auto &ebo = render_state->line_buffer_.EBO();

        size_t index_current = vbo.Size();

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
        ebo.Append(index_current + 0);
        ebo.Append(index_current + 2);
        ebo.Append(index_current + 1);
        ebo.Append(index_current + 1);
        ebo.Append(index_current + 2);
        ebo.Append(index_current + 3);

        if (state.line_counter > 1) {
            // connect previous segment
            ebo.Append(state.index_prev + 2);
            ebo.Append(index_current + 0);
            ebo.Append(state.index_prev + 3);
            ebo.Append(state.index_prev + 3);
            ebo.Append(index_current + 0);
            ebo.Append(index_current + 1);
        }

        // update previous points
        state.line_prev = position;
        state.color_prev = state.color;
        state.size_prev = state.size;
        state.line_counter++;
        state.index_prev = index_current;
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

        // previous ebo index
        size_t index_prev = 0;
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