# glviskit

`glviskit` is a retained-mode 3D drawing toolkit for C++ and Python, built for practical real-time geometry visualization with OpenGL and SDL3.

It provides:

- a compact C++ API for windows, cameras, and drawing primitives
- Python bindings built with Cython on top of the C API
- examples for native and WebAssembly targets

The API is organized around a few core concepts:

- `Window`: owns an OpenGL context and renders one or more render lists
- `RenderList`: stores points, lines, circles, triangle meshes, paths, and drawing state such as color and size
- `Camera`: controls projection and view transforms
- controllers: optional camera input handlers such as first-person and spherical controls

## Why `glviskit`?

`glviskit` exists to remove the usual OpenGL setup cost when all you want to do is visualize geometry.

In typical OpenGL code, drawing a few 3D lines, points, paths, or circles often means setting up windows, contexts, shaders, buffers, and camera math before you can inspect any data. `glviskit` is meant to cut through that for practical visualization workloads such as point clouds, trajectories, debug geometry, simulation output, and similar datasets where clarity matters more than scene-graph features.

The API is intentionally close to a plotting-style or immediate-mode drawing workflow:

- create a window
- create a render list
- push lines, points, circles, or paths into it
- move the camera
- render

The renderer is retained rather than immediate: geometry stays in GPU buffers and is only re-uploaded when it changes. That keeps the API simple without giving up the performance benefits of a retained renderer.

A few details that matter for visualization workloads:

- line and point thickness are controlled explicitly rather than by distance to the camera
- multiple windows can share the same `RenderList`
- instancing is built in
- paths are efficient enough for dynamic line strips and trajectories
- triangle meshes can be appended directly from vertex and index arrays
- render lists can save and restore drawing state and stored geometry without rebuilding buffers
- paths keep their own state and geometry over time instead of being limited to a single frame
- primitives can be interleaved without breaking rendering order because shared buffers are stitched together through index-buffer layout

Because geometry remains on the GPU until you change it, the library stays practical for real-time visualization even with large amounts of geometry, including cases where scene updates are driven from Python.

## Repository layout

- `include/glviskit`: public C++ headers
- `python`: Python extension module and stub file
- `examples`: C++ and Python examples
- `cmake`: dependency and backend setup

## Build requirements

For a native build you will need:

- a C++20 compiler
- CMake 3.15 or newer
- Python 3.9 or newer if you want the Python package

The CMake build fetches `SDL3` and `glm` automatically. Python builds use `scikit-build-core` and Cython.

WebAssembly builds with Emscripten are also supported. For that target, `glm` is still fetched by CMake and SDL3 comes from the Emscripten toolchain. See `examples/demo_wasm` for a working example.

## Quick start

If you only want to try the Python package:

```bash
pip install glviskit
```

## Build the C++ examples

Configure and build the library:

```bash
cmake -S . -B build
cmake --build build
```

For building examples, invoke `cmake` similarly in the respective directory.

By default, the build uses bundled `glad` for OpenGL function loading. Native OpenGL and native OpenGL ES backends are also supported where available.

OpenGL backend selection is controlled by `GLVISKIT_GL_TYPE`:

- `AUTO` (default)
- `GLAD_GL`
- `GLAD_GLES2`
- `NATIVE_GL`
- `NATIVE_GLES2`

Example:

```bash
cmake -S . -B build -DGLVISKIT_GL_TYPE=GLAD_GL
cmake --build build
```

The Python package targets Python 3.8+ on Linux, macOS, and Windows.

## Build the Python package

From the repository root:

```bash
python -m pip install .
```

For editable development:

```bash
python -m pip install -e .
```

That builds the extension with scikit-build and installs the `glviskit` module.

## Minimal C++ example

```cpp
#include <glviskit/glviskit.hpp>

int main() {
    auto window = glviskit::CreateWindow("glviskit", 800, 600);
    auto render_list = glviskit::CreateRenderList();
    window->AddRenderList(render_list);

    render_list->Color({1.0F, 0.0F, 0.0F, 1.0F});
    render_list->Size(4.0F);
    render_list->Line({0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F});

    auto camera = window->GetCamera();
    camera->PerspectiveFov(60.0F, 60.0F);
    camera->SetDistance(5.0F);

    while (glviskit::Loop()) {
    }

    return 0;
}
```

That is the full loop: create a window, create a render list, add geometry, configure the camera, and keep calling `glviskit::Loop()`.

## Minimal Python example

```python
import glviskit

window = glviskit.create_window("glviskit", 800, 600)
render_list = glviskit.create_render_list()
window.add_render_list(render_list)

render_list.color([1.0, 0.0, 0.0, 1.0])
render_list.size(4.0)
render_list.line([0.0, 0.0, 0.0], [1.0, 0.0, 0.0])

camera = window.camera
camera.perspective_fov(60.0, 60.0)
camera.distance = 5.0

while glviskit.loop():
    pass
```

