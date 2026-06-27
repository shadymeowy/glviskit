#pragma once

#include "../gl/gl.hpp"

namespace glviskit {

// a 2D texture, used for the symbol atlas
class Texture {
   public:
    Texture() { glGenTextures(1, &texture); }

    ~Texture() { glDeleteTextures(1, &texture); }

    // non-copyable
    Texture(const Texture &) = delete;
    auto operator=(const Texture &) -> Texture & = delete;

    // movable
    Texture(Texture &&other) noexcept
        : texture{other.texture},
          width_{other.width_},
          height_{other.height_},
          loaded_{other.loaded_} {
        other.texture = 0;
        other.loaded_ = false;
    }

    auto operator=(Texture &&other) noexcept -> Texture & {
        if (this != &other) {
            glDeleteTextures(1, &texture);
            texture = other.texture;
            width_ = other.width_;
            height_ = other.height_;
            loaded_ = other.loaded_;
            other.texture = 0;
            other.loaded_ = false;
        }
        return *this;
    }

    // upload pixel data; channels is 3 (RGB) or 4 (RGBA)
    void Upload(const unsigned char *pixels, int width, int height,
                int channels) {
        const GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width,
                     height, 0, format, GL_UNSIGNED_BYTE, pixels);

        // linear filtering, clamped; msdf needs no mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        width_ = width;
        height_ = height;
        loaded_ = true;
    }

    void Bind(int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    [[nodiscard]] auto Loaded() const -> bool { return loaded_; }
    [[nodiscard]] auto Width() const -> int { return width_; }
    [[nodiscard]] auto Height() const -> int { return height_; }

   private:
    GLuint texture{};
    int width_{0};
    int height_{0};
    bool loaded_{false};
};

}  // namespace glviskit
