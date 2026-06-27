module GLViskit

using Libdl, Artifacts

function _resolve_library()
    deps = joinpath(@__DIR__, "..", "deps", "deps.jl")
    if isfile(deps)
        path = include(deps)
        path isa AbstractString && isfile(path) && return path
    end

    toml = joinpath(dirname(@__DIR__), "Artifacts.toml")
    if isfile(toml)
        hash = artifact_hash("glviskit_c", toml)
        if hash !== nothing && artifact_exists(hash)
            for (root, _, files) in walkdir(artifact_path(hash)), f in files
                if occursin("glviskit_c", f) && endswith(f, "." * Libdl.dlext)
                    return joinpath(root, f)
                end
            end
        end
    end

    error("GLViskit: no native library found. Published installs provide it via " *
          "Artifacts.toml; for a source build run `julia --project build_local.jl`.")
end

const lib = _resolve_library()
const F32 = Float32

export create_window, create_render_list, get_time_seconds, loop, render
export add_render_list!, make_current!, capture_rgba
export perspective_fov!, perspective!, calculate_transform, set_axis_rotation!
export point!, circle!, line!, polygon!, polyline!, fill_polygon!, triangles!
export path_begin, mesh_begin, color!, size!, add_instance!
export save!, restore!, clear!, save_instances!, restore_instances!, clear_instances!
export line_to!, close!, line_end!, vertex!, triangle!
export NullController, FirstPersonController, SphericalController
export WindowUI, ui_new_frame, panel
export text, separator, same_line, button, checkbox!
export slider_float!, slider_float3!, slider_int!, combo!, drag_float!
export color_edit3!, color_edit4!, plot_lines
export want_capture_mouse, want_capture_keyboard

# --- error handling -------------------------------------------------------

glverror() = unsafe_string(ccall((:glv_error, lib), Cstring, ()))

# raise on the C error code; nothing on success
@inline check(rc::Cint) = rc == 0 ? nothing : error(glverror())

# raise on -1; otherwise return the widget's changed/clicked flag as Bool
@inline check_ui(rc::Cint) = rc == -1 ? error(glverror()) : rc != 0

# --- handle types ---------------------------------------------------------

for (T, destroy) in ((:Window, :glv_window_destroy),
    (:Camera, :glv_camera_destroy),
    (:RenderList, :glv_render_list_destroy),
    (:Path, :glv_path_destroy),
    (:Mesh, :glv_mesh_destroy),
    (:BaseController, :glv_controller_destroy))
    @eval begin
        mutable struct $T
            ptr::Ptr{Cvoid}
            function $T(ptr::Ptr{Cvoid})
                ptr == C_NULL && error(glverror())
                h = new(ptr)
                finalizer(h) do x
                    p = getfield(x, :ptr)
                    if p != C_NULL
                        ccall(($(QuoteNode(destroy)), lib), Cvoid, (Ptr{Cvoid},), p)
                        setfield!(x, :ptr, C_NULL)
                    end
                end
                return h
            end
        end
        Base.cconvert(::Type{Ptr{Cvoid}}, h::$T) = getfield(h, :ptr)
    end
end

"""
    Window

A window owning an OpenGL context. Read/write properties: `background_color`
(`(r, g, b, a)`), `camera`, `controller`. Read-only property `ui` returns the
immediate-mode UI handle ([`WindowUI`](@ref)).
"""
Window

"""
    Camera

Projection and view transform. Read/write properties: `position`, `rotation`
(`(roll, pitch, yaw)`), `axis_rotation` (quaternion, read-only), `distance`,
`viewport_size` (read-only), `preserve_aspect_ratio`.
"""
Camera

"""
    RenderList

A retained drawing buffer. Property `enabled` toggles whether it is rendered.
"""
RenderList

"A persistent line-strip builder created with [`path_begin`](@ref)."
Path

"A persistent triangle-mesh builder created with [`mesh_begin`](@ref)."
Mesh

"Camera input handler; see [`NullController`](@ref), [`FirstPersonController`](@ref), [`SphericalController`](@ref)."
BaseController

"""
    WindowUI

Immediate-mode UI handle for a window, obtained with `window.ui`. Pass it to
[`panel`](@ref) and the widget functions ([`button`](@ref), [`slider_float!`](@ref), ...).
"""
struct WindowUI
    window::Window
end

# --- array coercion -------------------------------------------------------

asf32(A::AbstractArray) = convert(Array{F32}, A)
asi32(A::AbstractArray) = convert(Array{Int32}, A)
_optf32(::Nothing) = nothing
_optf32(A) = asf32(A)
_ptr(::Nothing) = C_NULL
_ptr(A::Array) = A

