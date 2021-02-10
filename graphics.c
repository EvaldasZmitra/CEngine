#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"

GLFWwindow *create_window(int width, int height)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        return 0;
    }
    GLFWwindow *window = glfwCreateWindow(width, height, "CEngine", NULL, NULL);
    if (!window)
    {
        return 0;
    }
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1); //vsync
    return window;
}

void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

char *read_file(char *file_name)
{
    FILE *infile = fopen(file_name, "r");
    char *buffer;
    long numbytes;
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    fseek(infile, 0L, SEEK_SET);
    buffer = (char *)calloc(numbytes, sizeof(char));
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);
    return buffer;
}

unsigned int create_shader(const char *code, unsigned int type)
{
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &code, NULL);
    glCompileShader(shaderId);
    return shaderId;
}

unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader)
{
    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    glDetachShader(program_id, fragment_shader);
    glDetachShader(program_id, vertex_shader);
    return program_id;
}

unsigned int create_shader_program_from_code(const char *vertex_code, const char *fragment_code)
{
    unsigned int vertexShader = create_shader(vertex_code, GL_VERTEX_SHADER);
    unsigned int fragmentShader = create_shader(fragment_code, GL_FRAGMENT_SHADER);
    unsigned int program_id = create_shader_program(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program_id;
}

void uniform_matrix_4x4(unsigned int shader, const float *matrix, char *name)
{
    unsigned int MatrixID = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(MatrixID, 1, GL_TRUE, matrix);
}

unsigned int create_vbo(const float *data, int size, int stride, int type)
{
    unsigned int vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(type);
    glVertexAttribPointer(type, stride, GL_FLOAT, GL_FALSE, 0, NULL);
    return vertexbuffer;
}

void create_gpu_mesh(const float *vertices, int num_vertices, unsigned int *out)
{
    glGenVertexArrays(1, &out[VAO]);
    glBindVertexArray(out[VAO]);
    out[VBO_VERTEX] = create_vbo(
        vertices,
        num_vertices * 3 * sizeof(float),
        3,
        VBO_VERTEX);
    out[NUM_VERTICES] = num_vertices;
}

void delete_gpu_mesh(unsigned int *mesh)
{
    for (int i = 0; i < MAX_VBOS; i++)
    {
        glDeleteBuffers(1, &mesh[i]);
    }
    glDeleteVertexArrays(1, &mesh[VAO]);
}

void draw_gpu_mesh(unsigned int *obj)
{
    glBindVertexArray(obj[VAO]);
    glDrawArrays(GL_TRIANGLES, 0, obj[NUM_VERTICES]);
}

Camera create_default_camera()
{
    return (Camera){
        .position = (float[]){0, 0, 0},
        .forward = (float[]){0, 0, -1},
        .aspect = 1,
        .nearClipPlane = 0.1f,
        .farClipPlane = 1000.0f,
        .fov = M_PI_2};
}

void load_shaded_mesh(unsigned int *out, char *vertex_shader_file, char *fragment_shader_file)
{
    GLuint shader_program = create_shader_program_from_code(
        read_file(vertex_shader_file),
        read_file(fragment_shader_file));
    GLfloat vertices[] = {
        -1.0f, -1.0f, -1.0f, // triangle 1 : begin
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f, -1.0f, // triangle 2 : begin
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 2 : end
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};
    out[SHADER] = shader_program;
    unsigned int num_vertices = sizeof(vertices) / sizeof(vertices[0]) / 3;
    create_gpu_mesh(vertices, num_vertices, out);
    free(vertices);
}

void draw(unsigned int **shaded_meshes, int count, float *view_projection_m, GLFWwindow *window)
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < count; i++)
    {
        glUseProgram(shaded_meshes[i][SHADER]);
        uniform_matrix_4x4(shaded_meshes[i][SHADER], view_projection_m, "VP");
        draw_gpu_mesh(shaded_meshes[i]);
    }
    glfwSwapBuffers(window);
}