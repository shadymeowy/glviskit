#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "renderer.hpp"

float angle = 0.0f;

void resizeCallback(GLFWwindow *window, int width, int height) {
    glfwGetFramebufferSize(window, &width, &height);

    GladGLContext &gl = *(GladGLContext *)glfwGetWindowUserPointer(window);
    gl.Viewport(0, 0, width, height);
}

float randFloat() { return static_cast<float>(rand()) / RAND_MAX; }

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window =
        glfwCreateWindow(800, 600, "Hello, Modern OpenGL!", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GladGLContext gl;
    if (!gladLoadGLContext(&gl, (GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD Context" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << gl.GetString(GL_VERSION) << std::endl;
    glfwSetWindowUserPointer(window, &gl);
    glfwSetFramebufferSizeCallback(window, resizeCallback);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    gl.Viewport(0, 0, width, height);

    gl.Enable(GL_DEPTH_TEST);
    gl.Enable(GL_BLEND);
    gl.Disable(GL_CULL_FACE);
    gl.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl.BlendEquation(GL_FUNC_ADD);
    gl.Enable(GL_PROGRAM_POINT_SIZE);
    gl.Enable(GL_MULTISAMPLE);

    Renderer renderer{gl};
    auto render_buffer = renderer.CreateRenderBuffer();

    render_buffer->ClearInstances();
    for (int i = 1; i < 5; i++) {
        int s = (i % 2 == 0) ? 1 : -1;
        render_buffer->RenderInstance(
            glm::rotate(glm::mat4(1.0f), 0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(3.0f * (i - 0.5), 0, 0)));
        render_buffer->RenderInstance(
            glm::rotate(glm::mat4(1.0f), -0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(-3.0f * (i - 0.5), 0, 0)));
    }

    auto render_buffer_sine = renderer.CreateRenderBuffer();

    render_buffer->Color({1.0f, 1.0f, 1.0f, 1.0f});
    render_buffer->Size(3.0f);
    for (int i = 0; i < 10; i++) {
        render_buffer->Point({randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f});
    }

    double prev_time = glfwGetTime();
    int fps_counter = 0;
    int frame_index = 0;
    while (!glfwWindowShouldClose(window)) {
        double curr_time = glfwGetTime();

        fps_counter++;
        if (curr_time - prev_time >= 1.0) {
            std::cout << fps_counter / (curr_time - prev_time) << std::endl;
            prev_time = curr_time;
            fps_counter = 0;
        }

        frame_index++;
        angle += 0.001f;

        auto model = glm::mat4(1.0f);
        auto view = glm::lookAt(
            glm::vec3(15.0f * sin(angle), 5.0f, 15.0f * cos(angle)),
            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        auto projection = glm::perspective(
            glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        auto mvp = projection * view * model;

        gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl.ClearColor(0.0f, 0.f, 0.0f, 0.0f);

        for (int i = 0; i < 10; i++) {
            render_buffer->Size(randFloat() * 1.0f + 1.0f);
            render_buffer->Color(
                {randFloat(), randFloat(), randFloat(), randFloat()});
            render_buffer->Point({randFloat() * 2.0f - 1.0f,
                                  randFloat() * 2.0f - 1.0f,
                                  randFloat() * 2.0f - 1.0f});
        }

        if (frame_index % 10 == 0) {
            render_buffer->Color(
                {randFloat(), randFloat(), randFloat(), randFloat()});
            render_buffer->Size(randFloat() * 4.0f);
            render_buffer->Line(
                {randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f,
                 randFloat() * 2.0f - 1.0f},
                {randFloat() * 2.0f - 1.0f, randFloat() * 2.0f - 1.0f,
                 randFloat() * 2.0f - 1.0f});
        }

        render_buffer_sine->Restore();
        render_buffer_sine->Color({1.0f, 0.0f, 0.0f, 1.0f});
        render_buffer_sine->Size(4.0f);
        for (float x = -1.0f; x <= 1.0f; x += 0.001f) {
            float y = sinf(50.0f * x + 10 * static_cast<float>(curr_time));
            float z = cosf(50.0f * x + 10 * static_cast<float>(curr_time));

            render_buffer_sine->Color(
                {x * 0.5f + 0.5f, y * 0.5f + 0.5f, 0.5f, 1.0f});
            render_buffer_sine->LineTo({20.0f * x, 1.5 * y, 1.5 * z});
        }
        render_buffer_sine->LineEnd();

        renderer.SetMVP(mvp);
        renderer.SetScreenSize(glm::vec2(width, height));
        renderer.Render();

        // check for errors
        GLenum err;
        while ((err = gl.GetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
            return -1;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}