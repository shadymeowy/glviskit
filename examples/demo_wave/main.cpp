#include <array>
#include <cmath>
#include <cstddef>
#include <glviskit/glviskit.hpp>
#include <random>
#include <string>

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0F, 1.0F);
    auto rnf = [&]() -> float { return dis(gen); };

    // create a window 1
    auto window1 = glviskit::CreateWindow("Window1", 800, 600);

    // create a window 2
    auto render_list = glviskit::CreateRenderList();
    window1->AddRenderList(render_list);

    auto render_list_axes = glviskit::CreateRenderList();
    window1->AddRenderList(render_list_axes);

    render_list_axes->Size(5.0F);
    render_list_axes->Color({1.0F, 0.0F, 0.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F});
    render_list_axes->Color({0.0F, 1.0F, 0.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    render_list_axes->Color({0.0F, 0.0F, 1.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F});

    auto camera = window1->GetCamera();
    camera->PerspectiveFov(60.0F, 60.0F);
    camera->SetPosition({0.0F, 0.0F, 0.0F});
    camera->SetRotation({0.0F, 0.0F, 0.0F});
    camera->SetPreserveAspectRatio(true);
    camera->SetDistance(10.0F);

    int frame_index = 0;

    // ui-controlled parameters
    bool animate = true;
    float rot_speed = 0.005F;
    float amplitude = 1.0F;
    float frequency = 1.0F;
    int line_size = 2;
    int wave_mode = 0;
    std::array<float, 3> line_color = {0.2F, 0.7F, 1.0F};
    std::array<float, 4> bg_color = {0.0F, 0.0F, 0.0F, 1.0F};
    std::vector<float> history(100, 0.0F);

    size_t n = 100;
    auto x_stop = -10.0F;
    auto x_start = 10.0F;
    auto y_stop = -10.0F;
    auto y_start = 10.0F;

    std::vector<float> xs;
    xs.reserve(n);
    std::vector<float> ys;
    ys.reserve(n);

    for (int i = 0; i < n; i++) {
        xs.push_back(x_start +
                     ((x_stop - x_start) *
                      (static_cast<float>(i) / static_cast<float>(n - 1))));
        ys.push_back(y_start +
                     ((y_stop - y_start) *
                      (static_cast<float>(i) / static_cast<float>(n - 1))));
    }

    render_list->Text("glviskit\nwave demo", {0.0F, 20.0F, 0.0F}, {0.0F, 60.0F},
                      {1.0F, 1.0F, 1.0F, 1.0F}, 20.0F,
                      glviskit::TextAlign::Center, /*overlay=*/1);
    render_list->Marker(glviskit::MarkerType::Diamond, {0.0F, 20.0F, 0.0F},
                        {0.0F, 0.0F}, {1.0F, 0.8F, 0.2F, 1.0F}, 32.0F);
    render_list->Save();

    std::vector<float> zs;
    zs.reserve(n * n);

    while (glviskit::Loop()) {
        const float curr_time = glviskit::GetTimeSeconds();
        frame_index++;

        // build the control panel
        auto &ui = window1->Ui();
        ui.Begin("Controls");
        ui.Text("surface plot demo");
        ui.Text(("frame: " + std::to_string(frame_index)).c_str());
        ui.Separator();
        ui.Checkbox("animate", animate);
        ui.SliderFloat("rotation speed", rot_speed, 0.0F, 0.05F);
        ui.SliderFloat("amplitude", amplitude, 0.0F, 3.0F);
        ui.SliderFloat("frequency", frequency, 0.1F, 3.0F);
        ui.SliderInt("line size", line_size, 1, 8);
        ui.Combo("wave mode", wave_mode, "ripple|sine x|sine y");
        ui.ColorEdit3("line color", line_color.data());
        ui.ColorEdit4("background", bg_color.data());
        history.erase(history.begin());
        history.push_back(amplitude * std::sin(frequency * curr_time));
        ui.PlotLines("signal", history.data(),
                     static_cast<int>(history.size()));
        ui.Separator();
        if (ui.Button("reset view")) {
            camera->SetRotation({-0.5F, 0.0F, 0.0F});
        }
        ui.SameLine();
        if (ui.Button("stop")) {
            animate = false;
        }
        ui.End();

        // animate by nudging the camera's yaw, composing with the controller
        if (animate) {
            auto rot = camera->GetRotation();
            rot.y += rot_speed;
            camera->SetRotation(rot);
        }
        window1->SetBackgroundColor(
            {bg_color[0], bg_color[1], bg_color[2], bg_color[3]});

        render_list->Restore();

        render_list->Color({line_color[0], line_color[1], line_color[2], 1.0F});
        render_list->Size(static_cast<float>(line_size));

        // compute zs
        zs.clear();
        for (auto x : xs) {
            for (auto y : ys) {
                float z = 0.0F;
                if (wave_mode == 1) {
                    z = amplitude * std::sin(frequency * (x - curr_time));
                } else if (wave_mode == 2) {
                    z = amplitude * std::cos(frequency * (y - curr_time));
                } else {
                    z = amplitude * std::sin(frequency * (x - curr_time)) *
                        std::cos(frequency * (y - curr_time));
                }
                zs.push_back(z);
            }
        }

        // horizontal lines
        auto path = render_list->PathBegin();
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                path->LineTo({xs[j], zs[(i * n) + j], ys[i]});
            }
            path->LineEnd();
        }

        // vertical line
        for (size_t j = 0; j < n; j++) {
            for (size_t i = 0; i < n; i++) {
                path->LineTo({xs[j], zs[(i * n) + j], ys[i]});
            }
            path->LineEnd();
        }
    }
    return 0;
}