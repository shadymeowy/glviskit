#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>

#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <glviskit/glviskit.hpp>
#include <sstream>

namespace nb = nanobind;
using namespace nb::literals;

using Points32 =
    nb::ndarray<float, nb::shape<-1, 3>, nb::c_contig, nb::device::cpu>;
using Points64 =
    nb::ndarray<double, nb::shape<-1, 3>, nb::c_contig, nb::device::cpu>;
using Polygons32 =
    nb::ndarray<float, nb::shape<-1, -1, 3>, nb::c_contig, nb::device::cpu>;
using Polygons64 =
    nb::ndarray<double, nb::shape<-1, -1, 3>, nb::c_contig, nb::device::cpu>;
using IndicesI32 =
    nb::ndarray<int32_t, nb::shape<-1, 3>, nb::c_contig, nb::device::cpu>;
using Matrix44f =
    nb::ndarray<float, nb::shape<4, 4>, nb::c_contig, nb::device::cpu>;
using Matrix44d =
    nb::ndarray<double, nb::shape<4, 4>, nb::c_contig, nb::device::cpu>;

namespace {

template <typename View>
void RequireMatchingLineCount(const View &starts, const View &ends) {
    if (starts.shape(0) != ends.shape(0)) {
        throw nb::value_error(
            "line(starts, ends) requires the same number of start and end "
            "points");
    }
}

template <typename View>
void RequireAtLeastVertices(const View &vertices, size_t min_count,
                            const char *name) {
    if (vertices.shape(0) < min_count) {
        std::ostringstream oss;
        oss << name << " requires at least " << min_count << " vertices";
        throw nb::value_error(oss.str().c_str());
    }
}

template <typename IndexView>
void RequireNonNegativeIndices(const IndexView &indices, const char *name) {
    for (size_t i = 0; i < indices.shape(0); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (indices(i, j) < 0) {
                std::ostringstream oss;
                oss << name << " does not allow negative indices";
                throw nb::value_error(oss.str().c_str());
            }
        }
    }
}

template <typename IndexView>
void RequireIndicesInRange(const IndexView &indices, size_t vertex_count,
                           const char *name) {
    for (size_t i = 0; i < indices.shape(0); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (static_cast<size_t>(indices(i, j)) >= vertex_count) {
                std::ostringstream oss;
                oss << name << " index out of range for vertex array of size "
                    << vertex_count;
                throw nb::index_error(oss.str().c_str());
            }
        }
    }
}

}  // namespace

