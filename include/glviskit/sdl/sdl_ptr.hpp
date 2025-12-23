#pragma once

// a generic raii wrapper for SDL resources

namespace glviskit::sdl {

template <typename T, typename U, U (*Deleter)(T)>
class SDLPtr {
   public:
    explicit SDLPtr(T handle = nullptr) : ptr{handle} {}

    ~SDLPtr() {
        if (ptr != nullptr) {
            Deleter(ptr);
        }
    }

    // uncopyable but movable
    SDLPtr(const SDLPtr &) = delete;
    auto operator=(const SDLPtr &) -> SDLPtr & = delete;

    SDLPtr(SDLPtr &&other) noexcept : ptr{other.ptr} { other.ptr = nullptr; }

    auto operator=(SDLPtr &&other) noexcept -> SDLPtr & {
        if (this != &other) {
            if (ptr != nullptr) {
                Deleter(ptr);
            }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    [[nodiscard]] auto Get() const -> T { return ptr; }

   private:
    T ptr;
};

}  // namespace glviskit::sdl