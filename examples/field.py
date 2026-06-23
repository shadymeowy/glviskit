import glviskit
import numpy as np

window1 = glviskit.create_window("Window1", 800, 600)

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)

render_list_axes = glviskit.create_render_list()
window1.add_render_list(render_list_axes)

camera = window1.camera
camera.perspective_fov(60.0, 60.0, far=1000.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [-0.5, 0.0, 0.0]
camera.distance = 50.0
camera.set_axis_rotation(1, 2, 0)

window1.controller = glviskit.SphericalController()


# in place height field and height based color
def heights(p, t):
    p[:, 2] = (
        np.sin(0.1 * (p[:, 0] + p[:, 1]))
        * (
            3 * np.sin(0.5 * p[:, 0] + t)
            + 2 * np.sin(0.5 * p[:, 1] + t)
        )
    )


def colorize(cols, p):
    cols[:, 0] = (p[:, 2] * 0.25) + 0.5
    cols[:, 1] = 0.5
    cols[:, 2] = 0.5 - (p[:, 2] * 0.25)
    cols[:, 3] = 1.0


xs = np.linspace(-30.0, 30.0, 25)
ys = np.linspace(-30.0, 30.0, 25)

from_p = []
to_p = []
# horizontal segments along every row
for i in range(len(xs) - 1):
    for j in range(len(ys)):
        from_p.append((xs[i], ys[j], 0.0))
        to_p.append((xs[i + 1], ys[j], 0.0))
# vertical segments along every column
for i in range(len(xs)):
    for j in range(len(ys) - 1):
        from_p.append((xs[i], ys[j], 0.0))
        to_p.append((xs[i], ys[j + 1], 0.0))

# grid node points, drawn as same-size circles to round off the joints
points = []
for i in range(len(xs)):
    for j in range(len(ys)):
        points.append((xs[i], ys[j], 0.0))

from_p = np.array(from_p, dtype=np.float32)
to_p = np.array(to_p, dtype=np.float32)
points = np.array(points, dtype=np.float32)

line_cols = np.empty((len(from_p), 4), dtype=np.float32)
node_cols = np.empty((len(points), 4), dtype=np.float32)

t0 = glviskit.get_time_seconds()
while glviskit.loop():
    t = glviskit.get_time_seconds() - t0

    heights(from_p, t)
    heights(to_p, t)
    heights(points, t)
    colorize(line_cols, from_p)
    colorize(node_cols, points)

    # redraw the field each frame
    render_list.clear()
    render_list.size(5.0)
    render_list.line(from_p, to_p, line_cols)
    render_list.circle(points, node_cols)
