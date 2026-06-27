#include "glviskit/c_api.h"

#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glviskit/glviskit.hpp>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>

struct glv_window {
    explicit glv_window(std::shared_ptr<glviskit::sdl::Window> p)
        : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::sdl::Window> ptr;
};

struct glv_camera {
    explicit glv_camera(std::shared_ptr<glviskit::Camera> p)
        : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::Camera> ptr;
};

struct glv_render_list {
    explicit glv_render_list(std::shared_ptr<glviskit::RenderList> p)
        : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::RenderList> ptr;
};

struct glv_path {
    explicit glv_path(std::shared_ptr<glviskit::Path> p) : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::Path> ptr;
};

struct glv_mesh {
    explicit glv_mesh(std::shared_ptr<glviskit::Mesh> p) : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::Mesh> ptr;
};

struct glv_controller {
    explicit glv_controller(std::shared_ptr<glviskit::BaseController> p)
        : ptr{std::move(p)} {}
    std::shared_ptr<glviskit::BaseController> ptr;
};

namespace {

thread_local std::string last_error;

#define GLV_TRY(failure, ...)                     \
    do {                                          \
        last_error.clear();                       \
        try {                                     \
            __VA_ARGS__                           \
        } catch (const std::exception &e) {       \
            last_error = e.what();                \
            return failure;                       \
        } catch (...) {                           \
            last_error = "unknown C++ exception"; \
            return failure;                       \
        }                                         \
    } while (false)

#define GLV_OK_TRY(...) GLV_TRY(GLV_ERROR, __VA_ARGS__ return GLV_OK;)

void Require(const void *ptr, const char *name) {
    if (ptr == nullptr) {
        throw std::invalid_argument(std::string{name} + " must not be null");
    }
}

auto Window(glv_window *h) -> glviskit::sdl::Window & {
    Require(h, "window");
    Require(h->ptr.get(), "window");
    return *h->ptr;
}

auto Shared(glv_window *h) -> std::shared_ptr<glviskit::sdl::Window> & {
    Require(h, "window");
    Require(h->ptr.get(), "window");
    return h->ptr;
}

auto Camera(glv_camera *h) -> glviskit::Camera & {
    Require(h, "camera");
    Require(h->ptr.get(), "camera");
    return *h->ptr;
}

auto Shared(glv_camera *h) -> std::shared_ptr<glviskit::Camera> & {
    Require(h, "camera");
    Require(h->ptr.get(), "camera");
    return h->ptr;
}

auto RenderList(glv_render_list *h) -> glviskit::RenderList & {
    Require(h, "render_list");
    Require(h->ptr.get(), "render_list");
    return *h->ptr;
}

auto Shared(glv_render_list *h) -> std::shared_ptr<glviskit::RenderList> & {
    Require(h, "render_list");
    Require(h->ptr.get(), "render_list");
    return h->ptr;
}

auto Path(glv_path *h) -> glviskit::Path & {
    Require(h, "path");
    Require(h->ptr.get(), "path");
    return *h->ptr;
}

auto Mesh(glv_mesh *h) -> glviskit::Mesh & {
    Require(h, "mesh");
    Require(h->ptr.get(), "mesh");
    return *h->ptr;
}

auto Controller(glv_controller *h) -> glviskit::BaseController & {
    Require(h, "controller");
    Require(h->ptr.get(), "controller");
    return *h->ptr;
}

auto Shared(glv_controller *h) -> std::shared_ptr<glviskit::BaseController> & {
    Require(h, "controller");
    Require(h->ptr.get(), "controller");
    return h->ptr;
}

auto Vec3(float x, float y, float z) -> glm::vec3 { return {x, y, z}; }
auto Vec4(float r, float g, float b, float a) -> glm::vec4 {
    return {r, g, b, a};
}

auto FirstPerson(glv_controller *controller)
    -> glviskit::FirstPersonController * {
    return dynamic_cast<glviskit::FirstPersonController *>(
        &Controller(controller));
}

auto Spherical(glv_controller *controller) -> glviskit::SphericalController * {
    return dynamic_cast<glviskit::SphericalController *>(
        &Controller(controller));
}

template <typename FnFirstPerson, typename FnSpherical>
void WithMoveController(glv_controller *controller, const char *property,
                        FnFirstPerson &&first_person, FnSpherical &&spherical) {
    if (auto *c = FirstPerson(controller); c != nullptr) {
        first_person(*c);
        return;
    }
    if (auto *c = Spherical(controller); c != nullptr) {
        spherical(*c);
        return;
    }
    throw std::invalid_argument(std::string{"controller does not support "} +
                                property);
}

template <typename Fn>
void WithSpherical(glv_controller *controller, const char *property, Fn &&fn) {
    if (auto *c = Spherical(controller); c != nullptr) {
        fn(*c);
        return;
    }
    throw std::invalid_argument(std::string{"controller does not support "} +
                                property);
}

}  // namespace

