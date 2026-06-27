# cython: language_level=3
# cython: boundscheck=False
# cython: wraparound=False

from libc.stddef cimport size_t
from libc.stdint cimport uint8_t, int32_t

from glviskit_c_api cimport *

import enum

import numpy as np


class MarkerType(enum.IntEnum):
    Square = 0
    Triangle = 1
    Diamond = 2
    Ring = 3
    Circle = 4


class TextAlign(enum.IntEnum):
    Left = 0
    Center = 1
    Right = 2


cdef str _error():
    cdef const char *message = glv_error()
    if message == NULL or message[0] == 0:
        return "glviskit C API call failed"
    return message.decode("utf-8", "replace")


cdef void _check(int status) except *:
    if status != GLV_OK:
        raise RuntimeError(_error())


cdef bint _check_ui(int status) except *:
    if status == GLV_ERROR:
        raise RuntimeError(_error())
    return status != 0


cdef int _rank(object value):
    try:
        return memoryview(value).ndim
    except TypeError:
        return 0


# color/size fall back to the builder's current state when not given
cdef tuple _rl_color_size(glv_render_list *ptr, color, size):
    cdef float cr, cg, cb, ca, sz
    if color is None:
        _check(glv_render_list_get_color(ptr, &cr, &cg, &cb, &ca))
        color = (cr, cg, cb, ca)
    if size is None:
        _check(glv_render_list_get_size(ptr, &sz))
        size = sz
    return color, size


cdef tuple _path_color_size(glv_path *ptr, color, size):
    cdef float cr, cg, cb, ca, sz
    if color is None:
        _check(glv_path_get_color(ptr, &cr, &cg, &cb, &ca))
        color = (cr, cg, cb, ca)
    if size is None:
        _check(glv_path_get_size(ptr, &sz))
        size = sz
    return color, size


cdef _mesh_color(glv_mesh *ptr, color):
    cdef float cr, cg, cb, ca
    if color is None:
        _check(glv_mesh_get_color(ptr, &cr, &cg, &cb, &ca))
        color = (cr, cg, cb, ca)
    return color


