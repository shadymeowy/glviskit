#pragma once

#include <array>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>
#include <stdexcept>

namespace glviskit {

class Camera {
   public:
    Camera() { PerspectiveFov(glm::radians(60.0F), glm::radians(60.0F)); }

    // get the transformation matrix of the camera
    // M = M_intr * T_spherical * R_cam^-1 * T_cam^-1
    // where:
    //     M_intr: intrinsic matrix (projection + viewport scaling)
    //     T_spherical: translation matrix for spherical camera
    //     R_cam: rotation matrix of the camera center
    //     T_cam: translation matrix of the camera center

    [[nodiscard]] auto CalculateTransform() const -> glm::mat4 {
        auto acc = glm::mat4(1.0F);
        const auto ar = glm::mat4{axis_rot_};

        // translation inverse
        auto t = glm::translate(glm::mat4{1.0F}, -position);
        acc = t * acc;

        // axis convention remap in world space
        acc = ar * acc;

        // rotation inverse
        auto ri = glm::mat4(1.0F);
        // roll/pitch/yaw hence z x y
        ri = glm::rotate(ri, -rotation.z, glm::vec3(0, 0, 1));
        ri = glm::rotate(ri, -rotation.x, glm::vec3(1, 0, 0));
        ri = glm::rotate(ri, -rotation.y, glm::vec3(0, 1, 0));
        acc = ri * acc;

        // translation for spherical camera
        auto s =
            glm::translate(glm::mat4{1.0F}, glm::vec3(0.0F, 0.0F, -distance));
        acc = s * acc;

        auto mi = m_intrinsic;

        float width = viewport.x;
        float height = viewport.y;

        // adjust for aspect ratio preservation
        float new_ratio = height / width;
        if (preserve_aspect_ratio) {
            if (new_ratio < aspect_ratio) {
                width = height / aspect_ratio;
            } else {
                height = width * aspect_ratio;
            }
        }

        // viewport scaling
        mi[0][0] *= width / viewport.x;
        mi[1][1] *= height / viewport.y;
        acc = mi * acc;
        return acc;
    }

    void PerspectiveFov(float hfov, float vfov, float near_ = 0.1F,
                        float far_ = 100.0F) {
        float fxn = 0.5F / tanf(glm::radians(hfov) / 2.0F);
        float fyn = 0.5F / tanf(glm::radians(vfov) / 2.0F);
        Perspective(fxn, fyn, 0.5F, 0.5F, near_, far_);
    }

    void Perspective(float fxn, float fyn, float cx = 0.5, float cy = 0.5,
                     float near_ = 0.1, float far_ = 100.0F) {
        m_intrinsic = glm::mat4(0.0F);
        m_intrinsic[0][0] = 2 * fxn;
        m_intrinsic[1][1] = 2 * fyn;
        m_intrinsic[2][0] = (2 * cx) - 1;
        m_intrinsic[2][1] = (2 * cy) - 1;
        m_intrinsic[2][2] = -(far_ + near_) / (far_ - near_);
        m_intrinsic[3][2] = -2.0F * far_ * near_ / (far_ - near_);
        m_intrinsic[2][3] = -1.0F;
        aspect_ratio = fxn / fyn;
    }

    // position, rotation, distance, viewport size getters/setters
    void SetPosition(const glm::vec3 &position) { this->position = position; }
    void SetRotation(const glm::vec3 &rotation) { this->rotation = rotation; }
    [[nodiscard]] auto GetPosition() const -> glm::vec3 { return position; }
    [[nodiscard]] auto GetRotation() const -> glm::vec3 { return rotation; }
    void SetDistance(float distance) { this->distance = distance; }
    [[nodiscard]] auto GetDistance() const -> float { return distance; }
    void SetViewportSize(glm::vec2 size) { viewport = size; }
    [[nodiscard]] auto GetViewportSize() const -> glm::vec2 { return viewport; }

    // aspect ratio preservation under viewport resize
    void SetPreserveAspectRatio(bool preserve) {
        preserve_aspect_ratio = preserve;
    }

    [[nodiscard]] auto GetPreserveAspectRatio() const -> bool {
        return preserve_aspect_ratio;
    }

    void SetAxisRotation(const glm::quat &rotation) {
        const auto q = glm::normalize(rotation);
        axis_rot_ = glm::mat3_cast(q);
        axis_rot_inv_ = glm::transpose(axis_rot_);
    }

    [[nodiscard]] auto GetAxisRotation() const -> glm::quat {
        return glm::quat_cast(axis_rot_);
    }

    void SetAxisRotation(int x, int y, int z, bool inv_x = false,
                         bool inv_y = false, bool inv_z = false) {
        const std::array<int, 3> perm{x, y, z};
        std::array<bool, 3> used{false, false, false};
        for (int i : perm) {
            if (i < 0 || i > 2 || used[i]) {
                throw std::invalid_argument(
                    "SetAxisRotation(x, y, z, inv_x, inv_y, inv_z) requires "
                    "a permutation of 0, 1, 2");
            }
            used[i] = true;
        }

        glm::mat3 rot{0.0F};
        rot[x][0] = inv_x ? -1.0F : 1.0F;
        rot[y][1] = inv_y ? -1.0F : 1.0F;
        rot[z][2] = inv_z ? -1.0F : 1.0F;

        axis_rot_ = rot;
        axis_rot_inv_ = glm::transpose(axis_rot_);
    }

    // for handling time-based updates
    void Update(float deltaTime);

   private:
    // intrinsic matrix
    glm::mat4 m_intrinsic{1.0F};

    // viewport size (width, height)
    glm::vec2 viewport{1.0F, 1.0F};

    // position and rotation of camera center
    glm::vec3 position = glm::vec3(0.0F);
    glm::vec3 rotation = glm::vec3(0.0F);

    // spherical camera distance
    float distance{0.0F};

    // axis-convention rotation and its inverse
    glm::mat3 axis_rot_{1.0F};
    glm::mat3 axis_rot_inv_{1.0F};

    // aspect ratio preservation
    bool preserve_aspect_ratio{true};
    float aspect_ratio{1.0F};
};

}  // namespace glviskit