extern "C" {

const char *glv_error(void) { return last_error.c_str(); }

void glv_c_api_version(int *major, int *minor, int *patch) {
    if (major != nullptr) {
        *major = 0;
    }
    if (minor != nullptr) {
        *minor = 1;
    }
    if (patch != nullptr) {
        *patch = 0;
    }
}

void glv_window_destroy(glv_window *window) { delete window; }
void glv_camera_destroy(glv_camera *camera) { delete camera; }
void glv_render_list_destroy(glv_render_list *render_list) {
    delete render_list;
}
void glv_path_destroy(glv_path *path) { delete path; }
void glv_mesh_destroy(glv_mesh *mesh) { delete mesh; }
void glv_controller_destroy(glv_controller *controller) { delete controller; }

glv_window *glv_create_window(const char *title, int width, int height) {
    GLV_TRY(nullptr, const char *actual_title =
                         title != nullptr ? title : "glviskit Window";
            return new glv_window(
                glviskit::CreateWindow(actual_title, width, height)););
}

glv_render_list *glv_create_render_list(void) {
    GLV_TRY(nullptr, return new glv_render_list(glviskit::CreateRenderList()););
}

float glv_get_time_seconds(void) { return glviskit::GetTimeSeconds(); }

int glv_loop(void) { GLV_TRY(GLV_ERROR, return glviskit::Loop() ? 1 : 0;); }

int glv_render(void) { GLV_OK_TRY(glviskit::Render();); }

int glv_window_add_render_list(glv_window *window,
                               glv_render_list *render_list) {
    GLV_OK_TRY(Window(window).AddRenderList(Shared(render_list)););
}

glv_camera *glv_window_get_camera(glv_window *window) {
    GLV_TRY(nullptr, return new glv_camera(Window(window).GetCamera()););
}

int glv_window_set_camera(glv_window *window, glv_camera *camera) {
    GLV_OK_TRY(Window(window).SetCamera(Shared(camera)););
}

glv_controller *glv_window_get_controller(glv_window *window) {
    GLV_TRY(nullptr,
            return new glv_controller(Window(window).GetController()););
}

int glv_window_set_controller(glv_window *window, glv_controller *controller) {
    GLV_OK_TRY(Window(window).SetController(Shared(controller)););
}

int glv_window_set_background_color(glv_window *window, float r, float g,
                                    float b, float a) {
    GLV_OK_TRY(Window(window).SetBackgroundColor(Vec4(r, g, b, a)););
}

int glv_window_get_background_color(glv_window *window, float *out_r,
                                    float *out_g, float *out_b, float *out_a) {
    GLV_OK_TRY(Require(out_r, "out_r"); Require(out_g, "out_g");
               Require(out_b, "out_b"); Require(out_a, "out_a");
               const auto color = Window(window).GetBackgroundColor();
               *out_r = color.r; *out_g = color.g; *out_b = color.b;
               *out_a = color.a;);
}

int glv_window_get_size_in_pixels(glv_window *window, int *out_width,
                                  int *out_height) {
    GLV_OK_TRY(Require(out_width, "out_width");
               Require(out_height, "out_height");
               const auto size = Window(window).GetSizeInPixels();
               *out_width = size[0]; *out_height = size[1];);
}

int glv_window_capture_rgba(glv_window *window, uint8_t *pixels,
                            size_t pixel_count) {
    GLV_OK_TRY(
        if (pixel_count > 0) { Require(pixels, "pixels"); } Window(window)
            .CaptureRGBA(std::span<unsigned char>{pixels, pixel_count}););
}

int glv_window_make_current(glv_window *window) {
    GLV_OK_TRY(Window(window).MakeCurrent(););
}

int glv_window_render(glv_window *window) {
    GLV_OK_TRY(Window(window).Render(););
}

int glv_camera_calculate_transform(glv_camera *camera,
                                   float out_row_major_16[16]) {
    GLV_OK_TRY(Require(out_row_major_16, "out_row_major_16");
               const auto matrix = Camera(camera).CalculateTransform();
               for (int row = 0; row < 4; ++row) {
                   for (int col = 0; col < 4; ++col) {
                       out_row_major_16[(row * 4) + col] = matrix[col][row];
                   }
               });
}

int glv_camera_perspective_fov(glv_camera *camera, float hfov, float vfov,
                               float near_plane, float far_plane) {
    GLV_OK_TRY(
        Camera(camera).PerspectiveFov(hfov, vfov, near_plane, far_plane););
}

int glv_camera_perspective(glv_camera *camera, float fxn, float fyn, float cx,
                           float cy, float near_plane, float far_plane) {
    GLV_OK_TRY(
        Camera(camera).Perspective(fxn, fyn, cx, cy, near_plane, far_plane););
}

int glv_camera_set_position(glv_camera *camera, float x, float y, float z) {
    GLV_OK_TRY(Camera(camera).SetPosition(Vec3(x, y, z)););
}

int glv_camera_get_position(glv_camera *camera, float *out_x, float *out_y,
                            float *out_z) {
    GLV_OK_TRY(Require(out_x, "out_x"); Require(out_y, "out_y");
               Require(out_z, "out_z");
               const auto position = Camera(camera).GetPosition();
               *out_x = position.x; *out_y = position.y; *out_z = position.z;);
}

int glv_camera_set_rotation(glv_camera *camera, float roll, float pitch,
                            float yaw) {
    GLV_OK_TRY(Camera(camera).SetRotation(Vec3(roll, pitch, yaw)););
}

int glv_camera_get_rotation(glv_camera *camera, float *out_roll,
                            float *out_pitch, float *out_yaw) {
    GLV_OK_TRY(Require(out_roll, "out_roll"); Require(out_pitch, "out_pitch");
               Require(out_yaw, "out_yaw");
               const auto rotation = Camera(camera).GetRotation();
               *out_roll = rotation.x; *out_pitch = rotation.y;
               *out_yaw = rotation.z;);
}

int glv_camera_set_distance(glv_camera *camera, float distance) {
    GLV_OK_TRY(Camera(camera).SetDistance(distance););
}

int glv_camera_get_distance(glv_camera *camera, float *out_distance) {
    GLV_OK_TRY(Require(out_distance, "out_distance");
               *out_distance = Camera(camera).GetDistance(););
}

int glv_camera_set_axis_rotation(glv_camera *camera, float w, float x, float y,
                                 float z) {
    GLV_OK_TRY(Camera(camera).SetAxisRotation(glm::quat{w, x, y, z}););
}

int glv_camera_get_axis_rotation(glv_camera *camera, float *out_w, float *out_x,
                                 float *out_y, float *out_z) {
    GLV_OK_TRY(Require(out_w, "out_w"); Require(out_x, "out_x");
               Require(out_y, "out_y"); Require(out_z, "out_z");
               const auto quat = Camera(camera).GetAxisRotation();
               *out_w = quat.w; *out_x = quat.x; *out_y = quat.y;
               *out_z = quat.z;);
}

int glv_camera_set_axis_rotation_axes(glv_camera *camera, int x, int y, int z,
                                      int inv_x, int inv_y, int inv_z) {
    GLV_OK_TRY(Camera(camera).SetAxisRotation(x, y, z, inv_x != 0, inv_y != 0,
                                              inv_z != 0););
}

int glv_camera_get_viewport_size(glv_camera *camera, float *out_width,
                                 float *out_height) {
    GLV_OK_TRY(Require(out_width, "out_width");
               Require(out_height, "out_height");
               const auto viewport = Camera(camera).GetViewportSize();
               *out_width = viewport.x; *out_height = viewport.y;);
}

int glv_camera_set_preserve_aspect_ratio(glv_camera *camera, int preserve) {
    GLV_OK_TRY(Camera(camera).SetPreserveAspectRatio(preserve != 0););
}

int glv_camera_get_preserve_aspect_ratio(glv_camera *camera,
                                         int *out_preserve) {
    GLV_OK_TRY(Require(out_preserve, "out_preserve");
               *out_preserve =
                   Camera(camera).GetPreserveAspectRatio() ? 1 : 0;);
}

int glv_render_list_color(glv_render_list *render_list, float r, float g,
                          float b, float a) {
    GLV_OK_TRY(RenderList(render_list).Color(Vec4(r, g, b, a)););
}

int glv_render_list_size(glv_render_list *render_list, float size) {
    GLV_OK_TRY(RenderList(render_list).Size(size););
}

int glv_render_list_point(glv_render_list *render_list, float x, float y,
                          float z) {
    GLV_OK_TRY(RenderList(render_list).Point(Vec3(x, y, z)););
}

int glv_render_list_point_color(glv_render_list *render_list, float x, float y,
                                float z, float r, float g, float b, float a) {
    GLV_OK_TRY(RenderList(render_list).Point(Vec3(x, y, z), Vec4(r, g, b, a)););
}

int glv_render_list_point_color_size(glv_render_list *render_list, float x,
                                     float y, float z, float r, float g,
                                     float b, float a, float size) {
    GLV_OK_TRY(
        RenderList(render_list).Point(Vec3(x, y, z), Vec4(r, g, b, a), size););
}

int glv_render_list_line(glv_render_list *render_list, float x0, float y0,
                         float z0, float x1, float y1, float z1) {
    GLV_OK_TRY(
        RenderList(render_list).Line(Vec3(x0, y0, z0), Vec3(x1, y1, z1)););
}

int glv_render_list_line_color(glv_render_list *render_list, float x0, float y0,
                               float z0, float x1, float y1, float z1, float r,
                               float g, float b, float a) {
    GLV_OK_TRY(
        RenderList(render_list)
            .Line(Vec3(x0, y0, z0), Vec3(x1, y1, z1), Vec4(r, g, b, a)););
}

int glv_render_list_line_color_size(glv_render_list *render_list, float x0,
                                    float y0, float z0, float x1, float y1,
                                    float z1, float r, float g, float b,
                                    float a, float size) {
    GLV_OK_TRY(
        RenderList(render_list)
            .Line(Vec3(x0, y0, z0), Vec3(x1, y1, z1), Vec4(r, g, b, a), size););
}

int glv_render_list_circle(glv_render_list *render_list, float x, float y,
                           float z) {
    GLV_OK_TRY(RenderList(render_list).Circle(Vec3(x, y, z)););
}

int glv_render_list_circle_color(glv_render_list *render_list, float x, float y,
                                 float z, float r, float g, float b, float a) {
    GLV_OK_TRY(
        RenderList(render_list).Circle(Vec3(x, y, z), Vec4(r, g, b, a)););
}

int glv_render_list_circle_color_size(glv_render_list *render_list, float x,
                                      float y, float z, float r, float g,
                                      float b, float a, float size) {
    GLV_OK_TRY(
        RenderList(render_list).Circle(Vec3(x, y, z), Vec4(r, g, b, a), size););
}

namespace {

auto At3(const float *xyz, size_t i) -> glm::vec3 {
    return {xyz[(i * 3) + 0], xyz[(i * 3) + 1], xyz[(i * 3) + 2]};
}

auto At4(const float *rgba, size_t i) -> glm::vec4 {
    return {rgba[(i * 4) + 0], rgba[(i * 4) + 1], rgba[(i * 4) + 2],
            rgba[(i * 4) + 3]};
}

}  // namespace

int glv_render_list_points(glv_render_list *render_list, const float *xyz,
                           const float *rgba, const float *sizes,
                           size_t count) {
    GLV_OK_TRY(
        if (count > 0) { Require(xyz, "xyz"); } auto &rl =
            RenderList(render_list);
        for (size_t i = 0; i < count; ++i) {
            if (rgba == nullptr) {
                rl.Point(At3(xyz, i));
            } else if (sizes == nullptr) {
                rl.Point(At3(xyz, i), At4(rgba, i));
            } else {
                rl.Point(At3(xyz, i), At4(rgba, i), sizes[i]);
            }
        });
}

int glv_render_list_circles(glv_render_list *render_list, const float *xyz,
                            const float *rgba, const float *sizes,
                            size_t count) {
    GLV_OK_TRY(
        if (count > 0) { Require(xyz, "xyz"); } auto &rl =
            RenderList(render_list);
        for (size_t i = 0; i < count; ++i) {
            if (rgba == nullptr) {
                rl.Circle(At3(xyz, i));
            } else if (sizes == nullptr) {
                rl.Circle(At3(xyz, i), At4(rgba, i));
            } else {
                rl.Circle(At3(xyz, i), At4(rgba, i), sizes[i]);
            }
        });
}

int glv_render_list_lines(glv_render_list *render_list, const float *starts,
                          const float *ends, const float *rgba,
                          const float *sizes, size_t count) {
    GLV_OK_TRY(
        if (count > 0) {
            Require(starts, "starts");
            Require(ends, "ends");
        } auto &rl = RenderList(render_list);
        for (size_t i = 0; i < count; ++i) {
            if (rgba == nullptr) {
                rl.Line(At3(starts, i), At3(ends, i));
            } else if (sizes == nullptr) {
                rl.Line(At3(starts, i), At3(ends, i), At4(rgba, i));
            } else {
                rl.Line(At3(starts, i), At3(ends, i), At4(rgba, i), sizes[i]);
            }
        });
}

int glv_render_list_polygons(glv_render_list *render_list, const float *xyz,
                             const float *rgba, const float *sizes,
                             size_t groups, size_t count) {
    GLV_OK_TRY(
        if (groups > 0 && count > 0) { Require(xyz, "xyz"); } auto &rl =
            RenderList(render_list);
        for (size_t g = 0; g < groups; ++g) {
            auto path = rl.PathBegin();
            for (size_t i = 0; i < count; ++i) {
                size_t o = (g * count) + i;
                if (rgba != nullptr) {
                    path->Color(At4(rgba, o));
                    if (sizes != nullptr) {
                        path->Size(sizes[o]);
                    }
                }
                path->LineTo(At3(xyz, o));
            }
            path->Close();
        });
}

int glv_render_list_polylines(glv_render_list *render_list, const float *xyz,
                              const float *rgba, const float *sizes,
                              size_t groups, size_t count) {
    GLV_OK_TRY(
        if (groups > 0 && count > 0) { Require(xyz, "xyz"); } auto &rl =
            RenderList(render_list);
        for (size_t g = 0; g < groups; ++g) {
            auto path = rl.PathBegin();
            for (size_t i = 0; i < count; ++i) {
                size_t o = (g * count) + i;
                if (rgba != nullptr) {
                    path->Color(At4(rgba, o));
                    if (sizes != nullptr) {
                        path->Size(sizes[o]);
                    }
                }
                path->LineTo(At3(xyz, o));
            }
            path->LineEnd();
        });
}

int glv_render_list_fill_polygons(glv_render_list *render_list,
                                  const float *xyz, const float *rgba,
                                  size_t groups, size_t count) {
    GLV_OK_TRY(
        if (groups > 0 && count > 0) { Require(xyz, "xyz"); } auto &rl =
            RenderList(render_list);
        for (size_t g = 0; g < groups; ++g) {
            auto mesh = rl.MeshBegin();
            for (size_t i = 0; i < count; ++i) {
                size_t o = (g * count) + i;
                if (rgba == nullptr) {
                    mesh->Vertex(At3(xyz, o));
                } else {
                    mesh->Vertex(At3(xyz, o), At4(rgba, o));
                }
            }
            for (size_t i = 1; i + 1 < count; ++i) {
                mesh->Triangle(0, i, i + 1);
            }
        });
}

int glv_render_list_triangles(glv_render_list *render_list, const float *xyz,
                              const float *rgba, size_t vertex_count,
                              const int32_t *indices, size_t triangle_count) {
    GLV_OK_TRY(
        if (vertex_count > 0) { Require(xyz, "xyz"); } if (triangle_count > 0) {
            Require(indices, "indices");
        } auto mesh = RenderList(render_list).MeshBegin();
        for (size_t i = 0; i < vertex_count; ++i) {
            if (rgba == nullptr) {
                mesh->Vertex(At3(xyz, i));
            } else {
                mesh->Vertex(At3(xyz, i), At4(rgba, i));
            }
        } for (size_t t = 0; t < triangle_count; ++t) {
            mesh->Triangle(static_cast<size_t>(indices[(t * 3) + 0]),
                           static_cast<size_t>(indices[(t * 3) + 1]),
                           static_cast<size_t>(indices[(t * 3) + 2]));
        });
}

glv_path *glv_render_list_path_begin(glv_render_list *render_list) {
    GLV_TRY(nullptr, return new glv_path(RenderList(render_list).PathBegin()););
}

glv_mesh *glv_render_list_mesh_begin(glv_render_list *render_list) {
    GLV_TRY(nullptr, return new glv_mesh(RenderList(render_list).MeshBegin()););
}

int glv_render_list_add_instance(glv_render_list *render_list, float x, float y,
                                 float z, float rx, float ry, float rz,
                                 float sx, float sy, float sz) {
    GLV_OK_TRY(
        RenderList(render_list)
            .AddInstance(Vec3(x, y, z), Vec3(rx, ry, rz), Vec3(sx, sy, sz)););
}

int glv_render_list_add_instance_quat(glv_render_list *render_list, float x,
                                      float y, float z, float rw, float rx,
                                      float ry, float rz, float sx, float sy,
                                      float sz) {
    GLV_OK_TRY(RenderList(render_list)
                   .AddInstance(Vec3(x, y, z), glm::quat{rw, rx, ry, rz},
                                Vec3(sx, sy, sz)););
}

int glv_render_list_add_instance_matrix(glv_render_list *render_list,
                                        const float row_major_16[16]) {
    GLV_OK_TRY(Require(row_major_16, "row_major_16"); glm::mat4 matrix{1.0F};
               for (int row = 0; row < 4; ++row) {
                   for (int col = 0; col < 4; ++col) {
                       matrix[col][row] = row_major_16[(row * 4) + col];
                   }
               } RenderList(render_list)
                   .AddInstance(matrix););
}

int glv_render_list_save(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).Save(););
}

