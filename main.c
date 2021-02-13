#define GLEW_STATIC
#include "./include/GL/glew.h"
#include "./include/GLFW/glfw3.h"
#include "graphics.h"
#include "util.h"
#include "file_util.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float *world_up = (float[]){0, 1, 0};
int window_width = 500;
int window_height = 500;

void main_loop(
    GLFWwindow *window,
    Entity *entities,
    int num_entities)
{
    Camera camera = create_default_camera();
    camera.position = (float[]){0, 0, 10};
    camera.forward = (float[]){0, 0, -1};
    camera.aspect = (float)window_width / (float)window_height;

    float projection_m[16] = {0};
    float view_m[16] = {0};
    float view_projection_m[16] = {0};
    project(camera.fov, camera.aspect, camera.nearClipPlane, camera.farClipPlane, projection_m);
    while (!glfwWindowShouldClose(window))
    {
        view(camera.position, camera.forward, world_up, view_m);
        multiply_4x4_matrices(projection_m, view_m, view_projection_m);
        draw(entities, num_entities, view_projection_m, window);
    }
}

int main(int argc, char *argv[])
{
    GLFWwindow *window = create_window(window_width, window_height);

    char *text = read_file("entities.txt");
    int num_entities = 0;
    Entity *entities = load_entities_from_text(text, &num_entities);
    main_loop(window, entities, num_entities);

    glfwTerminate();
    return 0;
}
