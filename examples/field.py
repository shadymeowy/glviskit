import glviskit
import numpy as np

window1 = glviskit.create_window("Window1", 800, 600)

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)

render_list_axes = glviskit.create_render_list()
window1.add_render_list(render_list_axes)

camera = window1.camera
camera.perspective_fov(60.0, 60.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [-0.5, 0.0, 0.0]
camera.distance = 50.0

window1.controller = glviskit.SphericalController()

xs = np.linspace(-30.0, 30.0, 100)
ys = np.linspace(-30.0, 30.0, 100)

from_p = []
to_p = []
for i in range(len(xs) - 1):
    for j in range(len(ys) - 1):
        from_p.append((xs[i], ys[j], 0.0))
        to_p.append((xs[i + 1], ys[j], 0.0))
        from_p.append((xs[i], ys[j], 0.0))
        to_p.append((xs[i], ys[j + 1], 0.0))


from_p = np.array(from_p, dtype=np.float32)
to_p = np.array(to_p, dtype=np.float32)


t0 = glviskit.get_time_seconds()
while glviskit.loop():
    t = glviskit.get_time_seconds() - t0

    render_list.clear()
    render_list.size(5.0)
    from_p[:, 2] = np.sin(from_p[:, 0] + t) + np.sin(from_p[:, 1] + t)
    to_p[:, 2] = np.sin(to_p[:, 0] + t) + np.sin(to_p[:, 1] + t)
    render_list.line(from_p, to_p)
