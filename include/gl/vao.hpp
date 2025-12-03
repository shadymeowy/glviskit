#pragma once

#include "gl/glad.hpp"

namespace glviskit {

class VAO {
   public:
    explicit VAO(GladGLContext &gl) : gl{gl} { gl.GenVertexArrays(1, &vao); }

    // destructor
    ~VAO() { gl.DeleteVertexArrays(1, &vao); }

    // this class is non-copyable
    VAO(const VAO &) = delete;
    auto operator=(const VAO &) -> VAO & = delete;

    // but movable
    VAO(VAO &&other) noexcept : gl{other.gl}, vao{other.vao} { other.vao = 0; }

    auto operator=(VAO &&other) noexcept -> VAO & {
        if (this != &other) {
            gl.DeleteVertexArrays(1, &vao);

            gl = other.gl;
            vao = other.vao;

            other.vao = 0;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> GLuint { return vao; }
    void Bind() const { gl.BindVertexArray(vao); }
    void Unbind() const { gl.BindVertexArray(0); }

   private:
    GladGLContext &gl;
    GLuint vao{};
};

}  // namespace glviskit