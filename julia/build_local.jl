using Libdl, Downloads

const PKG_ROOT = @__DIR__
const BUILD_DIR = joinpath(PKG_ROOT, "build")
const DEPS_FILE = joinpath(PKG_ROOT, "deps", "deps.jl")
const CMAKE = get(ENV, "CMAKE", "cmake")
const REPO_URL = "https://github.com/shadymeowy/glviskit"

is_source(dir) = isfile(joinpath(dir, "CMakeLists.txt")) && isdir(joinpath(dir, "src"))

function package_version()
    for line in eachline(joinpath(PKG_ROOT, "Project.toml"))
        m = match(r"^\s*version\s*=\s*\"([^\"]+)\"", line)
        m === nothing || return m.captures[1]
    end
    error("could not read version from Project.toml")
end

function download_source()
    tag = "v" * package_version()
    dest = joinpath(BUILD_DIR, "glviskit-$tag")
    is_source(dest) && return dest
    mkpath(BUILD_DIR)
    tarball = joinpath(BUILD_DIR, "$tag.tar.gz")
    url = "$REPO_URL/archive/refs/tags/$tag.tar.gz"
    @info "Downloading glviskit sources" url
    Downloads.download(url, tarball)
    mkpath(dest)
    run(`tar -xzf $tarball -C $dest --strip-components=1`)
    is_source(dest) || error("downloaded archive did not contain glviskit sources")
    return dest
end

function source_root()
    src = get(ENV, "GLVISKIT_SOURCE", "")
    isempty(src) || return src
    parent = normpath(joinpath(PKG_ROOT, ".."))
    is_source(parent) && return parent
    return download_source()
end

function find_library(dir)
    for (root, _, files) in walkdir(dir), f in files
        if occursin("glviskit_c", f) && endswith(f, "." * Libdl.dlext)
            return joinpath(root, f)
        end
    end
    return nothing
end

src = source_root()
mkpath(BUILD_DIR)
run(`$CMAKE -S $src -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release -DGLVISKIT_STATIC_RUNTIME=ON`)
run(`$CMAKE --build $BUILD_DIR --target glviskit_c --config Release`)

lib = find_library(BUILD_DIR)
lib === nothing && error("CMake build produced no glviskit_c library under $BUILD_DIR")

mkpath(dirname(DEPS_FILE))
open(io -> println(io, "const libglviskit_c = ", repr(lib)), DEPS_FILE, "w")
@info "GLViskit: built glviskit_c" lib source = src