cdef class Window:
    cdef glv_window *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        raise TypeError("glviskit.Window cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_window_destroy(self.ptr)
            self.ptr = NULL

    def add_render_list(self, RenderList rb):
        _check(glv_window_add_render_list(self.ptr, rb.ptr))

    @property
    def camera(self):
        cdef glv_camera *handle = glv_window_get_camera(self.ptr)
        cdef Camera obj
        if handle == NULL:
            raise RuntimeError(_error())
        obj = Camera.__new__(Camera)
        obj.ptr = handle
        return obj

    @camera.setter
    def camera(self, Camera camera):
        _check(glv_window_set_camera(self.ptr, camera.ptr))

    @property
    def background_color(self):
        cdef float r, g, b, a
        _check(glv_window_get_background_color(self.ptr, &r, &g, &b, &a))
        return [r, g, b, a]

    @background_color.setter
    def background_color(self, color):
        _check(glv_window_set_background_color(
            self.ptr, <float>color[0], <float>color[1],
            <float>color[2], <float>color[3]))

    @property
    def controller(self):
        cdef glv_controller *handle = glv_window_get_controller(self.ptr)
        cdef int kind
        cdef BaseController obj
        if handle == NULL:
            raise RuntimeError(_error())
        kind = glv_controller_get_type(handle)
        if kind == GLV_CONTROLLER_NULL:
            obj = NullController.__new__(NullController)
        elif kind == GLV_CONTROLLER_FIRST_PERSON:
            obj = FirstPersonController.__new__(FirstPersonController)
        elif kind == GLV_CONTROLLER_SPHERICAL:
            obj = SphericalController.__new__(SphericalController)
        else:
            obj = BaseController.__new__(BaseController)
        obj.ptr = handle
        return obj

    @controller.setter
    def controller(self, BaseController controller):
        _check(glv_window_set_controller(self.ptr, controller.ptr))

    def make_current(self):
        _check(glv_window_make_current(self.ptr))

    def capture_rgba(self, image):
        cdef int width, height
        cdef unsigned char[:, :, ::1] pixels = image
        _check(glv_window_get_size_in_pixels(self.ptr, &width, &height))
        if pixels.shape[0] != height or pixels.shape[1] != width or pixels.shape[2] != 4:
            raise ValueError("capture_rgba(image) requires image.shape == (window_height, window_width, 4)")
        _check(glv_window_capture_rgba(
            self.ptr, <uint8_t *>&pixels[0, 0, 0],
            <size_t>width * <size_t>height * 4))

    def render(self):
        _check(glv_window_render(self.ptr))

    @property
    def ui(self):
        return WindowUI(self)


cdef class WindowUI:
    cdef Window window

    def __cinit__(self, Window window):
        self.window = window

    def begin(self, str title):
        return _check_ui(glv_ui_begin(self.window.ptr, title.encode("utf-8")))

    def end(self):
        _check(glv_ui_end(self.window.ptr))

    def panel(self, str title):
        return _Panel(self, title)

    def text(self, str text):
        _check(glv_ui_text(self.window.ptr, text.encode("utf-8")))

    def separator(self):
        _check(glv_ui_separator(self.window.ptr))

    def same_line(self):
        _check(glv_ui_same_line(self.window.ptr))

    def button(self, str label):
        return _check_ui(glv_ui_button(self.window.ptr, label.encode("utf-8")))

    def checkbox(self, str label, value):
        cdef int v = 1 if value else 0
        cdef bint changed = _check_ui(
            glv_ui_checkbox(self.window.ptr, label.encode("utf-8"), &v))
        return changed, v != 0

    def slider_float(self, str label, value, float vmin, float vmax):
        cdef float v = value
        cdef bint changed = _check_ui(
            glv_ui_slider_float(self.window.ptr, label.encode("utf-8"), &v, vmin, vmax))
        return changed, v

    def slider_float3(self, str label, value, float vmin, float vmax):
        cdef float[3] v = [value[0], value[1], value[2]]
        cdef bint changed = _check_ui(
            glv_ui_slider_float3(self.window.ptr, label.encode("utf-8"), v, vmin, vmax))
        return changed, (v[0], v[1], v[2])

    def slider_int(self, str label, value, int vmin, int vmax):
        cdef int v = value
        cdef bint changed = _check_ui(
            glv_ui_slider_int(self.window.ptr, label.encode("utf-8"), &v, vmin, vmax))
        return changed, v

    def combo(self, str label, current, str items):
        cdef int c = current
        cdef bint changed = _check_ui(
            glv_ui_combo(self.window.ptr, label.encode("utf-8"), &c,
                         items.encode("utf-8")))
        return changed, c

    def drag_float(self, str label, value, float speed=1.0,
                   float vmin=0.0, float vmax=0.0):
        cdef float v = value
        cdef bint changed = _check_ui(
            glv_ui_drag_float(self.window.ptr, label.encode("utf-8"), &v, speed,
                              vmin, vmax))
        return changed, v

    def color_edit3(self, str label, color):
        cdef float[3] c = [color[0], color[1], color[2]]
        cdef bint changed = _check_ui(
            glv_ui_color_edit3(self.window.ptr, label.encode("utf-8"), c))
        return changed, (c[0], c[1], c[2])

    def color_edit4(self, str label, color):
        cdef float[4] c = [color[0], color[1], color[2], color[3]]
        cdef bint changed = _check_ui(
            glv_ui_color_edit4(self.window.ptr, label.encode("utf-8"), c))
        return changed, (c[0], c[1], c[2], c[3])

    def plot_lines(self, str label, values):
        cdef float[::1] buf = np.ascontiguousarray(values, dtype=np.float32)
        cdef float *data = NULL
        if buf.shape[0] > 0:
            data = &buf[0]
        _check(glv_ui_plot_lines(self.window.ptr, label.encode("utf-8"), data,
                                 <int>buf.shape[0]))

    def want_capture_mouse(self):
        return _check_ui(glv_ui_want_capture_mouse(self.window.ptr))

    def want_capture_keyboard(self):
        return _check_ui(glv_ui_want_capture_keyboard(self.window.ptr))


cdef class _Panel:
    cdef WindowUI ui
    cdef str title

    def __cinit__(self, WindowUI ui, str title):
        self.ui = ui
        self.title = title

    def __enter__(self):
        return self.ui.begin(self.title)

    def __exit__(self, exc_type, exc, tb):
        self.ui.end()
        return False


cdef class Camera:
    cdef glv_camera *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        raise TypeError("glviskit.Camera cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_camera_destroy(self.ptr)
            self.ptr = NULL

    def calculate_transform(self):
        result = np.empty((4, 4), dtype=np.float32)
        cdef float[:, ::1] matrix = result
        _check(glv_camera_calculate_transform(self.ptr, &matrix[0, 0]))
        return result

    def perspective_fov(self, hfov, vfov, near=0.1, far=100.0):
        _check(glv_camera_perspective_fov(
            self.ptr, <float>hfov, <float>vfov, <float>near, <float>far))

    def perspective(self, fxn, fyn, cx=0.5, cy=0.5, near=0.1, far=100.0):
        _check(glv_camera_perspective(
            self.ptr, <float>fxn, <float>fyn, <float>cx, <float>cy,
            <float>near, <float>far))

    @property
    def position(self):
        cdef float x, y, z
        _check(glv_camera_get_position(self.ptr, &x, &y, &z))
        return [x, y, z]

    @position.setter
    def position(self, value):
        _check(glv_camera_set_position(
            self.ptr, <float>value[0], <float>value[1], <float>value[2]))

    @property
    def rotation(self):
        cdef float roll, pitch, yaw
        _check(glv_camera_get_rotation(self.ptr, &roll, &pitch, &yaw))
        return [roll, pitch, yaw]

    @rotation.setter
    def rotation(self, value):
        _check(glv_camera_set_rotation(
            self.ptr, <float>value[0], <float>value[1], <float>value[2]))

    @property
    def distance(self):
        cdef float value
        _check(glv_camera_get_distance(self.ptr, &value))
        return value

    @distance.setter
    def distance(self, value):
        _check(glv_camera_set_distance(self.ptr, <float>value))

    @property
    def axis_rotation(self):
        cdef float w, x, y, z
        _check(glv_camera_get_axis_rotation(self.ptr, &w, &x, &y, &z))
        return [w, x, y, z]

    @axis_rotation.setter
    def axis_rotation(self, value):
        _check(glv_camera_set_axis_rotation(
            self.ptr, <float>value[0], <float>value[1],
            <float>value[2], <float>value[3]))

    def set_axis_rotation(self, x, y, z, inv_x=False, inv_y=False, inv_z=False):
        _check(glv_camera_set_axis_rotation_axes(
            self.ptr, <int>x, <int>y, <int>z,
            <int>bool(inv_x), <int>bool(inv_y), <int>bool(inv_z)))

    @property
    def viewport_size(self):
        cdef float width, height
        _check(glv_camera_get_viewport_size(self.ptr, &width, &height))
        return [width, height]

    @property
    def preserve_aspect_ratio(self):
        cdef int value
        _check(glv_camera_get_preserve_aspect_ratio(self.ptr, &value))
        return value != 0

    @preserve_aspect_ratio.setter
    def preserve_aspect_ratio(self, value):
        _check(glv_camera_set_preserve_aspect_ratio(self.ptr, <int>bool(value)))


cdef class RenderList:
    cdef glv_render_list *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        raise TypeError("glviskit.RenderList cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_render_list_destroy(self.ptr)
            self.ptr = NULL

    def line(self, start, end, color=None, size=None):
        cdef const float[::1] s, e, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        if _rank(start) == 2:
            sa = np.ascontiguousarray(start, dtype=np.float32)
            ea = np.ascontiguousarray(end, dtype=np.float32)
            if sa.ndim != 2 or sa.shape[1] != 3 or ea.shape != sa.shape:
                raise ValueError("line batch expects starts/ends shape (N, 3)")
            if color is not None:
                ca = np.ascontiguousarray(color, dtype=np.float32)
                if ca.shape[0] != sa.shape[0] or ca.shape[1] != 4:
                    raise ValueError("line batch expects colors shape (N, 4)")
                c = ca.reshape(-1)
                c_ptr = &c[0]
                if size is not None:
                    za = np.ascontiguousarray(size, dtype=np.float32)
                    if za.shape[0] != sa.shape[0]:
                        raise ValueError("line batch expects sizes shape (N,)")
                    z = za.reshape(-1)
                    z_ptr = &z[0]
            s = sa.reshape(-1)
            e = ea.reshape(-1)
            _check(glv_render_list_lines(self.ptr, &s[0], &e[0], c_ptr, z_ptr, sa.shape[0]))
            return None

        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_line(
            self.ptr, <float>start[0], <float>start[1], <float>start[2],
            <float>end[0], <float>end[1], <float>end[2],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size))

    def point(self, point, color=None, size=None):
        cdef const float[::1] p, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        if _rank(point) == 2:
            pa = np.ascontiguousarray(point, dtype=np.float32)
            if pa.ndim != 2 or pa.shape[1] != 3:
                raise ValueError("point batch expects points shape (N, 3)")
            if color is not None:
                ca = np.ascontiguousarray(color, dtype=np.float32)
                if ca.shape[0] != pa.shape[0] or ca.shape[1] != 4:
                    raise ValueError("point batch expects colors shape (N, 4)")
                c = ca.reshape(-1)
                c_ptr = &c[0]
                if size is not None:
                    za = np.ascontiguousarray(size, dtype=np.float32)
                    if za.shape[0] != pa.shape[0]:
                        raise ValueError("point batch expects sizes shape (N,)")
                    z = za.reshape(-1)
                    z_ptr = &z[0]
            p = pa.reshape(-1)
            _check(glv_render_list_points(self.ptr, &p[0], c_ptr, z_ptr, pa.shape[0]))
            return None

        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_point(
            self.ptr, <float>point[0], <float>point[1], <float>point[2],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size))

    def circle(self, point, color=None, size=None):
        cdef const float[::1] p, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        if _rank(point) == 2:
            pa = np.ascontiguousarray(point, dtype=np.float32)
            if pa.ndim != 2 or pa.shape[1] != 3:
                raise ValueError("circle batch expects points shape (N, 3)")
            if color is not None:
                ca = np.ascontiguousarray(color, dtype=np.float32)
                if ca.shape[0] != pa.shape[0] or ca.shape[1] != 4:
                    raise ValueError("circle batch expects colors shape (N, 4)")
                c = ca.reshape(-1)
                c_ptr = &c[0]
                if size is not None:
                    za = np.ascontiguousarray(size, dtype=np.float32)
                    if za.shape[0] != pa.shape[0]:
                        raise ValueError("circle batch expects sizes shape (N,)")
                    z = za.reshape(-1)
                    z_ptr = &z[0]
            p = pa.reshape(-1)
            _check(glv_render_list_circles(self.ptr, &p[0], c_ptr, z_ptr, pa.shape[0]))
            return None

        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_circle(
            self.ptr, <float>point[0], <float>point[1], <float>point[2],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size))

    def polygon(self, vertices, colors=None, sizes=None):
        cdef const float[::1] v, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        va = np.ascontiguousarray(vertices, dtype=np.float32)
        if va.ndim == 2:
            va = va[None]
        if va.ndim != 3 or va.shape[1] < 2 or va.shape[2] != 3:
            raise ValueError("polygon expects vertices shape (N, 3) or (B, N, 3), N >= 2")
        if colors is not None:
            ca = np.ascontiguousarray(colors, dtype=np.float32)
            if ca.ndim == 2:
                ca = ca[None]
            if ca.shape[0] != va.shape[0] or ca.shape[1] != va.shape[1] or ca.shape[2] != 4:
                raise ValueError("polygon expects colors shape (N, 4) or (B, N, 4)")
            c = ca.reshape(-1)
            c_ptr = &c[0]
            if sizes is not None:
                za = np.ascontiguousarray(sizes, dtype=np.float32)
                if za.ndim == 1:
                    za = za[None]
                if za.shape[0] != va.shape[0] or za.shape[1] != va.shape[1]:
                    raise ValueError("polygon expects sizes shape (N,) or (B, N)")
                z = za.reshape(-1)
                z_ptr = &z[0]
        v = va.reshape(-1)
        _check(glv_render_list_polygons(self.ptr, &v[0], c_ptr, z_ptr, va.shape[0], va.shape[1]))

    def polyline(self, vertices, colors=None, sizes=None):
        cdef const float[::1] v, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        va = np.ascontiguousarray(vertices, dtype=np.float32)
        if va.ndim == 2:
            va = va[None]
        if va.ndim != 3 or va.shape[1] < 2 or va.shape[2] != 3:
            raise ValueError("polyline expects vertices shape (N, 3) or (B, N, 3), N >= 2")
        if colors is not None:
            ca = np.ascontiguousarray(colors, dtype=np.float32)
            if ca.ndim == 2:
                ca = ca[None]
            if ca.shape[0] != va.shape[0] or ca.shape[1] != va.shape[1] or ca.shape[2] != 4:
                raise ValueError("polyline expects colors shape (N, 4) or (B, N, 4)")
            c = ca.reshape(-1)
            c_ptr = &c[0]
            if sizes is not None:
                za = np.ascontiguousarray(sizes, dtype=np.float32)
                if za.ndim == 1:
                    za = za[None]
                if za.shape[0] != va.shape[0] or za.shape[1] != va.shape[1]:
                    raise ValueError("polyline expects sizes shape (N,) or (B, N)")
                z = za.reshape(-1)
                z_ptr = &z[0]
        v = va.reshape(-1)
        _check(glv_render_list_polylines(self.ptr, &v[0], c_ptr, z_ptr, va.shape[0], va.shape[1]))

    def fill_polygon(self, vertices, colors=None):
        cdef const float[::1] v, c
        cdef const float *c_ptr = NULL

        va = np.ascontiguousarray(vertices, dtype=np.float32)
        if va.ndim == 2:
            va = va[None]
        if va.ndim != 3 or va.shape[1] < 3 or va.shape[2] != 3:
            raise ValueError("fill_polygon expects vertices shape (N, 3) or (B, N, 3), N >= 3")
        if colors is not None:
            ca = np.ascontiguousarray(colors, dtype=np.float32)
            if ca.ndim == 2:
                ca = ca[None]
            if ca.shape[0] != va.shape[0] or ca.shape[1] != va.shape[1] or ca.shape[2] != 4:
                raise ValueError("fill_polygon expects colors shape (N, 4) or (B, N, 4)")
            c = ca.reshape(-1)
            c_ptr = &c[0]
        v = va.reshape(-1)
        _check(glv_render_list_fill_polygons(self.ptr, &v[0], c_ptr, va.shape[0], va.shape[1]))

    def symbol(self, int idx, anchor, offset, color=None, size=None, int overlay=0):
        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_symbol(
            self.ptr, idx,
            <float>anchor[0], <float>anchor[1], <float>anchor[2],
            <float>offset[0], <float>offset[1],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size, overlay))

    def character(self, ch, anchor, offset, color=None, size=None, int overlay=0):
        cdef int cp = ord(ch) if isinstance(ch, str) else int(ch)
        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_character(
            self.ptr, cp,
            <float>anchor[0], <float>anchor[1], <float>anchor[2],
            <float>offset[0], <float>offset[1],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size, overlay))

    def marker(self, mtype, anchor, offset, color=None, size=None, int overlay=0):
        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_marker(
            self.ptr, int(mtype),
            <float>anchor[0], <float>anchor[1], <float>anchor[2],
            <float>offset[0], <float>offset[1],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size, overlay))

    def text(self, str text, anchor, offset, color=None, size=None,
             align=TextAlign.Left, int overlay=0):
        color, size = _rl_color_size(self.ptr, color, size)
        _check(glv_render_list_text(
            self.ptr, text.encode("utf-8"),
            <float>anchor[0], <float>anchor[1], <float>anchor[2],
            <float>offset[0], <float>offset[1],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size, int(align), overlay))

    def triangles(self, vertices, indices, colors=None):
        cdef const float[::1] v, c
        cdef const int32_t[::1] idx
        cdef const float *c_ptr = NULL

        va = np.ascontiguousarray(vertices, dtype=np.float32)
        if va.ndim != 2 or va.shape[1] != 3:
            raise ValueError("triangles expects vertices shape (N, 3)")
        ia = np.ascontiguousarray(indices, dtype=np.int32)
        if ia.ndim != 2 or ia.shape[1] != 3:
            raise ValueError("triangles expects indices shape (T, 3)")
        if (ia < 0).any() or (ia >= va.shape[0]).any():
            raise IndexError("triangles index out of range")
        if colors is not None:
            ca = np.ascontiguousarray(colors, dtype=np.float32)
            if ca.ndim != 2 or ca.shape[0] != va.shape[0] or ca.shape[1] != 4:
                raise ValueError("triangles expects colors shape (N, 4)")
            c = ca.reshape(-1)
            c_ptr = &c[0]
        v = va.reshape(-1)
        idx = ia.reshape(-1)
        _check(glv_render_list_triangles(self.ptr, &v[0], c_ptr, va.shape[0], &idx[0], ia.shape[0]))

    def path_begin(self):
        cdef glv_path *handle = glv_render_list_path_begin(self.ptr)
        cdef Path obj
        if handle == NULL:
            raise RuntimeError(_error())
        obj = Path.__new__(Path)
        obj.ptr = handle
        return obj

    def mesh_begin(self):
        cdef glv_mesh *handle = glv_render_list_mesh_begin(self.ptr)
        cdef Mesh obj
        if handle == NULL:
            raise RuntimeError(_error())
        obj = Mesh.__new__(Mesh)
        obj.ptr = handle
        return obj

    def color(self, color):
        _check(glv_render_list_color(
            self.ptr, <float>color[0], <float>color[1],
            <float>color[2], <float>color[3]))

    def size(self, size):
        _check(glv_render_list_size(self.ptr, <float>size))

    def add_instance(self, pos=None, rot=None, scale=None):
        cdef float matrix[16]
        cdef Py_ssize_t row, col
        cdef const float[:, :] m

        if pos is not None and rot is None and scale is None and _rank(pos) == 2:
            m = np.asarray(pos, dtype=np.float32)
            if m.shape[0] != 4 or m.shape[1] != 4:
                raise ValueError("add_instance(transform) expects shape (4, 4)")
            for row in range(4):
                for col in range(4):
                    matrix[(row * 4) + col] = m[row, col]
            _check(glv_render_list_add_instance_matrix(self.ptr, matrix))
            return None

        if pos is None:
            pos = (0.0, 0.0, 0.0)
        if rot is None:
            rot = (0.0, 0.0, 0.0)
        if scale is None:
            scale = (1.0, 1.0, 1.0)
        if len(rot) == 4:
            _check(glv_render_list_add_instance_quat(
                self.ptr, <float>pos[0], <float>pos[1], <float>pos[2],
                <float>rot[0], <float>rot[1], <float>rot[2], <float>rot[3],
                <float>scale[0], <float>scale[1], <float>scale[2]))
        else:
            _check(glv_render_list_add_instance(
                self.ptr, <float>pos[0], <float>pos[1], <float>pos[2],
                <float>rot[0], <float>rot[1], <float>rot[2],
                <float>scale[0], <float>scale[1], <float>scale[2]))

    def save(self):
        _check(glv_render_list_save(self.ptr))

    def restore(self):
        _check(glv_render_list_restore(self.ptr))

    def clear(self):
        _check(glv_render_list_clear(self.ptr))

    def save_instances(self):
        _check(glv_render_list_save_instances(self.ptr))

    def restore_instances(self):
        _check(glv_render_list_restore_instances(self.ptr))

    def clear_instances(self):
        _check(glv_render_list_clear_instances(self.ptr))

    @property
    def enabled(self):
        cdef int value
        _check(glv_render_list_get_enabled(self.ptr, &value))
        return value != 0

    @enabled.setter
    def enabled(self, value):
        _check(glv_render_list_set_enabled(self.ptr, <int>bool(value)))


