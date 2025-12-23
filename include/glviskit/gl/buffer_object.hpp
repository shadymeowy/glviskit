#pragma once

#include "../gl/gl.hpp"

#include <cstddef>

namespace glviskit {

template <typename T, GLenum TYPE = GL_ARRAY_BUFFER,
          GLenum USAGE = GL_DYNAMIC_DRAW>
class BufferObject {
   public:
    explicit BufferObject(size_t size)
        : size_{size} {
        glGenBuffers(1, &buffer);
        Bind();
        glBufferData(TYPE, size * sizeof(T), nullptr, USAGE);
        Unbind();
    }

    // destructor
    ~BufferObject() { glDeleteBuffers(1, &buffer); }

    // this class is non-copyable
    BufferObject(const BufferObject &) = delete;
    auto operator=(const BufferObject &) -> BufferObject & = delete;

    // movable
    BufferObject(BufferObject &&other) noexcept
        : size_(other.size_), buffer(other.buffer) {
        other.size_ = 0;
        other.buffer = 0;
    }

    auto operator=(BufferObject &&other) noexcept -> BufferObject & {
        if (this != &other) {
            glDeleteBuffers(1, &buffer);

            size_ = other.size_;
            buffer = other.buffer;

            other.size_ = 0;
            other.buffer = 0;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> GLuint { return buffer; }
    void Bind() { glBindBuffer(TYPE, buffer); }
    void Unbind() { glBindBuffer(TYPE, 0); }
    [[nodiscard]] auto Size() const -> size_t { return size_; }

   private:
    size_t size_;

    GLuint buffer{};
};

}  // namespace glviskit