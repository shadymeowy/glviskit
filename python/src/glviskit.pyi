from collections.abc import Sequence
from typing import Annotated, overload

import numpy
from numpy.typing import NDArray

def create_window(
    width: str = 800, height: int = 600, title: int = "glviskit Window"
) -> Window: ...
def create_render_buffer() -> RenderBuffer:
    """Create a new RenderBuffer"""

def get_time_seconds() -> float:
    """Get the current time in seconds since the program started"""

def loop() -> bool:
    """Run the event loop for single iteration and render all windows"""

def render() -> None:
    """Render all windows without processing events"""

class Window:
    def add_render_buffer(self, rb: RenderBuffer) -> None:
        """Add a RenderBuffer to the window for rendering"""

    @property
    def camera(self) -> Camera:
        """Camera of the window"""

    @camera.setter
    def camera(self, arg: Camera, /) -> None: ...
    def make_current(self) -> None:
        """Make the window's OpenGL context current"""

    def render(self) -> None:
        """Render the window's contents"""

class Camera:
    def calculate_transform(
        self,
    ) -> Annotated[NDArray[numpy.float32], dict(shape=(4, 4), order="C")]:
        """Calculate the camera transformation matrix"""

    def perspective_fov(
        self,
        hfov: float,
        vfov: float,
        near: float = 0.10000000149011612,
        far: float = 100.0,
    ) -> None:
        """Set perspective projection using horizontal and vertical FOV"""

    def perspective(
        self,
        fxn: float,
        fyn: float,
        cx: float = 0.5,
        cy: float = 0.5,
        near: float = 0.10000000149011612,
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
    def viewport_size(self) -> list[float]:
        """Viewport size of the camera"""

    @property
    def preserve_aspect_ratio(self) -> bool:
        """Whether to preserve aspect ratio when resizing viewport"""

    @preserve_aspect_ratio.setter
    def preserve_aspect_ratio(self, arg: bool, /) -> None: ...

class RenderBuffer:
    @overload
    def line(self, start: Sequence[float], end: Sequence[float]) -> None:
        """Draw a line from start to end"""

    @overload
    def line(
        self,
        starts: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), order="C", device="cpu")
        ],
        ends: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), order="C", device="cpu")
        ],
    ) -> None:
        """Draw multiple lines from starts to ends"""

    @overload
    def point(self, p: Sequence[float]) -> None:
        """Draw a point at position p"""

    @overload
    def point(
        self,
        points: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), order="C", device="cpu")
        ],
    ) -> None:
        """Draw multiple points at given positions"""

    @overload
    def line_to(self, p: Sequence[float]) -> None:
        """Draw a line to position p"""

    @overload
    def line_to(
        self,
        points: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), order="C", device="cpu")
        ],
    ) -> None:
        """Call line_to for multiple points consecutively"""

    def line_end(self) -> None:
        """End the current line sequence"""

    @overload
    def circle(self, pos: Sequence[float]) -> None:
        """Draw an circle at position pos"""

    @overload
    def circle(
        self,
        points: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), order="C", device="cpu")
        ],
    ) -> None:
        """Draw multiple circle at given positions"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

    def size(self, size: float) -> None:
        """Set the current drawing size"""

    def add_instance(
        self,
        pos: Sequence[float] = [0.0, 0.0, 0.0],
        rot: Sequence[float] = [0.0, 0.0, 0.0],
        scale: Sequence[float] = [1.0, 1.0, 1.0],
    ) -> None:
        """Add an instance with given position, rotation and scale"""

    def save(self) -> None:
        """Save the current render buffer state"""

    def restore(self) -> None:
        """Restore the previously saved render buffer state"""

    def clear(self) -> None:
        """Clear the render buffer"""

    def save_instances(self) -> None:
        """Save the current instances"""

    def restore_instances(self) -> None:
        """Restore the previously saved instances"""

    def clear_instances(self) -> None:
        """Clear the instances"""
