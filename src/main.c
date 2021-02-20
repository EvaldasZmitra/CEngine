#define GLEW_STATIC
#include "../include/GL/glew.h"
#include "../include/GLFW/glfw3.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int window_width = 512;
int window_height = 512;

typedef enum StructTypes
{
    ENTITY
} StructTypes;

typedef struct SceneNode
{
    StructTypes type;
    unsigned int num_children;
    void **children;
    void *data;
} SceneNode;

// void load(void *data)
// {
//     StructTypes type;
//     switch (type)
//     {
//     case ENTITY:;
//         Entity *entity1 = (Entity *)(data);
//         break;

//     default:
//         break;
//     }
// }

void draw(GLFWwindow *window, Camera camera, SceneNode scene)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0.2, 0.2, 1);

    switch (scene.type)
    {
    default:
        draw_entity((Entity *)(scene.data), camera);
        break;
    }
    glfwSwapBuffers(window);
}

void handle_movement(GLFWwindow *window, Camera *camera)
{
    static double x_pos_old, y_pos_old;
    static double pitch, yaw;
    double x_pos, y_pos;
    double dx, dy;
    glfwGetCursorPos(window, &x_pos, &y_pos);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        dx = x_pos - x_pos_old;
        yaw += dx / 100.0;

        dy = y_pos - y_pos_old;
        pitch -= dy / 100.0;
        pitch = fmax(fmin(pitch, 1.5), -1.5);
    }

    x_pos_old = x_pos;
    y_pos_old = y_pos;

    float q[4] = {0};
    float m[16] = {0};
    euler_to_quaternion((float[]){pitch, yaw, 0}, q);
    quaterion_to_4x4_matrix(q, m);
    float fwd[4] = {0, 0, -1, 0};
    float new_fwd[4] = {0};
    multiply_vec3_by_4x4_matrix(m, fwd, new_fwd);
    //printf("%f %f %f %f\n", new_fwd[0], new_fwd[1], new_fwd[2], new_fwd[3]);
    //printf("%f %f %f\n", camera->forward[0], camera->forward[1], camera->forward[2]);
    //printf("%f %f\n", pitch, yaw);
    normalize_vector3(new_fwd);
    float right[3] = {0};
    cross_vector3(new_fwd, (float[]){0, 1, 0}, right);
    normalize_vector3(right);
    camera->forward[0] = new_fwd[0];
    camera->forward[1] = new_fwd[1];
    camera->forward[2] = new_fwd[2];

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->position[0] += new_fwd[0] * 0.1f;
        camera->position[1] += new_fwd[1] * 0.1f;
        camera->position[2] += new_fwd[2] * 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->position[0] -= new_fwd[0] * 0.1f;
        camera->position[1] -= new_fwd[1] * 0.1f;
        camera->position[2] -= new_fwd[2] * 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->position[0] += right[0] * 0.1f;
        camera->position[1] += right[1] * 0.1f;
        camera->position[2] += right[2] * 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->position[0] -= right[0] * 0.1f;
        camera->position[1] -= right[1] * 0.1f;
        camera->position[2] -= right[2] * 0.1f;
    }
}

void game_logic(GLFWwindow *window, Camera *camera)
{
    handle_movement(window, camera);
}

void game_loop(GLFWwindow *window, SceneNode scene)
{
    Camera camera = create_default_camera();
    camera.position = (float[]){0, 2, 0};
    camera.forward = (float[]){0, 0, -1};
    camera.aspect = (float)window_width / (float)window_height;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        game_logic(window, &camera);
        draw(window, camera, scene);
    }
}

SceneNode load_scene(const char *file)
{
    char *text = read_file(file);
    int num_entities = 0;
    Entity *entities = load_entities(text, &num_entities);
    save_entities("./resources/entities.txt", entities, num_entities);

    return (SceneNode){
        .type = ENTITY,
        .num_children = 0,
        .children = NULL,
        .data = (void *)&entities[0]};
}

int main(int argc, char *argv[])
{
    GLFWwindow *window = create_window(window_width, window_height);
    SceneNode scene = load_scene("./resources/entities.txt");

    game_loop(window, scene);

    glfwTerminate();
    return 0;
}
