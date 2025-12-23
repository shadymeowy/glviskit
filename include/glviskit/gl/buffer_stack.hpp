#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "../gl/gl.hpp"
#include "buffer_object.hpp"

namespace glviskit {

template <typename T, GLenum TYPE = GL_ARRAY_BUFFER>
class BufferStack {
   public:
    explicit BufferStack(size_t capacity = 4) : buffer{capacity} {}

    void Append(const T &element) { elements.push_back(element); }

    auto Sync() -> bool {
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
                std::exchange(buffer, BufferObject<T, TYPE>(new_capacity));

            // copy old data
            glBindBuffer(GL_COPY_READ_BUFFER, old_buffer.Get());
            glBindBuffer(GL_COPY_WRITE_BUFFER, buffer.Get());
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0,
                                size * sizeof(T));
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

            reallocated = true;
        }

        // map the rest of the buffer and copy new data
        buffer.Bind();

#if defined(__EMSCRIPTEN__)
        // slower emscripten compatible but slower
        glBufferSubData(TYPE, size * sizeof(T),
                        (elements.size() - size) * sizeof(T),
                        elements.data() + size);

#else
        // fast but not emscripten compatible
        void *ptr = glMapBufferRange(
            TYPE, size * sizeof(T), (elements.size() - size) * sizeof(T),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
        std::copy(elements.data() + size, elements.data() + elements.size(),
                  static_cast<T *>(ptr));
        glUnmapBuffer(TYPE);
#endif

        buffer.Unbind();

        size = elements.size();
        return reallocated;
    }

    void Save() { restore_point = elements.size(); }

    void Restore() {
        elements.resize(restore_point);

        size = (std::min)(size, restore_point);
    }

    void Clear() {
        elements.clear();
        size = 0;
    }

    [[nodiscard]] auto Get() const -> GLuint { return buffer.Get(); }
    void Bind() { buffer.Bind(); }
    void Unbind() { buffer.Unbind(); }

    [[nodiscard]] auto Capacity() const -> size_t { return buffer.Size(); }
    [[nodiscard]] auto Size() const -> size_t { return elements.size(); }

   private:
    size_t size{};
    size_t restore_point{};
    std::vector<T> elements;

    BufferObject<T, TYPE> buffer;
};

}  // namespace glviskit