int glv_render_list_restore(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).Restore(););
}

int glv_render_list_clear(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).Clear(););
}

int glv_render_list_save_instances(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).SaveInstances(););
}

int glv_render_list_restore_instances(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).RestoreInstances(););
}

int glv_render_list_clear_instances(glv_render_list *render_list) {
    GLV_OK_TRY(RenderList(render_list).ClearInstances(););
}

int glv_render_list_set_enabled(glv_render_list *render_list, int enabled) {
    GLV_OK_TRY(RenderList(render_list).SetEnabled(enabled != 0););
}

int glv_render_list_get_enabled(glv_render_list *render_list,
                                int *out_enabled) {
    GLV_OK_TRY(Require(out_enabled, "out_enabled");
               *out_enabled = RenderList(render_list).IsEnabled() ? 1 : 0;);
}

int glv_path_line_to(glv_path *path, float x, float y, float z) {
    GLV_OK_TRY(Path(path).LineTo(Vec3(x, y, z)););
}

int glv_path_line_to_color(glv_path *path, float x, float y, float z, float r,
                           float g, float b, float a) {
    GLV_OK_TRY(Path(path).LineTo(Vec3(x, y, z), Vec4(r, g, b, a)););
}

int glv_path_line_to_color_size(glv_path *path, float x, float y, float z,
                                float r, float g, float b, float a,
                                float size) {
    GLV_OK_TRY(Path(path).LineTo(Vec3(x, y, z), Vec4(r, g, b, a), size););
}

