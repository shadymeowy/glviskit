#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/shared_ptr.h>

#include <glm/gtc/type_ptr.hpp>

#include "glviskit.hpp"

namespace nb = nanobind;
using namespace nb::literals;

using Points =
    nb::ndarray<float, nb::shape<-1, 3>, nb::c_contig, nb::device::cpu>;

NB_MODULE(glviskit, m) {
    nb::set_leak_warnings(false);

    m.doc() = "Python bindings for glviskit";

    m.def("create_window", &glviskit::CreateWindow, "width"_a = 800,
          "height"_a = 600, "title"_a = "glviskit Window");
    m.def("create_render_buffer", &glviskit::CreateRenderBuffer,
          "Create a new RenderBuffer");
    m.def("get_time_seconds", &glviskit::GetTimeSeconds,
          "Get the current time in seconds since the program started");
    m.def("loop", &glviskit::Loop, nb::call_guard<nb::gil_scoped_release>(),
          "Run the event loop for single iteration and render all windows");
    m.def("render", &glviskit::Render, nb::call_guard<nb::gil_scoped_release>(),
          "Render all windows without processing events");

    nb::class_<glviskit::sdl::Window>(m, "Window")
        .def("add_render_buffer", &glviskit::sdl::Window::AddRenderBuffer,
             "rb"_a, "Add a RenderBuffer to the window for rendering")
        .def_prop_rw("camera", &glviskit::sdl::Window::GetCamera,
                     &glviskit::sdl::Window::SetCamera, "Camera of the window")
        .def("make_current", &glviskit::sdl::Window::MakeCurrent,
             "Make the window's OpenGL context current")
        .def("render", &glviskit::sdl::Window::Render,
             "Render the window's contents");

    nb::class_<glviskit::Camera>(m, "Camera")
        .def(
            "calculate_transform",
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
            },
            "Calculate the camera transformation matrix")
        .def("perspective_fov", &glviskit::Camera::PerspectiveFov, "hfov"_a,
             "vfov"_a, "near"_a = 0.1F, "far"_a = 100.0F,
             "Set perspective projection using horizontal and vertical FOV")
        .def("perspective", &glviskit::Camera::Perspective, "fxn"_a, "fyn"_a,
             "cx"_a = 0.5F, "cy"_a = 0.5F, "near"_a = 0.1F, "far"_a = 100.0F,
             "Set perspective projection using focal lengths and principal "
             "point")
        .def_prop_rw(
            "position",
            [](const glviskit::Camera &cam) {
                glm::vec3 pos = cam.GetPosition();
                return std::array<float, 3>{pos.x, pos.y, pos.z};
            },
            [](glviskit::Camera &cam, const std::array<float, 3> &pos) {
                cam.SetPosition(glm::make_vec3(pos.data()));
            },
            "Position of the camera")
        .def_prop_rw(
            "rotation",
            [](const glviskit::Camera &cam) {
                glm::vec3 rot = cam.GetRotation();
                return std::array<float, 3>{rot.x, rot.y, rot.z};
            },
            [](glviskit::Camera &cam, const std::array<float, 3> &rot) {
                cam.SetRotation(glm::make_vec3(rot.data()));
            },
            "Rotation of the camera (roll, pitch, yaw in radians)")
        .def_prop_rw("distance", &glviskit::Camera::GetDistance,
                     &glviskit::Camera::SetDistance,
                     "Distance of the camera from its center")
        .def_prop_ro(
            "viewport_size",
            [](const glviskit::Camera &cam) {
                glm::vec2 vp = cam.GetViewportSize();
                return std::array<float, 2>{vp.x, vp.y};
            },
            "Viewport size of the camera")
        .def_prop_rw("preserve_aspect_ratio",
                     &glviskit::Camera::GetPreserveAspectRatio,
                     &glviskit::Camera::SetPreserveAspectRatio,
                     "Whether to preserve aspect ratio when resizing viewport");

    nb::class_<glviskit::RenderBuffer>(m, "RenderBuffer")
        .def(
            "line",
            [](glviskit::RenderBuffer &rb, const std::array<float, 3> &start,
               const std::array<float, 3> &end) {
                rb.Line(glm::make_vec3(start.data()),
                        glm::make_vec3(end.data()));
            },
            "start"_a, "end"_a, "Draw a line from start to end")
        .def(
            "line",
            [](glviskit::RenderBuffer &rb, const Points &starts,
               const Points &ends) {
                auto s = starts.view();
                auto e = ends.view();
                for (size_t i = 0; i < s.shape(0); ++i) {
                    rb.Line({s(i, 0), s(i, 1), s(i, 2)},
                            {e(i, 0), e(i, 1), e(i, 2)});
                }
            },
            "starts"_a, "ends"_a, "Draw multiple lines from starts to ends")
        .def(
            "point",
            [](glviskit::RenderBuffer &rb, const std::array<float, 3> &p) {
                rb.Point(glm::make_vec3(p.data()));
            },
            "p"_a, "Draw a point at position p")
        .def(
            "point",
            [](glviskit::RenderBuffer &rb, const Points &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    for (size_t j = 0; j < v.shape(1); ++j) {
                        rb.Point({v(i, 0), v(i, 1), v(i, 2)});
                    }
                }
            },
            "points"_a, "Draw multiple points at given positions")
        .def(
            "line_to",
            [](glviskit::RenderBuffer &rb, const std::array<float, 3> &p) {
                rb.LineTo(glm::make_vec3(p.data()));
            },
            "p"_a, "Draw a line to position p")
        .def(
            "line_to",
            [](glviskit::RenderBuffer &rb, const Points &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.LineTo({v(i, 0), v(i, 1), v(i, 2)});
                }
            },
            "points"_a, "Call line_to for multiple points consecutively")
        .def("line_end", &glviskit::RenderBuffer::LineEnd,
             "End the current line sequence")
        .def(
            "anchored_square",
            [](glviskit::RenderBuffer &rb, const std::array<float, 3> &pos) {
                rb.AnchoredSquare(glm::make_vec3(pos.data()));
            },
            "pos"_a, "Draw an anchored square at position pos")
        .def(
            "anchored_square",
            [](glviskit::RenderBuffer &rb, const Points &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.AnchoredSquare({v(i, 0), v(i, 1), v(i, 2)});
                }
            },
            "points"_a, "Draw multiple anchored squares at given positions")
        .def(
            "color",
            [](glviskit::RenderBuffer &rb, const std::array<float, 4> &c) {
                rb.Color({c[0], c[1], c[2], c[3]});
            },
            "c"_a, "Set the current drawing color")
        .def("size", &glviskit::RenderBuffer::Size, "size"_a,
             "Set the current drawing size")
        .def(
            "add_instance",
            [](glviskit::RenderBuffer &rb, const std::array<float, 3> &pos,
               const std::array<float, 3> &rot,
               const std::array<float, 3> &scale) {
                rb.AddInstance(glm::make_vec3(pos.data()),
                               glm::make_vec3(rot.data()),
                               glm::make_vec3(scale.data()));
            },
            "pos"_a = std::array<float, 3>{0.0f, 0.0f, 0.0f},
            "rot"_a = std::array<float, 3>{0.0f, 0.0f, 0.0f},
            "scale"_a = std::array<float, 3>{1.0f, 1.0f, 1.0f},
            "Add an instance with given position, rotation and scale")
        .def("save", &glviskit::RenderBuffer::Save,
             "Save the current render buffer state")
        .def("restore", &glviskit::RenderBuffer::Restore,
             "Restore the previously saved render buffer state")
        .def("clear", &glviskit::RenderBuffer::Clear, "Clear the render buffer")
        .def("save_instances", &glviskit::RenderBuffer::SaveInstances,
             "Save the current instances")
        .def("restore_instances", &glviskit::RenderBuffer::RestoreInstances,
             "Restore the previously saved instances")
        .def("clear_instances", &glviskit::RenderBuffer::ClearInstances,
             "Clear the instances");
}
