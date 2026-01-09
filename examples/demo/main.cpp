#include <cmath>
#include <glviskit/glviskit.hpp>
#include <random>

auto main() -> int {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0F, 1.0F);
    auto rnf = [&]() -> float { return dis(gen); };

    // create a window 1
    auto window1 = glviskit::CreateWindow("Window1", 800, 600);

    // create a window 2
    auto window2 = glviskit::CreateWindow("Window2", 800, 600);
    auto render_list = glviskit::CreateRenderList();
    window1->AddRenderList(render_list);
    window2->AddRenderList(render_list);

    render_list->ClearInstances();
    for (int i = 1; i < 5; i++) {
        const double s = (i % 2 == 0) ? 1 : -1;
        render_list->AddInstance({3.0F * (i - 0.5), 0, 0}, {0.5F * s, 0, 0});
        render_list->AddInstance({-3.0F * (i - 0.5), 0, 0},
                                   {-0.5F * s, 0, 0});
    }

    auto render_list_sine = glviskit::CreateRenderList();
    window1->AddRenderList(render_list_sine);
    window2->AddRenderList(render_list_sine);

    auto render_list_axes = glviskit::CreateRenderList();
    window1->AddRenderList(render_list_axes);
    window2->AddRenderList(render_list_axes);

    render_list_axes->Size(5.0F);
    render_list_axes->Color({1.0F, 0.0F, 0.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F});
    render_list_axes->Color({0.0F, 1.0F, 0.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    render_list_axes->Color({0.0F, 0.0F, 1.0F, 1.0F});
    render_list_axes->Line({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F});

    render_list->Color({1.0F, 1.0F, 1.0F, 1.0F});
    render_list->Size(10.0F);
    for (int i = 0; i < 10; i++) {
        render_list->Circle({(rnf() * 2.0F) - 1.0F, (rnf() * 2.0F) - 1.0F,
                               (rnf() * 2.0F) - 1.0F});
    }

    auto camera = window1->GetCamera();
    camera->PerspectiveFov(60.0F, 60.0F);
    camera->SetPosition({0.0F, 0.0F, 0.0F});
    camera->SetRotation({0.0F, 0.0F, 0.0F});
    camera->SetPreserveAspectRatio(true);
    camera->SetDistance(15.0F);

    auto camera2 = window2->GetCamera();
    camera2->PerspectiveFov(60.0F, 60.0F);
    camera2->SetPosition({0.0F, 0.0F, 0.0F});
    camera2->SetRotation({0.0F, 0.0F, 0.0F});
    camera2->SetPreserveAspectRatio(true);
    camera2->SetDistance(15.0F);

    float angle = 0.0F;
    int frame_index = 0;

    while (glviskit::Loop()) {
        const float curr_time = glviskit::GetTimeSeconds();

        frame_index++;
        angle += 0.005F;
        camera->SetRotation({-0.5F, angle, 0.0F});
        camera2->SetRotation({-0.5F, -angle, 0.0F});

        for (int i = 0; i < 10; i++) {
            render_list->Size((rnf() * 1.0F) + 1.0F);
            render_list->Color({
                rnf(),
                rnf(),
                rnf(),
                rnf(),
            });
            render_list->Point({
                (rnf() * 2.0F) - 1.0F,
                (rnf() * 2.0F) - 1.0F,
                (rnf() * 2.0F) - 1.0F,
            });
        }

        if (frame_index % 10 == 0) {
            render_list->Color({rnf(), rnf(), rnf(), rnf()});
            render_list->Size(rnf() * 4.0F);
            render_list->Line(
                {
                    (rnf() * 2.0F) - 1.0F,
                    (rnf() * 2.0F) - 1.0F,
                    (rnf() * 2.0F) - 1.0F,
                },
                {
                    (rnf() * 2.0F) - 1.0F,
                    (rnf() * 2.0F) - 1.0F,
                    (rnf() * 2.0F) - 1.0F,
                });
        }

        render_list_sine->Restore();
        auto path = render_list_sine->PathBegin();
        path->Size(4.0F);
        for (int ix = -1000; ix <= 1000; ix++) {
            const float x = static_cast<float>(ix) / 1000.0F;
            const float y = sinf((50.0F * x) + (10 * curr_time));
            const float z = cosf((50.0F * x) + (10 * curr_time));

            path->Color({(x * 0.5F) + 0.5F, (y * 0.5F) + 0.5F, 0.5F, 1.0F});
            path->LineTo({20.0F * x, 1.5 * y, 1.5 * z});
        }
        path->LineEnd();
    }

    return 0;
}