int glv_path_close(glv_path *path) { GLV_OK_TRY(Path(path).Close();); }

int glv_path_line_end(glv_path *path) { GLV_OK_TRY(Path(path).LineEnd();); }

int glv_path_color(glv_path *path, float r, float g, float b, float a) {
    GLV_OK_TRY(Path(path).Color(Vec4(r, g, b, a)););
}

int glv_path_size(glv_path *path, float size) {
    GLV_OK_TRY(Path(path).Size(size););
}

int glv_path_line_to_many(glv_path *path, const float *xyz, const float *rgba,
                          const float *sizes, size_t count) {
    GLV_OK_TRY(
        if (count > 0) { Require(xyz, "xyz"); } auto &p = Path(path);
        for (size_t i = 0; i < count; ++i) {
            if (rgba == nullptr) {
                p.LineTo(At3(xyz, i));
            } else if (sizes == nullptr) {
                p.LineTo(At3(xyz, i), At4(rgba, i));
            } else {
                p.LineTo(At3(xyz, i), At4(rgba, i), sizes[i]);
            }
        });
}

int glv_mesh_vertex(glv_mesh *mesh, float x, float y, float z,
                    size_t *out_index) {
    GLV_OK_TRY(Require(out_index, "out_index");
               *out_index = Mesh(mesh).Vertex(Vec3(x, y, z)););
}

