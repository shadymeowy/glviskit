#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/vector.h>

#include <cstddef>
#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <glviskit/glviskit.hpp>
#include <span>

namespace nb = nanobind;
using namespace nb::literals;

template <typename T, typename Shape>
using Array = nb::ndarray<T, Shape, nb::device::cpu>;

template <typename T>
using Points1 = Array<T, nb::shape<-1, 3>>;
template <typename T>
using Points2 = Array<T, nb::shape<-1, -1, 3>>;

template <typename T>
using Colors1 = Array<T, nb::shape<-1, 4>>;
template <typename T>
using Colors2 = Array<T, nb::shape<-1, -1, 4>>;

template <typename T>
using Sizes1 = Array<T, nb::shape<-1>>;
template <typename T>
using Sizes2 = Array<T, nb::shape<-1, -1>>;

template <typename T>
using Indices = Array<T, nb::shape<-1, 3>>;

using ImageRGBA8 =
    nb::ndarray<uint8_t, nb::shape<-1, -1, 4>, nb::c_contig, nb::device::cpu>;
using Matrix44f = nb::ndarray<float, nb::shape<4, 4>, nb::device::cpu>;
using Matrix44d = nb::ndarray<double, nb::shape<4, 4>, nb::device::cpu>;

namespace Helpers {

// line
template <typename T>
void Line1(glviskit::RenderList &rb, const Points1<T> &starts,
           const Points1<T> &ends) {
    auto s = starts.view();
    auto e = ends.view();

    if (s.shape(0) != e.shape(0)) {
        throw nb::value_error(
            "line(starts, ends) requires starts.shape[0] == ends.shape[0]");
    }

    auto path = rb.PathBegin();
    for (size_t i = 0; i < s.shape(0); ++i) {
        path->LineTo({s(i, 0), s(i, 1), s(i, 2)});
        path->LineTo({e(i, 0), e(i, 1), e(i, 2)});
        path->LineEnd();
    }
}

template <typename T1, typename T2>
void LineC1(glviskit::RenderList &rb, const Points1<T1> &starts,
            const Points1<T1> &ends, const Colors1<T2> &colors) {
    auto s = starts.view();
    auto e = ends.view();
    auto c = colors.view();

    if (s.shape(0) != e.shape(0) || s.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "line(starts, ends, colors) requires starts.shape[0] == "
            "ends.shape[0] == colors.shape[0]");
    }

    auto path = rb.PathBegin();
    for (size_t i = 0; i < s.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->LineTo({s(i, 0), s(i, 1), s(i, 2)});
        path->LineTo({e(i, 0), e(i, 1), e(i, 2)});
        path->LineEnd();
    }
}

template <typename T1, typename T2, typename T3>
void LineCS1(glviskit::RenderList &rb, const Points1<T1> &starts,
             const Points1<T1> &ends, const Colors1<T2> &colors,
             const Sizes1<T3> &sizes) {
    auto s = starts.view();
    auto e = ends.view();
    auto c = colors.view();
    auto sz = sizes.view();

    if (s.shape(0) != e.shape(0) || s.shape(0) != c.shape(0) ||
        s.shape(0) != sz.shape(0)) {
        throw nb::value_error(
            "line(starts, ends, colors, sizes) requires starts.shape[0] == "
            "ends.shape[0] == colors.shape[0] == sizes.shape[0]");
    }

    auto path = rb.PathBegin();
    for (size_t i = 0; i < s.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->Size(sz(i));
        path->LineTo({s(i, 0), s(i, 1), s(i, 2)});
        path->LineTo({e(i, 0), e(i, 1), e(i, 2)});
        path->LineEnd();
    }
}

template <typename T>
void Line0(glviskit::RenderList &rb, const std::array<T, 3> &start,
           const std::array<T, 3> &end) {
    auto path = rb.PathBegin();
    path->LineTo({start[0], start[1], start[2]});
    path->LineTo({end[0], end[1], end[2]});
    path->LineEnd();
}

template <typename T1, typename T2>
void LineC0(glviskit::RenderList &rb, const std::array<T1, 3> &start,
            const std::array<T1, 3> &end, const std::array<T2, 4> &color) {
    auto path = rb.PathBegin();
    path->Color({color[0], color[1], color[2], color[3]});
    path->LineTo({start[0], start[1], start[2]});
    path->LineTo({end[0], end[1], end[2]});
    path->LineEnd();
}

template <typename T1, typename T2, typename T3>
void LineCS0(glviskit::RenderList &rb, const std::array<T1, 3> &start,
             const std::array<T1, 3> &end, const std::array<T2, 4> &color,
             T3 size) {
    auto path = rb.PathBegin();
    path->Color({color[0], color[1], color[2], color[3]});
    path->Size(size);
    path->LineTo({start[0], start[1], start[2]});
    path->LineTo({end[0], end[1], end[2]});
    path->LineEnd();
}

