using GLViskit

window1 = create_window("Wave", 800, 600)
ui1 = window1.ui

render_list = create_render_list()
add_render_list!(window1, render_list)

render_list_axes = create_render_list()
add_render_list!(window1, render_list_axes)

size!(render_list_axes, 5.0)
color!(render_list_axes, [1.0, 0.0, 0.0, 1.0])
line!(render_list_axes, [0, 0, 0], [1, 0, 0])
color!(render_list_axes, [0.0, 1.0, 0.0, 1.0])
line!(render_list_axes, [0, 0, 0], [0, 1, 0])
color!(render_list_axes, [0.0, 0.0, 1.0, 1.0])
line!(render_list_axes, [0, 0, 0], [0, 0, 1])

camera = window1.camera
perspective_fov!(camera, 60.0, 60.0)
camera.position = (0, 0, 0)
camera.rotation = (-0.5, 0.0, 0.0)
camera.preserve_aspect_ratio = true
camera.distance = 10.0

# ui-controlled parameters
animate = Ref(true)
rot_speed = Ref(0.005f0)
amplitude = Ref(1.0f0)
frequency = Ref(1.0f0)
line_size = Ref(2)
wave_mode = Ref(0)
line_color = Float32[0.2, 0.7, 1.0]
bg_color = Float32[0.0, 0.0, 0.0, 1.0]
history = zeros(Float32, 100)

n = 80
xs = collect(range(-10.0f0, 10.0f0, length=n))
ys = collect(range(-10.0f0, 10.0f0, length=n))
gx = repeat(xs, 1, n)   # gx[j, i] = xs[j]
gy = repeat(ys', n, 1)  # gy[j, i] = ys[i]

frame_index = 0
while loop()
    global frame_index
    t = get_time_seconds()
    frame_index += 1

    # build the control panel
    panel(ui1, "Controls") do
        text(ui1, "surface plot demo")
        text(ui1, "frame: $frame_index")
        separator(ui1)
        checkbox!(ui1, "animate", animate)
        slider_float!(ui1, "rotation speed", rot_speed, 0.0, 0.05)
        slider_float!(ui1, "amplitude", amplitude, 0.0, 3.0)
        slider_float!(ui1, "frequency", frequency, 0.1, 3.0)
        slider_int!(ui1, "line size", line_size, 1, 8)
        combo!(ui1, "wave mode", wave_mode, "ripple|sine x|sine y")
        color_edit3!(ui1, "line color", line_color)
        color_edit4!(ui1, "background", bg_color)
        history[1:end-1] .= history[2:end]
        history[end] = amplitude[] * sin(frequency[] * t)
        plot_lines(ui1, "signal", history)
        separator(ui1)
        if button(ui1, "reset view")
            camera.rotation = (-0.5, 0.0, 0.0)
        end
        same_line(ui1)
        if button(ui1, "stop")
            animate[] = false
        end
    end

    # animate by nudging the camera's yaw, composing with the controller
    if animate[]
        rot = camera.rotation
        camera.rotation = (rot[1], rot[2] + rot_speed[], rot[3])
    end
    window1.background_color = (bg_color[1], bg_color[2], bg_color[3], bg_color[4])

    # height field for the current mode
    if wave_mode[] == 1
        gz = amplitude[] .* sin.(frequency[] .* (gx .- t))
    elseif wave_mode[] == 2
        gz = amplitude[] .* cos.(frequency[] .* (gy .- t))
    else
        gz = amplitude[] .* sin.(frequency[] .* (gx .- t)) .* cos.(frequency[] .* (gy .- t))
    end

    # rows and columns as polyline grids, height on the middle axis
    rows = Array{Float32}(undef, 3, n, n)
    rows[1, :, :] .= gx
    rows[2, :, :] .= gz
    rows[3, :, :] .= gy
    cols = permutedims(rows, (1, 3, 2))

    clear!(render_list)
    color!(render_list, (line_color[1], line_color[2], line_color[3], 1.0))
    size!(render_list, line_size[])
    polyline!(render_list, rows)
    polyline!(render_list, cols)
end
