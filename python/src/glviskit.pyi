from collections.abc import Sequence
from typing import Annotated, Any, overload

import numpy
from numpy.typing import NDArray

# batch drawing arrays accept float32 or float64
_Floats = numpy.floating[Any]
Points1 = Annotated[NDArray[_Floats], dict(shape=(None, 3))]
Colors1 = Annotated[NDArray[_Floats], dict(shape=(None, 4))]
Sizes1 = Annotated[NDArray[_Floats], dict(shape=(None,))]
Points2 = Annotated[NDArray[_Floats], dict(shape=(None, None, 3))]
Colors2 = Annotated[NDArray[_Floats], dict(shape=(None, None, 4))]
Sizes2 = Annotated[NDArray[_Floats], dict(shape=(None, None))]
Indices = Annotated[NDArray[numpy.int32], dict(shape=(None, 3))]
Matrix44 = Annotated[NDArray[_Floats], dict(shape=(4, 4))]

def create_window(
    title: str = "glviskit Window", width: int = 800, height: int = 600
) -> Window: ...
def create_render_list() -> RenderList:
    """Create a new RenderList"""

def get_time_seconds() -> float:
    """Get the current time in seconds since the program started"""

def loop() -> bool:
    """Run the event loop for single iteration and render all windows"""

def render() -> None:
    """Render all windows without processing events"""

def ui_new_frame() -> None:
    """Open a fresh UI frame for every window (gl_loop calls this for you)"""

class Window:
    def add_render_list(self, rb: RenderList) -> None:
        """Add a RenderList to the window for rendering"""

    @property
    def camera(self) -> Camera:
        """Camera of the window"""

    @camera.setter
    def camera(self, arg: Camera, /) -> None: ...
    @property
    def background_color(self) -> list[float]:
        """Background clear color of the window"""

    @background_color.setter
    def background_color(self, arg: Sequence[float], /) -> None: ...
    @property
    def controller(self) -> BaseController:
        """Camera controller of the window"""

    @controller.setter
    def controller(self, arg: BaseController, /) -> None: ...
    def make_current(self) -> None:
        """Make the window's OpenGL context current"""

    def capture_rgba(
        self,
        image: Annotated[NDArray[numpy.uint8], dict(shape=(None, None, 4), order="C")],
    ) -> None:
        """Capture the current window into a preallocated RGBA uint8 array"""

    def render(self) -> None:
        """Render the window's contents"""

    def ui_begin(self, title: str) -> bool:
        """Begin a UI panel; returns whether it is visible"""

    def ui_end(self) -> None:
        """End the current UI panel"""

    def ui_text(self, text: str) -> None:
        """Draw a line of text"""

    def ui_separator(self) -> None:
        """Draw a horizontal separator"""

    def ui_same_line(self) -> None:
        """Keep the next widget on the same line as the previous one"""

    def ui_button(self, label: str) -> bool:
        """Draw a button; returns True if clicked this frame"""

    def ui_checkbox(self, label: str, value: bool) -> tuple[bool, bool]:
        """Draw a checkbox; returns (changed, new_value)"""

    def ui_slider_float(
        self, label: str, value: float, vmin: float, vmax: float
    ) -> tuple[bool, float]:
        """Draw a float slider; returns (changed, new_value)"""

    def ui_slider_float3(
        self, label: str, value: Sequence[float], vmin: float, vmax: float
    ) -> tuple[bool, tuple[float, float, float]]:
        """Draw a 3-component float slider; returns (changed, new_value)"""

    def ui_slider_int(
        self, label: str, value: int, vmin: int, vmax: int
    ) -> tuple[bool, int]:
        """Draw an int slider; returns (changed, new_value)"""

    def ui_combo(
        self, label: str, current: int, items: str
    ) -> tuple[bool, int]:
        """Draw a combo from "a|b|c" options; returns (changed, new_index)"""

    def ui_drag_float(
        self,
        label: str,
        value: float,
        speed: float = 1.0,
        vmin: float = 0.0,
        vmax: float = 0.0,
    ) -> tuple[bool, float]:
        """Draw a draggable float; returns (changed, new_value)"""

    def ui_color_edit3(
        self, label: str, color: Sequence[float]
    ) -> tuple[bool, tuple[float, float, float]]:
        """Edit an RGB color; returns (changed, new_color)"""

    def ui_color_edit4(
        self, label: str, color: Sequence[float]
    ) -> tuple[bool, tuple[float, float, float, float]]:
        """Edit an RGBA color; returns (changed, new_color)"""

    def ui_plot_lines(self, label: str, values: Sequence[float]) -> None:
        """Plot a line graph of the given values"""

    def ui_want_capture_mouse(self) -> bool:
        """Whether the UI is currently capturing the mouse"""

    def ui_want_capture_keyboard(self) -> bool:
        """Whether the UI is currently capturing the keyboard"""

