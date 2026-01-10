#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "path.hpp"
#include "render_state.hpp"

namespace glviskit {

class RenderList {
   public:
    RenderList() : render_state_{std::make_shared<RenderState>()} {
        // create identity instance
        AddInstance(glm::mat4{1.0F});
    }

    void Line(glm::vec3 start, glm::vec3 end) {
        // if there is an ongoing line, end it first
        // otherwise, this is noop
        Path path{render_state_};
        path.Color(state.color);
        path.Size(state.size);
        path.LineEnd();
        path.LineTo(start);
        path.LineTo(end);
        path.LineEnd();
    }

    auto PathBegin() -> std::shared_ptr<Path> {
        // get an unused path slot
        auto idx = GetUnusedPath();

        // create new path
        auto path = std::make_shared<Path>(render_state_);

        // store weak reference
        paths_[idx] = path;

        // initialize path state with current state
        path->Color(state.color);
        path->Size(state.size);

        // return shared pointer
        return path;
    }

    void Point(glm::vec3 position) {
        auto &vbo = render_state_->point_buffer_.VBO();
        auto &ebo = render_state_->point_buffer_.EBO();

        size_t index = vbo.Size();
        vbo.Append(
            {.position = position, .color = state.color, .size = state.size});
        ebo.Append(index);
    }

    void Circle(glm::vec3 circle) {
        auto &vbo = render_state_->circle_buffer_.VBO();
        auto &ebo = render_state_->circle_buffer_.EBO();
        size_t index = vbo.Size();
        auto s = state.size;
        // four vertices
        vbo.Append(
            {.circle = circle, .position = {-s, -s, 0}, .color = state.color});
        vbo.Append(
            {.circle = circle, .position = {s, -s, 0}, .color = state.color});
        vbo.Append(
            {.circle = circle, .position = {s, s, 0}, .color = state.color});
        vbo.Append(
            {.circle = circle, .position = {-s, s, 0}, .color = state.color});
        // two triangles
        ebo.Append(index + 0);
        ebo.Append(index + 1);
        ebo.Append(index + 2);
        ebo.Append(index + 2);
        ebo.Append(index + 3);
        ebo.Append(index + 0);
    }

    // attributes for subsequent drawing
    void Color(const glm::vec4 &c) { state.color = c; }
    void Size(float s) { state.size = s; }

    // instancing
    void AddInstance(const glm::mat4 &transform) {
        render_state_->vbo_inst.Append({transform});
    }

    void AddInstance(const glm::vec3 &position,
                     const glm::vec3 &rotation = glm::vec3{0.0F},
                     const glm::vec3 &scale = glm::vec3{1.0F}) {
        // translation
        auto t = glm::translate(glm::mat4{1.0F}, position);

        // rotation
        auto angle = glm::length(rotation);
        glm::mat4 r{1.0F};
        if (angle > 1e-6F) {
            auto axis = rotation / angle;
            r = glm::rotate(glm::mat4{1.0F}, angle, axis);
        }

        // scale
        auto s = glm::scale(glm::mat4{1.0F}, scale);
        AddInstance(t * r * s);
    }

    void Save() {
        // save render state
        render_state_->Save();

        // save current drawing state
        state_saved = state;

        // save all paths if in use
        for (auto &i : paths_) {
            if (auto path = i.lock()) {
                path->Save();
            }
        }
    }

    void Restore() {
        // restore render state
        render_state_->Restore();

        // restore current drawing state
        state = state_saved;

        // restore all paths if in use
        for (auto &i : paths_) {
            if (auto path = i.lock()) {
                path->Restore();
            }
        }
    }

    void Clear() {
        // clear render state
        render_state_->Clear();

        // clear current drawing state
        state = State{};

        // clear all paths if in use
        for (auto &i : paths_) {
            if (auto path = i.lock()) {
                path->Clear();
            }
        }
    }

    void SaveInstances() { render_state_->SaveInstances(); }
    void RestoreInstances() { render_state_->RestoreInstances(); }
    void ClearInstances() { render_state_->ClearInstances(); }

    void SetEnabled(bool enabled) { render_state_->enabled_ = enabled; }
    auto IsEnabled() -> bool { return render_state_->enabled_; }

   private:
    struct State {
        // attributes for rendering
        glm::vec4 color{1.0F};
        float size = 1.0F;
    };

    // render state
    std::shared_ptr<RenderState> render_state_;

    // hold weak reference to paths
    std::vector<std::weak_ptr<Path>> paths_;

    // current and saved drawing state
    State state{};
    State state_saved{};

    // get an unused path or create a new one
    auto GetUnusedPath() -> size_t {
        // check if any path slot is unused
        for (size_t i = 0; i < paths_.size(); ++i) {
            if (paths_[i].expired()) {
                return i;
            }
        }

        // create a new path slot
        paths_.emplace_back();
        return paths_.size() - 1;
    }

    friend class WindowRenderer;
};

}  // namespace glviskit