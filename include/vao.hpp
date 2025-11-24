#pragma once

#include "glad.hpp"

namespace glviskit {

class VAO {
   public:
    explicit VAO(GladGLContext &gl) : gl{gl}, vao{0} {
        gl.GenVertexArrays(1, &vao);
    }

    // destructor
    ~VAO() { gl.DeleteVertexArrays(1, &vao); }

    // this class is non-copyable
    VAO(const VAO &) = delete;
    VAO &operator=(const VAO &) = delete;

    // but movable
    VAO(VAO &&other) noexcept : gl{other.gl}, vao{other.vao} { other.vao = 0; }

    VAO &operator=(VAO &&other) noexcept {
        if (this != &other) {
            gl.DeleteVertexArrays(1, &vao);

            gl = other.gl;
            vao = other.vao;

            other.vao = 0;
        }
        return *this;
    }

    GLuint Get() const { return vao; }
    void Bind() { gl.BindVertexArray(vao); }
    void Unbind() { gl.BindVertexArray(0); }

   private:
    GladGLContext &gl;
    GLuint vao;
};

}  // namespace glviskit