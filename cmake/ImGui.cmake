# fetch Dear ImGui
include(FetchContent)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.8
)
FetchContent_MakeAvailable(imgui)

# imgui ships no cmake target, compile its sources into the main library
target_sources(glviskit_lib PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# the ui headers include imgui, so consumers need its include dirs too
target_include_directories(glviskit_lib PUBLIC
    $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
    $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
)

# use the GLES3 backend path for GLES and web targets
if(GLVISKIT_GL_TYPE STREQUAL "GLAD_GLES2" OR
   GLVISKIT_GL_TYPE STREQUAL "NATIVE_GLES2" OR
   EMSCRIPTEN)
    target_compile_definitions(glviskit_lib PUBLIC IMGUI_IMPL_OPENGL_ES3=1)
endif()