int glv_mesh_vertex_color(glv_mesh *mesh, float x, float y, float z, float r,
                          float g, float b, float a, size_t *out_index) {
    GLV_OK_TRY(Require(out_index, "out_index");
               *out_index =
                   Mesh(mesh).Vertex(Vec3(x, y, z), Vec4(r, g, b, a)););
}

int glv_mesh_triangle(glv_mesh *mesh, size_t i0, size_t i1, size_t i2) {
    GLV_OK_TRY(Mesh(mesh).Triangle(i0, i1, i2););
}

int glv_mesh_color(glv_mesh *mesh, float r, float g, float b, float a) {
    GLV_OK_TRY(Mesh(mesh).Color(Vec4(r, g, b, a)););
}

int glv_mesh_vertex_count(glv_mesh *mesh, size_t *out_vertex_count) {
    GLV_OK_TRY(Require(out_vertex_count, "out_vertex_count");
               *out_vertex_count = Mesh(mesh).VertexCount(););
}

int glv_mesh_vertices(glv_mesh *mesh, const float *xyz, const float *rgba,
                      size_t *out_indices, size_t count) {
    GLV_OK_TRY(
        if (count > 0) {
            Require(xyz, "xyz");
            Require(out_indices, "out_indices");
        } auto &m = Mesh(mesh);
        for (size_t i = 0; i < count; ++i) {
            out_indices[i] = rgba == nullptr
                                 ? m.Vertex(At3(xyz, i))
                                 : m.Vertex(At3(xyz, i), At4(rgba, i));
        });
}