class Camera:
    def calculate_transform(
        self,
    ) -> Annotated[NDArray[numpy.float32], dict(shape=(4, 4), order="C")]:
        """Calculate the camera transformation matrix"""

    def perspective_fov(
        self, hfov: float, vfov: float, near: float = 0.1, far: float = 100.0
    ) -> None:
        """Set perspective projection using horizontal and vertical FOV"""

    def perspective(
        self,
        fxn: float,
        fyn: float,
        cx: float = 0.5,
        cy: float = 0.5,
        near: float = 0.1,
        far: float = 100.0,
    ) -> None:
        """Set perspective projection using focal lengths and principal point"""

    @property
    def position(self) -> list[float]:
        """Position of the camera"""

    @position.setter
    def position(self, arg: Sequence[float], /) -> None: ...
    @property
    def rotation(self) -> list[float]:
        """Rotation of the camera (roll, pitch, yaw in radians)"""

    @rotation.setter
    def rotation(self, arg: Sequence[float], /) -> None: ...
    @property
    def distance(self) -> float:
        """Distance of the camera from its center"""

    @distance.setter
    def distance(self, arg: float, /) -> None: ...
    @property
    def axis_rotation(self) -> list[float]:
        """Axis-convention quaternion rotation (w, x, y, z)"""

    @axis_rotation.setter
    def axis_rotation(self, arg: Sequence[float], /) -> None: ...
    def set_axis_rotation(
        self,
        x: int,
        y: int,
        z: int,
        inv_x: bool = False,
        inv_y: bool = False,
        inv_z: bool = False,
    ) -> None:
        """Set axis-convention transform from an axis permutation with optional axis inversions"""

    @property
    def viewport_size(self) -> list[float]:
        """Viewport size of the camera"""

    @property
    def preserve_aspect_ratio(self) -> bool:
        """Whether to preserve aspect ratio when resizing viewport"""

    @preserve_aspect_ratio.setter
    def preserve_aspect_ratio(self, arg: bool, /) -> None: ...

