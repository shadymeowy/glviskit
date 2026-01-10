from typing import Any, Optional, overload, Typing, Sequence, Iterable, Union, Callable
from enum import Enum
import glviskit

class BaseController:
    """
    None
    """

    def __init__(*args, **kwargs):
        """
        Initialize self.  See help(type(self)) for accurate signature.
        """
        ...
    
class Camera:
    """
    None
    """

    def __init__(*args, **kwargs):
        """
        Initialize self.  See help(type(self)) for accurate signature.
        """
        ...
    
    def calculate_transform(self) -> numpy.typing.NDArray:
        """
        Calculate the camera transformation matrix
        """
        ...
    
    @property
    def distance(self) -> float:
        """
        Distance of the camera from its center
        """
        ...
    @distance.setter
    def distance(self, arg: float, /) -> None:
        """
        Distance of the camera from its center
        """
        ...
    
    def perspective(self, fxn: float, fyn: float, cx: float = 0.5, cy: float = 0.5, near: float = 0.10000000149011612, far: float = 100.0) -> None:
        """
        Set perspective projection using focal lengths and principal point
        """
        ...
    
    def perspective_fov(self, hfov: float, vfov: float, near: float = 0.10000000149011612, far: float = 100.0) -> None:
        """
        Set perspective projection using horizontal and vertical FOV
        """
        ...
    
    @property
    def position(self) -> list[float]:
        """
        Position of the camera
        """
        ...
    @position.setter
    def position(self, arg: collections.abc.Sequence[float], /) -> None:
        """
        Position of the camera
        """
        ...
    
    @property
    def preserve_aspect_ratio(self) -> bool:
        """
        Whether to preserve aspect ratio when resizing viewport
        """
        ...
    @preserve_aspect_ratio.setter
    def preserve_aspect_ratio(self, arg: bool, /) -> None:
        """
        Whether to preserve aspect ratio when resizing viewport
        """
        ...
    
    @property
    def rotation(self) -> list[float]:
        """
        Rotation of the camera (roll, pitch, yaw in radians)
        """
        ...
    @rotation.setter
    def rotation(self, arg: collections.abc.Sequence[float], /) -> None:
        """
        Rotation of the camera (roll, pitch, yaw in radians)
        """
        ...
    
    @property
    def viewport_size(self) -> list[float]:
        """
        Viewport size of the camera
        """
        ...
    
class FirstPersonController:
    """
    None
    """

    def __init__(self) -> None:
        """
        Create a FirstPersonController
        """
        ...
    
    @property
    def key_move_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of movement to key presses
        """
        ...
    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of movement to key presses
        """
        ...
    
    @property
    def key_rot_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to key presses
        """
        ...
    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to key presses
        """
        ...
    
    @property
    def mouse_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to mouse movement
        """
        ...
    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to mouse movement
        """
        ...
    
class NullController:
    """
    None
    """

    def __init__(self) -> None:
        """
        Create a NullController
        """
        ...
    
class Path:
    """
    None
    """

    def __init__(*args, **kwargs):
        """
        Initialize self.  See help(type(self)) for accurate signature.
        """
        ...
    
    def color(self, c: collections.abc.Sequence[float]) -> None:
        """
        Set the current drawing color
        """
        ...
    
    def line_end(self) -> None:
        """
        End the current line sequence
        """
        ...
    
    def line_to(self, p: collections.abc.Sequence[float]) -> None:
        """
        Draw a line to position p
        """
        ...
    
    @overload
    def line_to(self, points: numpy.typing.NDArray) -> None:
        """
        Call line_to for multiple points consecutively
        """
        ...
    
    @overload
    def line_to(self, points: numpy.typing.NDArray) -> None:
        """
        Call line_to for multiple points consecutively
        """
        ...
    
    def size(self, size: float) -> None:
        """
        Set the current drawing size
        """
        ...
    
