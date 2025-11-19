#pragma once

#include <memory>
#include <set>
#include <stdexcept>

#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

// RAII wrapper for GLFW window
class GLFWWindow {
   public:
    GLFWWindow(GLFWwindow *handle) : ptr{handle} {}
    ~GLFWWindow() {
        if (ptr) {
            glfwDestroyWindow(ptr);
            ptr = nullptr;
        }
    }
    // uncopyable but movable
    GLFWWindow(const GLFWWindow &) = delete;
    GLFWWindow &operator=(const GLFWWindow &) = delete;
    GLFWWindow(GLFWWindow &&other) noexcept : ptr{other.ptr} {
        other.ptr = nullptr;
    }
    GLFWWindow &operator=(GLFWWindow &&other) noexcept {
        if (this != &other) {
            if (ptr) {
                glfwDestroyWindow(ptr);
            }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    GLFWwindow *ptr;
};

class Window {
   public:
    Window(GLFWwindow *window, GLuint window_id, GladGLContext &gl)
        : window_(window), gl(gl), renderer(gl), window_id_(window_id) {}

    void Render() {
        // make context current
        // renderer expects the context to be current
        glfwMakeContextCurrent(window_.ptr);

        // update screen size
        int width, height;
        glfwGetFramebufferSize(window_.ptr, &width, &height);

        // do rendering
        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.Render(window_id_, width, height);
        glfwSwapBuffers(window_.ptr);

        // check for errors
        GLenum err;
        while ((err = gl.GetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
            throw std::runtime_error("OpenGL error during rendering");
        }
    }

    Camera &GetCamera() { return renderer.GetCamera(); }

    void AddRenderBuffer(std::shared_ptr<RenderBuffer> render_buffer) {
        renderer.AddRenderBuffer(render_buffer);
    }

    GLFWwindow *GetHandle() { return window_.ptr; }

   private:
    GladGLContext &gl;
    // window handle
    GLFWWindow window_;
    // renderer for this window
    Renderer renderer;
    // window id for bookkeeping
    GLuint window_id_;

    friend class GLFWManager;
};

class GLFWManager {
   public:
    GLFWManager() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
    }

    ~GLFWManager() { glfwTerminate(); }
    // uncopyable and unmovable, we holding opengl context here
    GLFWManager(const GLFWManager &) = delete;
    GLFWManager &operator=(const GLFWManager &) = delete;
    GLFWManager(GLFWManager &&) = delete;
    GLFWManager &operator=(GLFWManager &&) = delete;

    std::shared_ptr<Window> CreateWindow(int width, int height,
                                         const char *title) {
        // check if we have existing windows to share context
        GLFWwindow *shared_handle = nullptr;
        if (!windows_.empty()) {
            shared_handle = windows_.begin()->get()->GetHandle();
        }

        // set window hints for OpenGL context
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // enable multisampling
        glfwWindowHint(GLFW_SAMPLES, 4);

        // create window
        GLFWwindow *window_handle =
            glfwCreateWindow(width, height, title, NULL, shared_handle);
        // check for errors at creation
        if (window_handle == NULL) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        // init opengl context if this is the first window
        if (!gl_initialized_) {
            glfwMakeContextCurrent(window_handle);
            if (!gladLoadGLContext(&gl, (GLADloadfunc)glfwGetProcAddress)) {
                throw std::runtime_error("Failed to initialize GLAD Context");
            }
            gl_initialized_ = true;
        }

        // store window handle in shared ptr
        auto window =
            std::make_shared<Window>(window_handle, next_window_id_++, gl);
        windows_.insert(window);
        return window;
    }

    std::shared_ptr<RenderBuffer> CreateRenderBuffer() {
        return std::make_shared<RenderBuffer>(gl);
    }

    bool LoopEvents() {
        glfwPollEvents();
        return !windows_.empty();
    }

   private:
    GladGLContext gl;
    bool gl_initialized_{false};
    std::set<std::shared_ptr<Window>> windows_;
    GLuint next_window_id_{0};
};