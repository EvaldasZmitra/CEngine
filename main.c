#define GLEW_STATIC
#include "./include/GL/glew.h"
#include "./include/GLFW/glfw3.h"
#include "graphics.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>

float *world_up = (float[]){0, 1, 0};
int window_width = 500;
int window_height = 500;

void main_loop(GLFWwindow *window, unsigned int **shaded_meshes)
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
        draw(shaded_meshes, 1, view_projection_m, window);
    }
}

int main(int argc, char *argv[])
{
    GLFWwindow *window = create_window(window_width, window_height);

    unsigned int shaded_mesh[MAX_ATTRIBUTES] = {0};
    load_shaded_mesh(shaded_mesh, "shader-vert.glsl", "shader-frag.glsl");
    unsigned int *shaded_meshes[1] = {shaded_mesh};

    main_loop(window, shaded_meshes);

    glfwTerminate();
    return 0;
}
