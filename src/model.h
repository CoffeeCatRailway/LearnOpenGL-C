/*
 * Created by Duncan on 07/04/2025.
 */

#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>

#include <glad/glad.h>

#include <array.h>
#include <linmath.h>

#define VERTEX_STRIDE 8

typedef struct mesh_t
{
	GLsizei numVertices;
	array_float_t* vertices; // Includes position, normal & uv (8 floats)
	GLuint vao, vbo;
} mesh_t;

typedef struct model_t
{
	mesh_t* mesh;
	vec3 position;
	vec3 rotation;
	vec3 scale;
	GLuint renderMethod;
} model_t;

mesh_t* meshCreate(const char* filename, const bool instanced);
void meshDestroy(mesh_t* mesh);

model_t* modelCreate(mesh_t* mesh);
void modelDestroy(model_t* model);

#endif //MODEL_H
