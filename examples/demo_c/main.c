#include <glviskit/c_api.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHECK(call)                                                 \
    do {                                                            \
        int glv_status = (call);                                    \
        if (glv_status != GLV_OK) {                                 \
            fprintf(stderr, "%s failed: %s\n", #call, glv_error()); \
            return EXIT_FAILURE;                                    \
        }                                                           \
    } while (0)

#define CHECK_PTR(expr)                                             \
    do {                                                            \
        if ((expr) == NULL) {                                       \
            fprintf(stderr, "%s failed: %s\n", #expr, glv_error()); \
            return EXIT_FAILURE;                                    \
        }                                                           \
    } while (0)

static float rnf(void) { return (float)rand() / (float)RAND_MAX; }

int main(void) {
    srand((unsigned int)time(NULL));

    glv_window *window1 = NULL;
    glv_window *window2 = NULL;
    glv_render_list *render_list = NULL;
    glv_render_list *render_list_sine = NULL;
    glv_render_list *render_list_axes = NULL;
    glv_camera *camera = NULL;
    glv_camera *camera2 = NULL;

    window1 = glv_create_window("Window1", 800, 600);
    CHECK_PTR(window1);
    window2 = glv_create_window("Window2", 800, 600);
    CHECK_PTR(window2);

    render_list = glv_create_render_list();
    CHECK_PTR(render_list);
    CHECK(glv_window_add_render_list(window1, render_list));
    CHECK(glv_window_add_render_list(window2, render_list));

    CHECK(glv_render_list_clear_instances(render_list));
    for (int i = 1; i < 5; i++) {
        float s = (i % 2 == 0) ? 1.0F : -1.0F;
        float x = 3.0F * ((float)i - 0.5F);
        CHECK(glv_render_list_add_instance(render_list, x, 0.0F, 0.0F, 0.5F * s,
                                           0.0F, 0.0F, 1.0F, 1.0F, 1.0F));
        CHECK(glv_render_list_add_instance(render_list, -x, 0.0F, 0.0F,
                                           -0.5F * s, 0.0F, 0.0F, 1.0F, 1.0F,
                                           1.0F));
    }

    render_list_sine = glv_create_render_list();
    CHECK_PTR(render_list_sine);
    CHECK(glv_window_add_render_list(window1, render_list_sine));
    CHECK(glv_window_add_render_list(window2, render_list_sine));

    render_list_axes = glv_create_render_list();
    CHECK_PTR(render_list_axes);
    CHECK(glv_window_add_render_list(window1, render_list_axes));
    CHECK(glv_window_add_render_list(window2, render_list_axes));

    CHECK(glv_render_list_size(render_list_axes, 5.0F));
    CHECK(glv_render_list_color(render_list_axes, 1.0F, 0.0F, 0.0F, 1.0F));
    CHECK(glv_render_list_line(render_list_axes, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F,
                               0.0F));
    CHECK(glv_render_list_color(render_list_axes, 0.0F, 1.0F, 0.0F, 1.0F));
    CHECK(glv_render_list_line(render_list_axes, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F,
                               0.0F));
    CHECK(glv_render_list_color(render_list_axes, 0.0F, 0.0F, 1.0F, 1.0F));
    CHECK(glv_render_list_line(render_list_axes, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F,
                               1.0F));

    CHECK(glv_render_list_color(render_list, 1.0F, 1.0F, 1.0F, 1.0F));
    CHECK(glv_render_list_size(render_list, 10.0F));
    for (int i = 0; i < 10; i++) {
        CHECK(glv_render_list_circle(render_list, (rnf() * 2.0F) - 1.0F,
                                     (rnf() * 2.0F) - 1.0F,
                                     (rnf() * 2.0F) - 1.0F));
    }

    camera = glv_window_get_camera(window1);
    CHECK_PTR(camera);
    CHECK(glv_camera_perspective_fov(camera, 60.0F, 60.0F, 0.1F, 100.0F));
    CHECK(glv_camera_set_position(camera, 0.0F, 0.0F, 0.0F));
    CHECK(glv_camera_set_rotation(camera, 0.0F, 0.0F, 0.0F));
    CHECK(glv_camera_set_preserve_aspect_ratio(camera, 1));
    CHECK(glv_camera_set_distance(camera, 15.0F));

    camera2 = glv_window_get_camera(window2);
    CHECK_PTR(camera2);
    CHECK(glv_camera_perspective_fov(camera2, 60.0F, 60.0F, 0.1F, 100.0F));
    CHECK(glv_camera_set_position(camera2, 0.0F, 0.0F, 0.0F));
    CHECK(glv_camera_set_rotation(camera2, 0.0F, 0.0F, 0.0F));
    CHECK(glv_camera_set_preserve_aspect_ratio(camera2, 1));
    CHECK(glv_camera_set_distance(camera2, 15.0F));

    int frame_index = 0;
    int running = 1;
    while (running) {
        running = glv_loop();
        if (running == GLV_ERROR) {
            fprintf(stderr, "glv_loop failed: %s\n", glv_error());
            return EXIT_FAILURE;
        }
        if (running == 0) {
            break;
        }

        float curr_time = glv_get_time_seconds();
        frame_index++;

        for (int i = 0; i < 10; i++) {
            CHECK(glv_render_list_size(render_list, rnf() + 1.0F));
            CHECK(
                glv_render_list_color(render_list, rnf(), rnf(), rnf(), rnf()));
            CHECK(glv_render_list_point(render_list, (rnf() * 2.0F) - 1.0F,
                                        (rnf() * 2.0F) - 1.0F,
                                        (rnf() * 2.0F) - 1.0F));
        }

        if (frame_index % 10 == 0) {
            CHECK(
                glv_render_list_color(render_list, rnf(), rnf(), rnf(), rnf()));
            CHECK(glv_render_list_size(render_list, rnf() * 4.0F));
            CHECK(glv_render_list_line(
                render_list, (rnf() * 2.0F) - 1.0F, (rnf() * 2.0F) - 1.0F,
                (rnf() * 2.0F) - 1.0F, (rnf() * 2.0F) - 1.0F,
                (rnf() * 2.0F) - 1.0F, (rnf() * 2.0F) - 1.0F));
        }

        CHECK(glv_render_list_restore(render_list_sine));

        glv_path *path = glv_render_list_path_begin(render_list_sine);
        CHECK_PTR(path);
        CHECK(glv_path_size(path, 4.0F));
        for (int ix = -1000; ix <= 1000; ix++) {
            float x = (float)ix / 1000.0F;
            float y = sinf((50.0F * x) + (10.0F * curr_time));
            float z = cosf((50.0F * x) + (10.0F * curr_time));

            CHECK(glv_path_color(path, (x * 0.5F) + 0.5F, (y * 0.5F) + 0.5F,
                                 0.5F, 1.0F));
            CHECK(glv_path_line_to(path, 20.0F * x, 1.5F * y, 1.5F * z));
        }
        CHECK(glv_path_line_end(path));
        glv_path_destroy(path);
    }

    glv_camera_destroy(camera);
    glv_camera_destroy(camera2);
    glv_render_list_destroy(render_list_axes);
    glv_render_list_destroy(render_list_sine);
    glv_render_list_destroy(render_list);
    glv_window_destroy(window2);
    glv_window_destroy(window1);

    return EXIT_SUCCESS;
}
