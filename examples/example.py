import glviskit
import numpy as np

window1 = glviskit.create_window("Window1", 800, 600)
window2 = glviskit.create_window("Window2", 800, 600)

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)
window2.add_render_list(render_list)
render_list.clear_instances()

for i in range(1, 5):
    s = 1 if i % 2 == 0 else -1
    render_list.add_instance([3.0 * (i - 0.5), 0, 0], [0.5 * s, 0, 0], [1.0, 1.0, 1.0])
    render_list.add_instance(
        [-3.0 * (i - 0.5), 0, 0], [-0.5 * s, 0, 0], [1.0, 1.0, 1.0]
    )

render_list_sine = glviskit.create_render_list()
window1.add_render_list(render_list_sine)
window2.add_render_list(render_list_sine)

render_list_axes = glviskit.create_render_list()
window1.add_render_list(render_list_axes)
window2.add_render_list(render_list_axes)

render_list_axes.size(5.0)
render_list_axes.color([1.0, 0.0, 0.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [1.0, 0.0, 0.0])
render_list_axes.color([0.0, 1.0, 0.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [0.0, 1.0, 0.0])
render_list_axes.color([0.0, 0.0, 1.0, 1.0])
render_list_axes.line([0.0, 0.0, 0.0], [0.0, 0.0, 1.0])

render_list.color([1.0, 1.0, 1.0, 1.0])
render_list.size(5.0)
render_list.circle(np.random.uniform(-1.0, 1.0, size=(10, 3)))

camera = window1.camera
camera.perspective_fov(60.0, 60.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [0.0, 0.0, 0.0]
camera.preserve_aspect_ratio = True
camera.distance = 15.0

camera2 = window2.camera
camera2.perspective_fov(60.0, 60.0)
camera2.position = [0.0, 0.0, 0.0]
camera2.rotation = [0.0, 0.0, 0.0]
camera2.preserve_aspect_ratio = True
camera2.distance = 15.0

angle = 0.0
frame_index = 0

while glviskit.loop():
    curr_time = glviskit.get_time_seconds()

    frame_index += 1
    angle += 0.005
    camera.rotation = [-0.5, angle, 0.0]
    camera2.rotation = [-0.5, -angle, 0.0]

    render_list.point(
        np.random.uniform(-1.0, 1.0, size=(10, 3)),
        np.random.uniform(0.0, 1.0, size=(10, 4)),
        np.random.uniform(1.0, 2.0, size=10),
    )

    if frame_index % 10 == 0:
        render_list.polyline(
            np.random.uniform(-1.0, 1.0, size=(10, 2, 3)),
            np.random.uniform(0.0, 1.0, size=(1, 2, 4)).repeat(10, axis=0),
            np.random.uniform(1.0, 4.0, size=(1, 2)).repeat(10, axis=0),
        )

    render_list_sine.restore()
    path = render_list_sine.path_begin()

    path.size(4.0)
    ix = np.linspace(-1.0, 1.0, 2000)
    points = np.zeros((2000, 3))
    points[:, 0] = ix * 20.0
    points[:, 1] = 1.5 * np.sin((50.0 * ix) + (10 * curr_time))
    points[:, 2] = 1.5 * np.cos((50.0 * ix) + (10 * curr_time))
    colors = np.zeros((2000, 4))
    colors[:, 0] = (points[:, 0] * 0.5) + 0.5
    colors[:, 1] = (points[:, 1] * 0.5) + 0.5
    colors[:, 2] = 0.5
    colors[:, 3] = 1.0
    path.line_to(points, colors)
