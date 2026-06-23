using GLViskit
using Test

if Sys.islinux() && !haskey(ENV, "DISPLAY") && !haskey(ENV, "WAYLAND_DISPLAY")
    get!(ENV, "SDL_VIDEODRIVER", "offscreen")
end

@testset "GLViskit" begin
    @test GLViskit.lib isa AbstractString
    @test get_time_seconds() isa Real

    win = if get(ENV, "GLVISKIT_TEST_GL", "1") == "0"
        @info "GLVISKIT_TEST_GL=0; skipping rendering tests"
        nothing
    else
        try
            create_window("test", 64, 64)
        catch err
            @warn "no GL context available; skipping rendering tests" err
            nothing
        end
    end

    if win !== nothing
        rl = create_render_list()
        add_render_list!(win, rl)

        @testset "render list batches" begin
            @test point!(rl, zeros(Float32, 3, 5)) === rl
            @test point!(rl, rand(3, 4), colors=rand(4, 4), sizes=rand(4)) === rl  # Float64 cast
            @test circle!(rl, zeros(Float32, 3, 3)) === rl
            @test line!(rl, zeros(Float32, 3, 6), ones(Float32, 3, 6)) === rl
            @test polygon!(rl, zeros(Float32, 3, 4)) === rl              # single
            @test polygon!(rl, zeros(Float32, 3, 4, 2)) === rl           # batch of 2
            @test polyline!(rl, zeros(Float32, 3, 5, 3)) === rl
            @test fill_polygon!(rl, zeros(Float32, 3, 5), colors=rand(Float32, 4, 5)) === rl
            @test triangles!(rl, zeros(Float32, 3, 4), Int32[0 1; 1 2; 2 3]) === rl

            # scalar convenience forms
            @test point!(rl, [0.0, 1.0, 2.0], color=[1, 0, 0, 1], size=3) === rl
            @test line!(rl, [0, 0, 0], [1, 1, 1]) === rl
        end

        @testset "path and mesh" begin
            p = path_begin(rl)
            line_to!(p, zeros(Float32, 3, 4))
            close!(p)

            m = mesh_begin(rl)
            idx = vertex!(m, zeros(Float32, 3, 6))
            @test idx == 0:5
            triangle!(m, Int32[0 3; 1 4; 2 5])
            @test vertex!(m, [1.0, 2.0, 3.0]) == 6
            triangle!(m, 0, 1, 2)
        end

        @testset "validation" begin
            @test_throws ArgumentError point!(rl, zeros(Float32, 2, 5))      # wrong rows
            @test_throws ArgumentError point!(rl, zeros(Float32, 3, 5), colors=zeros(Float32, 4, 4))
            @test_throws BoundsError triangles!(rl, zeros(Float32, 3, 2), Int32[0; 9; 1;;])
        end

        @testset "camera" begin
            cam = win.camera
            cam.position = (1, 2, 3)
            @test all(isapprox.(cam.position, (1.0f0, 2.0f0, 3.0f0)))
            cam.distance = 5
            @test cam.distance ≈ 5.0f0
            @test size(calculate_transform(cam)) == (4, 4)
        end

        @testset "controllers" begin
            win.controller = SphericalController()
            c = win.controller
            c.mouse_sensitivity = 0.25
            @test c.mouse_sensitivity ≈ 0.25f0
        end
    end
end