# --- top level ------------------------------------------------------------

"""
    create_window(title="GLViskit Window", width=800, height=600) -> Window

Create a window owning an OpenGL context.
"""
create_window(title::AbstractString="GLViskit Window", width::Integer=800, height::Integer=600) =
    Window(ccall((:glv_create_window, lib), Ptr{Cvoid}, (Cstring, Cint, Cint), title, width, height))

"""
    create_render_list() -> RenderList

Create a new render list.
"""
create_render_list() = RenderList(ccall((:glv_create_render_list, lib), Ptr{Cvoid}, ()))

"Current time in seconds since the program started."
get_time_seconds() = ccall((:glv_get_time_seconds, lib), Cfloat, ())

"""
    loop() -> Bool

Run one iteration of the event loop and render every window. Returns `false`
once the application should quit (also opens a fresh UI frame for each window).
"""
function loop()
    rc = ccall((:glv_loop, lib), Cint, ())
    rc == -1 && error(glverror())
    return rc != 0
end

"Render every window without processing events."
render() = check(ccall((:glv_render, lib), Cint, ()))

# --- window ---------------------------------------------------------------

"Add a render list to the window for rendering."
add_render_list!(w::Window, rl::RenderList) =
    check(ccall((:glv_window_add_render_list, lib), Cint, (Ptr{Cvoid}, Ptr{Cvoid}), w, rl))

"Make the window's OpenGL context current on this thread."
make_current!(w::Window) = check(ccall((:glv_window_make_current, lib), Cint, (Ptr{Cvoid},), w))

"Render this window's contents."
render(w::Window) = check(ccall((:glv_window_render, lib), Cint, (Ptr{Cvoid},), w))

"Capture the window into a fresh `4 × W × H` `UInt8` RGBA array."
function capture_rgba(w::Window)
    width = Ref{Cint}()
    height = Ref{Cint}()
    check(ccall((:glv_window_get_size_in_pixels, lib), Cint,
        (Ptr{Cvoid}, Ptr{Cint}, Ptr{Cint}), w, width, height))
    buf = Array{UInt8}(undef, 4, width[], height[])  # column-major == C row-major (H, W, 4)
    check(ccall((:glv_window_capture_rgba, lib), Cint, (Ptr{Cvoid}, Ptr{UInt8}, Csize_t),
        w, buf, length(buf)))
    return buf
end

function Base.getproperty(w::Window, s::Symbol)
    s === :ptr && return getfield(w, :ptr)
    if s === :background_color
        r = Ref{F32}()
        g = Ref{F32}()
        b = Ref{F32}()
        a = Ref{F32}()
        check(ccall((:glv_window_get_background_color, lib), Cint,
            (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Ptr{F32}), w, r, g, b, a))
        return (r[], g[], b[], a[])
    elseif s === :camera
        return Camera(ccall((:glv_window_get_camera, lib), Ptr{Cvoid}, (Ptr{Cvoid},), w))
    elseif s === :controller
        return BaseController(ccall((:glv_window_get_controller, lib), Ptr{Cvoid}, (Ptr{Cvoid},), w))
    elseif s === :ui
        return WindowUI(w)
    end
    error("Window has no property `$s`")
end

function Base.setproperty!(w::Window, s::Symbol, v)
    s === :ptr && return setfield!(w, :ptr, v)
    if s === :background_color
        check(ccall((:glv_window_set_background_color, lib), Cint,
            (Ptr{Cvoid}, F32, F32, F32, F32), w, v[1], v[2], v[3], v[4]))
    elseif s === :camera
        check(ccall((:glv_window_set_camera, lib), Cint, (Ptr{Cvoid}, Ptr{Cvoid}), w, v))
    elseif s === :controller
        check(ccall((:glv_window_set_controller, lib), Cint, (Ptr{Cvoid}, Ptr{Cvoid}), w, v))
    else
        error("Window has no property `$s`")
    end
    return v
end

# --- camera ---------------------------------------------------------------

"""
    perspective_fov!(camera, hfov, vfov; near=0.1, far=100.0)

Set a perspective projection from horizontal/vertical field of view (degrees).
"""
perspective_fov!(c::Camera, hfov, vfov; near=0.1, far=100.0) =
    check(ccall((:glv_camera_perspective_fov, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32),
        c, hfov, vfov, near, far))

"""
    perspective!(camera, fxn, fyn; cx=0.5, cy=0.5, near=0.1, far=100.0)

Set a perspective projection from normalized focal lengths and principal point.
"""
perspective!(c::Camera, fxn, fyn; cx=0.5, cy=0.5, near=0.1, far=100.0) =
    check(ccall((:glv_camera_perspective, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32, F32, F32),
        c, fxn, fyn, cx, cy, near, far))

