#include "rendering.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void invert_transform(Transform transform)
{
    negate_vec3(transform.position);
    negate_vec3(transform.rotation);
    invert_vec3(transform.scale);
}

char *read_file_stream(FILE *infile)
{
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

char *read_file(const char *file_name)
{
    FILE *infile = fopen(file_name, "r");
    return read_file_stream(infile);
}

char *read_file_binary(const char *file_name)
{
    FILE *infile = fopen(file_name, "rb");
    return read_file_stream(infile);
}

unsigned int load_dds(const char *path)
{
    unsigned int width, height, mip_map_count, block_size, format;
    unsigned char *data = (unsigned char *)read_file_binary(path);
    unsigned char *header = (unsigned char *)&(data[0]);
    unsigned char *buffer = (unsigned char *)&(data[128]);

    GLuint tid = 0;
    height = *(unsigned int *)&(header[12]);
    width = *(unsigned int *)&(header[16]);
    mip_map_count = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

    if (header[84] == 'D')
    {
        switch (header[87])
        {
        case '1': // DXT1
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            block_size = 8;
            break;
        case '3': // DXT3
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            block_size = 16;
            break;
        case '5': // DXT5
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            block_size = 16;
            break;
        default:
            break;
        }
    }
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1); // opengl likes array length of mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int offset = 0;
    unsigned int size = 0;
    unsigned int w = width;
    unsigned int h = height;
    for (unsigned int i = 0; i < mip_map_count; i++)
    {
        if (w == 0 || h == 0)
        {
            mip_map_count--;
            continue;
        }
        size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;
        glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_map_count - 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(data);
    return tid;
}

GLFWwindow *create_window(int width, int height)
{
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
    glDepthFunc(GL_LESS);
    glfwSwapInterval(1); //vsync
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    return window;
}

unsigned int create_shader(const char *code, unsigned int type)
{
    unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &code, NULL);
    glCompileShader(shaderId);

    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *VertexShaderErrorMessage = malloc(sizeof(char) * InfoLogLength);
        glGetShaderInfoLog(shaderId, InfoLogLength, NULL, VertexShaderErrorMessage);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }
    return shaderId;
}

unsigned int create_shader_program(unsigned int fragment_shader, unsigned int vertex_shader)
{
    unsigned int program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);

    glLinkProgram(program_id);
    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetProgramiv(program_id, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *VertexShaderErrorMessage = malloc(sizeof(char) * InfoLogLength);
        glGetProgramInfoLog(program_id, InfoLogLength, NULL, VertexShaderErrorMessage);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

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

void update_camera_uniforms(Camera camera)
{
}

unsigned int create_vbo(const void *data, int size, int stride, int type)
{
    unsigned int vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(type);
    glVertexAttribPointer(type, stride, GL_FLOAT, GL_FALSE, 0, NULL);
    return vertexbuffer;
}

void free_mesh_gpu(Mesh *mesh)
{
    glDeleteBuffers(1, &mesh->vbo_indices);
    glDeleteBuffers(1, &mesh->vbo_normal);
    glDeleteBuffers(1, &mesh->vbo_uv);
    glDeleteBuffers(1, &mesh->vbo_vertices);
    glDeleteVertexArrays(1, &mesh->vao);
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

void load_mesh_to_gpu(Mesh *mesh)
{
    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);
    mesh->vbo_vertices = create_vbo(
        mesh->vertices,
        mesh->num_vertices * 3 * sizeof(float),
        3,
        0);

    mesh->vbo_normal = create_vbo(
        mesh->normals,
        mesh->num_vertices * 3 * sizeof(float),
        3,
        1);

    mesh->vbo_uv = create_vbo(
        mesh->uvs,
        mesh->num_vertices * 2 * sizeof(float),
        2,
        2);

    glGenBuffers(1, &mesh->vbo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo_indices);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh->num_indices * 3 * sizeof(unsigned int),
        mesh->indices,
        GL_STATIC_DRAW);
    mesh->num_vertices = mesh->num_vertices;
    mesh->num_indices = mesh->num_indices * 3;
}

void draw_entity(Entity *entity, Camera camera)
{
    float projection_m[16] = {0};
    float view_m[16] = {0};
    float view_projection_m[16] = {0};
    create_projection(camera.fov, camera.aspect, camera.nearClipPlane, camera.farClipPlane, projection_m);
    create_view(camera.position, camera.forward, (float[]){0, 1, 0}, view_m);
    multiply_4x4_matrices(projection_m, view_m, view_projection_m);

    float mvp[16] = {0};
    float m[16] = {0};
    glUseProgram(entity->shader);
    create_mvp(
        entity->transform.position,
        entity->transform.rotation,
        entity->transform.scale,
        view_projection_m,
        mvp);
    create_transform(entity->transform.position,
                     entity->transform.rotation,
                     entity->transform.scale, m);
    uniform_matrix_4x4(entity->shader, mvp, "VP");
    uniform_matrix_4x4(entity->shader, m, "M");
    GLuint TextureID = glGetUniformLocation(entity->shader, "myTextureSampler");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, entity->texture);
    glUniform1i(TextureID, 0);
    glBindVertexArray(entity->mesh->vao);
    glDrawElements(
        GL_TRIANGLES,
        entity->mesh->num_indices,
        GL_UNSIGNED_INT,
        (void *)0);
}

