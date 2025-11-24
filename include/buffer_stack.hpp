#pragma once

#include "glad.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "buffer_object.hpp"

namespace glviskit {

template <typename T, GLenum TYPE = GL_ARRAY_BUFFER>
class BufferStack {
   public:
    explicit BufferStack(GladGLContext &gl, size_t capacity = 4)
        : gl{gl}, buffer{gl, capacity}, size{0}, restore_point{0} {}

    void Append(const T &element) { elements.push_back(element); }

    bool Sync() {
        // check is there anything to sync
        if (size == elements.size()) {
            return false;
        }

        // check if we need to reallocate
        bool reallocated = false;
        if (elements.size() > buffer.Size()) {
            // double the capacity until it fits
            size_t new_capacity = buffer.Size();
            while (elements.size() > new_capacity) {
                new_capacity *= 2;
            }

            // create new buffer object with new capacity
            // and get old buffer object for data copy
            auto old_buffer =
                std::exchange(buffer, BufferObject<T, TYPE>(gl, new_capacity));

            // copy old data
            gl.BindBuffer(GL_COPY_READ_BUFFER, old_buffer.Get());
            gl.BindBuffer(GL_COPY_WRITE_BUFFER, buffer.Get());
            gl.CopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0,
                                 0, size * sizeof(T));
            gl.BindBuffer(GL_COPY_READ_BUFFER, 0);
            gl.BindBuffer(GL_COPY_WRITE_BUFFER, 0);

            reallocated = true;
        }

        // map the rest of the buffer and copy new data
        buffer.Bind();
        void *ptr = gl.MapBufferRange(
            TYPE, size * sizeof(T), (elements.size() - size) * sizeof(T),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
        std::copy(elements.data() + size, elements.data() + elements.size(),
                  static_cast<T *>(ptr));
        gl.UnmapBuffer(TYPE);
        buffer.Unbind();

        size = elements.size();
        return reallocated;
    }

    void Save() { restore_point = elements.size(); }

    void Restore() {
        elements.resize(restore_point);
        size = std::min(size, restore_point);
    }

    void Clear() {
        elements.clear();
        size = 0;
    }

    GLuint Get() const { return buffer.Get(); }
    void Bind() { buffer.Bind(); }
    void Unbind() { buffer.Unbind(); }

    size_t Capacity() const { return buffer.Size(); }
    size_t Size() const { return elements.size(); }

   private:
    size_t size;
    size_t restore_point;
    std::vector<T> elements;

    GladGLContext &gl;
    BufferObject<T, TYPE> buffer;
};

}  // namespace glviskit