"""
    set_axis_rotation!(camera, x, y, z; inv_x=false, inv_y=false, inv_z=false)

Remap which world axes the camera's roll/pitch/yaw rotate about.
"""
set_axis_rotation!(c::Camera, x::Integer, y::Integer, z::Integer;
    inv_x=false, inv_y=false, inv_z=false) =
    check(ccall((:glv_camera_set_axis_rotation_axes, lib), Cint,
        (Ptr{Cvoid}, Cint, Cint, Cint, Cint, Cint, Cint), c, x, y, z, inv_x, inv_y, inv_z))

"4×4 camera transform in standard (row-major) orientation."
function calculate_transform(c::Camera)
    m = Matrix{F32}(undef, 4, 4)
    check(ccall((:glv_camera_calculate_transform, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, m))
    return permutedims(m)
end

function Base.getproperty(c::Camera, s::Symbol)
    s === :ptr && return getfield(c, :ptr)
    if s === :position
        x = Ref{F32}()
        y = Ref{F32}()
        z = Ref{F32}()
        check(ccall((:glv_camera_get_position, lib), Cint,
            (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}), c, x, y, z))
        return (x[], y[], z[])
    elseif s === :rotation
        a = Ref{F32}()
        b = Ref{F32}()
        d = Ref{F32}()
        check(ccall((:glv_camera_get_rotation, lib), Cint,
            (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}), c, a, b, d))
        return (a[], b[], d[])
    elseif s === :axis_rotation
        w = Ref{F32}()
        x = Ref{F32}()
        y = Ref{F32}()
        z = Ref{F32}()
        check(ccall((:glv_camera_get_axis_rotation, lib), Cint,
            (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Ptr{F32}), c, w, x, y, z))
        return (w[], x[], y[], z[])
    elseif s === :distance
        d = Ref{F32}()
        check(ccall((:glv_camera_get_distance, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, d))
        return d[]
    elseif s === :viewport_size
        w = Ref{F32}()
        h = Ref{F32}()
        check(ccall((:glv_camera_get_viewport_size, lib), Cint,
            (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}), c, w, h))
        return (w[], h[])
    elseif s === :preserve_aspect_ratio
        v = Ref{Cint}()
        check(ccall((:glv_camera_get_preserve_aspect_ratio, lib), Cint, (Ptr{Cvoid}, Ptr{Cint}), c, v))
        return v[] != 0
    end
    error("Camera has no property `$s`")
end

function Base.setproperty!(c::Camera, s::Symbol, v)
    s === :ptr && return setfield!(c, :ptr, v)
    if s === :position
        check(ccall((:glv_camera_set_position, lib), Cint, (Ptr{Cvoid}, F32, F32, F32), c, v[1], v[2], v[3]))
    elseif s === :rotation
        check(ccall((:glv_camera_set_rotation, lib), Cint, (Ptr{Cvoid}, F32, F32, F32), c, v[1], v[2], v[3]))
    elseif s === :axis_rotation
        check(ccall((:glv_camera_set_axis_rotation, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32),
            c, v[1], v[2], v[3], v[4]))
    elseif s === :distance
        check(ccall((:glv_camera_set_distance, lib), Cint, (Ptr{Cvoid}, F32), c, v))
    elseif s === :preserve_aspect_ratio
        check(ccall((:glv_camera_set_preserve_aspect_ratio, lib), Cint, (Ptr{Cvoid}, Cint), c, v))
    else
        error("Camera has no settable property `$s`")
    end
    return v
end

# --- render list: state ---------------------------------------------------

"Set the current draw color `(r, g, b, a)` for subsequent geometry."
color!(rl::RenderList, c) =
    check(ccall((:glv_render_list_color, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32),
        rl, c[1], c[2], c[3], c[4]))

"Set the current point/line size for subsequent geometry."
size!(rl::RenderList, s) = check(ccall((:glv_render_list_size, lib), Cint, (Ptr{Cvoid}, F32), rl, s))

for (jl, c) in ((:save!, :glv_render_list_save), (:restore!, :glv_render_list_restore),
    (:clear!, :glv_render_list_clear), (:save_instances!, :glv_render_list_save_instances),
    (:restore_instances!, :glv_render_list_restore_instances),
    (:clear_instances!, :glv_render_list_clear_instances))
    @eval $jl(rl::RenderList) = check(ccall(($(QuoteNode(c)), lib), Cint, (Ptr{Cvoid},), rl))
end