void write_vec3(FILE *fp, float *vec3)
{
    fprintf(fp, "%f %f %f\n",
            vec3[0],
            vec3[1],
            vec3[2]);
}

void save_entities(const char *file_name, Entity *entities, unsigned int num_entities)
{
    FILE *fp = fopen(file_name, "w");
    fprintf(fp, "%d\n", num_entities);
    for (int i = 0; i < num_entities; i++)
    {
        fprintf(fp, "_entity_\n");
        fprintf(fp, "ASD\n");
        write_vec3(fp, entities[i].transform.position);
        write_vec3(fp, entities[i].transform.rotation);
        write_vec3(fp, entities[i].transform.scale);
        fprintf(fp, "%s\n", entities[i].mesh->file_name);
        fprintf(fp, "%s\n", entities[i].texture_name);
        fprintf(fp, "%s\n", entities[i].vert_name);
        fprintf(fp, "%s\n", entities[i].frag_name);
    }
    fclose(fp);
}

Entity *load_entities(char *text, int *num_entities)
{
    char *token = strtok(text, "\n");
    int n = atoi(token);
    Entity *entities = malloc(sizeof(Entity) * n);
    (*num_entities) = n;

    int c = 0;
    while (token != NULL)
    {
        if (strcmp(token, "_entity_") == 0)
        {
            Entity entity;
            char *name = strtok(NULL, "\n");

            float *pos = malloc(3 * sizeof(float));
            pos[0] = atof(strtok(NULL, " "));
            pos[1] = atof(strtok(NULL, " "));
            pos[2] = atof(strtok(NULL, "\n"));

            float *rot = malloc(3 * sizeof(float));
            rot[0] = atof(strtok(NULL, " "));
            rot[1] = atof(strtok(NULL, " "));
            rot[2] = atof(strtok(NULL, "\n"));

            float *scale = malloc(3 * sizeof(float));
            scale[0] = atof(strtok(NULL, " "));
            scale[1] = atof(strtok(NULL, " "));
            scale[2] = atof(strtok(NULL, "\n"));

            char *mesh_file = strtok(NULL, "\n");
            char *texture_file = strtok(NULL, "\n");
            char *vshader = strtok(NULL, "\n");
            char *fshader = strtok(NULL, "\n");

            char *buffer = read_file_binary(mesh_file);
            Mesh *mesh = malloc(sizeof(Mesh));
            mesh->file_name = mesh_file;
            unsigned int offset = 0;
            mesh->num_vertices = *(unsigned int *)&(buffer[offset]);
            offset += 4;
            mesh->num_indices = *(unsigned int *)&(buffer[offset]);
            offset += 4;
            mesh->vertices = (float *)&(buffer[offset]);
            offset += mesh->num_vertices * 3 * 4;
            mesh->normals = (float *)&(buffer[offset]);
            offset += mesh->num_vertices * 3 * 4;
            mesh->uvs = (float *)&(buffer[offset]);
            offset += mesh->num_vertices * 2 * 4;
            mesh->indices = (unsigned int *)&(buffer[offset]);
            entity.mesh = mesh;

            char *v_shader_code = read_file(vshader);
            char *f_shader_code = read_file(fshader);
            load_mesh_to_gpu(entity.mesh);
            entities[c] = entity;
            entities[c].shader = create_shader_program_from_code(
                v_shader_code,
                f_shader_code);
            free(v_shader_code);
            free(f_shader_code);
            entities[c].texture = load_dds(texture_file);
            entities[c].name = name;
            entities[c].transform.position = pos;
            entities[c].transform.rotation = rot;
            entities[c].transform.scale = scale;

            entities[c].texture_name = texture_file;
            entities[c].vert_name = vshader;
            entities[c].frag_name = fshader;

            //free(buffer);
            c++;
        }
        token = strtok(NULL, "\n");
    }
    return entities;
}

void create_mvp(
    float *position,
    float *rotation,
    float *scale,
    float *view_projection,
    float *out)
{
    float transform[16] = {0};
    create_transform(position, rotation, scale, transform);
    multiply_4x4_matrices(view_projection, transform, out);
}

int raycast(float *origin, float *direction, Entity *entities, int num_entities)
{
    int r = 0;
    for (int i = 0; i < num_entities; i++)
    {
        Entity entity = entities[i];
        for (int y = 0; y < entity.mesh->num_indices; y += 3)
        {
            int ind0 = entity.mesh->indices[y];
            int ind1 = entity.mesh->indices[y + 1];
            int ind2 = entity.mesh->indices[y + 2];
            float d = 0;
            r = intersect_triangle(
                origin, direction,
                &entity.mesh->vertices[ind0 * 3],
                &entity.mesh->vertices[ind1 * 3],
                &entity.mesh->vertices[ind2 * 3],
                &d);
        }
        if (r != 0)
        {
            return r;
        }
    }
    return r;
}