The Python API follows the same retained drawing model, while adding NumPy-friendly batched overloads and a few convenience helpers where they make Python usage noticeably better.

## Drawing model

`RenderList` is a retained drawing buffer. Calls such as `color()` and `size()` affect subsequent draw commands, and submitted geometry stays on the GPU until you change it, clear it, or restore past it.

At the core, the API is built around:

- points
- lines
- circles
- indexed triangles
- persistent `Path` builders for line geometry
- persistent `Mesh` builders for triangle geometry

`RenderList` also supports:

- instancing with `add_instance(...)`
- state save and restore with `save()` and `restore()`
- instance stack save and restore with `save_instances()` and `restore_instances()`

A `triangles(vertices, indices)` call appends indexed triangle geometry using the current drawing color, and a `mesh_begin()` object lets you build the same kind of geometry incrementally with mesh-local vertex indices.

This makes it practical to build a stable base scene, save the current state, append temporary or dynamic geometry, and restore back to the saved state later without rebuilding what you kept. In normal use, changing camera parameters, restoring saved render-list state, or restoring saved instance state does not require re-uploading unchanged geometry.

Paths and meshes are also retained objects rather than frame-local helpers. A `Path` can be extended across frames, closed later, and restored back to an earlier state. A `Mesh` can keep its local vertex mapping and triangle data across frames and continue growing later.

## Examples

Useful starting points:

- `examples/demo/main.cpp`
- `examples/example.py`
- `examples/demo_wasm/main.cpp`

## Current state

This project is small and focused. The public API is already usable, but it is still a lightweight visualization library rather than a full engine or plotting framework.

If you are looking for:

- simple 3D geometry visualization
- a small dependency surface at the API level
- both C++ and Python entry points

then this is probably a good fit.

## Roadmap

Planned next steps:

- [x] triangle rendering as a first-class primitive
- [x] solid geometry features built on top of triangle primitives
- [ ] MSDF text rendering for 3D billboard text with distance-invariant sizing
- [ ] a richer and more flexible API for the existing primitives

## API Reference

The intended C++ entry point is:

```cpp
#include <glviskit/glviskit.hpp>
```

In normal use, the library is driven through the top-level helper functions:

- `glviskit::CreateWindow(...)`
- `glviskit::CreateRenderList()`
- `glviskit::GetTimeSeconds()`
- `glviskit::Loop()`
- `glviskit::Render()`

Direct use of the SDL manager singleton is generally not part of normal application code. Direct construction of `Window`, `RenderList`, and `Camera` is also not the intended public workflow, even though those types are visible in the headers.

### Typical flow

A normal application looks like this:

1. create one or more windows with `CreateWindow()`
2. create one or more render lists with `CreateRenderList()`
3. attach render lists to windows
4. configure the window camera
5. populate render lists with geometry
6. run `Loop()` every frame

### Window and camera

A `Window` owns an OpenGL context and renders one or more `RenderList` objects.

Typical window operations include:

- attaching render lists with `AddRenderList(...)`
- accessing or replacing the current camera with `GetCamera()` and `SetCamera(...)`
- accessing or replacing the current controller with `GetController()` and `SetController(...)`
- rendering manually with `Render()` if you are not using `Loop()`

Each window starts with a `SphericalController` by default.

The camera controls both view and projection:

- `PerspectiveFov(...)` sets projection using horizontal and vertical field of view in degrees
- `Perspective(...)` sets projection using focal-length-style parameters
- `SetPosition(...)` and `SetRotation(...)` control the camera center
- `SetDistance(...)` adds spherical-camera-style distance from that center
- `SetPreserveAspectRatio(...)` controls aspect-ratio handling during resize
- `CalculateTransform()` returns the final transform used for rendering

The camera also supports an axis-convention rotation. This is useful when world data uses another axis convention such as Y-up, Z-up, ENU, or NED. The axis-convention transform is applied in world space before the camera’s own Euler rotation, so it remaps the world axes without changing the meaning of the camera’s roll, pitch, and yaw controls.

In normal use, viewport size is managed by the window renderer.

### Render lists

`RenderList` is the main retained drawing container. It stores geometry and drawing state rather than acting like a true immediate-mode API.

Primitive drawing:

- `Line(start, end)`
- `Point(position)`
- `Circle(position)`
- `Triangles(vertices, indices)`
- `PathBegin()`
- `MeshBegin()`

Drawing state:

- `Color(rgba)`
- `Size(value)`

Instancing:

- `AddInstance(transform)`
- `AddInstance(position, rotation, scale)`

For instancing, rotation can be supplied either as the existing axis-angle `vec3` form or as a quaternion `wxyz`.

State and geometry control:

- `Save()` and `Restore()`
- `Clear()`
- `SaveInstances()` and `RestoreInstances()`
- `ClearInstances()`
- `SetEnabled(...)` and `IsEnabled()`