"Save the current drawing state (color, size) and geometry mark."
save!

"Restore the drawing state and geometry to the last [`save!`](@ref)."
restore!

"Clear all stored geometry from the render list."
clear!

"Save the current instance stack."
save_instances!

"Restore the instance stack to the last [`save_instances!`](@ref)."
restore_instances!

"Clear all instances from the render list."
clear_instances!

function Base.getproperty(rl::RenderList, s::Symbol)
    s === :ptr && return getfield(rl, :ptr)
    s === :enabled || error("RenderList has no property `$s`")
    v = Ref{Cint}()
    check(ccall((:glv_render_list_get_enabled, lib), Cint, (Ptr{Cvoid}, Ptr{Cint}), rl, v))
    return v[] != 0
end

function Base.setproperty!(rl::RenderList, s::Symbol, v)
    s === :ptr && return setfield!(rl, :ptr, v)
    s === :enabled || error("RenderList has no property `$s`")
    check(ccall((:glv_render_list_set_enabled, lib), Cint, (Ptr{Cvoid}, Cint), rl, v))
    return v
end

"Begin a persistent [`Path`](@ref) line-strip builder on the render list."
path_begin(rl::RenderList) = Path(ccall((:glv_render_list_path_begin, lib), Ptr{Cvoid}, (Ptr{Cvoid},), rl))

"Begin a persistent [`Mesh`](@ref) builder on the render list."
mesh_begin(rl::RenderList) = Mesh(ccall((:glv_render_list_mesh_begin, lib), Ptr{Cvoid}, (Ptr{Cvoid},), rl))

# --- render list: batched drawing -----------------------------------------

function _check_flat(name, n, colors, sizes)
    colors === nothing || size(colors) == (4, n) ||
        throw(ArgumentError("$name colors must be 4 × N matching the point count"))
    sizes === nothing || size(sizes) == (n,) ||
        throw(ArgumentError("$name sizes must be a length-N vector"))
end

for (jl, c) in ((:point!, :glv_render_list_points), (:circle!, :glv_render_list_circles))
    @eval begin
        function $jl(rl::RenderList, pts::AbstractMatrix{<:Real}; colors=nothing, sizes=nothing)
            size(pts, 1) == 3 || throw(ArgumentError("points must be 3 × N"))
            n = size(pts, 2)
            _check_flat($(string(jl)), n, colors, sizes)
            check(ccall(($(QuoteNode(c)), lib), Cint,
                (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Csize_t),
                rl, asf32(pts), _ptr(_optf32(colors)), _ptr(_optf32(sizes)), n))
            return rl
        end
        $jl(rl::RenderList, p::AbstractVector{<:Real}; color=nothing, size=nothing) =
            $jl(rl, reshape(collect(F32, p), 3, 1);
                colors=color === nothing ? nothing : reshape(collect(F32, color), 4, 1),
                sizes=size === nothing ? nothing : F32[size])
    end
end

"""
    point!(rl, pts; colors=nothing, sizes=nothing)
    point!(rl, p; color=nothing, size=nothing)

Add points from a `3 × N` matrix (optionally `4 × N` colors and length-`N`
sizes), or a single `3`-vector. Returns `rl`.
"""
point!

"""
    circle!(rl, pts; colors=nothing, sizes=nothing)
    circle!(rl, p; color=nothing, size=nothing)

Add screen-facing circles from a `3 × N` matrix or a single `3`-vector. Returns `rl`.
"""
circle!

"""
    line!(rl, starts, ends; colors=nothing, sizes=nothing)
    line!(rl, s, e; color=nothing, size=nothing)

Add line segments from matching `3 × N` start/end matrices, or a single
start/end `3`-vector pair. Returns `rl`.
"""
function line!(rl::RenderList, starts::AbstractMatrix{<:Real}, ends::AbstractMatrix{<:Real};
    colors=nothing, sizes=nothing)
    size(starts, 1) == 3 && size(ends) == size(starts) ||
        throw(ArgumentError("starts and ends must be matching 3 × N"))
    n = size(starts, 2)
    _check_flat("line!", n, colors, sizes)
    check(ccall((:glv_render_list_lines, lib), Cint,
        (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Csize_t),
        rl, asf32(starts), asf32(ends), _ptr(_optf32(colors)), _ptr(_optf32(sizes)), n))
    return rl
end

line!(rl::RenderList, s::AbstractVector{<:Real}, e::AbstractVector{<:Real};
    color=nothing, size=nothing) =
    line!(rl, reshape(collect(F32, s), 3, 1), reshape(collect(F32, e), 3, 1);
        colors=color === nothing ? nothing : reshape(collect(F32, color), 4, 1),
        sizes=size === nothing ? nothing : F32[size])

