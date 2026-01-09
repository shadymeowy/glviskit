#pragma once

#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "base_controller.hpp"

namespace glviskit {

class SphericalController : public BaseController {
   public:
    explicit SphericalController() : BaseController() {};

    void Update(float dt, Camera &camera) override {
        // acquire velocity accumulations
        accum_rot_ += velocity_rot_ * key_rot_sensitivity_ * dt;
        distance_accum_ += distance_speed_ * key_move_sensitivity_ * dt;

        // update rotation
        auto rot = camera.GetRotation();
        rot += accum_rot_;
        camera.SetRotation(rot);

        // update distance
        float distance = camera.GetDistance();
        distance += distance_accum_;
        distance = std::max(distance, 0.0F);
        camera.SetDistance(distance);

        // clear accumulators
        accum_rot_ = glm::vec3{0.0F};
        distance_accum_ = 0.0F;
    }

    void KeyEvent(SDL_Keycode key, bool pressed) override {
        auto sgn = pressed ? 1.0F : -1.0F;
        switch (key) {
            case SDLK_LEFT:
                velocity_rot_.y -= sgn;
                break;
            case SDLK_RIGHT:
                velocity_rot_.y += sgn;
                break;
            case SDLK_UP:
                velocity_rot_.x -= sgn;
                break;
            case SDLK_DOWN:
                velocity_rot_.x += sgn;
                break;
            case SDLK_W:
                distance_speed_ -= sgn;
                break;
            case SDLK_S:
                distance_speed_ += sgn;
                break;
            default:
                break;
        }

        // clamp velocities to max sensitivities
        velocity_rot_ = glm::clamp(velocity_rot_, glm::vec3{-1}, glm::vec3{1});
        distance_speed_ = std::clamp(distance_speed_, -1.0F, 1.0F);
    }

    void ButtonEvent(uint8_t button, bool pressed) override {
        if (button == SDL_BUTTON_LEFT) {
            mouse_left_ = pressed;
        }
    }

    void MouseMotionEvent(float xrel, float yrel) override {
        if (!mouse_left_) {
            return;
        }
        accum_rot_.y -= mouse_sensitivity_ * xrel;
        accum_rot_.x -= mouse_sensitivity_ * yrel;
    }

    void MouseWheelEvent(float y) override {
        distance_accum_ -= wheel_sensitivity_ * y;
    }

    void SetMouseSensitivity(float s) { mouse_sensitivity_ = s; }
    auto GetMouseSensitivity(float &s) const { s = mouse_sensitivity_; }
    void SetWheelSensitivity(float s) { wheel_sensitivity_ = s; }
    auto GetWheelSensitivity(float &s) const { s = wheel_sensitivity_; }
    void SetKeyRotSensitivity(float s) { key_rot_sensitivity_ = s; }
    auto GetKeyRotSensitivity(float &s) const { s = key_rot_sensitivity_; }
    void SetKeyMoveSensitivity(float s) { key_move_sensitivity_ = s; }
    auto GetKeyMoveSensitivity(float &s) const { s = key_move_sensitivity_; }

   private:
    // mouse state
    bool mouse_left_ = false;

    // sensitivity settings
    float mouse_sensitivity_ = 0.01F;
    float wheel_sensitivity_ = 0.9F;
    float key_rot_sensitivity_ = 1.0F;
    float key_move_sensitivity_ = 5.0F;

    // movement speeds and accumulators
    float distance_speed_ = 0.0F;
    float distance_accum_ = 0.0F;
    glm::vec3 velocity_rot_{0.0F};
    glm::vec3 accum_rot_{0.0F};
};

}  // namespace glviskit