Triangle geometry uses the same retained model as the rest of the API through `Triangles(vertices, indices)` and `Triangles(vertices, indices, colors)`. `Mesh` is obtained through the `MeshBegin()` builder, which lets you construct the same kind of geometry incrementally with mesh-local vertex indices.

`Save()` and `Restore()` preserve both drawing state and stored geometry. That makes it practical to keep a stable base scene, append temporary geometry, and then restore back to the saved state without rebuilding what you kept.

### Python API

The Python bindings keep the same retained model and the same main objects:

- `Window`
- `Camera`
- `RenderList`
- `Path`
- `Mesh`

Compared with C++, the Python API adds NumPy-friendly batching and a few helper entry points so common workflows do not turn into Python loops.

Python also exposes the same retained instancing model through `add_instance(...)`, including both full `4 x 4` transform matrices and position/rotation/scale forms.

Core Python geometry calls accept arrays directly:

- `point(points)`
- `point(points, colors)`
- `point(points, colors, sizes)`
- `circle(points)`
- `circle(points, colors)`
- `circle(points, colors, sizes)`
- `triangles(vertices, indices)`
- `triangles(vertices, indices, colors)`

For line-strip-style geometry, Python also exposes higher-level helpers built on top of `Path`:

- `polyline(vertices)`
- `polyline(vertices, colors)`
- `polyline(vertices, colors, sizes)`
- `polygon(vertices)`
- `polygon(vertices, colors)`
- `polygon(vertices, colors, sizes)`

These accept either a single `M x 3` vertex array or a batched `N x M x 3` array.

For filled polygons, Python provides:

- `fill_polygon(vertices)`
- `fill_polygon(vertices, colors)`

These also accept either a single `M x 3` polygon or a batched `N x M x 3` array.

`polyline(vertices)` creates an open connected line strip, `polygon(vertices)` creates a closed outline, and `fill_polygon(vertices)` fills a polygon using a triangle fan, so it is best suited to convex polygons or vertex orders that already match fan triangulation.

For lower-level incremental building, Python exposes the same builder objects as C++:

- `path_begin()` with `line_to(...)`, `line_end()`, and `close()`
- `mesh_begin()` with `vertex(...)` and `triangle(...)`

`Path.line_to(...)` accepts batched point arrays, optional matching color arrays, and optional size arrays. `Mesh.vertex(...)` accepts batched point arrays and optional matching color arrays. `Mesh.triangle(...)` accepts either an `N x 3` index array or a single `(i0, i1, i2)` triangle.

The C++ API stays smaller and more explicit, while the Python API adds batching and convenience where it actually avoids Python-side loops. The package also ships generated type stubs and docstrings.

### Paths

`Path` is used for connected line strips bound to a render list.

Typical usage:

- start with `PathBegin()`
- append points with `LineTo(...)`
- finish the strip with `LineEnd()`
- optionally change path-local drawing state with `Color(...)` and `Size(...)`

The first `LineTo()` stores the starting point. Each later `LineTo()` appends a connected segment.

Paths are not frame-local helpers. A path is bound to a `RenderList`, but it keeps its own drawing state and accumulated geometry. That means a path can outlive a single render cycle: you can keep a path object around, append another segment in a later frame, and continue building the same polyline over time.

Path state is also separate from the parent render-list state. Path-local `Color(...)` and `Size(...)` settings are preserved independently, and they participate in `RenderList::Save()` and `RenderList::Restore()` together with the rest of the retained geometry.

Internally, line primitives and path segments share the same line vertex and index buffers. Interleaving different primitives does not break connectivity or ordering, because the geometry is stitched together through the index-buffer structure rather than by assuming a single contiguous submission pattern.

### Mesh builders

`Mesh` is a retained triangle builder bound to a render list.

Typical usage:

- start with `MeshBegin()`
- append vertices with `Vertex(...)`
- add triangles with `Triangle(i0, i1, i2)`
- optionally change mesh-local color with `Color(...)`

Like `Path`, a `Mesh` object is not limited to a single frame. It can outlive a render cycle, keep its own local state, and continue receiving geometry later.

Each mesh keeps its own local vertex-index mapping. `Vertex(...)` returns indices relative to that mesh object, and `Triangle(...)` resolves them internally to the shared retained mesh buffers. That local mapping participates in `RenderList::Save()` and `RenderList::Restore()`, so incremental mesh construction works correctly across saved and restored states.

### Controllers

Controllers handle camera motion from input.

Built-in controllers:

- `NullController`: disables interactive motion
- `FirstPersonController`: translation plus look rotation
- `SphericalController`: orbit rotation plus distance control

All controllers derive from `BaseController`, which defines update and input-event hooks. In typical usage you do not call those hooks directly. The window and event loop drive them for you.

The built-in controllers expose sensitivity controls for keyboard, mouse, and wheel behavior through getters and setters.