# polygon family: a single shape (3 × N) or a batch (3 × N × B)
for (jl, c, hassize) in ((:polygon!, :glv_render_list_polygons, true),
    (:polyline!, :glv_render_list_polylines, true),
    (:fill_polygon!, :glv_render_list_fill_polygons, false))
    prelude = quote
        ndims(v) in (2, 3) && size(v, 1) == 3 ||
            throw(ArgumentError("vertices must be 3 × N or 3 × N × B"))
        tail = size(v)[2:end]               # (count,) or (count, groups)
        count = tail[1]
        groups = length(tail) == 2 ? tail[2] : 1
        colors === nothing || size(colors) == (4, tail...) ||
            throw(ArgumentError("colors must be 4 × N[ × B] matching vertices"))
    end
    if hassize
        @eval function $jl(rl::RenderList, v::AbstractArray{<:Real}; colors=nothing, sizes=nothing)
            $prelude
            sizes === nothing || size(sizes) == tail ||
                throw(ArgumentError("sizes must be N[ × B] matching vertices"))
            check(ccall(($(QuoteNode(c)), lib), Cint,
                (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Csize_t, Csize_t),
                rl, asf32(v), _ptr(_optf32(colors)), _ptr(_optf32(sizes)), groups, count))
            return rl
        end
    else
        @eval function $jl(rl::RenderList, v::AbstractArray{<:Real}; colors=nothing)
            $prelude
            check(ccall(($(QuoteNode(c)), lib), Cint,
                (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Csize_t, Csize_t),
                rl, asf32(v), _ptr(_optf32(colors)), groups, count))
            return rl
        end
    end
end

"""
    polygon!(rl, v; colors=nothing, sizes=nothing)

Add closed polygon outlines from a single `3 × N` shape or a `3 × N × B` batch.
Returns `rl`.
"""
polygon!

"""
    polyline!(rl, v; colors=nothing, sizes=nothing)

Add open line strips from a single `3 × N` shape or a `3 × N × B` batch. Returns `rl`.
"""
polyline!

"""
    fill_polygon!(rl, v; colors=nothing)

Add filled polygons from a single `3 × N` shape or a `3 × N × B` batch. Returns `rl`.
"""
fill_polygon!

"""
    triangles!(rl, vertices, indices; colors=nothing)

Add indexed triangles from a `3 × N` vertex matrix and a `3 × T` matrix of
0-based vertex indices. Returns `rl`.
"""
function triangles!(rl::RenderList, vertices::AbstractMatrix{<:Real},
    indices::AbstractMatrix{<:Integer}; colors=nothing)
    size(vertices, 1) == 3 || throw(ArgumentError("vertices must be 3 × N"))
    size(indices, 1) == 3 || throw(ArgumentError("indices must be 3 × T"))
    nverts = size(vertices, 2)
    all(i -> 0 <= i < nverts, indices) ||
        throw(BoundsError("triangle index out of range 0:$(nverts - 1)"))
    colors === nothing || size(colors) == (4, nverts) ||
        throw(ArgumentError("colors must be 4 × N"))
    check(ccall((:glv_render_list_triangles, lib), Cint,
        (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Csize_t, Ptr{Int32}, Csize_t),
        rl, asf32(vertices), _ptr(_optf32(colors)), nverts, asi32(indices), size(indices, 2)))
    return rl
end

# --- render list: instances -----------------------------------------------

"""
    add_instance!(rl, transform)
    add_instance!(rl; pos=(0,0,0), rot=(0,0,0), scale=(1,1,1))

Add an instance of the render list's geometry, either from a `4 × 4` transform
matrix or from position/rotation/scale. A length-4 `rot` is treated as a
quaternion, a length-3 `rot` as Euler angles.
"""
function add_instance!(rl::RenderList, transform::AbstractMatrix{<:Real})
    size(transform) == (4, 4) || throw(ArgumentError("transform must be 4 × 4"))
    # C expects row-major 16; permutedims turns the column-major matrix row-major
    check(ccall((:glv_render_list_add_instance_matrix, lib), Cint, (Ptr{Cvoid}, Ptr{F32}),
        rl, asf32(permutedims(transform))))
end