int glv_mesh_triangles(glv_mesh *mesh, const int32_t *indices, size_t count) {
    GLV_OK_TRY(
        if (count > 0) { Require(indices, "indices"); } auto &m = Mesh(mesh);
        for (size_t t = 0; t < count; ++t) {
            m.Triangle(static_cast<size_t>(indices[(t * 3) + 0]),
                       static_cast<size_t>(indices[(t * 3) + 1]),
                       static_cast<size_t>(indices[(t * 3) + 2]));
        });
}

glv_controller *glv_create_null_controller(void) {
    GLV_TRY(nullptr, return new glv_controller(
                         std::make_shared<glviskit::NullController>()););
}

glv_controller *glv_create_first_person_controller(void) {
    GLV_TRY(nullptr, return new glv_controller(
                         std::make_shared<glviskit::FirstPersonController>()););
}

glv_controller *glv_create_spherical_controller(void) {
    GLV_TRY(nullptr, return new glv_controller(
                         std::make_shared<glviskit::SphericalController>()););
}

int glv_controller_get_type(glv_controller *controller) {
    GLV_TRY(
        GLV_CONTROLLER_UNKNOWN, const auto &base = Controller(controller);
        if (dynamic_cast<const glviskit::NullController *>(&base) != nullptr) {
            return GLV_CONTROLLER_NULL;
        } if (dynamic_cast<const glviskit::FirstPersonController *>(&base) !=
              nullptr) {
            return GLV_CONTROLLER_FIRST_PERSON;
        } if (dynamic_cast<const glviskit::SphericalController *>(&base) !=
              nullptr) {
            return GLV_CONTROLLER_SPHERICAL;
        } return GLV_CONTROLLER_UNKNOWN;);
}

