#pragma once

#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "base_controller.hpp"

namespace glviskit {

class FirstPersonController : public BaseController {
   public:
    explicit FirstPersonController() : BaseController() {};

    void Update(float delta_time, Camera &camera) override {
        // apply accumulated changes
        accum_pos_ += velocity_pos_ * key_move_sensitivity_ * delta_time;
        accum_rot_ += velocity_rot_ * key_rot_sensitivity_ * delta_time;

        // update rotation
        auto rot = camera.GetRotation();
        rot += accum_rot_;
        camera.SetRotation(rot);

        // calculate forward, up, right directions
        glm::vec3 forward_dir{-sin(rot.y) * cos(rot.x), sin(rot.x),
                              -cos(rot.y) * cos(rot.x)};
        forward_dir = glm::normalize(forward_dir);
        auto up_dir = glm::vec3(0.0F, 1.0F, 0.0F);
        auto right_dir = glm::cross(forward_dir, up_dir);

        // construct rotation matrix
        glm::mat3 rotation_matrix{1.0F};
        rotation_matrix[0] = -right_dir;
        rotation_matrix[1] = up_dir;
        rotation_matrix[2] = forward_dir;

        // update position
        auto pos = camera.GetPosition();
        pos += rotation_matrix * accum_pos_;
        camera.SetPosition(pos);

        // clear accumulators
        accum_pos_ = glm::vec3{0.0F};
        accum_rot_ = glm::vec3{0.0F};
    }

    void KeyEvent(SDL_Keycode key, bool pressed) override {
        float sgn = pressed ? 1.0F : -1.0F;

        switch (key) {
            case SDLK_A:
                velocity_pos_.x += sgn;
                break;
            case SDLK_D:
                velocity_pos_.x -= sgn;
                break;
            case SDLK_W:
                velocity_pos_.z += sgn;
                break;
            case SDLK_S:
                velocity_pos_.z -= sgn;
                break;
            case SDLK_UP:
                velocity_rot_.x += sgn;
                break;
            case SDLK_DOWN:
                velocity_rot_.x -= sgn;
                break;
            case SDLK_LEFT:
                velocity_rot_.y += sgn;
                break;
            case SDLK_RIGHT:
                velocity_rot_.y -= sgn;
                break;
            default:
                break;
        }

        // clamp velocities to max sensitivities
        velocity_pos_ = glm::clamp(velocity_pos_, glm::vec3{-1}, glm::vec3{1});
        velocity_rot_ = glm::clamp(velocity_rot_, glm::vec3{-1}, glm::vec3{1});
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

    void MouseWheelEvent(float /*y*/) override {
        // no-op
    }

    void SetMouseSensitivity(float s) { mouse_sensitivity_ = s; }
    auto GetMouseSensitivity(float &s) const { s = mouse_sensitivity_; }
    void SetKeyRotSensitivity(float s) { key_rot_sensitivity_ = s; }
    auto GetKeyRotSensitivity(float &s) const { s = key_rot_sensitivity_; }
    void SetKeyMoveSensitivity(float s) { key_move_sensitivity_ = s; }
    auto GetKeyMoveSensitivity(float &s) const { s = key_move_sensitivity_; }

   private:
    // mouse state
    bool mouse_left_ = false;

    // sensitivity settings
    float mouse_sensitivity_ = 0.01F;
    float key_rot_sensitivity_ = 1.0F;
    float key_move_sensitivity_ = 5.0F;

    // movement speeds and accumulators
    glm::vec3 velocity_pos_{0.0F};
    glm::vec3 accum_pos_{0.0F};
    glm::vec3 velocity_rot_{0.0F};
    glm::vec3 accum_rot_{0.0F};
};

}  // namespace glviskit