function add_instance!(rl::RenderList; pos=(0, 0, 0), rot=(0, 0, 0), scale=(1, 1, 1))
    if length(rot) == 4
        check(ccall((:glv_render_list_add_instance_quat, lib), Cint,
            (Ptr{Cvoid}, F32, F32, F32, F32, F32, F32, F32, F32, F32, F32), rl,
            pos[1], pos[2], pos[3], rot[1], rot[2], rot[3], rot[4], scale[1], scale[2], scale[3]))
    else
        check(ccall((:glv_render_list_add_instance, lib), Cint,
            (Ptr{Cvoid}, F32, F32, F32, F32, F32, F32, F32, F32, F32), rl,
            pos[1], pos[2], pos[3], rot[1], rot[2], rot[3], scale[1], scale[2], scale[3]))
    end
end

# --- path -----------------------------------------------------------------

"""
    line_to!(path, pts; colors=nothing, sizes=nothing)
    line_to!(path, pt; color=nothing, size=nothing)

Extend the path by a `3 × N` run of points or a single `3`-vector. Returns the path.
"""
function line_to!(p::Path, pts::AbstractMatrix{<:Real}; colors=nothing, sizes=nothing)
    size(pts, 1) == 3 || throw(ArgumentError("points must be 3 × N"))
    n = size(pts, 2)
    _check_flat("line_to!", n, colors, sizes)
    check(ccall((:glv_path_line_to_many, lib), Cint,
        (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{F32}, Csize_t),
        p, asf32(pts), _ptr(_optf32(colors)), _ptr(_optf32(sizes)), n))
    return p
end

line_to!(p::Path, pt::AbstractVector{<:Real}; color=nothing, size=nothing) =
    line_to!(p, reshape(collect(F32, pt), 3, 1);
        colors=color === nothing ? nothing : reshape(collect(F32, color), 4, 1),
        sizes=size === nothing ? nothing : F32[size])

"Close the current path back to its start."
close!(p::Path) = check(ccall((:glv_path_close, lib), Cint, (Ptr{Cvoid},), p))

"End the current line strip; subsequent points start a new strip."
line_end!(p::Path) = check(ccall((:glv_path_line_end, lib), Cint, (Ptr{Cvoid},), p))

"Set the current color `(r, g, b, a)` for subsequent path points."
color!(p::Path, c) = check(ccall((:glv_path_color, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32),
    p, c[1], c[2], c[3], c[4]))

"Set the current line size for subsequent path points."
size!(p::Path, s) = check(ccall((:glv_path_size, lib), Cint, (Ptr{Cvoid}, F32), p, s))

# --- mesh -----------------------------------------------------------------

"Add vertices (`3 × N`) and return their 0-based mesh-local indices."
function vertex!(m::Mesh, pts::AbstractMatrix{<:Real}; colors=nothing)
    size(pts, 1) == 3 || throw(ArgumentError("points must be 3 × N"))
    n = size(pts, 2)
    colors === nothing || size(colors) == (4, n) || throw(ArgumentError("colors must be 4 × N"))
    out = Vector{Csize_t}(undef, n)
    check(ccall((:glv_mesh_vertices, lib), Cint,
        (Ptr{Cvoid}, Ptr{F32}, Ptr{F32}, Ptr{Csize_t}, Csize_t),
        m, asf32(pts), _ptr(_optf32(colors)), out, n))
    return out
end

vertex!(m::Mesh, p::AbstractVector{<:Real}; color=nothing) =
    vertex!(m, reshape(collect(F32, p), 3, 1);
        colors=color === nothing ? nothing : reshape(collect(F32, color), 4, 1))[1]

"""
    triangle!(mesh, indices)
    triangle!(mesh, i0, i1, i2)

Add triangles from a `3 × T` matrix of 0-based mesh-local indices, or a single triple.
"""
function triangle!(m::Mesh, indices::AbstractMatrix{<:Integer})
    size(indices, 1) == 3 || throw(ArgumentError("indices must be 3 × T"))
    all(>=(0), indices) || throw(ArgumentError("triangle indices must be non-negative"))
    check(ccall((:glv_mesh_triangles, lib), Cint, (Ptr{Cvoid}, Ptr{Int32}, Csize_t),
        m, asi32(indices), size(indices, 2)))
    return m
end

triangle!(m::Mesh, i0::Integer, i1::Integer, i2::Integer) =
    check(ccall((:glv_mesh_triangle, lib), Cint, (Ptr{Cvoid}, Csize_t, Csize_t, Csize_t), m, i0, i1, i2))

"Set the current color `(r, g, b, a)` for subsequent mesh vertices."
color!(m::Mesh, c) = check(ccall((:glv_mesh_color, lib), Cint, (Ptr{Cvoid}, F32, F32, F32, F32),
    m, c[1], c[2], c[3], c[4]))

# --- controllers ----------------------------------------------------------

"A controller that ignores all input."
NullController() = BaseController(ccall((:glv_create_null_controller, lib), Ptr{Cvoid}, ()))

