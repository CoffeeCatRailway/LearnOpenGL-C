/*
 * Created by Duncan on 05/04/2025.
 */

#include "mesh.h"

GLuint VBO, EBO;

void setupMesh(mesh_t* mesh);

mesh_t* meshCreate(array_vertex_t_t* vertices, array_uint_t* indices, array_texture_t_t* textures)
{
	mesh_t* mesh = (mesh_t*)malloc(sizeof(mesh_t));
	mesh->vertices = vertices;
	mesh->indices = indices;
	mesh->textures = textures;
	setupMesh(mesh);
	return mesh;
}

void meshDestroy(mesh_t* mesh)
{
	array_vertex_t_delete(mesh->vertices);
	array_uint_delete(mesh->indices);
	array_texture_t_delete(mesh->textures);
	free(mesh);
}

void meshDraw(const mesh_t* mesh, const GLuint* shader)
{
	uint diffuseN = 1, specularN = 1, normalN = 1, heightN = 1;
	for (uint i = 0; i < mesh->textures->size; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		char number[2];
		char name[strlen(mesh->textures->array[i].type) + 1];
		strcpy(number, mesh->textures->array[i].type);
		if (strcmp(number, "texture_diffuse") == 0)
		{
			snprintf(number, 0, "%d", diffuseN++);
		} else if (strcmp(number, "texture_specular") == 0)
		{
			snprintf(number, 0, "%d", specularN++);
		} else if (strcmp(number, "texture_normal") == 0)
		{
			snprintf(number, 0, "%d", normalN++);
		} else if (strcmp(number, "texture_height") == 0)
		{
			snprintf(number, 0, "%d", heightN++);
		}
		strcat(name, number);
		glUniform1i(glGetUniformLocation(*shader, name), i);
		glBindTexture(GL_TEXTURE_2D, mesh->textures->array[i].id);
	}

	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->indices->size, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void setupMesh(mesh_t* mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices->size * sizeof(vertex_t), &mesh->vertices->array[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->size * sizeof(uint), &mesh->indices->array[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, biTangent));

	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(vertex_t), (void*) offsetof(vertex_t, boneIds));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, weights));
	glBindVertexArray(0);
}
