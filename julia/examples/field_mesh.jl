using GLViskit

window1 = create_window("Window1", 800, 600)

render_list = create_render_list()
add_render_list!(window1, render_list)

# perspective camera tilted down
camera = window1.camera
perspective_fov!(camera, 60.0, 60.0; far=1000.0)
camera.position = (0, 0, 0)
camera.rotation = (-0.5, 0, 0)
camera.distance = 50.0
set_axis_rotation!(camera, 1, 2, 0)

window1.controller = SphericalController()

# grid of vertices
nx = 100
ny = 100
xs = range(-30.0f0, 30.0f0, length=nx)
ys = range(-30.0f0, 30.0f0, length=ny)

verts = Array{Float32}(undef, 3, ny, nx)
verts[1, :, :] .= reshape(xs, 1, nx)
verts[2, :, :] .= ys
verts[3, :, :] .= 0

# two triangles per cell (0-indexed)
lin = LinearIndices((ny, nx)) .- 1
v00 = @view lin[1:(ny-1), 1:(nx-1)]
v10 = @view lin[1:(ny-1), 2:nx]
v01 = @view lin[2:ny, 1:(nx-1)]
v11 = @view lin[2:ny, 2:nx]

tris = Array{Int32}(undef, 3, 2, ny - 1, nx - 1)
tris[1, 1, :, :] .= v00;
tris[2, 1, :, :] .= v10;
tris[3, 1, :, :] .= v11
tris[1, 2, :, :] .= v00;
tris[2, 2, :, :] .= v11;
tris[3, 2, :, :] .= v01

cols = Array{Float32}(undef, 4, ny, nx)

t0 = get_time_seconds()
while loop()
    t = get_time_seconds() - t0

    @. verts[3, :, :] .= (
        sin(0.1f0 * (verts[1, :, :] + verts[2, :, :]))
        *
        (
            3 * sin(0.5f0 * verts[1, :, :] + t)
            +
            2 * sin(0.5f0 * verts[2, :, :] + t)
        )
    )
    cols[1, :, :] .= (verts[3, :, :] .* 0.25f0) .+ 0.5f0
    cols[2, :, :] .= 0.5f0
    cols[3, :, :] .= 0.5f0 .- (verts[3, :, :] .* 0.25f0)
    cols[4, :, :] .= 1.0f0

    # rebuild the surface mesh
    clear!(render_list)
    triangles!(
        render_list,
        reshape(verts, 3, :),
        reshape(tris, 3, :);
        colors=reshape(cols, 4, :)
    )
end