"A first-person (fly) camera controller."
FirstPersonController() = BaseController(ccall((:glv_create_first_person_controller, lib), Ptr{Cvoid}, ()))

"A spherical (orbit) camera controller."
SphericalController() = BaseController(ccall((:glv_create_spherical_controller, lib), Ptr{Cvoid}, ()))

function Base.getproperty(c::BaseController, s::Symbol)
    s === :ptr && return getfield(c, :ptr)
    v = Ref{F32}()
    if s === :key_move_sensitivity
        check(ccall((:glv_controller_get_key_move_sensitivity, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, v))
    elseif s === :key_rot_sensitivity
        check(ccall((:glv_controller_get_key_rot_sensitivity, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, v))
    elseif s === :mouse_sensitivity
        check(ccall((:glv_controller_get_mouse_sensitivity, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, v))
    elseif s === :wheel_sensitivity
        check(ccall((:glv_controller_get_wheel_sensitivity, lib), Cint, (Ptr{Cvoid}, Ptr{F32}), c, v))
    else
        error("controller has no property `$s`")
    end
    return v[]
end

function Base.setproperty!(c::BaseController, s::Symbol, v)
    s === :ptr && return setfield!(c, :ptr, v)
    if s === :key_move_sensitivity
        check(ccall((:glv_controller_set_key_move_sensitivity, lib), Cint, (Ptr{Cvoid}, F32), c, v))
    elseif s === :key_rot_sensitivity
        check(ccall((:glv_controller_set_key_rot_sensitivity, lib), Cint, (Ptr{Cvoid}, F32), c, v))
    elseif s === :mouse_sensitivity
        check(ccall((:glv_controller_set_mouse_sensitivity, lib), Cint, (Ptr{Cvoid}, F32), c, v))
    elseif s === :wheel_sensitivity
        check(ccall((:glv_controller_set_wheel_sensitivity, lib), Cint, (Ptr{Cvoid}, F32), c, v))
    else
        error("controller has no settable property `$s`")
    end
    return v
end

# --- ui -------------------------------------------------------------------
#
# Immediate-mode widgets (Dear ImGui). Get a per-window handle with `w.ui`,
# group widgets in a `panel(ui, title) do ... end` block, and issue them each
# frame between `loop()` calls. Scalar in/out values are passed as `Ref` and
# arrays as `Vector{Float32}`, both mutated in place; widgets return a `Bool`
# that is `true` on the frame the value changed or the button was clicked.

"""
    ui_new_frame()

Open a fresh UI frame for every window. `loop()` already calls this, so it is
only needed for custom (non-`loop`) drivers.
"""
ui_new_frame() = check(ccall((:glv_ui_new_frame, lib), Cint, ()))

# internal panel begin/end (`begin`/`end` are reserved words, so use `panel`)
_panel_begin(ui::WindowUI, title::AbstractString) =
    check_ui(ccall((:glv_ui_begin, lib), Cint, (Ptr{Cvoid}, Cstring), ui.window, title))
_panel_end(ui::WindowUI) = check(ccall((:glv_ui_end, lib), Cint, (Ptr{Cvoid},), ui.window))

"""
    panel(f, ui, title) -> Bool

Open a UI panel, run `f` (its widgets), and always close it. Returns whether the
panel is visible (expanded); widgets in a collapsed panel are cheap no-ops. Use
the `do`-block form:

```julia
panel(ui, "Controls") do
    _ = button(ui, "reset")
end
```
"""
function panel(f, ui::WindowUI, title::AbstractString)
    visible = _panel_begin(ui, title)
    try
        f()
    finally
        _panel_end(ui)
    end
    return visible
end

"Draw a line of text."
text(ui::WindowUI, s::AbstractString) =
    check(ccall((:glv_ui_text, lib), Cint, (Ptr{Cvoid}, Cstring), ui.window, s))

"Draw a horizontal separator."
separator(ui::WindowUI) = check(ccall((:glv_ui_separator, lib), Cint, (Ptr{Cvoid},), ui.window))

"Keep the next widget on the same line as the previous one."
same_line(ui::WindowUI) = check(ccall((:glv_ui_same_line, lib), Cint, (Ptr{Cvoid},), ui.window))

"Draw a button; returns `true` on the frame it is clicked."
button(ui::WindowUI, label::AbstractString) =
    check_ui(ccall((:glv_ui_button, lib), Cint, (Ptr{Cvoid}, Cstring), ui.window, label))

"""
    checkbox!(ui, label, value::Ref) -> Bool

Draw a checkbox bound to `value` (mutated in place); returns whether it changed.
"""
function checkbox!(ui::WindowUI, label::AbstractString, value::Ref)
    v = Ref{Cint}(value[] ? 1 : 0)
    changed = check_ui(ccall((:glv_ui_checkbox, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{Cint}), ui.window, label, v))
    value[] = v[] != 0
    return changed
end

"""
    slider_float!(ui, label, value::Ref, vmin, vmax) -> Bool

Draw a float slider bound to `value` (mutated in place); returns whether it changed.
"""
function slider_float!(ui::WindowUI, label::AbstractString, value::Ref, vmin, vmax)
    v = Ref{F32}(value[])
    changed = check_ui(ccall((:glv_ui_slider_float, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}, F32, F32), ui.window, label, v, vmin, vmax))
    value[] = v[]
    return changed
end

"""
    slider_float3!(ui, label, value::Vector{Float32}, vmin, vmax) -> Bool

Draw a 3-component float slider bound to `value` (mutated in place); returns
whether it changed.
"""
function slider_float3!(ui::WindowUI, label::AbstractString, value::Vector{F32}, vmin, vmax)
    length(value) >= 3 || error("slider_float3! expects a 3-element Float32 vector")
    return check_ui(ccall((:glv_ui_slider_float3, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}, F32, F32), ui.window, label, value, vmin, vmax))
end

"""
    slider_int!(ui, label, value::Ref, vmin, vmax) -> Bool

Draw an integer slider bound to `value` (mutated in place); returns whether it changed.
"""
function slider_int!(ui::WindowUI, label::AbstractString, value::Ref, vmin, vmax)
    v = Ref{Cint}(value[])
    changed = check_ui(ccall((:glv_ui_slider_int, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{Cint}, Cint, Cint), ui.window, label, v, vmin, vmax))
    value[] = v[]
    return changed
end

"""
    combo!(ui, label, current::Ref, items) -> Bool

Draw a combo box from `"a|b|c"` style options; `current` holds the selected
index (mutated in place). Returns whether it changed.
"""
function combo!(ui::WindowUI, label::AbstractString, current::Ref, items::AbstractString)
    c = Ref{Cint}(current[])
    changed = check_ui(ccall((:glv_ui_combo, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{Cint}, Cstring), ui.window, label, c, items))
    current[] = c[]
    return changed
end

"""
    drag_float!(ui, label, value::Ref, speed=1.0, vmin=0.0, vmax=0.0) -> Bool

Draw a draggable float bound to `value` (mutated in place); returns whether it changed.
"""
function drag_float!(ui::WindowUI, label::AbstractString, value::Ref, speed=1.0, vmin=0.0, vmax=0.0)
    v = Ref{F32}(value[])
    changed = check_ui(ccall((:glv_ui_drag_float, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}, F32, F32, F32), ui.window, label, v, speed, vmin, vmax))
    value[] = v[]
    return changed