// RenderList.point
template <typename T>
void Point1(glviskit::RenderList &rb, const Points1<T> &points) {
    auto v = points.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        rb.Point({v(i, 0), v(i, 1), v(i, 2)});
    }
}

template <typename T1, typename T2>
void PointC1(glviskit::RenderList &rb, const Points1<T1> &points,
             const Colors1<T2> &colors) {
    auto v = points.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "point(points, colors) requires points.shape[0] == "
            "colors.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        rb.Point({v(i, 0), v(i, 1), v(i, 2)}, color);
    }
}

template <typename T1, typename T2, typename T3>
void PointCS1(glviskit::RenderList &rb, const Points1<T1> &points,
              const Colors1<T2> &colors, const Sizes1<T3> &sizes) {
    auto v = points.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(0) != s.shape(0)) {
        throw nb::value_error(
            "point(points, colors, sizes) requires points.shape[0] == "
            "colors.shape[0] == sizes.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        float size = s(i);
        rb.Point({v(i, 0), v(i, 1), v(i, 2)}, color, size);
    }
}

template <typename T>
void Point0(glviskit::RenderList &rb, const std::array<T, 3> &point) {
    rb.Point({point[0], point[1], point[2]});
}

template <typename T1, typename T2>
void PointC0(glviskit::RenderList &rb, const std::array<T1, 3> &point,
             const std::array<T2, 4> &color) {
    rb.Point({point[0], point[1], point[2]},
             {color[0], color[1], color[2], color[3]});
}

template <typename T1, typename T2, typename T3>
void PointCS0(glviskit::RenderList &rb, const std::array<T1, 3> &point,
              const std::array<T2, 4> &color, T3 size) {
    rb.Point({point[0], point[1], point[2]},
             {color[0], color[1], color[2], color[3]}, size);
}

// RenderList.circle
template <typename T>
void Circle1(glviskit::RenderList &rb, const Points1<T> &points) {
    auto v = points.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        rb.Circle({v(i, 0), v(i, 1), v(i, 2)});
    }
}

template <typename T1, typename T2>
void CircleC1(glviskit::RenderList &rb, const Points1<T1> &points,
              const Colors1<T2> &colors) {
    auto v = points.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "circle(points, colors) requires points.shape[0] == "
            "colors.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        rb.Circle({v(i, 0), v(i, 1), v(i, 2)}, color);
    }
}

template <typename T1, typename T2, typename T3>
void CircleCS1(glviskit::RenderList &rb, const Points1<T1> &points,
               const Colors1<T2> &colors, const Sizes1<T3> &sizes) {
    auto v = points.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(0) != s.shape(0)) {
        throw nb::value_error(
            "circle(points, colors, sizes) requires points.shape[0] == "
            "colors.shape[0] == sizes.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        float size = s(i);
        rb.Circle({v(i, 0), v(i, 1), v(i, 2)}, color, size);
    }
}

template <typename T>
void Circle0(glviskit::RenderList &rb, const std::array<T, 3> &point) {
    rb.Circle({point[0], point[1], point[2]});
}

template <typename T1, typename T2>
void CircleC0(glviskit::RenderList &rb, const std::array<T1, 3> &point,
              const std::array<T2, 4> &color) {
    rb.Circle({point[0], point[1], point[2]},
              {color[0], color[1], color[2], color[3]});
}

template <typename T1, typename T2, typename T3>
void CircleCS0(glviskit::RenderList &rb, const std::array<T1, 3> &point,
               const std::array<T2, 4> &color, T3 size) {
    rb.Circle({point[0], point[1], point[2]},
              {color[0], color[1], color[2], color[3]}, size);
}

// RenderList.polygon
template <typename T>
void Polygon2(glviskit::RenderList &rb, const Points2<T> &vertices) {
    auto v = vertices.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polygon(vertices) requires at least 2 vertices per polygon");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->Close();
    }
}

template <typename T1, typename T2>
void PolygonC2(glviskit::RenderList &rb, const Points2<T1> &vertices,
               const Colors2<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0) || v.shape(1) != c.shape(1)) {
        throw nb::value_error(
            "polygon(vertices, colors) requires vertices.shape[:2] == "
            "colors.shape[:2]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polygon(vertices, colors) requires at least 2 vertices per "
                "polygon");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->Color({c(i, j, 0), c(i, j, 1), c(i, j, 2), c(i, j, 3)});
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->Close();
    }
}

template <typename T1, typename T2, typename T3>
void PolygonCS2(glviskit::RenderList &rb, const Points2<T1> &vertices,
                const Colors2<T2> &colors, const Sizes2<T3> &sizes) {
    auto v = vertices.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(1) != c.shape(1) ||
        v.shape(0) != s.shape(0) || v.shape(1) != s.shape(1)) {
        throw nb::value_error(
            "polygon(vertices, colors, sizes) requires vertices.shape[:2] == "
            "colors.shape[:2] == sizes.shape[:2]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polygon(vertices, colors, sizes) requires at least 2 vertices "
                "per polygon");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->Color({c(i, j, 0), c(i, j, 1), c(i, j, 2), c(i, j, 3)});
            path->Size(s(i, j));
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->Close();
    }
}

