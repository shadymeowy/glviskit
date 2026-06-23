using Pkg
Pkg.activate(; temp=true)
Pkg.add("ArtifactUtils")
using ArtifactUtils
using Base.BinaryPlatforms

length(ARGS) == 1 || error("usage: julia gen_artifacts.jl <release-tag>")
const TAG = ARGS[1]
const BASE = "https://github.com/shadymeowy/glviskit/releases/download/$TAG"
const TOML = joinpath(@__DIR__, "Artifacts.toml")

const TARGETS = [
    ("x86_64-linux-gnu", Platform("x86_64", "linux")),
    ("aarch64-linux-gnu", Platform("aarch64", "linux")),
    ("x86_64-linux-musl", Platform("x86_64", "linux"; libc="musl")),
    ("aarch64-linux-musl", Platform("aarch64", "linux"; libc="musl")),
    ("x86_64-apple-darwin", Platform("x86_64", "macos")),
    ("aarch64-apple-darwin", Platform("aarch64", "macos")),
    ("x86_64-w64-mingw32", Platform("x86_64", "windows")),
]

isfile(TOML) && rm(TOML)
for (triplet, platform) in TARGETS
    url = "$BASE/glviskit_c-$triplet.tar.gz"
    @info "adding artifact" triplet url
    add_artifact!(TOML, "glviskit_c", url; platform=platform, lazy=false, force=true)
end
@info "wrote" TOML
