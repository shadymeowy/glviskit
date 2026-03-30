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
    void LineTo(glm::vec3 position, glm::vec4 color, float size) {
        if (state.line_counter == 0) {
            // first point just initializes the current contour state
            state.line_prev = position;
            state.color_prev = color;
            state.size_prev = size;
            state.line_first = position;
            state.color_first = color;
            state.size_first = size;
            state.line_counter++;
            return;
        }

        AppendSegment(position, color, size);
    }

    void LineTo(glm::vec3 position, glm::vec4 color) {
        LineTo(position, color, state.size);
    }

    void LineTo(glm::vec3 position) {
        LineTo(position, state.color, state.size);
    }

    void LineEnd() {
        // reset line drawing state
        state.line_counter = 0;
    }

    void Close() {
        if (state.line_counter < 2) {
            LineEnd();
            return;
        }

        const size_t index_current = AppendSegment(
            state.line_first, state.color_first, state.size_first);

        // connect the closing segment back to the first segment
        auto &ebo = render_state->line_buffer_.EBO();
        ebo.Append(index_current + 2);
        ebo.Append(state.index_first + 0);
        ebo.Append(index_current + 3);
        ebo.Append(index_current + 3);
        ebo.Append(state.index_first + 0);
        ebo.Append(state.index_first + 1);

        LineEnd();
    }

    void Color(const glm::vec4 &c) { state.color = c; }
    void Size(float s) { state.size = s; }

   private:
    auto AppendSegment(glm::vec3 position, const glm::vec4 &color, float size)
        -> size_t {
        auto &vbo = render_state->line_buffer_.VBO();
        auto &ebo = render_state->line_buffer_.EBO();

        const size_t index_current = vbo.Size();
        auto direction = position - state.line_prev;

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
                    .color = color,
                    .size = size});
        vbo.Append({.position = position,
                    .velocity = direction,
                    .color = color,
                    .size = -size});

        ebo.Append(index_current + 0);
        ebo.Append(index_current + 2);
        ebo.Append(index_current + 1);
        ebo.Append(index_current + 1);
        ebo.Append(index_current + 2);
        ebo.Append(index_current + 3);

        if (state.line_counter > 1) {
            ebo.Append(state.index_prev + 2);
            ebo.Append(index_current + 0);
            ebo.Append(state.index_prev + 3);
            ebo.Append(state.index_prev + 3);
            ebo.Append(index_current + 0);
            ebo.Append(index_current + 1);
        } else {
            state.index_first = index_current;
        }

        state.line_prev = position;
        state.color_prev = color;
        state.size_prev = size;
        state.line_counter++;
        state.index_prev = index_current;

        return index_current;
    }

    struct State {
        // attributes for rendering
        glm::vec4 color{1.0F};
        float size = 1.0F;

        // line drawing state
        size_t line_counter = 0;
        glm::vec3 line_prev{0.0F};
        glm::vec4 color_prev{1.0F};
        float size_prev = 1.0F;
        glm::vec3 line_first{0.0F};
        glm::vec4 color_first{1.0F};
        float size_first = 1.0F;

        // previous ebo index
        size_t index_prev = 0;
        size_t index_first = 0;
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