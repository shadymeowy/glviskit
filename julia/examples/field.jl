using GLViskit

window1 = create_window("Window1", 800, 600)

render_list = create_render_list()
add_render_list!(window1, render_list)

render_list_axes = create_render_list()
add_render_list!(window1, render_list_axes)

# perspective camera tilted down
camera = window1.camera
perspective_fov!(camera, 60.0, 60.0; far=1000.0)
camera.position = (0, 0, 0)
camera.rotation = (-0.5, 0, 0)
camera.distance = 50.0
set_axis_rotation!(camera, 1, 2, 0)

window1.controller = SphericalController()

# in place height field and height based color
function heights!(p, t)
    @. p[3, :] = (
        sin(0.1f0 * (p[1, :] + p[2, :]))
        *
        (
            3 * sin(0.5f0 * p[1, :] + t)
            +
            2 * sin(0.5f0 * p[2, :] + t)
        )
    )
end

function colorize!(cols, p)
    @. cols[1, :] = (p[3, :] * 0.25f0) + 0.5f0
    cols[2, :] .= 0.5f0
    @. cols[3, :] = 0.5f0 - (p[3, :] * 0.25f0)
    cols[4, :] .= 1.0f0
end

# build the grid
xs = range(-30.0f0, 30.0f0, length=25)
ys = range(-30.0f0, 30.0f0, length=25)

from_list = NTuple{3,Float32}[]
to_list = NTuple{3,Float32}[]
# horizontal segments along every row
for i in 1:(length(xs)-1), j in 1:length(ys)
    push!(from_list, (xs[i], ys[j], 0))
    push!(to_list, (xs[i+1], ys[j], 0))
end
# vertical segments along every column
for i in 1:length(xs), j in 1:(length(ys)-1)
    push!(from_list, (xs[i], ys[j], 0))
    push!(to_list, (xs[i], ys[j+1], 0))
end

# grid node points, drawn as same-size circles to round off the joints
node_list = NTuple{3,Float32}[]
for i in 1:length(xs), j in 1:length(ys)
    push!(node_list, (xs[i], ys[j], 0))
end

# pack
from_p = Float32[p[r] for r in 1:3, p in from_list]
to_p = Float32[p[r] for r in 1:3, p in to_list]
points = Float32[p[r] for r in 1:3, p in node_list]

line_cols = Matrix{Float32}(undef, 4, size(from_p, 2))
node_cols = Matrix{Float32}(undef, 4, size(points, 2))

t0 = get_time_seconds()
while loop()
    t = get_time_seconds() - t0

    heights!(from_p, t)
    heights!(to_p, t)
    heights!(points, t)
    colorize!(line_cols, from_p)
    colorize!(node_cols, points)

    # redraw the field each frame
    clear!(render_list)
    size!(render_list, 5.0)
    line!(render_list, from_p, to_p; colors=line_cols)
    size!(render_list, 10.0)
    circle!(render_list, points; colors=node_cols)
end
