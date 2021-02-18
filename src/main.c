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
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera->position[2] -= 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera->position[2] += 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera->position[0] += 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera->position[0] -= 0.1f;
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
