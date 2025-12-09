#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/shared_ptr.h>

#include <glm/gtc/type_ptr.hpp>

#include "glviskit.hpp"

namespace nb = nanobind;

NB_MODULE(glviskit, m) {
    nb::set_leak_warnings(false);

    m.doc() = "Python bindings for glviskit";

    m.def("create_window", &glviskit::CreateWindow);
    m.def("create_render_buffer", &glviskit::CreateRenderBuffer);
    m.def("get_time_seconds", &glviskit::GetTimeSeconds);
    m.def("loop", &glviskit::Loop,
          nb::call_guard<nb::gil_scoped_release>());
    m.def("render", &glviskit::Render,
          nb::call_guard<nb::gil_scoped_release>());

    nb::class_<glviskit::sdl::Window>(m, "Window")
        .def("add_render_buffer", &glviskit::sdl::Window::AddRenderBuffer)
        .def_prop_rw("camera", &glviskit::sdl::Window::GetCamera,
                     &glviskit::sdl::Window::SetCamera)
        .def("make_current", &glviskit::sdl::Window::MakeCurrent)
        .def("render", &glviskit::sdl::Window::Render);

    nb::class_<glviskit::Camera>(m, "Camera")
        .def("calculate_transform",
             [](const glviskit::Camera &cam) {
                 std::array<float, 16> result{0};
                 auto mat = cam.CalculateTransform();
                 for (int i = 0; i < 4; i++) {
                     for (int j = 0; j < 4; j++) {
                         result[i * 4 + j] = mat[j][i];
                     }
                 }
                 return nb::ndarray<float, nb::numpy, nb::shape<4, 4>,
                                    nb::c_contig>(result.data());
             })
        .def("perspective_fov", &glviskit::Camera::PerspectiveFov)
        .def("perspective", &glviskit::Camera::Perspective)
        .def_prop_rw(
            "position",
            [](const glviskit::Camera &cam) {
                glm::vec3 pos = cam.GetPosition();
                return std::array<float, 3>{pos.x, pos.y, pos.z};
            },
            [](glviskit::Camera &cam, const std::array<float, 3> &pos) {
                cam.SetPosition(glm::make_vec3(pos.data()));
            })
        .def_prop_rw(
            "rotation",
            [](const glviskit::Camera &cam) {
                glm::vec3 rot = cam.GetRotation();
                return std::array<float, 3>{rot.x, rot.y, rot.z};
            },
            [](glviskit::Camera &cam, const std::array<float, 3> &rot) {
                cam.SetRotation(glm::make_vec3(rot.data()));
            })
        .def_prop_rw("distance", &glviskit::Camera::GetDistance,
                     &glviskit::Camera::SetDistance)
        .def_prop_rw(
            "viewport_size",
            [](const glviskit::Camera &cam) {
                glm::vec2 vp = cam.GetViewportSize();
                return std::array<float, 2>{vp.x, vp.y};
            },
            [](glviskit::Camera &cam, const std::array<float, 2> &vp) {
                cam.SetViewportSize(glm::make_vec2(vp.data()));
            })
        .def_prop_rw("preserve_aspect_ratio",
                     &glviskit::Camera::GetPreserveAspectRatio,
                     &glviskit::Camera::SetPreserveAspectRatio);

    nb::class_<glviskit::RenderBuffer>(m, "RenderBuffer")
        .def("line",
             [](glviskit::RenderBuffer &rb, const std::array<float, 3> &start,
                const std::array<float, 3> &end) {
                 rb.Line(glm::make_vec3(start.data()),
                         glm::make_vec3(end.data()));
             })
        .def("point",
             [](glviskit::RenderBuffer &rb, const std::array<float, 3> &p) {
                 rb.Point(glm::make_vec3(p.data()));
             })
        .def("line_to",
             [](glviskit::RenderBuffer &rb, const std::array<float, 3> &p) {
                 rb.LineTo(glm::make_vec3(p.data()));
             })
        .def("line_end", &glviskit::RenderBuffer::LineEnd)
        .def("anchored_square",
             [](glviskit::RenderBuffer &rb, const std::array<float, 3> &pos) {
                 rb.AnchoredSquare(glm::make_vec3(pos.data()));
             })
        .def("color",
             [](glviskit::RenderBuffer &rb, const std::array<float, 4> &c) {
                 rb.Color(glm::make_vec4(c.data()));
             })
        .def("size", &glviskit::RenderBuffer::Size)
        .def("add_instance",
             [](glviskit::RenderBuffer &rb, const std::array<float, 3> &pos,
                const std::array<float, 3> &rot,
                const std::array<float, 3> &scale) {
                 rb.AddInstance(glm::make_vec3(pos.data()),
                                glm::make_vec3(rot.data()),
                                glm::make_vec3(scale.data()));
             })
        .def("save", &glviskit::RenderBuffer::Save)
        .def("restore", &glviskit::RenderBuffer::Restore)
        .def("clear", &glviskit::RenderBuffer::Clear)
        .def("save_instances", &glviskit::RenderBuffer::SaveInstances)
        .def("restore_instances", &glviskit::RenderBuffer::RestoreInstances)
        .def("clear_instances", &glviskit::RenderBuffer::ClearInstances);
}
