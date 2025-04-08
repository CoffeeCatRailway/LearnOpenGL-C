/*
 * Created by Duncan on 07/04/2025.
 * Based on: https://github.com/marichardson137/VerletIntegration/blob/main/src/model.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"

#define VERTEX_LIMIT 2000
#define MAX_LINE_LENGTH 50

array_float_t* loadOBJ(const char* filename);
void processVertex(array_float_t** vertices, char* vertexData[3], vec3 v[], vec3 vt[], vec3 vn[]);

mesh_t* meshCreate(const char* filename, const bool instanced)
{
	// array_float_t* vertices = loadOBJ(filename);

	mesh_t* mesh = (mesh_t*)malloc(sizeof(mesh_t));
	mesh->vertices = loadOBJ(filename);
	mesh->numVertices = mesh->vertices->size / VERTEX_STRIDE;

	// array_float_delete(vertices);

	// Create vao & vbo
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * VERTEX_STRIDE * sizeof(float), mesh->vertices->array, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void*) 0);
	// normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void*) (3 * sizeof(float)));
	// normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE * sizeof(float), (void*) (6 * sizeof(float)));

	if (instanced)
	{
		// stuff
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	printf("Mesh %s loaded\n", filename);
	return mesh;
}

void meshDestroy(mesh_t* mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(1, &mesh->vbo);
	// free(mesh->vertices);
	array_float_delete(mesh->vertices);
	free(mesh);
}

model_t* modelCreate(mesh_t* mesh)
{
	model_t* model = (model_t*)malloc(sizeof(model_t));
	model->mesh = mesh;
	model->position[0] = model->position[1] = model->position[2] = 0.f;
	model->rotation[0] = model->rotation[1] = model->rotation[2] = 0.f;
	model->scale[0] = model->scale[1] = model->scale[2] = 0.f;
	model->renderMethod = GL_TRIANGLES;
	return model;
}

void modelDestroy(model_t* model)
{
	meshDestroy(model->mesh);
	free(model);
}

array_float_t* loadOBJ(const char* filename)
{
	// I can use array.h for 'infinite' vertices
	vec3 v[VERTEX_LIMIT];
	vec3 vt[VERTEX_LIMIT];
	vec3 vn[VERTEX_LIMIT];

	int vCount = 0;
	int vtCount = 0;
	int vnCount = 0;

	array_float_t* vertices = array_float_create(3);
	vertices->capacityIncrement = 3;
	FILE* file = fopen(filename, "r");
	if (!file)
	{
		printf("Could not open file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
	{
		char* words[4];
		words[0] = strtok(line, " ");
		for (int i = 1; i < 4; i++)
			words[i] = strtok(NULL, " ");

		if (strcmp(words[0], "v") == 0)
		{
			v[vCount][0] = atof(words[1]);
			v[vCount][1] = atof(words[2]);
			v[vCount][2] = atof(words[3]);
			vCount++;
		} else if (strcmp(words[0], "vt") == 0)
		{
			vt[vtCount][0] = atof(words[1]);
			vt[vtCount][1] = atof(words[2]);
			vtCount++;
		} else if (strcmp(words[0], "vn") == 0)
		{
			vn[vnCount][0] = atof(words[1]);
			vn[vnCount][1] = atof(words[2]);
			vn[vnCount][2] = atof(words[3]);
			vnCount++;
		} else if (strcmp(words[0], "f") == 0)
		{
			char* v1[3];
			char* v2[3];
			char* v3[3];

			v1[0] = strtok(words[1], "/");
			v1[1] = strtok(NULL, "/");
			v1[2] = strtok(NULL, "/");
			v2[0] = strtok(words[2], "/");
			v2[1] = strtok(NULL, "/");
			v2[2] = strtok(NULL, "/");
			v3[0] = strtok(words[3], "/");
			v3[1] = strtok(NULL, "/");
			v3[2] = strtok(NULL, "/");

			processVertex(&vertices, v1, v, vt, vn);
			processVertex(&vertices, v2, v, vt, vn);
			processVertex(&vertices, v3, v, vt, vn);
		}
	}

	fclose(file);

	array_float_adjust(&vertices);
	return vertices;
}

void processVertex(array_float_t** vertices, char* vertexData[3], vec3 v[], vec3 vt[], vec3 vn[])
{
	const int vertex = atoi(vertexData[0]) - 1;
	const int texture = atoi(vertexData[1]) - 1;
	const int normal = atoi(vertexData[2]) - 1;

	array_float_push(vertices, v[vertex][0]);
	array_float_push(vertices, v[vertex][1]);
	array_float_push(vertices, v[vertex][2]);

	array_float_push(vertices, vn[normal][0]);
	array_float_push(vertices, vn[normal][1]);
	array_float_push(vertices, vn[normal][2]);

	array_float_push(vertices, vt[texture][0]);
	array_float_push(vertices, vt[texture][1]);
}