template <typename T>
void Polygon1(glviskit::RenderList &rb, const Points1<T> &vertices) {
    auto v = vertices.view();
    if (v.shape(0) < 2) {
        throw nb::value_error("polygon(vertices) requires at least 2 vertices");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->Close();
}

template <typename T1, typename T2>
void PolygonC1(glviskit::RenderList &rb, const Points1<T1> &vertices,
               const Colors1<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) < 2) {
        throw nb::value_error(
            "polygon(vertices, colors) requires at least 2 vertices");
    }
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "polygon(vertices, colors) requires vertices.shape[0] == "
            "colors.shape[0]");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->Close();
}

template <typename T1, typename T2, typename T3>
void PolygonCS1(glviskit::RenderList &rb, const Points1<T1> &vertices,
                const Colors1<T2> &colors, const Sizes1<T3> &sizes) {
    auto v = vertices.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) < 2) {
        throw nb::value_error(
            "polygon(vertices, colors, sizes) requires at least 2 vertices");
    }
    if (v.shape(0) != c.shape(0) || v.shape(0) != s.shape(0)) {
        throw nb::value_error(
            "polygon(vertices, colors, sizes) requires vertices.shape[0] == "
            "colors.shape[0] == sizes.shape[0]");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->Size(s(i));
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->Close();
}

// RenderList.polyline
template <typename T>
void Polyline2(glviskit::RenderList &rb, const Points2<T> &vertices) {
    auto v = vertices.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polyline(vertices) requires at least 2 vertices per polyline");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->LineEnd();
    }
}

template <typename T1, typename T2>
void PolylineC2(glviskit::RenderList &rb, const Points2<T1> &vertices,
                const Colors2<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0) || v.shape(1) != c.shape(1)) {
        throw nb::value_error(
            "polyline(vertices, colors) requires vertices.shape[:2] == "
            "colors.shape[:2]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polyline(vertices, colors) requires at least 2 vertices per "
                "polyline");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->Color({c(i, j, 0), c(i, j, 1), c(i, j, 2), c(i, j, 3)});
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->LineEnd();
    }
}

template <typename T1, typename T2, typename T3>
void PolylineCS2(glviskit::RenderList &rb, const Points2<T1> &vertices,
                 const Colors2<T2> &colors, const Sizes2<T3> &sizes) {
    auto v = vertices.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(1) != c.shape(1) ||
        v.shape(0) != s.shape(0) || v.shape(1) != s.shape(1)) {
        throw nb::value_error(
            "polyline(vertices, colors, sizes) requires vertices.shape[:2] == "
            "colors.shape[:2] == sizes.shape[:2]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 2) {
            throw nb::value_error(
                "polyline(vertices, colors, sizes) requires at least 2 "
                "vertices per polyline");
        }
        auto path = rb.PathBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            path->Color({c(i, j, 0), c(i, j, 1), c(i, j, 2), c(i, j, 3)});
            path->Size(s(i, j));
            path->LineTo({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        path->LineEnd();
    }
}

template <typename T>
void Polyline1(glviskit::RenderList &rb, const Points1<T> &vertices) {
    auto v = vertices.view();
    if (v.shape(0) < 2) {
        throw nb::value_error(
            "polyline(vertices) requires at least 2 vertices");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->LineEnd();
}

template <typename T1, typename T2>
void PolylineC1(glviskit::RenderList &rb, const Points1<T1> &vertices,
                const Colors1<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "polyline(vertices, colors) requires vertices.shape[0] == "
            "colors.shape[0]");
    }
    if (v.shape(0) < 2) {
        throw nb::value_error(
            "polyline(vertices, colors) requires at least 2 vertices");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->LineEnd();
}

template <typename T1, typename T2, typename T3>
void PolylineCS1(glviskit::RenderList &rb, const Points1<T1> &vertices,
                 const Colors1<T2> &colors, const Sizes1<T3> &sizes) {
    auto v = vertices.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(0) != s.shape(0)) {
        throw nb::value_error(
            "polyline(vertices, colors, sizes) requires vertices.shape[0] == "
            "colors.shape[0] == sizes.shape[0]");
    }
    if (v.shape(0) < 2) {
        throw nb::value_error(
            "polyline(vertices, colors, sizes) requires at least 2 vertices");
    }
    auto path = rb.PathBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        path->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        path->Size(s(i));
        path->LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
    path->LineEnd();
}

