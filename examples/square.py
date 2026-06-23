import glviskit
import numpy as np

window1 = glviskit.create_window("Window1", 800, 600)

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)

render_list_axes = glviskit.create_render_list()
window1.add_render_list(render_list_axes)

render_list_axes.size(5.0)
render_list_axes.color([1.0, 0.0, 0.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [1.0, 0.0, 0.0])
render_list_axes.color([0.0, 1.0, 0.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [0.0, 1.0, 0.0])
render_list_axes.color([0.0, 0.0, 1.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [0.0, 0.0, 1.0])

render_list.color([1.0, 1.0, 1.0, 1.0])
render_list.size(5.0)
# random points in a cube
render_list.circle(
    np.random.uniform(-1.0, 1.0, size=(100, 3)),
    np.random.uniform(0.0, 1.0, size=(100, 4)),
    np.random.uniform(5.0, 15.0, size=100),
)
# a big surface plane
points = np.array(
    [
        [-1.0, 0.0, -1.0],
        [1.0, 0.0, -1.0],
        [1.0, 0.0, 1.0],
        [-1.0, 0.0, 1.0],
    ]
)
points *= 10.0
points += np.array([0.0, -1.0, 0.0])
render_list.color([0.7, 0.7, 0.7, 1.0])
render_list.fill_polygon(points)
render_list.save()

camera = window1.camera
camera.perspective_fov(60.0, 60.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [0.0, 0.0, 0.0]
camera.preserve_aspect_ratio = True
camera.distance = 5.0

angle = 0.0
frame_index = 0

square = np.array(
    [
        [-1.0, -1.0, 0.0],
        [1.0, -1.0, 0.0],
        [1.0, 1.0, 0.0],
        [-1.0, 1.0, 0.0],
    ]
)
square2 = square + np.array([0.0, 0.0, 1.0])
colors = np.array(
    [
        [1.0, 0.0, 0.0, 1.0],
        [0.0, 1.0, 0.0, 1.0],
        [0.0, 0.0, 1.0, 1.0],
        [1.0, 1.0, 0.0, 1.0],
    ]
)


last_t = glviskit.get_time_seconds()
while glviskit.loop():
    current_t = glviskit.get_time_seconds()
    dt = current_t - last_t

    render_list.restore()
    render_list.color([0.0, 1.0, 0.0, 1.0])
    render_list.size(10.0)

    pts = np.stack([square, square2], axis=0)
    render_list.polygon(pts)
    render_list.circle(pts.reshape(-1, 3))
    render_list.color([1.0, 0.0, 0.0, 0.5])
    render_list.fill_polygon(pts)