class RenderList:
    """
    None
    """

    def __init__(*args, **kwargs):
        """
        Initialize self.  See help(type(self)) for accurate signature.
        """
        ...
    
    def add_instance(self, pos: collections.abc.Sequence[float] = [0.0, 0.0, 0.0], rot: collections.abc.Sequence[float] = [0.0, 0.0, 0.0], scale: collections.abc.Sequence[float] = [1.0, 1.0, 1.0]) -> None:
        """
        Add an instance with given position, rotation and scale
        """
        ...
    
    def circle(self, pos: collections.abc.Sequence[float]) -> None:
        """
        Draw an circle at position pos
        """
        ...
    
    @overload
    def circle(self, points: numpy.typing.NDArray) -> None:
        """
        Draw multiple circle at given positions
        """
        ...
    
    @overload
    def circle(self, points: numpy.typing.NDArray) -> None:
        """
        Draw multiple circle at given positions
        """
        ...
    
    def clear(self) -> None:
        """
        Clear the render buffer
        """
        ...
    
    def clear_instances(self) -> None:
        """
        Clear the instances
        """
        ...
    
    def color(self, c: collections.abc.Sequence[float]) -> None:
        """
        Set the current drawing color
        """
        ...
    
    @property
    def enabled(self) -> bool:
        """
        Whether this RenderList is enabled for rendering
        """
        ...
    @enabled.setter
    def enabled(self, arg: bool, /) -> None:
        """
        Whether this RenderList is enabled for rendering
        """
        ...
    
    def line(self, start: collections.abc.Sequence[float], end: collections.abc.Sequence[float]) -> None:
        """
        Draw a line from start to end
        """
        ...
    
    @overload
    def line(self, starts: numpy.typing.NDArray, ends: numpy.typing.NDArray) -> None:
        """
        Draw multiple lines from starts to ends
        """
        ...
    
    @overload
    def line(self, starts: numpy.typing.NDArray, ends: numpy.typing.NDArray) -> None:
        """
        Draw multiple lines from starts to ends
        """
        ...
    
    def path_begin(self) -> glviskit.Path:
        """
        Create a Path object for drawing complex paths which is save/restore aware
        """
        ...
    
    def point(self, p: collections.abc.Sequence[float]) -> None:
        """
        Draw a point at position p
        """
        ...
    
    @overload
    def point(self, points: numpy.typing.NDArray) -> None:
        """
        Draw multiple points at given positions
        """
        ...
    
    @overload
    def point(self, points: numpy.typing.NDArray) -> None:
        """
        Draw multiple points at given positions
        """
        ...
    
    def restore(self) -> None:
        """
        Restore the previously saved render buffer state
        """
        ...
    
    def restore_instances(self) -> None:
        """
        Restore the previously saved instances
        """
        ...
    
    def save(self) -> None:
        """
        Save the current render buffer state
        """
        ...
    
    def save_instances(self) -> None:
        """
        Save the current instances
        """
        ...
    
    def size(self, size: float) -> None:
        """
        Set the current drawing size
        """
        ...
    
class SphericalController:
    """
    None
    """

    def __init__(self) -> None:
        """
        Create a SphericalController
        """
        ...
    
    @property
    def key_move_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of distance change to key presses
        """
        ...
    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of distance change to key presses
        """
        ...
    
    @property
    def key_rot_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to key presses
        """
        ...
    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to key presses
        """
        ...
    
    @property
    def mouse_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to mouse movement
        """
        ...
    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of rotation to mouse movement
        """
        ...
    
    @property
    def wheel_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of distance change to mouse wheel
        """
        ...
    @wheel_sensitivity.setter
    def wheel_sensitivity(self, arg: float, /) -> None:
        """
        Sensitivity of distance change to mouse wheel
        """
        ...
    
class Window:
    """
    None
    """

    def __init__(*args, **kwargs):
        """
        Initialize self.  See help(type(self)) for accurate signature.
        """
        ...
    
    def add_render_list(self, rb: glviskit.RenderList) -> None:
        """
        Add a RenderList to the window for rendering
        """
        ...
    
    @property
    def camera(self) -> glviskit.Camera:
        """
        Camera of the window
        """
        ...
    @camera.setter
    def camera(self, arg: glviskit.Camera, /) -> None:
        """
        Camera of the window
        """
        ...
    
    @property
    def controller(self) -> glviskit.BaseController:
        """
        Camera controller of the window
        """
        ...
    @controller.setter
    def controller(self, arg: glviskit.BaseController, /) -> None:
        """
        Camera controller of the window
        """
        ...
    
    def make_current(self) -> None:
        """
        Make the window's OpenGL context current
        """
        ...
    
    def render(self) -> None:
        """
        Render the window's contents
        """
        ...
    
def create_render_list() -> glviskit.RenderList:
    """
    Create a new RenderList
    """
    ...

def create_window(title: str = 'glviskit Window', width: int = 800, height: int = 600) -> glviskit.Window:
    ...

def get_time_seconds() -> float:
    """
    Get the current time in seconds since the program started
    """
    ...

def loop() -> bool:
    """
    Run the event loop for single iteration and render all windows
    """
    ...

def render() -> None:
    """
    Render all windows without processing events
    """
    ...

