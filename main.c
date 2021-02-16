#define GLEW_STATIC
#include "./include/GL/glew.h"
#include "./include/GLFW/glfw3.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

float *world_up = (float[]){0, 1, 0};
int window_width = 512;
int window_height = 512;

int main(int argc, char *argv[])
{

    GLFWwindow *window = create_window(window_width, window_height);

    char *text = read_file("entities.txt");
    int num_entities = 0;
    Entity *entities = load_entities(text, &num_entities);

    Camera camera = create_default_camera();
    camera.position = (float[]){0, 2, 0};
    camera.forward = (float[]){0, 0, -1};
    camera.aspect = (float)window_width / (float)window_height;

    float projection_m[16] = {0};
    float view_m[16] = {0};
    float view_projection_m[16] = {0};
    create_projection(camera.fov, camera.aspect, camera.nearClipPlane, camera.farClipPlane, projection_m);
    while (!glfwWindowShouldClose(window))
    {
        // Move forward
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            camera.position[2] -= 0.1f;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            camera.position[2] += 0.1f;
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            camera.position[0] += 0.1f;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            camera.position[0] -= 0.1f;
        }

        create_view(camera.position, camera.forward, world_up, view_m);
        multiply_4x4_matrices(projection_m, view_m, view_projection_m);
        draw_entities(entities, num_entities, view_projection_m, window);
    }

    glfwTerminate();
    return 0;
}
