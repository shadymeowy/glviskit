#pragma once

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) && !defined(GLVISKIT_C_STATIC)
#if defined(GLVISKIT_C_BUILD)
#define GLVISKIT_C_API __declspec(dllexport)
#else
#define GLVISKIT_C_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define GLVISKIT_C_API __attribute__((visibility("default")))
#else
#define GLVISKIT_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct glv_window glv_window;
typedef struct glv_camera glv_camera;
typedef struct glv_render_list glv_render_list;
typedef struct glv_path glv_path;
typedef struct glv_mesh glv_mesh;
typedef struct glv_controller glv_controller;

enum {
    GLV_ERROR = -1,
    GLV_OK = 0,
};

enum {
    GLV_CONTROLLER_UNKNOWN = -1,
    GLV_CONTROLLER_NULL = 0,
    GLV_CONTROLLER_FIRST_PERSON = 1,
    GLV_CONTROLLER_SPHERICAL = 2,
};

GLVISKIT_C_API const char *glv_error(void);
GLVISKIT_C_API void glv_c_api_version(int *major, int *minor, int *patch);

GLVISKIT_C_API void glv_window_destroy(glv_window *window);
GLVISKIT_C_API void glv_camera_destroy(glv_camera *camera);
GLVISKIT_C_API void glv_render_list_destroy(glv_render_list *render_list);
GLVISKIT_C_API void glv_path_destroy(glv_path *path);
GLVISKIT_C_API void glv_mesh_destroy(glv_mesh *mesh);
GLVISKIT_C_API void glv_controller_destroy(glv_controller *controller);

GLVISKIT_C_API glv_window *glv_create_window(const char *title, int width,
                                             int height);
GLVISKIT_C_API glv_render_list *glv_create_render_list(void);
GLVISKIT_C_API float glv_get_time_seconds(void);
GLVISKIT_C_API int glv_loop(void);
GLVISKIT_C_API int glv_render(void);

GLVISKIT_C_API int glv_window_add_render_list(glv_window *window,
                                              glv_render_list *render_list);
GLVISKIT_C_API glv_camera *glv_window_get_camera(glv_window *window);
GLVISKIT_C_API int glv_window_set_camera(glv_window *window,
                                         glv_camera *camera);
GLVISKIT_C_API glv_controller *glv_window_get_controller(glv_window *window);
GLVISKIT_C_API int glv_window_set_controller(glv_window *window,
                                             glv_controller *controller);
GLVISKIT_C_API int glv_window_set_background_color(glv_window *window, float r,
                                                   float g, float b, float a);
GLVISKIT_C_API int glv_window_get_background_color(glv_window *window,
                                                   float *out_r, float *out_g,
                                                   float *out_b, float *out_a);
GLVISKIT_C_API int glv_window_get_size_in_pixels(glv_window *window,
                                                 int *out_width,
                                                 int *out_height);
GLVISKIT_C_API int glv_window_capture_rgba(glv_window *window, uint8_t *pixels,
                                           size_t pixel_count);
GLVISKIT_C_API int glv_window_make_current(glv_window *window);
GLVISKIT_C_API int glv_window_render(glv_window *window);

GLVISKIT_C_API int glv_camera_calculate_transform(glv_camera *camera,
                                                  float out_row_major_16[16]);
GLVISKIT_C_API int glv_camera_perspective_fov(glv_camera *camera, float hfov,
                                              float vfov, float near_plane,
                                              float far_plane);
GLVISKIT_C_API int glv_camera_perspective(glv_camera *camera, float fxn,
                                          float fyn, float cx, float cy,
                                          float near_plane, float far_plane);
GLVISKIT_C_API int glv_camera_set_position(glv_camera *camera, float x, float y,
                                           float z);
GLVISKIT_C_API int glv_camera_get_position(glv_camera *camera, float *out_x,
                                           float *out_y, float *out_z);
GLVISKIT_C_API int glv_camera_set_rotation(glv_camera *camera, float roll,
                                           float pitch, float yaw);
GLVISKIT_C_API int glv_camera_get_rotation(glv_camera *camera, float *out_roll,
                                           float *out_pitch, float *out_yaw);
GLVISKIT_C_API int glv_camera_set_distance(glv_camera *camera, float distance);
GLVISKIT_C_API int glv_camera_get_distance(glv_camera *camera,
                                           float *out_distance);
GLVISKIT_C_API int glv_camera_set_axis_rotation(glv_camera *camera, float w,
                                                float x, float y, float z);
GLVISKIT_C_API int glv_camera_get_axis_rotation(glv_camera *camera,
                                                float *out_w, float *out_x,
                                                float *out_y, float *out_z);
GLVISKIT_C_API int glv_camera_set_axis_rotation_axes(glv_camera *camera, int x,
                                                     int y, int z, int inv_x,
                                                     int inv_y, int inv_z);
GLVISKIT_C_API int glv_camera_get_viewport_size(glv_camera *camera,
                                                float *out_width,
                                                float *out_height);
GLVISKIT_C_API int glv_camera_set_preserve_aspect_ratio(glv_camera *camera,
                                                        int preserve);
GLVISKIT_C_API int glv_camera_get_preserve_aspect_ratio(glv_camera *camera,
                                                        int *out_preserve);

GLVISKIT_C_API int glv_render_list_color(glv_render_list *render_list, float r,
                                         float g, float b, float a);