int glv_controller_set_key_move_sensitivity(glv_controller *controller,
                                            float sensitivity) {
    GLV_OK_TRY(WithMoveController(
                   controller, "key_move_sensitivity",
                   [&](auto &c) { c.SetKeyMoveSensitivity(sensitivity); },
                   [&](auto &c) { c.SetKeyMoveSensitivity(sensitivity); }););
}

int glv_controller_get_key_move_sensitivity(glv_controller *controller,
                                            float *out_sensitivity) {
    GLV_OK_TRY(
        Require(out_sensitivity, "out_sensitivity"); WithMoveController(
            controller, "key_move_sensitivity",
            [&](auto &c) { *out_sensitivity = c.GetKeyMoveSensitivity(); },
            [&](auto &c) { *out_sensitivity = c.GetKeyMoveSensitivity(); }););
}

int glv_controller_set_key_rot_sensitivity(glv_controller *controller,
                                           float sensitivity) {
    GLV_OK_TRY(WithMoveController(
                   controller, "key_rot_sensitivity",
                   [&](auto &c) { c.SetKeyRotSensitivity(sensitivity); },
                   [&](auto &c) { c.SetKeyRotSensitivity(sensitivity); }););
}

int glv_controller_get_key_rot_sensitivity(glv_controller *controller,
                                           float *out_sensitivity) {
    GLV_OK_TRY(
        Require(out_sensitivity, "out_sensitivity"); WithMoveController(
            controller, "key_rot_sensitivity",
            [&](auto &c) { *out_sensitivity = c.GetKeyRotSensitivity(); },
            [&](auto &c) { *out_sensitivity = c.GetKeyRotSensitivity(); }););
}

int glv_controller_set_mouse_sensitivity(glv_controller *controller,
                                         float sensitivity) {
    GLV_OK_TRY(WithMoveController(
                   controller, "mouse_sensitivity",
                   [&](auto &c) { c.SetMouseSensitivity(sensitivity); },
                   [&](auto &c) { c.SetMouseSensitivity(sensitivity); }););
}

