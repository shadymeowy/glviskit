#pragma once

#include "glad.hpp"

#include <cstddef>

namespace glviskit {

template <typename T, GLenum TYPE = GL_ARRAY_BUFFER,
          GLenum USAGE = GL_DYNAMIC_DRAW>
class BufferObject {
   public:
    explicit BufferObject(GladGLContext &gl, size_t size)
        : gl{gl}, size_{size} {
        gl.GenBuffers(1, &buffer);
        Bind();
        gl.BufferData(TYPE, size * sizeof(T), nullptr, USAGE);
        Unbind();
    }

    // destructor
    ~BufferObject() { gl.DeleteBuffers(1, &buffer); }

    // this class is non-copyable
    BufferObject(const BufferObject &) = delete;
    BufferObject &operator=(const BufferObject &) = delete;

    // movable
    BufferObject(BufferObject &&other) noexcept
        : size_(other.size_), gl(other.gl), buffer(other.buffer) {
        other.size_ = 0;
        other.buffer = 0;
    }

    BufferObject &operator=(BufferObject &&other) noexcept {
        if (this != &other) {
            gl.DeleteBuffers(1, &buffer);

            size_ = other.size_;
            gl = other.gl;
            buffer = other.buffer;

            other.size_ = 0;
            other.buffer = 0;
        }
        return *this;
    }

    GLuint Get() const { return buffer; }
    void Bind() { gl.BindBuffer(TYPE, buffer); }
    void Unbind() { gl.BindBuffer(TYPE, 0); }
    size_t Size() const { return size_; }

   private:
    size_t size_;

    GladGLContext &gl;
    GLuint buffer;
};

}  // namespace glviskit