GLVISKIT_C_API int glv_render_list_size(glv_render_list *render_list,
                                        float size);
GLVISKIT_C_API int glv_render_list_point(glv_render_list *render_list, float x,
                                         float y, float z);
GLVISKIT_C_API int glv_render_list_point_color(glv_render_list *render_list,
                                               float x, float y, float z,
                                               float r, float g, float b,
                                               float a);
GLVISKIT_C_API int glv_render_list_point_color_size(
    glv_render_list *render_list, float x, float y, float z, float r, float g,
    float b, float a, float size);
GLVISKIT_C_API int glv_render_list_line(glv_render_list *render_list, float x0,
                                        float y0, float z0, float x1, float y1,
                                        float z1);
GLVISKIT_C_API int glv_render_list_line_color(glv_render_list *render_list,
                                              float x0, float y0, float z0,
                                              float x1, float y1, float z1,
                                              float r, float g, float b,
                                              float a);
GLVISKIT_C_API int glv_render_list_line_color_size(glv_render_list *render_list,
                                                   float x0, float y0, float z0,
                                                   float x1, float y1, float z1,
                                                   float r, float g, float b,
                                                   float a, float size);
GLVISKIT_C_API int glv_render_list_circle(glv_render_list *render_list, float x,
                                          float y, float z);
GLVISKIT_C_API int glv_render_list_circle_color(glv_render_list *render_list,
                                                float x, float y, float z,
                                                float r, float g, float b,
                                                float a);
GLVISKIT_C_API int glv_render_list_circle_color_size(
    glv_render_list *render_list, float x, float y, float z, float r, float g,
    float b, float a, float size);
GLVISKIT_C_API glv_path *glv_render_list_path_begin(
    glv_render_list *render_list);
GLVISKIT_C_API glv_mesh *glv_render_list_mesh_begin(
    glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_add_instance(glv_render_list *render_list,
                                                float x, float y, float z,
                                                float rx, float ry, float rz,
                                                float sx, float sy, float sz);
GLVISKIT_C_API int glv_render_list_add_instance_quat(
    glv_render_list *render_list, float x, float y, float z, float rw, float rx,
    float ry, float rz, float sx, float sy, float sz);
GLVISKIT_C_API int glv_render_list_save(glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_restore(glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_clear(glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_save_instances(glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_restore_instances(
    glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_clear_instances(
    glv_render_list *render_list);
GLVISKIT_C_API int glv_render_list_set_enabled(glv_render_list *render_list,
                                               int enabled);
GLVISKIT_C_API int glv_render_list_get_enabled(glv_render_list *render_list,
                                               int *out_enabled);

GLVISKIT_C_API int glv_path_line_to(glv_path *path, float x, float y, float z);
GLVISKIT_C_API int glv_path_line_to_color(glv_path *path, float x, float y,
                                          float z, float r, float g, float b,
                                          float a);
GLVISKIT_C_API int glv_path_line_to_color_size(glv_path *path, float x, float y,
                                               float z, float r, float g,
                                               float b, float a, float size);
GLVISKIT_C_API int glv_path_close(glv_path *path);
GLVISKIT_C_API int glv_path_line_end(glv_path *path);
GLVISKIT_C_API int glv_path_color(glv_path *path, float r, float g, float b,
                                  float a);
GLVISKIT_C_API int glv_path_size(glv_path *path, float size);

GLVISKIT_C_API int glv_mesh_vertex(glv_mesh *mesh, float x, float y, float z,
                                   size_t *out_index);
GLVISKIT_C_API int glv_mesh_vertex_color(glv_mesh *mesh, float x, float y,
                                         float z, float r, float g, float b,
                                         float a, size_t *out_index);
GLVISKIT_C_API int glv_mesh_triangle(glv_mesh *mesh, size_t i0, size_t i1,
                                     size_t i2);
GLVISKIT_C_API int glv_mesh_color(glv_mesh *mesh, float r, float g, float b,
                                  float a);
GLVISKIT_C_API int glv_mesh_vertex_count(glv_mesh *mesh,
                                         size_t *out_vertex_count);

GLVISKIT_C_API glv_controller *glv_create_null_controller(void);
GLVISKIT_C_API glv_controller *glv_create_first_person_controller(void);
GLVISKIT_C_API glv_controller *glv_create_spherical_controller(void);
GLVISKIT_C_API int glv_controller_get_type(glv_controller *controller);
GLVISKIT_C_API int glv_controller_set_key_move_sensitivity(
    glv_controller *controller, float sensitivity);
GLVISKIT_C_API int glv_controller_get_key_move_sensitivity(
    glv_controller *controller, float *out_sensitivity);
GLVISKIT_C_API int glv_controller_set_key_rot_sensitivity(
    glv_controller *controller, float sensitivity);
GLVISKIT_C_API int glv_controller_get_key_rot_sensitivity(
    glv_controller *controller, float *out_sensitivity);
GLVISKIT_C_API int glv_controller_set_mouse_sensitivity(
    glv_controller *controller, float sensitivity);
GLVISKIT_C_API int glv_controller_get_mouse_sensitivity(
    glv_controller *controller, float *out_sensitivity);
GLVISKIT_C_API int glv_controller_set_wheel_sensitivity(
    glv_controller *controller, float sensitivity);
GLVISKIT_C_API int glv_controller_get_wheel_sensitivity(
    glv_controller *controller, float *out_sensitivity);

#ifdef __cplusplus
}
#endif