// RenderList.fill_polygon
template <typename T>
void FillPolygon2(glviskit::RenderList &rb, const Points2<T> &vertices) {
    auto v = vertices.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 3) {
            throw nb::value_error(
                "fill_polygon(vertices) requires at least 3 vertices per "
                "polygon");
        }
        auto mesh = rb.MeshBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            mesh->Vertex({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        for (size_t j = 1; j + 1 < v.shape(1); ++j) {
            mesh->Triangle(0, j, j + 1);
        }
    }
}

template <typename T1, typename T2>
void FillPolygonC2(glviskit::RenderList &rb, const Points2<T1> &vertices,
                   const Colors2<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0) || v.shape(1) != c.shape(1)) {
        throw nb::value_error(
            "fill_polygon(vertices, colors) requires vertices.shape[:2] == "
            "colors.shape[:2]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        if (v.shape(1) < 3) {
            throw nb::value_error(
                "fill_polygon(vertices, colors) requires at least 3 vertices "
                "per polygon");
        }
        auto mesh = rb.MeshBegin();
        for (size_t j = 0; j < v.shape(1); ++j) {
            mesh->Color({c(i, j, 0), c(i, j, 1), c(i, j, 2), c(i, j, 3)});
            mesh->Vertex({v(i, j, 0), v(i, j, 1), v(i, j, 2)});
        }
        for (size_t j = 1; j + 1 < v.shape(1); ++j) {
            mesh->Triangle(0, j, j + 1);
        }
    }
}

template <typename T>
void FillPolygon1(glviskit::RenderList &rb, const Points1<T> &vertices) {
    auto v = vertices.view();
    if (v.shape(0) < 3) {
        throw nb::value_error(
            "fill_polygon(vertices) requires at least 3 vertices");
    }
    auto mesh = rb.MeshBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        mesh->Vertex({v(i, 0), v(i, 1), v(i, 2)});
    }
    for (size_t i = 1; i + 1 < v.shape(0); ++i) {
        mesh->Triangle(0, i, i + 1);
    }
}

template <typename T1, typename T2>
void FillPolygonC1(glviskit::RenderList &rb, const Points1<T1> &vertices,
                   const Colors1<T2> &colors) {
    auto v = vertices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "fill_polygon(vertices, colors) requires vertices.shape[0] == "
            "colors.shape[0]");
    }
    if (v.shape(0) < 3) {
        throw nb::value_error(
            "fill_polygon(vertices, colors) requires at least 3 vertices");
    }
    auto mesh = rb.MeshBegin();
    for (size_t i = 0; i < v.shape(0); ++i) {
        mesh->Color({c(i, 0), c(i, 1), c(i, 2), c(i, 3)});
        mesh->Vertex({v(i, 0), v(i, 1), v(i, 2)});
    }
    for (size_t i = 1; i + 1 < v.shape(0); ++i) {
        mesh->Triangle(0, i, i + 1);
    }
}

// RenderList.triangles
template <typename T>
void Triangles1(glviskit::RenderList &rb, const Points1<T> &vertices,
                const Indices<int32_t> &indices) {
    auto v = vertices.view();
    auto t = indices.view();
    for (size_t i = 0; i < t.shape(0); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (t(i, j) < 0) {
                throw nb::value_error(
                    "triangles(vertices, indices) does not allow negative "
                    "indices");
            }
            if (static_cast<size_t>(t(i, j)) >= v.shape(0)) {
                throw nb::index_error(
                    "triangles(vertices, indices) index out of range");
            }
        }
    }
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
}

template <typename T1, typename T2>
void TrianglesC1(glviskit::RenderList &rb, const Points1<T1> &vertices,
                 const Indices<int32_t> &indices, const Colors1<T2> &colors) {
    auto v = vertices.view();
    auto t = indices.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "triangles(vertices, indices, colors) requires vertices.shape[0] "
            "== colors.shape[0]");
    }
    for (size_t i = 0; i < t.shape(0); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (t(i, j) < 0) {
                throw nb::value_error(
                    "triangles(vertices, indices, colors) does not allow "
                    "negative indices");
            }
            if (static_cast<size_t>(t(i, j)) >= v.shape(0)) {
                throw nb::index_error(
                    "triangles(vertices, indices, colors) index out of range");
            }
        }
    }
    std::vector<glm::vec3> vv;
    std::vector<glm::uvec3> ii;
    std::vector<glm::vec4> cc;
    vv.reserve(v.shape(0));
    ii.reserve(t.shape(0));
    cc.reserve(c.shape(0));
    for (size_t i = 0; i < v.shape(0); ++i) {
        vv.emplace_back(v(i, 0), v(i, 1), v(i, 2));
        cc.emplace_back(c(i, 0), c(i, 1), c(i, 2), c(i, 3));
    }
    for (size_t i = 0; i < t.shape(0); ++i) {
        ii.emplace_back(static_cast<uint32_t>(t(i, 0)),
                        static_cast<uint32_t>(t(i, 1)),
                        static_cast<uint32_t>(t(i, 2)));
    }
    rb.Triangles(vv, ii, cc);
}

