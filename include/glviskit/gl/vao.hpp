#pragma once

#include "../gl/gl.hpp"

namespace glviskit {

class VAO {
   public:
    VAO() { glGenVertexArrays(1, &vao); }

    // destructor
    ~VAO() { glDeleteVertexArrays(1, &vao); }

    // this class is non-copyable
    VAO(const VAO &) = delete;
    auto operator=(const VAO &) -> VAO & = delete;

    // but movable
    VAO(VAO &&other) noexcept : vao{other.vao} { other.vao = 0; }

    auto operator=(VAO &&other) noexcept -> VAO & {
        if (this != &other) {
            glDeleteVertexArrays(1, &vao);

            vao = other.vao;

            other.vao = 0;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> GLuint { return vao; }
    void Bind() const { glBindVertexArray(vao); }
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void Unbind() const { glBindVertexArray(0); }

   private:
    GLuint vao{};
};

}  // namespace glviskit