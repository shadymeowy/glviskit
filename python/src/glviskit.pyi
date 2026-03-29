from collections.abc import Sequence
from typing import Annotated, overload

import numpy
from numpy.typing import NDArray

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
        image: Annotated[
            NDArray[numpy.uint8], dict(shape=(None, None, 4), order="C", device="cpu")
        ],
    ) -> None:
        """Capture the current window into a preallocated RGBA uint8 array"""

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
        """
        Set axis-convention transform from an axis permutation with optional axis inversions
        """

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
        self,
        starts: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        ends: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple lines from starts to ends"""

    @overload
    def line(
        self,
        starts: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        ends: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple lines from starts to ends"""

    @overload
    def line(self, start: Sequence[float], end: Sequence[float]) -> None:
        """Draw a line from starts to ends"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple points at given positions with given colors and sizes"""

    @overload
    def point(self, point: Sequence[float]) -> None:
        """Draw a point at given position"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw multiple circles at given positions with given colors and sizes"""

    @overload
    def circle(self, point: Sequence[float]) -> None:
        """Draw a circle at given position"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polygons with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a polygon with given vertices"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a polygon with given vertices"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices and colors"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polygon with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
        sizes: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None), device="cpu")
        ],
    ) -> None:
        """Draw multiple polylines with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a polyline with given vertices"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a polyline with given vertices"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices and colors"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def polyline(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """Draw a polyline with given vertices, colors and sizes"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float64], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 3), device="cpu")
        ],
        colors: Annotated[
            NDArray[numpy.float32], dict(shape=(None, None, 4), device="cpu")
        ],
    ) -> None:
        """Draw multiple filled polygons with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a filled polygon with given vertices"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
    ) -> None:
        """Draw a filled polygon with given vertices"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a filled polygon with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a filled polygon with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a filled polygon with given vertices and colors"""

    @overload
    def fill_polygon(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw a filled polygon with given vertices and colors"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices and indices"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices and indices"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices, indices and colors"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices, indices and colors"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices, indices and colors"""

    @overload
    def triangles(
        self,
        vertices: Annotated[
            NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")
        ],
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Draw multiple triangles with given vertices, indices and colors"""

    def path_begin(self) -> Path:
        """
        Create a Path object for drawing complex paths which is save/restore aware
        """

    def mesh_begin(self) -> Mesh:
        """
        Create a Mesh object for incrementally building triangle geometry which is save/restore aware
        """

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

    def size(self, size: float) -> None:
        """Set the current drawing size"""

    @overload
    def add_instance(
        self,
        transform: Annotated[NDArray[numpy.float32], dict(shape=(4, 4), device="cpu")],
    ) -> None:
        """Add an instance using a 4x4 transform matrix"""

    @overload
    def add_instance(
        self,
        transform: Annotated[NDArray[numpy.float64], dict(shape=(4, 4), device="cpu")],
    ) -> None:
        """Add an instance using a 4x4 transform matrix"""

    @overload
    def add_instance(
        self,
        pos: Sequence[float],
        rot: Sequence[float],
        scale: Sequence[float] = [1.0, 1.0, 1.0],
    ) -> None:
        """
        Add an instance with position, quaternion rotation (w, x, y, z), and scale
        """

    @overload
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

    @property
    def enabled(self) -> bool:
        """Whether this RenderList is enabled for rendering"""

    @enabled.setter
    def enabled(self, arg: bool, /) -> None: ...

class Path:
    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively with given colors"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively with given colors"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively with given colors"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> None:
        """Call line_to for multiple points consecutively with given colors"""

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float64], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
        sizes: Annotated[NDArray[numpy.float32], dict(shape=(None,), device="cpu")],
    ) -> None:
        """
        Call line_to for multiple points consecutively with given colors and sizes
        """

    @overload
    def line_to(self, point: Sequence[float]) -> None:
        """Call line_to for a single point"""

    def close(self) -> None:
        """Close the current line sequence"""

    def line_end(self) -> None:
        """End the current line sequence"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

    def size(self, size: float) -> None:
        """Set the current drawing size"""

class Mesh:
    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
    ) -> list[int]:
        """Add multiple vertices and return their mesh-local indices"""

    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
    ) -> list[int]:
        """Add multiple vertices and return their mesh-local indices"""

    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> list[int]:
        """
        Add multiple vertices with given colors and return their mesh-local indices
        """

    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float64], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> list[int]:
        """
        Add multiple vertices with given colors and return their mesh-local indices
        """

    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float64], dict(shape=(None, 4), device="cpu")],
    ) -> list[int]:
        """
        Add multiple vertices with given colors and return their mesh-local indices
        """

    @overload
    def vertex(
        self,
        points: Annotated[NDArray[numpy.float32], dict(shape=(None, 3), device="cpu")],
        colors: Annotated[NDArray[numpy.float32], dict(shape=(None, 4), device="cpu")],
    ) -> list[int]:
        """
        Add multiple vertices with given colors and return their mesh-local indices
        """

    @overload
    def vertex(self, point: Sequence[float]) -> int:
        """Add a vertex and return its mesh-local index"""

    @overload
    def triangle(
        self,
        indices: Annotated[NDArray[numpy.int32], dict(shape=(None, 3), device="cpu")],
    ) -> None:
        """Add multiple triangles using mesh-local vertex indices"""

    @overload
    def triangle(self, i0: int, i1: int, i2: int) -> None:
        """Add a triangle using mesh-local vertex indices"""

    def color(self, c: Sequence[float]) -> None:
        """Set the current drawing color"""

class BaseController:
    pass

class NullController(BaseController):
    def __init__(self) -> None:
        """Create a NullController"""

class FirstPersonController(BaseController):
    def __init__(self) -> None:
        """Create a FirstPersonController"""

    @property
    def key_move_sensitivity(self) -> float:
        """Sensitivity of movement to key presses"""

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None: ...
    @property
    def key_rot_sensitivity(self) -> float:
        """Sensitivity of rotation to key presses"""

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None: ...
    @property
    def mouse_sensitivity(self) -> float:
        """Sensitivity of rotation to mouse movement"""

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None: ...

class SphericalController(BaseController):
    def __init__(self) -> None:
        """Create a SphericalController"""

    @property
    def key_move_sensitivity(self) -> float:
        """Sensitivity of distance change to key presses"""

    @key_move_sensitivity.setter
    def key_move_sensitivity(self, arg: float, /) -> None: ...
    @property
    def key_rot_sensitivity(self) -> float:
        """Sensitivity of rotation to key presses"""

    @key_rot_sensitivity.setter
    def key_rot_sensitivity(self, arg: float, /) -> None: ...
    @property
    def mouse_sensitivity(self) -> float:
        """Sensitivity of rotation to mouse movement"""

    @mouse_sensitivity.setter
    def mouse_sensitivity(self, arg: float, /) -> None: ...
    @property
    def wheel_sensitivity(self) -> float:
        """Sensitivity of distance change to mouse wheel"""

    @wheel_sensitivity.setter
    def wheel_sensitivity(self, arg: float, /) -> None: ...
