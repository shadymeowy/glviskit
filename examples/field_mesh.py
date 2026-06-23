import glviskit
import numpy as np

window1 = glviskit.create_window("Window1", 800, 600)

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)

# perspective camera tilted down
camera = window1.camera
camera.perspective_fov(60.0, 60.0, far=1000.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [-0.5, 0.0, 0.0]
camera.distance = 50.0
camera.set_axis_rotation(1, 2, 0)

window1.controller = glviskit.SphericalController()

# grid of vertices (nx*ny, 3); x along axis 0, y along axis 1
nx = 100
ny = 100
xs = np.linspace(-30.0, 30.0, nx, dtype=np.float32)
ys = np.linspace(-30.0, 30.0, ny, dtype=np.float32)
gx, gy = np.meshgrid(xs, ys, indexing="ij")
verts = np.zeros((nx * ny, 3), dtype=np.float32)
verts[:, 0] = gx.ravel()
verts[:, 1] = gy.ravel()

# two triangles per cell; vertex indices straight from a reshaped arange
idx = np.arange(nx * ny).reshape(nx, ny)
v00 = idx[:-1, :-1]
v10 = idx[1:, :-1]
v01 = idx[:-1, 1:]
v11 = idx[1:, 1:]
tris = np.concatenate(
    [
        np.stack([v00, v10, v11], axis=-1).reshape(-1, 3),
        np.stack([v00, v11, v01], axis=-1).reshape(-1, 3),
    ]
).astype(np.int32)

t0 = glviskit.get_time_seconds()
while glviskit.loop():
    t = glviskit.get_time_seconds() - t0

    x = verts[:, 0]
    y = verts[:, 1]
    # height = sin(0.1(x + y)) * (3 sin(0.5x + t) + 2 sin(0.5y + t)), colored by height
    verts[:, 2] = (
        np.sin(0.1 * (x + y))
        * (
            3 * np.sin(0.5 * x + t)
            + 2 * np.sin(0.5 * y + t)
        )
    )
    colors = np.empty((nx * ny, 4), dtype=np.float32)
    colors[:, 0] = (verts[:, 2] * 0.25) + 0.5
    colors[:, 1] = 0.5
    colors[:, 2] = 0.5 - (verts[:, 2] * 0.25)
    colors[:, 3] = 1.0

    # rebuild the surface mesh each frame
    render_list.clear()
    render_list.triangles(verts, tris, colors)
