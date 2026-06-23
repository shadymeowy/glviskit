using GLViskit

window1 = create_window("Window1", 800, 600)
window2 = create_window("Window2", 800, 600)

render_list = create_render_list()
add_render_list!(window1, render_list)
add_render_list!(window2, render_list)
clear_instances!(render_list)

for i in 1:4
    s = iseven(i) ? 1 : -1
    add_instance!(
        render_list;
        pos=(3.0 * (i - 0.5), 0, 0),
        rot=(0.5s, 0, 0),
        scale=(1, 1, 1)
    )
    add_instance!(
        render_list;
        pos=(-3.0 * (i - 0.5), 0, 0),
        rot=(-0.5s, 0, 0),
        scale=(1, 1, 1)
    )
end

render_list_sine = create_render_list()
add_render_list!(window1, render_list_sine)
add_render_list!(window2, render_list_sine)

render_list_axes = create_render_list()
add_render_list!(window1, render_list_axes)
add_render_list!(window2, render_list_axes)

size!(render_list_axes, 5.0)
color!(render_list_axes, [1.0, 0.0, 0.0, 1.0]);
line!(render_list_axes, [0, 0, 0], [1, 0, 0])
color!(render_list_axes, [0.0, 1.0, 0.0, 1.0]);
line!(render_list_axes, [0, 0, 0], [0, 1, 0])
color!(render_list_axes, [0.0, 0.0, 1.0, 1.0]);
line!(render_list_axes, [0, 0, 0], [0, 0, 1])

color!(render_list, [1.0, 1.0, 1.0, 1.0])
size!(render_list, 5.0)
circle!(render_list, rand(Float32, 3, 10) .* 2 .- 1)

camera = window1.camera
camera2 = window2.camera
for cam in (camera, camera2)
    perspective_fov!(cam, 60.0, 60.0)
    cam.position = (0, 0, 0)
    cam.rotation = (0, 0, 0)
    cam.preserve_aspect_ratio = true
    cam.distance = 15.0
end

angle = 0.0
frame_index = 0
while loop()
    global angle, frame_index
    curr_time = get_time_seconds()
    frame_index += 1
    # angle += 0.005
    # camera.rotation = (-0.5, angle, 0)
    # camera2.rotation = (-0.5, -angle, 0)

    point!(
        render_list,
        rand(Float32, 3, 10) .* 2 .- 1;
        colors=rand(Float32, 4, 10),
        sizes=rand(Float32, 10) .+ 1
    )

    if frame_index % 10 == 0
        polyline!(
            render_list,
            rand(Float32, 3, 2, 10) .* 2 .- 1;
            colors=repeat(rand(Float32, 4, 2), 1, 1, 10),
            sizes=repeat(rand(Float32, 2) .* 3 .+ 1, 1, 10)
        )
    end

    restore!(render_list_sine)
    path = path_begin(render_list_sine)
    size!(path, 4.0)

    ix = collect(range(-1.0f0, 1.0f0, length=2000))
    points = Matrix{Float32}(undef, 3, 2000)
    points[1, :] .= ix .* 20
    points[2, :] .= 1.5f0 .* sin.((50 .* ix) .+ (10 * curr_time))
    points[3, :] .= 1.5f0 .* cos.((50 .* ix) .+ (10 * curr_time))
    colors = Matrix{Float32}(undef, 4, 2000)
    colors[1, :] .= (points[1, :] .* 0.5f0) .+ 0.5f0
    colors[2, :] .= (points[2, :] .* 0.5f0) .+ 0.5f0
    colors[3, :] .= 0.5f0
    colors[4, :] .= 1.0f0
    line_to!(path, points; colors=colors)
end
