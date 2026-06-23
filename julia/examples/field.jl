using GLViskit

window1 = create_window("Window1", 800, 600)

render_list = create_render_list()
add_render_list!(window1, render_list)

render_list_axes = create_render_list()
add_render_list!(window1, render_list_axes)

# perspective camera tilted down
camera = window1.camera
perspective_fov!(camera, 60.0, 60.0)
camera.position = (0, 0, 0)
camera.rotation = (-0.5, 0, 0)
camera.distance = 50.0

window1.controller = SphericalController()

# build the grid
xs = range(-30.0f0, 30.0f0, length=100)
ys = range(-30.0f0, 30.0f0, length=100)

from_list = NTuple{3,Float32}[]
to_list = NTuple{3,Float32}[]
for i in 1:(length(xs)-1), j in 1:(length(ys)-1)
    push!(from_list, (xs[i], ys[j], 0));
    push!(to_list, (xs[i+1], ys[j], 0))
    push!(from_list, (xs[i], ys[j], 0));
    push!(to_list, (xs[i], ys[j+1], 0))
end

# pack
from_p = Float32[p[r] for r in 1:3, p in from_list]
to_p = Float32[p[r] for r in 1:3, p in to_list]

t0 = get_time_seconds()
while loop()
    t = get_time_seconds() - t0

    # redraw the field each frame
    # height = sin(x + t) + sin(y + t)
    clear!(render_list)
    size!(render_list, 5.0)
    from_p[3, :] .= sin.(from_p[1, :] .+ t) .+ sin.(from_p[2, :] .+ t)
    to_p[3, :] .= sin.(to_p[1, :] .+ t) .+ sin.(to_p[2, :] .+ t)
    line!(render_list, from_p, to_p)
end