cdef class Path:
    cdef glv_path *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        raise TypeError("glviskit.Path cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_path_destroy(self.ptr)
            self.ptr = NULL

    def line_to(self, point, color=None, size=None):
        cdef const float[::1] p, c, z
        cdef const float *c_ptr = NULL
        cdef const float *z_ptr = NULL

        if _rank(point) == 2:
            pa = np.ascontiguousarray(point, dtype=np.float32)
            if pa.ndim != 2 or pa.shape[1] != 3:
                raise ValueError("line_to batch expects points shape (N, 3)")
            if color is not None:
                ca = np.ascontiguousarray(color, dtype=np.float32)
                if ca.shape[0] != pa.shape[0] or ca.shape[1] != 4:
                    raise ValueError("line_to batch expects colors shape (N, 4)")
                c = ca.reshape(-1)
                c_ptr = &c[0]
                if size is not None:
                    za = np.ascontiguousarray(size, dtype=np.float32)
                    if za.shape[0] != pa.shape[0]:
                        raise ValueError("line_to batch expects sizes shape (N,)")
                    z = za.reshape(-1)
                    z_ptr = &z[0]
            p = pa.reshape(-1)
            _check(glv_path_line_to_many(self.ptr, &p[0], c_ptr, z_ptr, pa.shape[0]))
            return None

        color, size = _path_color_size(self.ptr, color, size)
        _check(glv_path_line_to(
            self.ptr, <float>point[0], <float>point[1], <float>point[2],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            <float>size))

    def close(self):
        _check(glv_path_close(self.ptr))

    def line_end(self):
        _check(glv_path_line_end(self.ptr))

    def color(self, color):
        _check(glv_path_color(
            self.ptr, <float>color[0], <float>color[1],
            <float>color[2], <float>color[3]))

    def size(self, size):
        _check(glv_path_size(self.ptr, <float>size))


cdef class Mesh:
    cdef glv_mesh *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        raise TypeError("glviskit.Mesh cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_mesh_destroy(self.ptr)
            self.ptr = NULL

    def vertex(self, point, color=None):
        cdef size_t index
        cdef const float[::1] p, c
        cdef const float *c_ptr = NULL

        if _rank(point) == 2:
            pa = np.ascontiguousarray(point, dtype=np.float32)
            if pa.ndim != 2 or pa.shape[1] != 3:
                raise ValueError("vertex batch expects points shape (N, 3)")
            if color is not None:
                ca = np.ascontiguousarray(color, dtype=np.float32)
                if ca.shape[0] != pa.shape[0] or ca.shape[1] != 4:
                    raise ValueError("vertex batch expects colors shape (N, 4)")
                c = ca.reshape(-1)
                c_ptr = &c[0]
            p = pa.reshape(-1)
            indices = np.empty(pa.shape[0], dtype=np.uintp)
            _check(glv_mesh_vertices(self.ptr, &p[0], c_ptr,
                                     <size_t *><size_t>indices.ctypes.data, pa.shape[0]))
            return indices.tolist()

        color = _mesh_color(self.ptr, color)
        _check(glv_mesh_vertex(
            self.ptr, <float>point[0], <float>point[1], <float>point[2],
            <float>color[0], <float>color[1], <float>color[2], <float>color[3],
            &index))
        return int(index)

    def triangle(self, *args):
        cdef const int32_t[::1] idx
        cdef long i0, i1, i2

        if len(args) == 1:
            ia = np.ascontiguousarray(args[0], dtype=np.int32)
            if ia.ndim != 2 or ia.shape[1] != 3:
                raise ValueError("triangle batch expects indices shape (T, 3)")
            if (ia < 0).any():
                raise ValueError("triangle indices must be non-negative")
            idx = ia.reshape(-1)
            _check(glv_mesh_triangles(self.ptr, &idx[0], ia.shape[0]))
            return None
        if len(args) != 3:
            raise TypeError("triangle() expects indices or i0, i1, i2")
        i0 = int(args[0])
        i1 = int(args[1])
        i2 = int(args[2])
        if i0 < 0 or i1 < 0 or i2 < 0:
            raise ValueError("triangle indices must be non-negative")
        _check(glv_mesh_triangle(self.ptr, <size_t>i0, <size_t>i1, <size_t>i2))

    def color(self, color):
        _check(glv_mesh_color(
            self.ptr, <float>color[0], <float>color[1],
            <float>color[2], <float>color[3]))


cdef class BaseController:
    cdef glv_controller *ptr

    def __cinit__(self):
        self.ptr = NULL

    def __init__(self):
        if type(self) is BaseController:
            raise TypeError("glviskit.BaseController cannot be constructed directly")

    def __dealloc__(self):
        if self.ptr != NULL:
            glv_controller_destroy(self.ptr)
            self.ptr = NULL


cdef class NullController(BaseController):
    def __init__(self):
        if self.ptr == NULL:
            self.ptr = glv_create_null_controller()
            if self.ptr == NULL:
                raise RuntimeError(_error())


cdef class FirstPersonController(BaseController):
    def __init__(self):
        if self.ptr == NULL:
            self.ptr = glv_create_first_person_controller()
            if self.ptr == NULL:
                raise RuntimeError(_error())

    @property
    def key_move_sensitivity(self):
        cdef float value
        _check(glv_controller_get_key_move_sensitivity(self.ptr, &value))
        return value

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, value):
        _check(glv_controller_set_key_move_sensitivity(self.ptr, <float>value))

    @property
    def key_rot_sensitivity(self):
        cdef float value
        _check(glv_controller_get_key_rot_sensitivity(self.ptr, &value))
        return value

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, value):
        _check(glv_controller_set_key_rot_sensitivity(self.ptr, <float>value))

    @property
    def mouse_sensitivity(self):
        cdef float value
        _check(glv_controller_get_mouse_sensitivity(self.ptr, &value))
        return value

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, value):
        _check(glv_controller_set_mouse_sensitivity(self.ptr, <float>value))