int glv_controller_get_mouse_sensitivity(glv_controller *controller,
                                         float *out_sensitivity) {
    GLV_OK_TRY(
        Require(out_sensitivity, "out_sensitivity"); WithMoveController(
            controller, "mouse_sensitivity",
            [&](auto &c) { *out_sensitivity = c.GetMouseSensitivity(); },
            [&](auto &c) { *out_sensitivity = c.GetMouseSensitivity(); }););
}

int glv_controller_set_wheel_sensitivity(glv_controller *controller,
                                         float sensitivity) {
    GLV_OK_TRY(WithSpherical(controller, "wheel_sensitivity", [&](auto &c) {
                   c.SetWheelSensitivity(sensitivity);
               }););
}

int glv_controller_get_wheel_sensitivity(glv_controller *controller,
                                         float *out_sensitivity) {
    GLV_OK_TRY(Require(out_sensitivity, "out_sensitivity");
               WithSpherical(controller, "wheel_sensitivity", [&](auto &c) {
                   *out_sensitivity = c.GetWheelSensitivity();
               }););
}

int glv_ui_new_frame(void) { GLV_OK_TRY(glviskit::UiBegin();); }

int glv_ui_begin(glv_window *window, const char *title) {
    GLV_TRY(GLV_ERROR, Require(title, "title");
            return Window(window).Ui().Begin(title) ? 1 : 0;);
}

int glv_ui_end(glv_window *window) { GLV_OK_TRY(Window(window).Ui().End();); }

int glv_ui_text(glv_window *window, const char *text) {
    GLV_OK_TRY(Require(text, "text"); Window(window).Ui().Text(text););
}

int glv_ui_separator(glv_window *window) {
    GLV_OK_TRY(Window(window).Ui().Separator(););
}

int glv_ui_same_line(glv_window *window) {
    GLV_OK_TRY(Window(window).Ui().SameLine(););
}

int glv_ui_button(glv_window *window, const char *label) {
    GLV_TRY(GLV_ERROR, Require(label, "label");
            return Window(window).Ui().Button(label) ? 1 : 0;);
}

int glv_ui_checkbox(glv_window *window, const char *label, int *value) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(value, "value");
            bool v = *value != 0;
            bool changed = Window(window).Ui().Checkbox(label, v);
            *value = v ? 1 : 0; return changed ? 1 : 0;);
}

int glv_ui_slider_float(glv_window *window, const char *label, float *value,
                        float min, float max) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(value, "value");
            return Window(window).Ui().SliderFloat(label, *value, min, max)
                       ? 1
                       : 0;);
}

int glv_ui_slider_float3(glv_window *window, const char *label, float *value,
                         float min, float max) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(value, "value");
            return Window(window).Ui().SliderFloat3(label, value, min, max)
                       ? 1
                       : 0;);
}

int glv_ui_slider_int(glv_window *window, const char *label, int *value,
                      int min, int max) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(value, "value");
            return Window(window).Ui().SliderInt(label, *value, min, max) ? 1
                                                                          : 0;);
}

int glv_ui_combo(glv_window *window, const char *label, int *current,
                 const char *items) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(current, "current");
            Require(items, "items");
            return Window(window).Ui().Combo(label, *current, items) ? 1 : 0;);
}

int glv_ui_drag_float(glv_window *window, const char *label, float *value,
                      float speed, float min, float max) {
    GLV_TRY(
        GLV_ERROR, Require(label, "label"); Require(value, "value");
        return Window(window).Ui().DragFloat(label, *value, speed, min, max)
                   ? 1
                   : 0;);
}

int glv_ui_color_edit3(glv_window *window, const char *label, float *rgb) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(rgb, "rgb");
            return Window(window).Ui().ColorEdit3(label, rgb) ? 1 : 0;);
}

int glv_ui_color_edit4(glv_window *window, const char *label, float *rgba) {
    GLV_TRY(GLV_ERROR, Require(label, "label"); Require(rgba, "rgba");
            return Window(window).Ui().ColorEdit4(label, rgba) ? 1 : 0;);
}

int glv_ui_plot_lines(glv_window *window, const char *label,
                      const float *values, int count) {
    GLV_OK_TRY(Require(label, "label");
               if (count > 0) { Require(values, "values"); }
               Window(window).Ui().PlotLines(label, values, count););
}

int glv_ui_want_capture_mouse(glv_window *window) {
    GLV_TRY(GLV_ERROR, return Window(window).Ui().WantCaptureMouse() ? 1 : 0;);
}

int glv_ui_want_capture_keyboard(glv_window *window) {
    GLV_TRY(GLV_ERROR,
            return Window(window).Ui().WantCaptureKeyboard() ? 1 : 0;);
}

}  // extern "C"
