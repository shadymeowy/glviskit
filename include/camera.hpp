#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

namespace glviskit {

class Camera {
   public:
    Camera() {
        SetPerspectiveFov(glm::radians(60.0F), glm::radians(60.0F));
    }

    // get the transformation matrix of the camera
    // M = M_intr * T_spherical * R_cam^-1 * T_cam^-1
    // where:
    //     M_intr: intrinsic matrix (projection + viewport scaling)
    //     T_spherical: translation matrix for spherical camera
    //     R_cam: rotation matrix of the camera center
    //     T_cam: translation matrix of the camera center

    [[nodiscard]] auto GetTransformMatrix() const -> glm::mat4 {
        auto acc = glm::mat4(1.0F);

        // translation inverse
        auto trans = glm::mat4(1.0F);
        trans = glm::translate(trans, -position);
        acc = trans * acc;

        // rotation inverse
        auto rot = glm::mat4(1.0F);
        // roll/pitch/yaw hence z x y
        rot = glm::rotate(rot, -rotation.z, glm::vec3(0, 0, 1));
        rot = glm::rotate(rot, -rotation.x, glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, -rotation.y, glm::vec3(0, 1, 0));
        acc = rot * acc;

        // translation for spherical camera
        auto sph = glm::mat4(1.0F);
        sph = glm::translate(sph, glm::vec3(0.0F, 0.0F, -distance));
        acc = sph * acc;

        auto mi = m_intrinsic;

        float width = this->width;
        float height = this->height;

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
        mi[0][0] *= width / this->width;
        mi[1][1] *= height / this->height;
        acc = mi * acc;
        return acc;
    }

    // set the viewport size
    void SetViewportSize(float width, float height) {
        this->width = width;
        this->height = height;
    }

    void SetPerspectiveFov(float hfov, float vfov) {
        float fxn = 0.5F / tanf(glm::radians(hfov) / 2.0F);
        float fyn = 0.5F / tanf(glm::radians(vfov) / 2.0F);
        SetPerspective(fxn, fyn);
    }

    void SetPerspective(float fxn, float fyn, float cx = 0.5, float cy = 0.5,
                        float near = 0.1, float far = 100.0F) {
        m_intrinsic = glm::mat4(0.0F);
        m_intrinsic[0][0] = 2 * fxn;
        m_intrinsic[1][1] = 2 * fyn;
        m_intrinsic[2][0] = (2 * cx) - 1;
        m_intrinsic[2][1] = (2 * cy) - 1;
        m_intrinsic[2][2] = -(far + near) / (far - near);
        m_intrinsic[3][2] = -2.0F * far * near / (far - near);
        m_intrinsic[2][3] = -1.0F;
        aspect_ratio = fxn / fyn;
    }

    // position, rotation, distance getters/setters
    void SetPosition(const glm::vec3 &position) { this->position = position; }
    void SetRotation(const glm::vec3 &rotation) { this->rotation = rotation; }
    [[nodiscard]] auto GetPosition() const -> glm::vec3 { return position; }
    [[nodiscard]] auto GetRotation() const -> glm::vec3 { return rotation; }
    void SetDistance(float distance) { this->distance = distance; }
    [[nodiscard]] auto GetDistance() const -> float { return distance; }

    // aspect ratio preservation under viewport resize
    void SetPreserveAspectRatio(bool preserve) {
        preserve_aspect_ratio = preserve;
    }

    [[nodiscard]] auto GetPreserveAspectRatio() const -> bool { return preserve_aspect_ratio; }

    // for handling time-based updates
    void Update(float deltaTime);

   private:
    // intrinsic matrix
    glm::mat4 m_intrinsic{1.0F};

    // viewport size
    float width{1.0F};
    float height{1.0F};

    // position and rotation of camera center
    glm::vec3 position = glm::vec3(0.0F);
    glm::vec3 rotation = glm::vec3(0.0F);

    // spherical camera distance
    float distance{0.0F};

    // aspect ratio preservation
    bool preserve_aspect_ratio{true};
    float aspect_ratio{1.0F};
};

}  // namespace glviskit