cdef class SphericalController(BaseController):
    def __init__(self):
        if self.ptr == NULL:
            self.ptr = glv_create_spherical_controller()
            if self.ptr == NULL:
                raise RuntimeError(_error())

    @property
    def key_move_sensitivity(self):
        cdef float value
        _check(glv_controller_get_key_move_sensitivity(self.ptr, &value))
        return value

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, value):
        _check(glv_controller_set_key_move_sensitivity(self.ptr, <float>value))

    @property
    def key_rot_sensitivity(self):
        cdef float value
        _check(glv_controller_get_key_rot_sensitivity(self.ptr, &value))
        return value

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, value):
        _check(glv_controller_set_key_rot_sensitivity(self.ptr, <float>value))

    @property
    def mouse_sensitivity(self):
        cdef float value
        _check(glv_controller_get_mouse_sensitivity(self.ptr, &value))
        return value

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, value):
        _check(glv_controller_set_mouse_sensitivity(self.ptr, <float>value))

    @property
    def wheel_sensitivity(self):
        cdef float value
        _check(glv_controller_get_wheel_sensitivity(self.ptr, &value))
        return value

    @wheel_sensitivity.setter
    def wheel_sensitivity(self, value):
        _check(glv_controller_set_wheel_sensitivity(self.ptr, <float>value))


def create_window(title="glviskit Window", width=800, height=600):
    cdef bytes encoded
    cdef glv_window *handle
    cdef Window obj
    encoded = title.encode("utf-8") if isinstance(title, str) else bytes(title)
    handle = glv_create_window(encoded, <int>width, <int>height)
    if handle == NULL:
        raise RuntimeError(_error())
    obj = Window.__new__(Window)
    obj.ptr = handle
    return obj


def create_render_list():
    cdef glv_render_list *handle = glv_create_render_list()
    cdef RenderList obj
    if handle == NULL:
        raise RuntimeError(_error())
    obj = RenderList.__new__(RenderList)
    obj.ptr = handle
    return obj


def get_time_seconds():
    return glv_get_time_seconds()


def loop():
    cdef int running = glv_loop()
    if running == GLV_ERROR:
        raise RuntimeError(_error())
    return running != 0


def render():
    _check(glv_render())


def ui_new_frame():
    _check(glv_ui_new_frame())
