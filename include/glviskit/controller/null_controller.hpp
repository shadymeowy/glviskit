#pragma once

#include "base_controller.hpp"

namespace glviskit {

class NullController : public BaseController {
   public:
    explicit NullController() : BaseController() {};

    // no-op implementations
    void Update(float /*delta_time*/, Camera & /*camera*/) override {}
    void KeyEvent(SDL_Keycode /*key*/, bool /*pressed*/) override {}
    void ButtonEvent(uint8_t /*button*/, bool /*pressed*/) override {}
    void MouseMotionEvent(float /*xrel*/, float /*yrel*/) override {}
    void MouseWheelEvent(float /*y*/) override {}
};

}  // namespace glviskit