NB_MODULE(glviskit, m) {
    nb::set_leak_warnings(false);

    m.doc() = "Python bindings for glviskit";

    m.def("create_window", &glviskit::CreateWindow,
          "title"_a = "glviskit Window", "width"_a = 800, "height"_a = 600);
    m.def("create_render_list", &glviskit::CreateRenderList,
          "Create a new RenderList");
    m.def("get_time_seconds", &glviskit::GetTimeSeconds,
          "Get the current time in seconds since the program started");
    m.def("loop", &glviskit::Loop, nb::call_guard<nb::gil_scoped_release>(),
          "Run the event loop for single iteration and render all windows");
    m.def("render", &glviskit::Render, nb::call_guard<nb::gil_scoped_release>(),
          "Render all windows without processing events");

    nb::class_<glviskit::sdl::Window>(m, "Window")
        .def("add_render_list", &glviskit::sdl::Window::AddRenderList, "rb"_a,
             "Add a RenderList to the window for rendering")
        .def_prop_rw("camera", &glviskit::sdl::Window::GetCamera,
                     &glviskit::sdl::Window::SetCamera, "Camera of the window")
        .def_prop_rw(
            "background_color",
            [](const glviskit::sdl::Window &window) {
                glm::vec4 color = window.GetBackgroundColor();
                return std::array<float, 4>{color.r, color.g, color.b, color.a};
            },
            [](glviskit::sdl::Window &window,
               const std::array<float, 4> &color) {
                window.SetBackgroundColor(
                    {color[0], color[1], color[2], color[3]});
            },
            "Background clear color of the window")
        .def_prop_rw("controller", &glviskit::sdl::Window::GetController,
                     &glviskit::sdl::Window::SetController,
                     "Camera controller of the window")
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

    nb::class_<glviskit::RenderList>(m, "RenderList")
        .def(
            "line",
            [](glviskit::RenderList &rb, const Points32 &starts,
               const Points32 &ends) {
                auto s = starts.view();
                auto e = ends.view();
                RequireMatchingLineCount(s, e);
                for (size_t i = 0; i < s.shape(0); ++i) {
                    rb.Line({s(i, 0), s(i, 1), s(i, 2)},
                            {e(i, 0), e(i, 1), e(i, 2)});
                }
            },
            "starts"_a.noconvert(), "ends"_a.noconvert(),
            "Draw multiple lines from starts to ends")
        .def(
            "line",
            [](glviskit::RenderList &rb, const Points64 &starts,
               const Points64 &ends) {
                auto s = starts.view();
                auto e = ends.view();
                RequireMatchingLineCount(s, e);
                for (size_t i = 0; i < s.shape(0); ++i) {
                    rb.Line({static_cast<float>(s(i, 0)),
                             static_cast<float>(s(i, 1)),
                             static_cast<float>(s(i, 2))},
                            {static_cast<float>(e(i, 0)),
                             static_cast<float>(e(i, 1)),
                             static_cast<float>(e(i, 2))});
                }
            },
            "starts"_a.noconvert(), "ends"_a.noconvert(),
            "Draw multiple lines from starts to ends")
        .def(
            "line",
            [](glviskit::RenderList &rb, const std::array<float, 3> &start,
               const std::array<float, 3> &end) {
                rb.Line(glm::make_vec3(start.data()),
                        glm::make_vec3(end.data()));
            },
            "start"_a, "end"_a, "Draw a line from start to end")
        .def(
            "point",
            [](glviskit::RenderList &rb, const Points32 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    glm::vec3 p{v(i, 0), v(i, 1), v(i, 2)};
                    rb.Point(p);
                }
            },
            "points"_a.noconvert(), "Draw multiple points at given positions")
        .def(
            "point",
            [](glviskit::RenderList &rb, const Points64 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    glm::vec3 p{static_cast<float>(v(i, 0)),
                                static_cast<float>(v(i, 1)),
                                static_cast<float>(v(i, 2))};
                    rb.Point(p);
                }
            },
            "points"_a.noconvert(), "Draw multiple points at given positions")
        .def(
            "point",
            [](glviskit::RenderList &rb, const std::array<float, 3> &p) {
                rb.Point(glm::make_vec3(p.data()));
            },
            "p"_a, "Draw a point at position p")
        .def("path_begin", &glviskit::RenderList::PathBegin,
             "Create a Path object for drawing complex paths which is "
             "save/restore aware")
        .def("mesh_begin", &glviskit::RenderList::MeshBegin,
             "Create a Mesh object for incrementally building triangle "
             "geometry which is save/restore aware")
        .def(
            "circle",
            [](glviskit::RenderList &rb, const Points32 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.Circle({v(i, 0), v(i, 1), v(i, 2)});
                }
            },
            "points"_a.noconvert(), "Draw multiple circle at given positions")
        .def(
            "circle",
            [](glviskit::RenderList &rb, const Points64 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.Circle({static_cast<float>(v(i, 0)),
                               static_cast<float>(v(i, 1)),
                               static_cast<float>(v(i, 2))});
                }
            },
            "points"_a.noconvert(), "Draw multiple circle at given positions")
        .def(
            "circle",
            [](glviskit::RenderList &rb, const std::array<float, 3> &pos) {
                rb.Circle(glm::make_vec3(pos.data()));
            },
            "pos"_a, "Draw an circle at position pos")
        .def(
            "polygon",
            [](glviskit::RenderList &rb, const Polygons32 &polygons) {
                auto polys = polygons.view();
                for (size_t i = 0; i < polys.shape(0); ++i) {
                    if (polys.shape(1) < 2) {
                        throw nb::value_error(
                            "polygon(vertices) requires at least 2 vertices "
                            "per polygon");
                    }
                    auto path = rb.PathBegin();
                    for (size_t j = 0; j < polys.shape(1); ++j) {
                        path->LineTo(
                            {polys(i, j, 0), polys(i, j, 1), polys(i, j, 2)});
                    }
                    path->Close();
                }
            },
            "vertices"_a.noconvert(),
            "Draw multiple closed polygonal outlines from an array of shape N "
            "x M x 3")
        .def(
            "polygon",
            [](glviskit::RenderList &rb, const Polygons64 &polygons) {
                auto polys = polygons.view();
                for (size_t i = 0; i < polys.shape(0); ++i) {
                    if (polys.shape(1) < 2) {
                        throw nb::value_error(
                            "polygon(vertices) requires at least 2 vertices "
                            "per polygon");
                    }
                    auto path = rb.PathBegin();
                    for (size_t j = 0; j < polys.shape(1); ++j) {
                        path->LineTo({static_cast<float>(polys(i, j, 0)),
                                      static_cast<float>(polys(i, j, 1)),
                                      static_cast<float>(polys(i, j, 2))});
                    }
                    path->Close();
                }
            },
            "vertices"_a.noconvert(),
            "Draw multiple closed polygonal outlines from an array of shape N "
            "x M x 3")
        .def(
            "polygon",
            [](glviskit::RenderList &rb, const Points32 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 2, "polygon(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(v(i, 0), v(i, 1), v(i, 2));
                }
                rb.Polygon(vv);
            },
            "vertices"_a.noconvert(),
            "Draw a closed polygonal outline through the given vertices")
        .def(
            "polygon",
            [](glviskit::RenderList &rb, const Points64 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 2, "polygon(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(static_cast<float>(v(i, 0)),
                                    static_cast<float>(v(i, 1)),
                                    static_cast<float>(v(i, 2)));
                }
                rb.Polygon(vv);
            },
            "vertices"_a.noconvert(),
            "Draw a closed polygonal outline through the given vertices")
        .def(
            "polyline",
            [](glviskit::RenderList &rb, const Points32 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 2, "polyline(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(v(i, 0), v(i, 1), v(i, 2));
                }
                rb.Polyline(vv);
            },
            "vertices"_a.noconvert(),
            "Draw an open polyline through the given vertices")
        .def(
            "polyline",
            [](glviskit::RenderList &rb, const Points64 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 2, "polyline(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(static_cast<float>(v(i, 0)),
                                    static_cast<float>(v(i, 1)),
                                    static_cast<float>(v(i, 2)));
                }
                rb.Polyline(vv);
            },
            "vertices"_a.noconvert(),
            "Draw an open polyline through the given vertices")
        .def(
            "fill_polygon",
            [](glviskit::RenderList &rb, const Polygons32 &polygons) {
                auto polys = polygons.view();
                for (size_t i = 0; i < polys.shape(0); ++i) {
                    if (polys.shape(1) < 3) {
                        throw nb::value_error(
                            "fill_polygon(vertices) requires at least 3 "
                            "vertices per polygon");
                    }
                    auto mesh = rb.MeshBegin();
                    for (size_t j = 0; j < polys.shape(1); ++j) {
                        mesh->Vertex(
                            {polys(i, j, 0), polys(i, j, 1), polys(i, j, 2)});
                    }
                    for (size_t j = 1; j + 1 < polys.shape(1); ++j) {
                        mesh->Triangle(0, j, j + 1);
                    }
                }
            },
            "vertices"_a.noconvert(),
            "Fill multiple polygons from an array of shape N x M x 3 using "
            "triangle fans")
        .def(
            "fill_polygon",
            [](glviskit::RenderList &rb, const Polygons64 &polygons) {
                auto polys = polygons.view();
                for (size_t i = 0; i < polys.shape(0); ++i) {
                    if (polys.shape(1) < 3) {
                        throw nb::value_error(
                            "fill_polygon(vertices) requires at least 3 "
                            "vertices per polygon");
                    }
                    auto mesh = rb.MeshBegin();
                    for (size_t j = 0; j < polys.shape(1); ++j) {
                        mesh->Vertex({static_cast<float>(polys(i, j, 0)),
                                      static_cast<float>(polys(i, j, 1)),
                                      static_cast<float>(polys(i, j, 2))});
                    }
                    for (size_t j = 1; j + 1 < polys.shape(1); ++j) {
                        mesh->Triangle(0, j, j + 1);
                    }
                }
            },
            "vertices"_a.noconvert(),
            "Fill multiple polygons from an array of shape N x M x 3 using "
            "triangle fans")
        .def(
            "fill_polygon",
            [](glviskit::RenderList &rb, const Points32 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 3, "fill_polygon(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(v(i, 0), v(i, 1), v(i, 2));
                }
                rb.FillPolygon(vv);
            },
            "vertices"_a.noconvert(),
            "Fill a polygon using a triangle fan through the given vertices")
        .def(
            "fill_polygon",
            [](glviskit::RenderList &rb, const Points64 &vertices) {
                auto v = vertices.view();
                RequireAtLeastVertices(v, 3, "fill_polygon(vertices)");
                std::vector<glm::vec3> vv;
                vv.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(static_cast<float>(v(i, 0)),
                                    static_cast<float>(v(i, 1)),
                                    static_cast<float>(v(i, 2)));
                }
                rb.FillPolygon(vv);
            },
            "vertices"_a.noconvert(),
            "Fill a polygon using a triangle fan through the given vertices")
        .def(
            "color",
            [](glviskit::RenderList &rb, const std::array<float, 4> &c) {
                rb.Color({c[0], c[1], c[2], c[3]});
            },
            "c"_a, "Set the current drawing color")
        .def("size", &glviskit::RenderList::Size, "size"_a,
             "Set the current drawing size")
        .def(
            "triangles",
            [](glviskit::RenderList &rb, const Points32 &vertices,
               const IndicesI32 &indices) {
                auto v = vertices.view();
                auto t = indices.view();
                RequireNonNegativeIndices(t, "triangles(vertices, indices)");
                RequireIndicesInRange(t, v.shape(0),
                                      "triangles(vertices, indices)");
                std::vector<glm::vec3> vv;
                std::vector<glm::uvec3> ii;
                vv.reserve(v.shape(0));
                ii.reserve(t.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(v(i, 0), v(i, 1), v(i, 2));
                }
                for (size_t i = 0; i < t.shape(0); ++i) {
                    ii.emplace_back(static_cast<uint32_t>(t(i, 0)),
                                    static_cast<uint32_t>(t(i, 1)),
                                    static_cast<uint32_t>(t(i, 2)));
                }
                rb.Triangles(vv, ii);
            },
            "vertices"_a.noconvert(), "indices"_a.noconvert(),
            "Draw a triangle mesh from vertices and triangle indices")
        .def(
            "triangles",
            [](glviskit::RenderList &rb, const Points64 &vertices,
               const IndicesI32 &indices) {
                auto v = vertices.view();
                auto t = indices.view();
                RequireNonNegativeIndices(t, "triangles(vertices, indices)");
                RequireIndicesInRange(t, v.shape(0),
                                      "triangles(vertices, indices)");
                std::vector<glm::vec3> vv;
                std::vector<glm::uvec3> ii;
                vv.reserve(v.shape(0));
                ii.reserve(t.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    vv.emplace_back(static_cast<float>(v(i, 0)),
                                    static_cast<float>(v(i, 1)),
                                    static_cast<float>(v(i, 2)));
                }
                for (size_t i = 0; i < t.shape(0); ++i) {
                    ii.emplace_back(static_cast<uint32_t>(t(i, 0)),
                                    static_cast<uint32_t>(t(i, 1)),
                                    static_cast<uint32_t>(t(i, 2)));
                }
                rb.Triangles(vv, ii);
            },
            "vertices"_a.noconvert(), "indices"_a.noconvert(),
            "Draw a triangle mesh from vertices and triangle indices")
        .def(
            "add_instance",
            [](glviskit::RenderList &rb, const Matrix44f &transform) {
                auto t = transform.view();
                glm::mat4 mat{1.0F};
                for (size_t row = 0; row < 4; ++row) {
                    for (size_t col = 0; col < 4; ++col) {
                        mat[col][row] = t(row, col);
                    }
                }
                rb.AddInstance(mat);
            },
            "transform"_a.noconvert(),
            "Add an instance using a 4x4 transform matrix")
        .def(
            "add_instance",
            [](glviskit::RenderList &rb, const Matrix44d &transform) {
                auto t = transform.view();
                glm::mat4 mat{1.0F};
                for (size_t row = 0; row < 4; ++row) {
                    for (size_t col = 0; col < 4; ++col) {
                        mat[col][row] = static_cast<float>(t(row, col));
                    }
                }
                rb.AddInstance(mat);
            },
            "transform"_a.noconvert(),
            "Add an instance using a 4x4 transform matrix")
        .def(
            "add_instance",
            [](glviskit::RenderList &rb, const std::array<float, 3> &pos,
               const std::array<float, 4> &rot,
               const std::array<float, 3> &scale) {
                rb.AddInstance(
                    glm::make_vec3(pos.data()),
                    glm::quat{rot[0], rot[1], rot[2], rot[3]},
                    glm::make_vec3(scale.data()));
            },
            "pos"_a,
            "rot"_a,
            "scale"_a = std::array<float, 3>{1.0f, 1.0f, 1.0f},
            "Add an instance with position, quaternion rotation "
            "(w, x, y, z), and scale")
        .def(
            "add_instance",
            [](glviskit::RenderList &rb, const std::array<float, 3> &pos,
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
        .def("save", &glviskit::RenderList::Save,
             "Save the current render buffer state")
        .def("restore", &glviskit::RenderList::Restore,
             "Restore the previously saved render buffer state")
        .def("clear", &glviskit::RenderList::Clear, "Clear the render buffer")
        .def("save_instances", &glviskit::RenderList::SaveInstances,
             "Save the current instances")
        .def("restore_instances", &glviskit::RenderList::RestoreInstances,
             "Restore the previously saved instances")
        .def("clear_instances", &glviskit::RenderList::ClearInstances,
             "Clear the instances")
        .def_prop_rw("enabled", &glviskit::RenderList::IsEnabled,
                     &glviskit::RenderList::SetEnabled,
                     "Whether this RenderList is enabled for rendering");

    nb::class_<glviskit::Path>(m, "Path")
        .def(
            "line_to",
            [](glviskit::Path &rb, const Points32 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.LineTo({v(i, 0), v(i, 1), v(i, 2)});
                }
            },
            "points"_a.noconvert(),
            "Call line_to for multiple points consecutively")
        .def(
            "line_to",
            [](glviskit::Path &rb, const Points64 &points) {
                auto v = points.view();
                for (size_t i = 0; i < v.shape(0); ++i) {
                    rb.LineTo({static_cast<float>(v(i, 0)),
                               static_cast<float>(v(i, 1)),
                               static_cast<float>(v(i, 2))});
                }
            },
            "points"_a.noconvert(),
            "Call line_to for multiple points consecutively")
        .def(
            "line_to",
            [](glviskit::Path &rb, const std::array<float, 3> &p) {
                rb.LineTo(glm::make_vec3(p.data()));
            },
            "p"_a, "Draw a line to position p")
        .def("close", &glviskit::Path::Close, "Close the current line sequence")
        .def("line_end", &glviskit::Path::LineEnd,
             "End the current line sequence")
        .def(
            "color",
            [](glviskit::Path &rb, const std::array<float, 4> &c) {
                rb.Color({c[0], c[1], c[2], c[3]});
            },
            "c"_a, "Set the current drawing color")
        .def("size", &glviskit::Path::Size, "size"_a,
             "Set the current drawing size");

    nb::class_<glviskit::Mesh>(m, "Mesh")
        .def(
            "vertex",
            [](glviskit::Mesh &mesh, const Points32 &points) {
                auto v = points.view();
                std::vector<size_t> indices;
                indices.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    indices.push_back(mesh.Vertex({v(i, 0), v(i, 1), v(i, 2)}));
                }
                return indices;
            },
            "points"_a.noconvert(),
            "Add multiple vertices and return their mesh-local indices")
        .def(
            "vertex",
            [](glviskit::Mesh &mesh, const Points64 &points) {
                auto v = points.view();
                std::vector<size_t> indices;
                indices.reserve(v.shape(0));
                for (size_t i = 0; i < v.shape(0); ++i) {
                    indices.push_back(
                        mesh.Vertex({static_cast<float>(v(i, 0)),
                                     static_cast<float>(v(i, 1)),
                                     static_cast<float>(v(i, 2))}));
                }
                return indices;
            },
            "points"_a.noconvert(),
            "Add multiple vertices and return their mesh-local indices")
        .def(
            "vertex",
            [](glviskit::Mesh &mesh, const std::array<float, 3> &p) {
                return mesh.Vertex(glm::make_vec3(p.data()));
            },
            "p"_a, "Add a vertex and return its mesh-local index")
        .def(
            "triangle",
            [](glviskit::Mesh &mesh, const IndicesI32 &triangles) {
                auto t = triangles.view();
                RequireNonNegativeIndices(t, "mesh.triangle(triangles)");
                RequireIndicesInRange(t, mesh.VertexCount(),
                                      "mesh.triangle(triangles)");
                for (size_t i = 0; i < t.shape(0); ++i) {
                    mesh.Triangle(static_cast<size_t>(t(i, 0)),
                                  static_cast<size_t>(t(i, 1)),
                                  static_cast<size_t>(t(i, 2)));
                }
            },
            "triangles"_a.noconvert(),
            "Add multiple triangles using mesh-local vertex indices")
        .def("triangle", &glviskit::Mesh::Triangle, "i0"_a, "i1"_a, "i2"_a,
             "Add a triangle using mesh-local vertex indices")
        .def(
            "color",
            [](glviskit::Mesh &mesh, const std::array<float, 4> &c) {
                mesh.Color({c[0], c[1], c[2], c[3]});
            },
            "c"_a, "Set the current drawing color");

    nb::class_<glviskit::BaseController>(m, "BaseController");

    nb::class_<glviskit::NullController, glviskit::BaseController>(
        m, "NullController")
        .def(nb::init<>(), "Create a NullController");

    nb::class_<glviskit::FirstPersonController, glviskit::BaseController>(
        m, "FirstPersonController")
        .def(nb::init<>(), "Create a FirstPersonController")
        .def_prop_rw("key_move_sensitivity",
                     &glviskit::FirstPersonController::GetKeyMoveSensitivity,
                     &glviskit::FirstPersonController::SetKeyMoveSensitivity,
                     "Sensitivity of movement to key presses")
        .def_prop_rw("key_rot_sensitivity",
                     &glviskit::FirstPersonController::GetKeyRotSensitivity,
                     &glviskit::FirstPersonController::SetKeyRotSensitivity,
                     "Sensitivity of rotation to key presses")
        .def_prop_rw("mouse_sensitivity",
                     &glviskit::FirstPersonController::GetMouseSensitivity,
                     &glviskit::FirstPersonController::SetMouseSensitivity,
                     "Sensitivity of rotation to mouse movement");

    nb::class_<glviskit::SphericalController, glviskit::BaseController>(
        m, "SphericalController")
        .def(nb::init<>(), "Create a SphericalController")
        .def_prop_rw("key_move_sensitivity",
                     &glviskit::SphericalController::GetKeyMoveSensitivity,
                     &glviskit::SphericalController::SetKeyMoveSensitivity,
                     "Sensitivity of distance change to key presses")
        .def_prop_rw("key_rot_sensitivity",
                     &glviskit::SphericalController::GetKeyRotSensitivity,
                     &glviskit::SphericalController::SetKeyRotSensitivity,
                     "Sensitivity of rotation to key presses")
        .def_prop_rw("mouse_sensitivity",
                     &glviskit::SphericalController::GetMouseSensitivity,
                     &glviskit::SphericalController::SetMouseSensitivity,
                     "Sensitivity of rotation to mouse movement")
        .def_prop_rw("wheel_sensitivity",
                     &glviskit::SphericalController::GetWheelSensitivity,
                     &glviskit::SphericalController::SetWheelSensitivity,
                     "Sensitivity of distance change to mouse wheel");
}
