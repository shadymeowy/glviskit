if (NOT EMSCRIPTEN)
    # fetch SDL3
    include(FetchContent)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.28
    )
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL3)

    # link SDL3 to the main library
    target_link_libraries(glviskit_lib PUBLIC
        SDL3::SDL3-static
    )
else()
    # emscripten has its own SDL3
    target_compile_options(glviskit_lib PUBLIC
        "-sUSE_SDL=3"
    )
    target_link_options(glviskit_lib PUBLIC
        "-sUSE_SDL=3"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
    )
    
    # link to web.cpp
    target_sources(glviskit_lib PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/web.cpp")
endif()