#include <cmath>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <iostream>

#include "window.hpp"

float randFloat() { return static_cast<float>(rand()) / RAND_MAX; }

int main() {
    GLFWManager manager{};

    auto window = manager.CreateWindow(800, 600, "First Window");
    auto window2 = manager.CreateWindow(400, 300, "Second Window");

    auto render_buffer = manager.CreateRenderBuffer();
    window->AddRenderBuffer(render_buffer);
    window2->AddRenderBuffer(render_buffer);

    render_buffer->ClearInstances();
    for (int i = 1; i < 5; i++) {
        int s = (i % 2 == 0) ? 1 : -1;
        render_buffer->AddInstance(
            glm::rotate(glm::mat4(1.0f), 0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(3.0f * (i - 0.5), 0, 0)));
        render_buffer->AddInstance(
            glm::rotate(glm::mat4(1.0f), -0.5f * s, glm::vec3(1, 0, 0)) *
            glm::translate(glm::mat4(1.0f),
                           glm::vec3(-3.0f * (i - 0.5), 0, 0)));
    }
    auto render_buffer_sine = manager.CreateRenderBuffer();
    window->AddRenderBuffer(render_buffer_sine);
    window2->AddRenderBuffer(render_buffer_sine);

    auto render_buffer_axes = manager.CreateRenderBuffer();
    window->AddRenderBuffer(render_buffer_axes);
    window2->AddRenderBuffer(render_buffer_axes);

    render_buffer_axes->Size(5.0f);
    render_buffer_axes->Color({1.0f, 0.0f, 0.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
    render_buffer_axes->Color({0.0f, 1.0f, 0.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    render_buffer_axes->Color({0.0f, 0.0f, 1.0f, 1.0f});
    render_buffer_axes->Line({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});

    render_buffer->Color({1.0f, 1.0f, 1.0f, 1.0f});
    render_buffer->Size(3.0f);
    for (int i = 0; i < 10; i++) {
        render_buffer->Point({randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f,
                              randFloat() * 2.0f - 1.0f});
    }

    auto &camera = window->GetCamera();
    camera.SetPerspectiveFov(glm::radians(60.0f), glm::radians(60.0f));
    camera.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.SetRotation(glm::vec3(0.0, 0.0f, 0.0f));
    camera.SetPreserveAspectRatio(true);
    camera.SetDistance(15.0f);

    auto &camera2 = window2->GetCamera();
    camera2.SetPerspectiveFov(glm::radians(60.0f), glm::radians(60.0f));
    camera2.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera2.SetRotation(glm::vec3(0.0, 0.0f, 0.0f));
    camera2.SetPreserveAspectRatio(true);
    camera2.SetDistance(15.0f);

    float angle = 0.0f;
    int fps_counter = 0;
    int frame_index = 0;

    double prev_time = glfwGetTime();
    while (manager.LoopEvents()) {
        double curr_time = glfwGetTime();

        fps_counter++;
        if (curr_time - prev_time >= 1.0) {
            std::cout << fps_counter / (curr_time - prev_time) << std::endl;
            prev_time = curr_time;
            fps_counter = 0;
        }

        frame_index++;
        angle += 0.005f;
        camera.SetRotation({-0.5f, angle, 0.0f});
        camera2.SetRotation({-0.5f, -angle, 0.0f});

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

        window->Render();
        window2->Render();
    }
    return 0;
}