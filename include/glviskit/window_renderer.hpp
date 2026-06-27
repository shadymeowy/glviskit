#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include "camera.hpp"
#include "gl/gl.hpp"
#include "gl/texture.hpp"
#include "primitive/circle.hpp"
#include "primitive/line.hpp"
#include "primitive/mesh.hpp"
#include "primitive/point.hpp"
#include "primitive/symbol.hpp"
#include "render_list.hpp"

namespace glviskit {

class WindowRenderer {
   public:
    WindowRenderer() : camera{std::make_shared<Camera>()} {}

    void Render(GLuint ctx_id, int _width, int _height) {
        // if gl context not initialized, do it now
        if (!initialized_) {
            InitializeContext();
        }

        const auto width = static_cast<float>(_width);
        const auto height = static_cast<float>(_height);

        // update camera viewport size
        camera->SetViewportSize({width, height});

        // get camera transform matrix
        auto mvp = camera->CalculateTransform();

        // set viewport
        glViewport(0, 0, _width, _height);

        // opaque objects first
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        // clear buffers
        glClearColor(background_color.r, background_color.g, background_color.b,
                     background_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw everything with alpha test 1
        RenderLists(ctx_id, width, height, mvp, 1);

        // render transparent objects
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        // draw everything with alpha test 0
        RenderLists(ctx_id, width, height, mvp, 0);
    }

    void AddRenderList(const std::shared_ptr<RenderList> &render_list) {
        buffers.push_back(render_list->render_state_);
    }

    auto GetCamera() -> std::shared_ptr<Camera> { return camera; }
    void SetCamera(std::shared_ptr<Camera> cam) { camera = std::move(cam); }
    void SetBackgroundColor(const glm::vec4 &color) {
        background_color = color;
    }
    [[nodiscard]] auto GetBackgroundColor() const -> glm::vec4 {
        return background_color;
    }

    void SetSymbolAtlas(const unsigned char *pixels, int width, int height,
                        int channels, float px_range) {
        if (!symbol_atlas_) {
            symbol_atlas_ = std::make_unique<Texture>();
        }
        symbol_atlas_->Upload(pixels, width, height, channels);
        symbol_px_range_ = px_range;
    }

    static void CaptureRGBA(int width, int height,
                            std::span<unsigned char> pixels) {
        const size_t expected = static_cast<size_t>(width) * height * 4;
        if (pixels.size() != expected) {
            throw std::invalid_argument(
                "CaptureRGBA buffer size must equal width * height * 4");
        }

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                     pixels.data());

        const size_t stride = static_cast<size_t>(width) * 4;
        std::vector<unsigned char> flipped(expected);
        for (int y = 0; y < height; ++y) {
            const size_t src = static_cast<size_t>(y) * stride;
            const size_t dst = static_cast<size_t>(height - 1 - y) * stride;
            std::copy_n(pixels.data() + src, stride, flipped.data() + dst);
        }
        std::ranges::copy(flipped, pixels.begin());
    }

   private:
    void InitializeContext() {
        program_line = std::make_unique<line::Program>();
        program_point = std::make_unique<point::Program>();
        program_circle = std::make_unique<circle::Program>();
        program_mesh = std::make_unique<mesh::Program>();
        program_symbol_ = std::make_unique<symbol::Program>();

        glDisable(GL_CULL_FACE);
#ifdef GLVISKIT_GL33
        glEnable(GL_MULTISAMPLE);
#endif
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LESS);

        initialized_ = true;
    }

    void RenderLists(GLuint ctx_id, float width, float height,
                     const glm::mat4 &mvp, int alpha_test) {
        // Render all line buffers
        program_line->Use();
        program_line->SetScreenSize({width, height});
        program_line->SetMVP(mvp);
        program_line->SetAlphaTest(alpha_test);
        for (auto &buf : buffers) {
            if (!buf->enabled_) {
                continue;
            }
            buf->line_buffer_.Render(ctx_id);
        }

        // Render all point buffers
        program_point->Use();
        program_point->SetScreenSize({width, height});
        program_point->SetMVP(mvp);
        program_point->SetAlphaTest(alpha_test);
        for (auto &buf : buffers) {
            if (!buf->enabled_) {
                continue;
            }
            buf->point_buffer_.Render(ctx_id);
        }

        // Render all circle buffers
        program_circle->Use();
        program_circle->SetScreenSize({width, height});
        program_circle->SetMVP(mvp);
        program_circle->SetAlphaTest(alpha_test);
        for (auto &buf : buffers) {
            if (!buf->enabled_) {
                continue;
            }
            buf->circle_buffer_.Render(ctx_id);
        }

        // Render all mesh buffers
        program_mesh->Use();
        program_mesh->SetMVP(mvp);
        program_mesh->SetAlphaTest(alpha_test);
        for (auto &buf : buffers) {
            if (!buf->enabled_) {
                continue;
            }
            buf->mesh_buffer_.Render(ctx_id);
        }

        // Render all symbol buffers, sampled from the atlas
        if (symbol_atlas_ && symbol_atlas_->Loaded()) {
            program_symbol_->Use();
            program_symbol_->SetScreenSize({width, height});
            program_symbol_->SetMVP(mvp);
            program_symbol_->SetAlphaTest(alpha_test);
            program_symbol_->SetPxRange(symbol_px_range_);
            symbol_atlas_->Bind(0);
            program_symbol_->SetAtlas(0);
            for (auto &buf : buffers) {
                if (!buf->enabled_) {
                    continue;
                }
                buf->symbol_buffer_.Render(ctx_id);
            }
        }
    }

    // TODO: share programs across multiple renderers?
    std::unique_ptr<line::Program> program_line{nullptr};
    std::unique_ptr<point::Program> program_point{nullptr};
    std::unique_ptr<circle::Program> program_circle{nullptr};
    std::unique_ptr<mesh::Program> program_mesh{nullptr};
    std::unique_ptr<symbol::Program> program_symbol_{nullptr};

    // msdf symbol atlas (created lazily once the context exists)
    std::unique_ptr<Texture> symbol_atlas_{nullptr};
    float symbol_px_range_{0.0F};

    // make camera shareable across windows
    std::shared_ptr<Camera> camera;
    glm::vec4 background_color{0.0F, 0.0F, 0.0F, 1.0F};
    bool initialized_{false};

    std::vector<std::shared_ptr<RenderState>> buffers;
};

}  // namespace glviskit