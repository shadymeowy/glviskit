using GLViskit

window1 = create_window("Window1", 800, 600)

render_list = create_render_list()
add_render_list!(window1, render_list)

render_list_axes = create_render_list()
add_render_list!(window1, render_list_axes)

# coordinate axes
size!(render_list_axes, 5.0)
color!(render_list_axes, [1.0, 0.0, 0.0, 1.0])
line!(render_list_axes, [0, 0, 0], [1, 0, 0])
color!(render_list_axes, [0.0, 1.0, 0.0, 1.0])
line!(render_list_axes, [0, 0, 0], [0, 1, 0])
color!(render_list_axes, [0.0, 0.0, 1.0, 1.0])
line!(render_list_axes, [0, 0, 0], [0, 0, 1])

# random circles in a cube
color!(render_list, [1.0, 1.0, 1.0, 1.0])
size!(render_list, 5.0)
circle!(render_list,
    rand(Float32, 3, 100) .* 2 .- 1;
    colors=rand(Float32, 4, 100),
    sizes=rand(Float32, 100) .* 10 .+ 5
)

# a big ground plane
plane = Float32[-1 1 1 -1
    0 0 0 0
    -1 -1 1 1] .* 10
plane[2, :] .-= 1
color!(render_list, [0.7, 0.7, 0.7, 1.0])
fill_polygon!(render_list, plane)
save!(render_list)

# perspective camera
camera = window1.camera
perspective_fov!(camera, 60.0, 60.0)
camera.position = (0, 0, 0)
camera.rotation = (0, 0, 0)
camera.preserve_aspect_ratio = true
camera.distance = 5.0

# two unit squares
square = Float32[-1 1 1 -1
    -1 -1 1 1
    0 0 0 0]
square2 = copy(square);
square2[3, :] .+= 1
pts = cat(square, square2; dims=3)

last_t = get_time_seconds()
while loop()
    current_t = get_time_seconds()
    dt = current_t - last_t

    # restore the render list to save
    restore!(render_list)

    # render the squares
    color!(render_list, [0.0, 1.0, 0.0, 1.0])
    size!(render_list, 10.0)
    polygon!(render_list, pts)
    # round the corners of the squares
    circle!(render_list, reshape(pts, 3, :))
    
    # fill semi transparent
    color!(render_list, [1.0, 0.0, 0.0, 0.5])
    fill_polygon!(render_list, pts)
end