// Path.line_to
template <typename T>
void PathLineTo1(glviskit::Path &rb, const Points1<T> &points) {
    auto v = points.view();
    for (size_t i = 0; i < v.shape(0); ++i) {
        rb.LineTo({v(i, 0), v(i, 1), v(i, 2)});
    }
}

template <typename T1, typename T2>
void PathLineToC1(glviskit::Path &rb, const Points1<T1> &points,
                  const Colors1<T2> &colors) {
    auto v = points.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "line_to(points, colors) requires points.shape[0] == "
            "colors.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        rb.LineTo({v(i, 0), v(i, 1), v(i, 2)}, color);
    }
}

template <typename T1, typename T2, typename T3>
void PathLineToCS1(glviskit::Path &rb, const Points1<T1> &points,
                   const Colors1<T2> &colors, const Sizes1<T3> &sizes) {
    auto v = points.view();
    auto c = colors.view();
    auto s = sizes.view();
    if (v.shape(0) != c.shape(0) || v.shape(0) != s.shape(0)) {
        throw nb::value_error(
            "line_to(points, colors, sizes) requires points.shape[0] == "
            "colors.shape[0] == sizes.shape[0]");
    }
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        float size = s(i);
        rb.LineTo({v(i, 0), v(i, 1), v(i, 2)}, color, size);
    }
}

template <typename T>
void PathLineTo0(glviskit::Path &rb, const std::array<T, 3> &point) {
    rb.LineTo({point[0], point[1], point[2]});
}

template <typename T1, typename T2>
void PathLineToC0(glviskit::Path &rb, const std::array<T1, 3> &point,
                  const std::array<T2, 4> &color) {
    rb.LineTo({point[0], point[1], point[2]},
              {color[0], color[1], color[2], color[3]});
}

template <typename T1, typename T2, typename T3>
void PathLineToCS0(glviskit::Path &rb, const std::array<T1, 3> &point,
                   const std::array<T2, 4> &color, T3 size) {
    rb.LineTo({point[0], point[1], point[2]},
              {color[0], color[1], color[2], color[3]}, size);
}

// mesh
template <typename T>
auto MeshVertex1(glviskit::Mesh &mesh, const Points1<T> &points)
    -> std::vector<size_t> {
    auto v = points.view();
    std::vector<size_t> indices;
    indices.reserve(v.shape(0));
    for (size_t i = 0; i < v.shape(0); ++i) {
        indices.push_back(mesh.Vertex({v(i, 0), v(i, 1), v(i, 2)}));
    }
    return indices;
}

template <typename T1, typename T2>
auto MeshVertexC1(glviskit::Mesh &mesh, const Points1<T1> &points,
                  const Colors1<T2> &colors) -> std::vector<size_t> {
    auto v = points.view();
    auto c = colors.view();
    if (v.shape(0) != c.shape(0)) {
        throw nb::value_error(
            "vertex(points, colors) requires points.shape[0] == "
            "colors.shape[0]");
    }
    std::vector<size_t> indices;
    indices.reserve(v.shape(0));
    for (size_t i = 0; i < v.shape(0); ++i) {
        glm::vec4 color{c(i, 0), c(i, 1), c(i, 2), c(i, 3)};
        indices.push_back(mesh.Vertex({v(i, 0), v(i, 1), v(i, 2)}, color));
    }
    return indices;
}

template <typename T>
auto MeshVertex0(glviskit::Mesh &mesh, const std::array<T, 3> &point)
    -> size_t {
    return mesh.Vertex({point[0], point[1], point[2]});
}

template <typename T1, typename T2>
auto MeshVertexC0(glviskit::Mesh &mesh, const std::array<T1, 3> &point,
                  const std::array<T2, 4> &color) -> size_t {
    return mesh.Vertex({point[0], point[1], point[2]},
                       {color[0], color[1], color[2], color[3]});
}

void MeshTriangle1(glviskit::Mesh &mesh, const Indices<int32_t> &indices) {
    auto t = indices.view();
    for (size_t i = 0; i < t.shape(0); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (t(i, j) < 0) {
                throw nb::value_error(
                    "triangle(indices) does not allow negative indices");
            }
        }
        mesh.Triangle(static_cast<size_t>(t(i, 0)),
                      static_cast<size_t>(t(i, 1)),
                      static_cast<size_t>(t(i, 2)));
    }
}

