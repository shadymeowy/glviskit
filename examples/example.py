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
    render_list.add_instance(
        [3.0 * (i - 0.5), 0, 0], [0.5 * s, 0, 0], [1.0, 1.0, 1.0]
    )
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

    render_list.line

    for _ in range(10):
        render_list.size(np.random.uniform(1.0, 2.0))
        render_list.color(np.random.uniform(0.0, 1.0, size=4))
        render_list.point(np.random.uniform(-1.0, 1.0, size=3))

    if frame_index % 10 == 0:
        render_list.color(np.random.uniform(0.0, 1.0, size=4))
        render_list.size(np.random.uniform(0.0, 4.0))
        render_list.line(
            np.random.uniform(-1.0, 1.0, size=3),
            np.random.uniform(-1.0, 1.0, size=3),
        )

    render_list_sine.restore()
    render_list_sine.color([1.0, 0.0, 0.0, 1.0])
    render_list_sine.size(4.0)
    for ix in range(-1000, 1001):
        x = ix / 1000.0
        y = np.sin((50.0 * x) + (10 * curr_time))
        z = np.cos((50.0 * x) + (10 * curr_time))

        render_list_sine.color([(x * 0.5) + 0.5, (y * 0.5) + 0.5, 0.5, 1.0])
        render_list_sine.line_to([20.0 * x, 1.5 * y, 1.5 * z])
    render_list_sine.line_end()
