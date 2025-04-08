/*
 * Created by Duncan on 05/04/2025.
 */

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <linmath.h>

#include <array.h>

#define MAX_BONE_INFLUENCE 4

typedef struct vertex_t
{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
	vec3 tangent;
	vec3 biTangent;
	int boneIds[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];
} vertex_t;

typedef struct texture_t
{
	GLuint id;
	char* type;
	char* path;
} texture_t;

ARRAY_H_DEFINE_ARRAY(vertex_t);
ARRAY_H_DEFINE_ARRAY(texture_t);

typedef unsigned int uint;
ARRAY_H_DEFINE_ARRAY(uint);

typedef struct mesh_t
{
	array_vertex_t_t* vertices;
	array_uint_t* indices;
	array_texture_t_t* textures;
	GLuint VAO;
} mesh_t;

ARRAY_H_DEFINE_ARRAY(mesh_t);

mesh_t* meshCreate(array_vertex_t_t* vertices, array_uint_t* indices, array_texture_t_t* textures);
void meshDestroy(mesh_t* mesh);
void meshDraw(const mesh_t* mesh, const GLuint* shader);

#endif //MESH_H
