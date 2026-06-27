import math

import glviskit
import numpy as np

window1 = glviskit.create_window("Wave", 800, 600)
ui1 = window1.ui

render_list = glviskit.create_render_list()
window1.add_render_list(render_list)

render_list_axes = glviskit.create_render_list()
window1.add_render_list(render_list_axes)

render_list_axes.size(5.0)
render_list_axes.color((1.0, 0.0, 0.0, 1.0))
render_list_axes.line((0.0, 0.0, 0.0), (1.0, 0.0, 0.0))
render_list_axes.color((0.0, 1.0, 0.0, 1.0))
render_list_axes.line((0.0, 0.0, 0.0), (0.0, 1.0, 0.0))
render_list_axes.color((0.0, 0.0, 1.0, 1.0))
render_list_axes.line((0.0, 0.0, 0.0), (0.0, 0.0, 1.0))

camera = window1.camera
camera.perspective_fov(60.0, 60.0)
camera.position = [0.0, 0.0, 0.0]
camera.rotation = [-0.5, 0.0, 0.0]
camera.preserve_aspect_ratio = True
camera.distance = 10.0

# ui-controlled parameters
animate = True
rot_speed = 0.005
amplitude = 1.0
frequency = 1.0
line_size = 2
wave_mode = 0
line_color = [0.2, 0.7, 1.0]
bg_color = [0.0, 0.0, 0.0, 1.0]
history = [0.0] * 100

n = 80
xs = np.linspace(-10.0, 10.0, n).astype(np.float32)
ys = np.linspace(-10.0, 10.0, n).astype(np.float32)
gx, gy = np.meshgrid(xs, ys)

frame_index = 0

while glviskit.loop():
    t = glviskit.get_time_seconds()
    frame_index += 1

    # build the control panel
    with ui1.panel("Controls"):
        ui1.text("surface plot demo")
        ui1.text(f"frame: {frame_index}")
        ui1.separator()
        _, animate = ui1.checkbox("animate", animate)
        _, rot_speed = ui1.slider_float("rotation speed", rot_speed, 0.0, 0.05)
        _, amplitude = ui1.slider_float("amplitude", amplitude, 0.0, 3.0)
        _, frequency = ui1.slider_float("frequency", frequency, 0.1, 3.0)
        _, line_size = ui1.slider_int("line size", line_size, 1, 8)
        _, wave_mode = ui1.combo("wave mode", wave_mode, "ripple|sine x|sine y")
        _, line_color = ui1.color_edit3("line color", line_color)
        _, bg_color = ui1.color_edit4("background", bg_color)
        history = history[1:] + [amplitude * math.sin(frequency * t)]
        ui1.plot_lines("signal", history)
        ui1.separator()
        if ui1.button("reset view"):
            camera.rotation = [-0.5, 0.0, 0.0]
        ui1.same_line()
        if ui1.button("stop"):
            animate = False

    # animate by nudging the camera's yaw, composing with the controller
    if animate:
        rot = camera.rotation
        rot[1] += rot_speed
        camera.rotation = rot
    window1.background_color = bg_color

    # height field for the current mode
    if wave_mode == 1:
        gz = amplitude * np.sin(frequency * (gx - t))
    elif wave_mode == 2:
        gz = amplitude * np.cos(frequency * (gy - t))
    else:
        gz = amplitude * np.sin(frequency * (gx - t)) * np.cos(frequency * (gy - t))

    # rows and columns as polyline grids, height on the middle axis
    rows = np.stack([gx, gz, gy], axis=-1).astype(np.float32)
    cols = np.transpose(rows, (1, 0, 2))

    render_list.clear()
    render_list.color((line_color[0], line_color[1], line_color[2], 1.0))
    render_list.size(float(line_size))
    render_list.polyline(rows)
    render_list.polyline(cols)

    # text test
    render_list.text(
        "glviskit\nwave demo", (0.0, 20.0, 0.0), (0.0, 60.0),
        (1.0, 1.0, 1.0, 1.0), 20.0, glviskit.TextAlign.Center, 1,
    )
    render_list.marker(
        glviskit.MarkerType.Diamond, (0.0, 20.0, 0.0), (0.0, 0.0),
        (1.0, 0.8, 0.2, 1.0), 32.0,
    )
