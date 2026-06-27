from libc.stddef cimport size_t
from libc.stdint cimport uint8_t, int32_t


cdef extern from "glviskit/c_api.h":
    ctypedef struct glv_window:
        pass
    ctypedef struct glv_camera:
        pass
    ctypedef struct glv_render_list:
        pass
    ctypedef struct glv_path:
        pass
    ctypedef struct glv_mesh:
        pass
    ctypedef struct glv_controller:
        pass

    enum:
        GLV_ERROR
        GLV_OK
        GLV_CONTROLLER_UNKNOWN
        GLV_CONTROLLER_NULL
        GLV_CONTROLLER_FIRST_PERSON
        GLV_CONTROLLER_SPHERICAL

    const char *glv_error()
    void glv_c_api_version(int *major, int *minor, int *patch)

    void glv_window_destroy(glv_window *window)
    void glv_camera_destroy(glv_camera *camera)
    void glv_render_list_destroy(glv_render_list *render_list)
    void glv_path_destroy(glv_path *path)
    void glv_mesh_destroy(glv_mesh *mesh)
    void glv_controller_destroy(glv_controller *controller)

    glv_window *glv_create_window(const char *title, int width, int height)
    glv_render_list *glv_create_render_list()
    float glv_get_time_seconds()
    int glv_loop()
    int glv_render()

    int glv_window_add_render_list(glv_window *window,
                                   glv_render_list *render_list)
    glv_camera *glv_window_get_camera(glv_window *window)
    int glv_window_set_camera(glv_window *window, glv_camera *camera)
    glv_controller *glv_window_get_controller(glv_window *window)
    int glv_window_set_controller(glv_window *window,
                                  glv_controller *controller)
    int glv_window_set_background_color(glv_window *window, float r, float g,
                                        float b, float a)
    int glv_window_get_background_color(glv_window *window, float *out_r,
                                        float *out_g, float *out_b,
                                        float *out_a)
    int glv_window_get_size_in_pixels(glv_window *window, int *out_width,
                                      int *out_height)
    int glv_window_capture_rgba(glv_window *window, uint8_t *pixels,
                                size_t pixel_count)
    int glv_window_make_current(glv_window *window)
    int glv_window_render(glv_window *window)

    int glv_camera_calculate_transform(glv_camera *camera,
                                       float out_row_major_16[16])
    int glv_camera_perspective_fov(glv_camera *camera, float hfov, float vfov,
                                   float near_plane, float far_plane)
    int glv_camera_perspective(glv_camera *camera, float fxn, float fyn,
                               float cx, float cy, float near_plane,
                               float far_plane)
    int glv_camera_set_position(glv_camera *camera, float x, float y, float z)
    int glv_camera_get_position(glv_camera *camera, float *out_x,
                                float *out_y, float *out_z)
    int glv_camera_set_rotation(glv_camera *camera, float roll, float pitch,
                                float yaw)
    int glv_camera_get_rotation(glv_camera *camera, float *out_roll,
                                float *out_pitch, float *out_yaw)
    int glv_camera_set_distance(glv_camera *camera, float distance)
    int glv_camera_get_distance(glv_camera *camera, float *out_distance)
    int glv_camera_set_axis_rotation(glv_camera *camera, float w, float x,
                                     float y, float z)
    int glv_camera_get_axis_rotation(glv_camera *camera, float *out_w,
                                     float *out_x, float *out_y, float *out_z)
    int glv_camera_set_axis_rotation_axes(glv_camera *camera, int x, int y,
                                          int z, int inv_x, int inv_y,
                                          int inv_z)
    int glv_camera_get_viewport_size(glv_camera *camera, float *out_width,
                                     float *out_height)
    int glv_camera_set_preserve_aspect_ratio(glv_camera *camera, int preserve)
    int glv_camera_get_preserve_aspect_ratio(glv_camera *camera,
                                             int *out_preserve)

    int glv_render_list_color(glv_render_list *render_list, float r, float g,
                              float b, float a)
    int glv_render_list_size(glv_render_list *render_list, float size)
    int glv_render_list_point(glv_render_list *render_list, float x, float y,
                              float z)
    int glv_render_list_point_color(glv_render_list *render_list, float x,
                                    float y, float z, float r, float g,
                                    float b, float a)
    int glv_render_list_point_color_size(glv_render_list *render_list,
                                         float x, float y, float z, float r,
                                         float g, float b, float a,
                                         float size)
    int glv_render_list_line(glv_render_list *render_list, float x0, float y0,
                             float z0, float x1, float y1, float z1)
    int glv_render_list_line_color(glv_render_list *render_list, float x0,
                                   float y0, float z0, float x1, float y1,
                                   float z1, float r, float g, float b,
                                   float a)
    int glv_render_list_line_color_size(glv_render_list *render_list,
                                        float x0, float y0, float z0,
                                        float x1, float y1, float z1,
                                        float r, float g, float b, float a,
                                        float size)
    int glv_render_list_circle(glv_render_list *render_list, float x, float y,
                               float z)
    int glv_render_list_circle_color(glv_render_list *render_list, float x,
                                     float y, float z, float r, float g,
                                     float b, float a)
    int glv_render_list_circle_color_size(glv_render_list *render_list,
                                          float x, float y, float z, float r,
                                          float g, float b, float a,
                                          float size)
    int glv_render_list_points(glv_render_list *render_list, const float *xyz,
                               const float *rgba, const float *sizes,
                               size_t count)
    int glv_render_list_circles(glv_render_list *render_list, const float *xyz,
                                const float *rgba, const float *sizes,
                                size_t count)
    int glv_render_list_lines(glv_render_list *render_list, const float *starts,
                              const float *ends, const float *rgba,
                              const float *sizes, size_t count)
    int glv_render_list_polygons(glv_render_list *render_list, const float *xyz,
                                 const float *rgba, const float *sizes,
                                 size_t groups, size_t count)
    int glv_render_list_polylines(glv_render_list *render_list,
                                  const float *xyz, const float *rgba,
                                  const float *sizes, size_t groups,
                                  size_t count)
    int glv_render_list_fill_polygons(glv_render_list *render_list,
                                      const float *xyz, const float *rgba,
                                      size_t groups, size_t count)
    int glv_render_list_triangles(glv_render_list *render_list,
                                  const float *xyz, const float *rgba,
                                  size_t vertex_count, const int32_t *indices,
                                  size_t triangle_count)
    glv_path *glv_render_list_path_begin(glv_render_list *render_list)
    glv_mesh *glv_render_list_mesh_begin(glv_render_list *render_list)
    int glv_render_list_add_instance(glv_render_list *render_list, float x,
                                     float y, float z, float rx, float ry,
                                     float rz, float sx, float sy, float sz)
    int glv_render_list_add_instance_quat(glv_render_list *render_list,
                                          float x, float y, float z, float rw,
                                          float rx, float ry, float rz,
                                          float sx, float sy, float sz)
    int glv_render_list_add_instance_matrix(glv_render_list *render_list,
                                            const float row_major_16[16])
    int glv_render_list_save(glv_render_list *render_list)
    int glv_render_list_restore(glv_render_list *render_list)
    int glv_render_list_clear(glv_render_list *render_list)
    int glv_render_list_save_instances(glv_render_list *render_list)
    int glv_render_list_restore_instances(glv_render_list *render_list)
    int glv_render_list_clear_instances(glv_render_list *render_list)
    int glv_render_list_set_enabled(glv_render_list *render_list, int enabled)
    int glv_render_list_get_enabled(glv_render_list *render_list,
                                    int *out_enabled)

    int glv_path_line_to(glv_path *path, float x, float y, float z)
    int glv_path_line_to_color(glv_path *path, float x, float y, float z,
                               float r, float g, float b, float a)
    int glv_path_line_to_color_size(glv_path *path, float x, float y, float z,
                                    float r, float g, float b, float a,
                                    float size)
    int glv_path_close(glv_path *path)
    int glv_path_line_end(glv_path *path)
    int glv_path_color(glv_path *path, float r, float g, float b, float a)
    int glv_path_size(glv_path *path, float size)
    int glv_path_line_to_many(glv_path *path, const float *xyz,
                              const float *rgba, const float *sizes,
                              size_t count)

    int glv_mesh_vertex(glv_mesh *mesh, float x, float y, float z,
                        size_t *out_index)
    int glv_mesh_vertex_color(glv_mesh *mesh, float x, float y, float z,
                              float r, float g, float b, float a,
                              size_t *out_index)
    int glv_mesh_triangle(glv_mesh *mesh, size_t i0, size_t i1, size_t i2)
    int glv_mesh_color(glv_mesh *mesh, float r, float g, float b, float a)
    int glv_mesh_vertex_count(glv_mesh *mesh, size_t *out_vertex_count)
    int glv_mesh_vertices(glv_mesh *mesh, const float *xyz, const float *rgba,
                          size_t *out_indices, size_t count)
    int glv_mesh_triangles(glv_mesh *mesh, const int32_t *indices, size_t count)

    glv_controller *glv_create_null_controller()
    glv_controller *glv_create_first_person_controller()
    glv_controller *glv_create_spherical_controller()
    int glv_controller_get_type(glv_controller *controller)
    int glv_controller_set_key_move_sensitivity(glv_controller *controller,
                                                float sensitivity)
    int glv_controller_get_key_move_sensitivity(glv_controller *controller,
                                                float *out_sensitivity)
    int glv_controller_set_key_rot_sensitivity(glv_controller *controller,
                                               float sensitivity)
    int glv_controller_get_key_rot_sensitivity(glv_controller *controller,
                                               float *out_sensitivity)
    int glv_controller_set_mouse_sensitivity(glv_controller *controller,
                                             float sensitivity)
    int glv_controller_get_mouse_sensitivity(glv_controller *controller,
                                             float *out_sensitivity)
    int glv_controller_set_wheel_sensitivity(glv_controller *controller,
                                             float sensitivity)
    int glv_controller_get_wheel_sensitivity(glv_controller *controller,
                                             float *out_sensitivity)

    int glv_ui_new_frame()
    int glv_ui_begin(glv_window *window, const char *title)
    int glv_ui_end(glv_window *window)
    int glv_ui_text(glv_window *window, const char *text)
    int glv_ui_separator(glv_window *window)
    int glv_ui_same_line(glv_window *window)
    int glv_ui_button(glv_window *window, const char *label)
    int glv_ui_checkbox(glv_window *window, const char *label, int *value)
    int glv_ui_slider_float(glv_window *window, const char *label,
                            float *value, float min, float max)
    int glv_ui_slider_float3(glv_window *window, const char *label,
                             float *value, float min, float max)
    int glv_ui_slider_int(glv_window *window, const char *label, int *value,
                          int min, int max)
    int glv_ui_combo(glv_window *window, const char *label, int *current,
                     const char *items)
    int glv_ui_drag_float(glv_window *window, const char *label, float *value,
                          float speed, float min, float max)
    int glv_ui_color_edit3(glv_window *window, const char *label, float *rgb)
    int glv_ui_color_edit4(glv_window *window, const char *label, float *rgba)
    int glv_ui_plot_lines(glv_window *window, const char *label,
                          const float *values, int count)
    int glv_ui_want_capture_mouse(glv_window *window)
    int glv_ui_want_capture_keyboard(glv_window *window)
