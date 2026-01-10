#pragma once

#include "../camera.hpp"
#include "../sdl/sdl.hpp"

namespace glviskit {

class BaseController {
   public:
    // update camera based on elapsed time
    virtual void Update(float delta_time, Camera &camera) = 0;

    // input event handlers
    virtual void KeyEvent(SDL_Keycode /*key*/, bool /*pressed*/) = 0;
    virtual void ButtonEvent(uint8_t /*button*/, bool /*pressed*/) = 0;
    virtual void MouseMotionEvent(float /*xrel*/, float /*yrel*/) = 0;
    virtual void MouseWheelEvent(float /*y*/) = 0;

    virtual ~BaseController() = default;
};

}  // namespace glviskit