end

"""
    color_edit3!(ui, label, color::Vector{Float32}) -> Bool

Edit an RGB color in `color` (mutated in place); returns whether it changed.
"""
function color_edit3!(ui::WindowUI, label::AbstractString, color::Vector{F32})
    length(color) >= 3 || error("color_edit3! expects a 3-element Float32 vector")
    return check_ui(ccall((:glv_ui_color_edit3, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}), ui.window, label, color))
end

"""
    color_edit4!(ui, label, color::Vector{Float32}) -> Bool

Edit an RGBA color in `color` (mutated in place); returns whether it changed.
"""
function color_edit4!(ui::WindowUI, label::AbstractString, color::Vector{F32})
    length(color) >= 4 || error("color_edit4! expects a 4-element Float32 vector")
    return check_ui(ccall((:glv_ui_color_edit4, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}), ui.window, label, color))
end

"Plot a line graph of `values` (any real vector)."
function plot_lines(ui::WindowUI, label::AbstractString, values::AbstractVector)
    buf = asf32(values)
    check(ccall((:glv_ui_plot_lines, lib), Cint,
        (Ptr{Cvoid}, Cstring, Ptr{F32}, Cint), ui.window, label, buf, length(buf)))
end

"Whether the UI is currently capturing the mouse (gate your own input on this)."
want_capture_mouse(ui::WindowUI) =
    check_ui(ccall((:glv_ui_want_capture_mouse, lib), Cint, (Ptr{Cvoid},), ui.window))

"Whether the UI is currently capturing the keyboard."
want_capture_keyboard(ui::WindowUI) =
    check_ui(ccall((:glv_ui_want_capture_keyboard, lib), Cint, (Ptr{Cvoid},), ui.window))

end # module