class RenderList:
    @overload
    def line(
        self, starts: Points1, ends: Points1, colors: Colors1 = ..., sizes: Sizes1 = ...
    ) -> None:
        """Draw one line segment per row of starts/ends, with optional colors and sizes"""

    @overload
    def line(
        self,
        start: Sequence[float],
        end: Sequence[float],
        color: Sequence[float] = ...,
        size: float = ...,
    ) -> None:
        """Draw a line segment, optionally with color and size"""

    @overload
    def point(
        self, points: Points1, colors: Colors1 = ..., sizes: Sizes1 = ...
    ) -> None:
        """Draw one point per row, with optional colors and sizes"""

    @overload
    def point(
        self, point: Sequence[float], color: Sequence[float] = ..., size: float = ...
    ) -> None:
        """Draw a point, optionally with color and size"""

    @overload
    def circle(
        self, points: Points1, colors: Colors1 = ..., sizes: Sizes1 = ...
    ) -> None:
        """Draw one circle per row, with optional colors and sizes"""

    @overload
    def circle(
        self, point: Sequence[float], color: Sequence[float] = ..., size: float = ...
    ) -> None:
        """Draw a circle, optionally with color and size"""

    def polygon(
        self,
        vertices: Points1 | Points2,
        colors: Colors1 | Colors2 = ...,
        sizes: Sizes1 | Sizes2 = ...,
    ) -> None:
        """Draw a closed polygon (N, 3) or a batch of polygons (B, N, 3)"""

    def polyline(
        self,
        vertices: Points1 | Points2,
        colors: Colors1 | Colors2 = ...,
        sizes: Sizes1 | Sizes2 = ...,
    ) -> None:
        """Draw an open polyline (N, 3) or a batch of polylines (B, N, 3)"""

    def fill_polygon(
        self, vertices: Points1 | Points2, colors: Colors1 | Colors2 = ...
    ) -> None:
        """Fill a polygon (N, 3) or a batch of polygons (B, N, 3) via fan triangulation"""

    def triangles(
        self, vertices: Points1, indices: Indices, colors: Colors1 = ...
    ) -> None:
        """Draw an indexed triangle mesh, optionally with per-vertex colors"""

    def path_begin(self) -> Path:
        """Begin a new path"""

    def mesh_begin(self) -> Mesh:
        """Begin a new mesh"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

    def size(self, size: float) -> None:
        """Set the current drawing size"""

    @overload
    def add_instance(self, transform: Matrix44) -> None:
        """Add an instance using a 4x4 row-major transform matrix"""

    @overload
    def add_instance(
        self,
        pos: Sequence[float] = ...,
        rot: Sequence[float] = ...,
        scale: Sequence[float] = ...,
    ) -> None:
        """Add an instance with position, rotation (euler or quaternion w, x, y, z), and scale"""

    def save(self) -> None:
        """Save the current drawing state"""

    def restore(self) -> None:
        """Restore the last saved drawing state"""

    def clear(self) -> None:
        """Clear all drawn geometry"""

    def save_instances(self) -> None:
        """Save the current instance list"""

    def restore_instances(self) -> None:
        """Restore the last saved instance list"""

    def clear_instances(self) -> None:
        """Clear all instances"""

    @property
    def enabled(self) -> bool:
        """Whether the render list is drawn"""

    @enabled.setter
    def enabled(self, arg: bool, /) -> None: ...

class Path:
    @overload
    def line_to(
        self, points: Points1, colors: Colors1 = ..., sizes: Sizes1 = ...
    ) -> None:
        """Append one segment per row, with optional colors and sizes"""

    @overload
    def line_to(
        self, point: Sequence[float], color: Sequence[float] = ..., size: float = ...
    ) -> None:
        """Append a segment to the path, optionally with color and size"""

    def close(self) -> None:
        """Close the current contour"""

    def line_end(self) -> None:
        """End the current contour without closing it"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

    def size(self, size: float) -> None:
        """Set the current drawing size"""

class Mesh:
    @overload
    def vertex(self, points: Points1, colors: Colors1 = ...) -> list[int]:
        """Add one vertex per row and return their mesh-local indices"""

    @overload
    def vertex(self, point: Sequence[float], color: Sequence[float] = ...) -> int:
        """Add a vertex and return its mesh-local index"""

    @overload
    def triangle(self, indices: Indices) -> None:
        """Add one triangle per row using mesh-local vertex indices"""

    @overload
    def triangle(self, i0: int, i1: int, i2: int) -> None:
        """Add a triangle using mesh-local vertex indices"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

class BaseController: ...

class NullController(BaseController):
    def __init__(self) -> None:
        """Create a controller that ignores all input"""

class FirstPersonController(BaseController):
    def __init__(self) -> None:
        """Create a first-person camera controller"""

    @property
    def key_move_sensitivity(self) -> float:
        """Keyboard movement sensitivity"""

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None: ...
    @property
    def key_rot_sensitivity(self) -> float:
        """Keyboard rotation sensitivity"""

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None: ...
    @property
    def mouse_sensitivity(self) -> float:
        """Mouse look sensitivity"""

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None: ...

class SphericalController(BaseController):
    def __init__(self) -> None:
        """Create an orbit (spherical) camera controller"""

    @property
    def key_move_sensitivity(self) -> float:
        """Keyboard movement sensitivity"""

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None: ...
    @property
    def key_rot_sensitivity(self) -> float:
        """Keyboard rotation sensitivity"""

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None: ...
    @property
    def mouse_sensitivity(self) -> float:
        """Mouse look sensitivity"""

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None: ...
    @property
    def wheel_sensitivity(self) -> float:
        """Mouse wheel zoom sensitivity"""

    @wheel_sensitivity.setter
    def wheel_sensitivity(self, arg: float, /) -> None: ...
