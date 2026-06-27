#pragma once

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "mesh.hpp"
#include "path.hpp"
#include "render_state.hpp"
#include "symbol_atlas.hpp"

namespace glviskit {

enum class MarkerType { Square, Triangle, Diamond, Ring, Circle };

enum class TextAlign { Left, Center, Right };

class RenderList {
   public:
    RenderList() : render_state_{std::make_shared<RenderState>()} {
        // create identity instance
        AddInstance(glm::mat4{1.0F});
    }

    void Line(glm::vec3 start, glm::vec3 end, glm::vec4 color, float size) {
        Path path{render_state_};
        path.Color(color);
        path.Size(size);
        path.LineEnd();
        path.LineTo(start);
        path.LineTo(end);
        path.LineEnd();
    }

    void Line(glm::vec3 start, glm::vec3 end, glm::vec4 color) {
        Line(start, end, color, state.size);
    }

    void Line(glm::vec3 start, glm::vec3 end) {
        Line(start, end, state.color, state.size);
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

    auto MeshBegin() -> std::shared_ptr<Mesh> {
        auto idx = GetUnusedMesh();

        auto mesh = std::make_shared<Mesh>(render_state_);
        meshes_[idx] = mesh;
        mesh->Color(state.color);

        return mesh;
    }

    void Point(glm::vec3 position, glm::vec4 color, float size) {
        auto &vbo = render_state_->point_buffer_.VBO();
        auto &ebo = render_state_->point_buffer_.EBO();

        size_t index = vbo.Size();
        vbo.Append({.position = position, .color = color, .size = size});
        ebo.Append(index);
    }

    void Point(glm::vec3 position, glm::vec4 color) {
        Point(position, color, state.size);
    }

    void Point(glm::vec3 position) { Point(position, state.color, state.size); }

    void Circle(glm::vec3 circle, glm::vec4 color, float size) {
        auto &vbo = render_state_->circle_buffer_.VBO();
        auto &ebo = render_state_->circle_buffer_.EBO();
        size_t index = vbo.Size();
        auto s = size;
        // four vertices
        vbo.Append({.circle = circle, .position = {-s, -s, 0}, .color = color});
        vbo.Append({.circle = circle, .position = {s, -s, 0}, .color = color});
        vbo.Append({.circle = circle, .position = {s, s, 0}, .color = color});
        vbo.Append({.circle = circle, .position = {-s, s, 0}, .color = color});
        // two triangles
        ebo.Append(index + 0);
        ebo.Append(index + 1);
        ebo.Append(index + 2);
        ebo.Append(index + 2);
        ebo.Append(index + 3);
        ebo.Append(index + 0);
    }

    void Circle(glm::vec3 circle, glm::vec4 color) {
        Circle(circle, color, state.size);
    }

    void Circle(glm::vec3 circle) { Circle(circle, state.color, state.size); }

    void Symbol(int idx, glm::vec3 anchor, glm::vec2 offset, glm::vec4 color,
                float size, int overlay = 0) {
        auto &vbo = render_state_->symbol_buffer_.VBO();
        auto &ebo = render_state_->symbol_buffer_.EBO();
        size_t index = vbo.Size();
        auto fidx = static_cast<float>(idx);

        auto ov = static_cast<float>(overlay);

        float h = size;
        float l = offset.x - h;
        float r = offset.x + h;
        float t = -offset.y + h;
        float b = -offset.y - h;
        // four corners (BL, BR, TR, TL)
        vbo.Append({.anchor = anchor,
                    .offset = {l, b},
                    .corner = {0.0F, 0.0F},
                    .idx = fidx,
                    .color = color,
                    .overlay = ov});
        vbo.Append({.anchor = anchor,
                    .offset = {r, b},
                    .corner = {1.0F, 0.0F},
                    .idx = fidx,
                    .color = color,
                    .overlay = ov});
        vbo.Append({.anchor = anchor,
                    .offset = {r, t},
                    .corner = {1.0F, 1.0F},
                    .idx = fidx,
                    .color = color,
                    .overlay = ov});
        vbo.Append({.anchor = anchor,
                    .offset = {l, t},
                    .corner = {0.0F, 1.0F},
                    .idx = fidx,
                    .color = color,
                    .overlay = ov});
        // two triangles
        ebo.Append(index + 0);
        ebo.Append(index + 1);
        ebo.Append(index + 2);
        ebo.Append(index + 2);
        ebo.Append(index + 3);
        ebo.Append(index + 0);
    }

    void Symbol(int idx, glm::vec3 anchor, glm::vec2 offset, glm::vec4 color) {
        Symbol(idx, anchor, offset, color, state.size);
    }

    void Symbol(int idx, glm::vec3 anchor, glm::vec2 offset) {
        Symbol(idx, anchor, offset, state.color, state.size);
    }

    void Character(char c, glm::vec3 anchor, glm::vec2 offset, glm::vec4 color,
                   float size, int overlay = 0) {
        const auto &atlas = DefaultSymbolAtlas();
        int idx = static_cast<unsigned char>(c) - atlas.first_codepoint;
        if (idx < 0 || idx >= atlas.char_count) {
            return;
        }
        Symbol(idx, anchor, offset, color, size, overlay);
    }

    void Character(char c, glm::vec3 anchor, glm::vec2 offset,
                   glm::vec4 color) {
        Character(c, anchor, offset, color, state.size);
    }

    void Character(char c, glm::vec3 anchor, glm::vec2 offset) {
        Character(c, anchor, offset, state.color, state.size);
    }

    void Marker(MarkerType type, glm::vec3 anchor, glm::vec2 offset,
                glm::vec4 color, float size, int overlay = 0) {
        const auto &atlas = DefaultSymbolAtlas();
        Symbol(atlas.char_count + static_cast<int>(type), anchor, offset, color,
               size, overlay);
    }

    void Marker(MarkerType type, glm::vec3 anchor, glm::vec2 offset,
                glm::vec4 color) {
        Marker(type, anchor, offset, color, state.size);
    }

    void Marker(MarkerType type, glm::vec3 anchor, glm::vec2 offset) {
        Marker(type, anchor, offset, state.color, state.size);
    }

    void Text(const std::string &text, glm::vec3 anchor, glm::vec2 offset,
              glm::vec4 color, float size, TextAlign align = TextAlign::Left,
              int overlay = 0) {
        const auto &atlas = DefaultSymbolAtlas();
        const float cell = 2.0F * size;
        const float char_w = atlas.advance * cell;
        const float line_h = atlas.line_height * cell;
        const size_t n = text.size();

        size_t max_chars = 0;
        size_t line_count = 1;
        for (size_t i = 0, cur = 0; i < n; ++i) {
            if (text[i] == '\n') {
                max_chars = std::max(max_chars, cur);
                cur = 0;
                ++line_count;
            } else {
                ++cur;
                if (i + 1 == n) {
                    max_chars = std::max(max_chars, cur);
                }
            }
        }

        const float block_w = static_cast<float>(max_chars) * char_w;
        const float block_h = static_cast<float>(line_count) * line_h;
        const float top = offset.y - (block_h * 0.5F);

        size_t start = 0;
        for (size_t li = 0; li < line_count; ++li) {
            size_t end = start;
            while (end < n && text[end] != '\n') {
                ++end;
            }
            const auto len = static_cast<float>(end - start);
            const float line_w = len * char_w;
            float left = offset.x - (block_w * 0.5F);
            if (align == TextAlign::Center) {
                left = offset.x - (line_w * 0.5F);
            } else if (align == TextAlign::Right) {
                left = offset.x + (block_w * 0.5F) - line_w;
            }

            const float cy = top + ((static_cast<float>(li) + 0.5F) * line_h);
            for (size_t k = start; k < end; ++k) {
                const float cx =
                    left + ((static_cast<float>(k - start) + 0.5F) * char_w);
                Character(text[k], anchor, {cx, cy}, color, size, overlay);
            }
            start = end + 1;
        }
    }

    void Text(const std::string &text, glm::vec3 anchor, glm::vec2 offset,
              glm::vec4 color) {
        Text(text, anchor, offset, color, state.size);
    }

    void Text(const std::string &text, glm::vec3 anchor, glm::vec2 offset,
              TextAlign align) {
        Text(text, anchor, offset, state.color, state.size, align);
    }

    void Text(const std::string &text, glm::vec3 anchor, glm::vec2 offset) {
        Text(text, anchor, offset, state.color, state.size);
    }

    void Triangles(std::span<const glm::vec3> vertices,
                   std::span<const glm::uvec3> indices) {
        auto &vbo = render_state_->mesh_buffer_.VBO();
        auto &ebo = render_state_->mesh_buffer_.EBO();
        const size_t base = vbo.Size();

        for (const auto &vertex : vertices) {
            vbo.Append({.position = vertex, .color = state.color});
        }

        for (const auto &triangle : indices) {
            ebo.Append(static_cast<GLuint>(base + triangle.x));
            ebo.Append(static_cast<GLuint>(base + triangle.y));
            ebo.Append(static_cast<GLuint>(base + triangle.z));
        }
    }

    void Triangles(std::span<const glm::vec3> vertices,
                   std::span<const glm::uvec3> indices,
                   std::span<const glm::vec4> colors) {
        auto &vbo = render_state_->mesh_buffer_.VBO();
        auto &ebo = render_state_->mesh_buffer_.EBO();
        const size_t base = vbo.Size();

        for (size_t i = 0; i < vertices.size(); ++i) {
            vbo.Append({.position = vertices[i], .color = colors[i]});
        }

        for (const auto &triangle : indices) {
            ebo.Append(static_cast<GLuint>(base + triangle.x));
            ebo.Append(static_cast<GLuint>(base + triangle.y));
            ebo.Append(static_cast<GLuint>(base + triangle.z));
        }
    }

    // attributes for subsequent drawing
    void Color(const glm::vec4 &c) { state.color = c; }
    void Size(float s) { state.size = s; }

    // instancing
    void AddInstance(const glm::mat4 &transform) {
        render_state_->vbo_inst.Append({transform});
    }

    void AddInstance(const glm::vec3 &position, const glm::quat &rotation,
                     const glm::vec3 &scale = glm::vec3{1.0F}) {
        auto t = glm::translate(glm::mat4{1.0F}, position);
        auto r = glm::mat4_cast(rotation);
        auto s = glm::scale(glm::mat4{1.0F}, scale);
        AddInstance(t * r * s);
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

        for (auto &i : meshes_) {
            if (auto mesh = i.lock()) {
                mesh->Save();
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

        for (auto &i : meshes_) {
            if (auto mesh = i.lock()) {
                mesh->Restore();
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

        for (auto &i : meshes_) {
            if (auto mesh = i.lock()) {
                mesh->Clear();
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
    std::vector<std::weak_ptr<Mesh>> meshes_;

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

    auto GetUnusedMesh() -> size_t {
        for (size_t i = 0; i < meshes_.size(); ++i) {
            if (meshes_[i].expired()) {
                return i;
            }
        }

        meshes_.emplace_back();
        return meshes_.size() - 1;
    }

    friend class WindowRenderer;
};

}  // namespace glviskit