void MeshTriangle0(glviskit::Mesh &mesh, int32_t i0, int32_t i1, int32_t i2) {
    if (i0 < 0 || i1 < 0 || i2 < 0) {
        throw nb::value_error(
            "triangle(i0, i1, i2) does not allow negative indices");
    }
    mesh.Triangle(static_cast<size_t>(i0), static_cast<size_t>(i1),
                  static_cast<size_t>(i2));
}
}  // namespace Helpers

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
        .def(
            "capture_rgba",
            [](glviskit::sdl::Window &window, const ImageRGBA8 &image) {
                auto [width, height] = window.GetSizeInPixels();
                if (image.shape(0) != static_cast<size_t>(height) ||
                    image.shape(1) != static_cast<size_t>(width)) {
                    throw nb::value_error(
                        "capture_rgba(image) requires image.shape == "
                        "(window_height, window_width, 4)");
                }
                auto view = image.view();
                auto *ptr = &view(0, 0, 0);
                window.CaptureRGBA(std::span<unsigned char>(
                    ptr, static_cast<size_t>(width) * height * 4));
            },
            "image"_a.noconvert(),
            "Capture the current window into a preallocated RGBA uint8 array")
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
                                   nb::c_contig>(result.data())
                    .cast();
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
        .def_prop_rw(
            "axis_rotation",
            [](const glviskit::Camera &cam) {
                glm::quat q = cam.GetAxisRotation();
                return std::array<float, 4>{q.w, q.x, q.y, q.z};
            },
            [](glviskit::Camera &cam, const std::array<float, 4> &q) {
                cam.SetAxisRotation(glm::quat{q[0], q[1], q[2], q[3]});
            },
            "Axis-convention quaternion rotation (w, x, y, z)")
        .def(
            "set_axis_rotation",
            [](glviskit::Camera &cam, int x, int y, int z, bool inv_x,
               bool inv_y, bool inv_z) {
                cam.SetAxisRotation(x, y, z, inv_x, inv_y, inv_z);
            },
            "x"_a, "y"_a, "z"_a, "inv_x"_a = false, "inv_y"_a = false,
            "inv_z"_a = false,
            "Set axis-convention transform from an axis permutation with "
            "optional "
            "axis inversions")
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
        // line
        .def("line", Helpers::Line1<double>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "Draw multiple lines from starts to ends")
        .def("line", Helpers::Line1<float>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "Draw multiple lines from starts to ends")
        .def("line", Helpers::LineC1<double, double>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors")
        .def("line", Helpers::LineC1<double, float>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors")
        .def("line", Helpers::LineC1<float, double>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors")
        .def("line", Helpers::LineC1<float, float>, "starts"_a.noconvert(),
             "ends"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors")
        .def("line", Helpers::LineCS1<double, double, double>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<double, double, float>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<double, float, double>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<double, float, float>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<float, double, double>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<float, double, float>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<float, float, double>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::LineCS1<float, float, float>,
             "starts"_a.noconvert(), "ends"_a.noconvert(),
             "colors"_a.noconvert(), "sizes"_a.noconvert(),
             "Draw multiple lines from starts to ends with given colors and "
             "sizes")
        .def("line", Helpers::Line0<float>, "start"_a, "end"_a,
             "Draw a line from start to end")
        .def("line", Helpers::LineC0<float, float>, "start"_a, "end"_a,
             "color"_a, "Draw a line from start to end with given color")

        .def("line", Helpers::LineCS0<float, float, float>, "start"_a, "end"_a,
             "color"_a, "size"_a,
             "Draw a line from start to end with given color and size")
        // point
        .def("point", Helpers::Point1<double>, "points"_a.noconvert(),
             "Draw multiple points at given positions")
        .def("point", Helpers::Point1<float>, "points"_a.noconvert(),
             "Draw multiple points at given positions")
        .def("point", Helpers::PointC1<double, double>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple points at given positions with given colors")
        .def("point", Helpers::PointC1<double, float>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple points at given positions with given colors")
        .def("point", Helpers::PointC1<float, double>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple points at given positions with given colors")
        .def("point", Helpers::PointC1<float, float>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple points at given positions with given colors")
        .def("point", Helpers::PointCS1<double, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<double, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<double, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<double, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<float, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<float, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<float, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::PointCS1<float, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple points at given positions with given colors and "
             "sizes")
        .def("point", Helpers::Point0<float>, "point"_a,
             "Draw a point at given position")
        .def("point", Helpers::PointC0<float, float>, "point"_a, "color"_a,
             "Draw a point at given position with given color")

        .def("point", Helpers::PointCS0<float, float, float>, "point"_a,
             "color"_a, "size"_a,
             "Draw a point at given position with given color and size")
        // circle
        .def("circle", Helpers::Circle1<double>, "points"_a.noconvert(),
             "Draw multiple circles at given positions")
        .def("circle", Helpers::Circle1<float>, "points"_a.noconvert(),
             "Draw multiple circles at given positions")
        .def("circle", Helpers::CircleC1<double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple circles at given positions with given colors")
        .def("circle", Helpers::CircleC1<double, float>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple circles at given positions with given colors")
        .def("circle", Helpers::CircleC1<float, double>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple circles at given positions with given colors")
        .def("circle", Helpers::CircleC1<float, float>, "points"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple circles at given positions with given colors")
        .def("circle", Helpers::CircleCS1<double, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<double, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<double, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<double, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<float, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<float, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<float, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::CircleCS1<float, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple circles at given positions with given colors and "
             "sizes")
        .def("circle", Helpers::Circle0<float>, "point"_a,
             "Draw a circle at given position")
        .def("circle", Helpers::CircleC0<float, float>, "point"_a, "color"_a,
             "Draw a circle at given position with given color")
        .def("circle", Helpers::CircleCS0<float, float, float>, "point"_a,
             "color"_a, "size"_a,
             "Draw a circle at given position with given color and size")
        // polygon
        .def("polygon", Helpers::Polygon2<double>, "vertices"_a.noconvert(),
             "Draw multiple polygons with given vertices")
        .def("polygon", Helpers::Polygon2<float>, "vertices"_a.noconvert(),
             "Draw multiple polygons with given vertices")
        .def("polygon", Helpers::PolygonC2<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polygons with given vertices and colors")
        .def("polygon", Helpers::PolygonC2<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polygons with given vertices and colors")
        .def("polygon", Helpers::PolygonC2<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polygons with given vertices and colors")
        .def("polygon", Helpers::PolygonC2<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polygons with given vertices and colors")
        .def("polygon", Helpers::PolygonCS2<double, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<double, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<double, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<double, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<float, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<float, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<float, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS2<float, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polygons with given vertices, colors and sizes")
        .def("polygon", Helpers::Polygon1<double>, "vertices"_a.noconvert(),
             "Draw a polygon with given vertices")
        .def("polygon", Helpers::Polygon1<float>, "vertices"_a.noconvert(),
             "Draw a polygon with given vertices")
        .def("polygon", Helpers::PolygonC1<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polygon with given vertices and colors")
        .def("polygon", Helpers::PolygonC1<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polygon with given vertices and colors")
        .def("polygon", Helpers::PolygonC1<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polygon with given vertices and colors")
        .def("polygon", Helpers::PolygonC1<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polygon with given vertices and colors")
        .def("polygon", Helpers::PolygonCS1<double, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<double, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<double, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<double, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<float, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<float, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<float, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        .def("polygon", Helpers::PolygonCS1<float, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polygon with given vertices, colors and sizes")
        // polyline
        .def("polyline", Helpers::Polyline2<double>, "vertices"_a.noconvert(),
             "Draw multiple polylines with given vertices")
        .def("polyline", Helpers::Polyline2<float>, "vertices"_a.noconvert(),
             "Draw multiple polylines with given vertices")
        .def("polyline", Helpers::PolylineC2<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polylines with given vertices and colors")
        .def("polyline", Helpers::PolylineC2<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polylines with given vertices and colors")
        .def("polyline", Helpers::PolylineC2<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polylines with given vertices and colors")
        .def("polyline", Helpers::PolylineC2<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple polylines with given vertices and colors")
        .def("polyline", Helpers::PolylineCS2<double, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<double, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<double, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<double, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<float, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<float, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<float, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS2<float, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw multiple polylines with given vertices, colors and sizes")
        .def("polyline", Helpers::Polyline1<double>, "vertices"_a.noconvert(),
             "Draw a polyline with given vertices")
        .def("polyline", Helpers::Polyline1<float>, "vertices"_a.noconvert(),
             "Draw a polyline with given vertices")
        .def("polyline", Helpers::PolylineC1<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polyline with given vertices and colors")
        .def("polyline", Helpers::PolylineC1<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polyline with given vertices and colors")
        .def("polyline", Helpers::PolylineC1<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polyline with given vertices and colors")
        .def("polyline", Helpers::PolylineC1<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a polyline with given vertices and colors")
        .def("polyline", Helpers::PolylineCS1<double, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<double, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<double, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<double, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<float, double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<float, double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<float, float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        .def("polyline", Helpers::PolylineCS1<float, float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Draw a polyline with given vertices, colors and sizes")
        // fillpolygon
        .def("fill_polygon", Helpers::FillPolygon2<double>,
             "vertices"_a.noconvert(),
             "Draw multiple filled polygons with given vertices")
        .def("fill_polygon", Helpers::FillPolygon2<float>,
             "vertices"_a.noconvert(),
             "Draw multiple filled polygons with given vertices")
        .def("fill_polygon", Helpers::FillPolygonC2<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple filled polygons with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC2<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple filled polygons with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC2<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple filled polygons with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC2<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw multiple filled polygons with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygon1<double>,
             "vertices"_a.noconvert(),
             "Draw a filled polygon with given vertices")
        .def("fill_polygon", Helpers::FillPolygon1<float>,
             "vertices"_a.noconvert(),
             "Draw a filled polygon with given vertices")
        .def("fill_polygon", Helpers::FillPolygonC1<double, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a filled polygon with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC1<double, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a filled polygon with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC1<float, double>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a filled polygon with given vertices and colors")
        .def("fill_polygon", Helpers::FillPolygonC1<float, float>,
             "vertices"_a.noconvert(), "colors"_a.noconvert(),
             "Draw a filled polygon with given vertices and colors")
        // triangles
        .def("triangles", Helpers::Triangles1<double>, "vertices"_a.noconvert(),
             "indices"_a.noconvert(),
             "Draw multiple triangles with given vertices and indices")
        .def("triangles", Helpers::Triangles1<float>, "vertices"_a.noconvert(),
             "indices"_a.noconvert(),
             "Draw multiple triangles with given vertices and indices")
        .def("triangles", Helpers::TrianglesC1<double, double>,
             "vertices"_a.noconvert(), "indices"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple triangles with given vertices, indices and colors")
        .def("triangles", Helpers::TrianglesC1<double, float>,
             "vertices"_a.noconvert(), "indices"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple triangles with given vertices, indices and colors")
        .def("triangles", Helpers::TrianglesC1<float, double>,
             "vertices"_a.noconvert(), "indices"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple triangles with given vertices, indices and colors")
        .def("triangles", Helpers::TrianglesC1<float, float>,
             "vertices"_a.noconvert(), "indices"_a.noconvert(),
             "colors"_a.noconvert(),
             "Draw multiple triangles with given vertices, indices and colors")
        //
        .def("path_begin", &glviskit::RenderList::PathBegin,
             "Create a Path object for drawing complex paths which is "
             "save/restore aware")
        .def("mesh_begin", &glviskit::RenderList::MeshBegin,
             "Create a Mesh object for incrementally building triangle "
             "geometry which is save/restore aware")
        .def(
            "color",
            [](glviskit::RenderList &rb, const std::array<float, 4> &c) {
                rb.Color({c[0], c[1], c[2], c[3]});
            },
            "c"_a, "Set the current drawing color")
        .def("size", &glviskit::RenderList::Size, "size"_a,
             "Set the current drawing size")
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
                        mat[col][row] = t(row, col);
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
                rb.AddInstance(glm::make_vec3(pos.data()),
                               glm::quat{rot[0], rot[1], rot[2], rot[3]},
                               glm::make_vec3(scale.data()));
            },
            "pos"_a, "rot"_a,
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
        .def("line_to", Helpers::PathLineTo1<double>, "points"_a.noconvert(),
             "Call line_to for multiple points consecutively")
        .def("line_to", Helpers::PathLineTo1<float>, "points"_a.noconvert(),
             "Call line_to for multiple points consecutively")
        .def("line_to", Helpers::PathLineToC1<double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors")
        .def("line_to", Helpers::PathLineToC1<double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors")
        .def("line_to", Helpers::PathLineToC1<float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors")
        .def("line_to", Helpers::PathLineToC1<float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors")
        .def("line_to", Helpers::PathLineToCS1<double, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<double, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<double, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<double, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<float, double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<float, double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<float, float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineToCS1<float, float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "sizes"_a.noconvert(),
             "Call line_to for multiple points consecutively with given colors "
             "and sizes")
        .def("line_to", Helpers::PathLineTo0<float>, "point"_a,
             "Call line_to for a single point")
        .def("line_to", Helpers::PathLineToC0<float, float>, "point"_a,
             "color"_a, "Call line_to for a single point with given color")
        .def("line_to", Helpers::PathLineToCS0<float, float, float>, "point"_a,
             "color"_a, "size"_a,
             "Call line_to for a single point with given color and size")
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
        .def("vertex", Helpers::MeshVertex1<double>, "points"_a.noconvert(),
             "Add multiple vertices and return their mesh-local indices")
        .def("vertex", Helpers::MeshVertex1<float>, "points"_a.noconvert(),
             "Add multiple vertices and return their mesh-local indices")
        .def("vertex", Helpers::MeshVertexC1<double, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Add multiple vertices with given colors and return their "
             "mesh-local indices")
        .def("vertex", Helpers::MeshVertexC1<double, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Add multiple vertices with given colors and return their "
             "mesh-local indices")
        .def("vertex", Helpers::MeshVertexC1<float, double>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Add multiple vertices with given colors and return their "
             "mesh-local indices")
        .def("vertex", Helpers::MeshVertexC1<float, float>,
             "points"_a.noconvert(), "colors"_a.noconvert(),
             "Add multiple vertices with given colors and return their "
             "mesh-local indices")
        .def("vertex", Helpers::MeshVertex0<float>, "point"_a,
             "Add a vertex and return its mesh-local index")
        .def("vertex", Helpers::MeshVertexC0<float, float>, "point"_a,
             "color"_a,
             "Add a vertex with given color and return its mesh-local index")
        .def("triangle", Helpers::MeshTriangle1, "indices"_a.noconvert(),
             "Add multiple triangles using mesh-local vertex indices")
        .def("triangle", Helpers::MeshTriangle0, "i0"_a, "i1"_a, "i2"_a,
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
