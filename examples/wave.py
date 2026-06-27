import math

import glviskit
import numpy as np

window1 = glviskit.create_window("Wave", 800, 600)

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
    window1.ui_begin("Controls")
    window1.ui_text("surface plot demo")
    window1.ui_text(f"frame: {frame_index}")
    window1.ui_separator()
    _, animate = window1.ui_checkbox("animate", animate)
    _, rot_speed = window1.ui_slider_float("rotation speed", rot_speed, 0.0, 0.05)
    _, amplitude = window1.ui_slider_float("amplitude", amplitude, 0.0, 3.0)
    _, frequency = window1.ui_slider_float("frequency", frequency, 0.1, 3.0)
    _, line_size = window1.ui_slider_int("line size", line_size, 1, 8)
    _, wave_mode = window1.ui_combo("wave mode", wave_mode, "ripple|sine x|sine y")
    _, line_color = window1.ui_color_edit3("line color", line_color)
    _, bg_color = window1.ui_color_edit4("background", bg_color)
    history = history[1:] + [amplitude * math.sin(frequency * t)]
    window1.ui_plot_lines("signal", history)
    window1.ui_separator()
    if window1.ui_button("reset view"):
        camera.rotation = [-0.5, 0.0, 0.0]
    window1.ui_same_line()
    if window1.ui_button("stop"):
        animate = False
    window1.ui_end()

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
