/*
 * Created by Duncan on 05/04/2025.
 */

#include <glad/glad.h>

#include <linmath.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <stdio.h>
#include <string.h>

#include "model.h"
#include "util.h"

void loadModel(model_t* model, const char* path);
void processNode(const model_t* model, const struct aiNode* node, const struct aiScene* scene);
mesh_t* processMesh(const model_t* model, const struct aiMesh* aiMesh, const struct aiScene* scene);
array_texture_t_t* loadMaterialTextures(const model_t* model, const struct aiMaterial* material, enum aiTextureType type, char* typeName);

model_t* modelCreate(const char* path, const int gammaCorrection)
{
	model_t* model = (model_t*)malloc(sizeof(model_t));
	model->gammaCorrection = gammaCorrection;
	loadModel(model, path);
	return model;
}

void modelDestroy(model_t* model)
{
	array_texture_t_delete(model->texturesLoaded);
	array_mesh_t_delete(model->meshes);
	free(model->directory);
	free(model);
}

void modelDraw(const model_t* model, const GLuint* shader)
{
	for (uint i = 0; i < model->meshes->size; i++)
		meshDraw(&model->meshes->array[i], shader);
}

void loadModel(model_t* model, const char* path)
{
	const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
	{
		printf("Error import model file: %s - %s\n", path, aiGetErrorString());
		aiReleaseImport(scene);
		return;
	}

	const char* dir = strrchr(path, '/');
	model->directory = memset(malloc(sizeof(dir)), '\0', sizeof(dir));
	strcpy(model->directory, dir);

	model->texturesLoaded = array_texture_t_create(scene->mNumTextures);
	model->meshes = array_mesh_t_create(scene->mRootNode->mNumMeshes);
	processNode(model, scene->mRootNode, scene);
}

void processNode(const model_t* model, const struct aiNode* node, const struct aiScene* scene)
{
	uint i;
	for (i = 0; i < node->mNumMeshes; i++)
	{
		struct aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
		mesh_t* mesh = processMesh(model, aiMesh, scene);
		array_mesh_t_push(model->meshes, *mesh);
		meshDestroy(mesh);
	}
	for (i = 0; i < node->mNumChildren; i++)
		processNode(model, node->mChildren[i], scene);
}

mesh_t* processMesh(const model_t* model, const struct aiMesh* aiMesh, const struct aiScene* scene)
{
	array_vertex_t_t* vertices = array_vertex_t_create(aiMesh->mNumVertices);
	vertices->capacityIncrement = 3;
	uint i;
	for (i = 0; i < aiMesh->mNumVertices; i++)
	{
		vertex_t vertex;
		vec3 vector;
		vector[0] = aiMesh->mVertices[i].x;
		vector[1] = aiMesh->mVertices[i].y;
		vector[2] = aiMesh->mVertices[i].z;
		memcpy(vertex.position, vector, sizeof(vec3));

		if (aiMesh->mNormals)
		{
			vector[0] = aiMesh->mNormals[i].x;
			vector[1] = aiMesh->mNormals[i].y;
			vector[2] = aiMesh->mNormals[i].z;
			memcpy(vertex.normal, vector, sizeof(vec3));
		}

		if (aiMesh->mTextureCoords[0])
		{
			vec2 vec;
			vec[0] = aiMesh->mTextureCoords[0][i].x;
			vec[1] = aiMesh->mTextureCoords[0][i].y;
			memcpy(vertex.texCoords, vec, sizeof(vec2));

			vector[0] = aiMesh->mTangents[i].x;
			vector[1] = aiMesh->mTangents[i].y;
			vector[2] = aiMesh->mTangents[i].z;
			memcpy(vertex.tangent, vector, sizeof(vec3));

			vector[0] = aiMesh->mBitangents[i].x;
			vector[1] = aiMesh->mBitangents[i].y;
			vector[2] = aiMesh->mBitangents[i].z;
			memcpy(vertex.biTangent, vector, sizeof(vec3));
		} else
		{
			const vec2 vec = {0.f, 0.f};
			memcpy(vertex.texCoords, vec, sizeof(vec2));
		}
		array_vertex_t_push(vertices, vertex);
	}

	array_uint_t* indices = array_uint_create(aiMesh->mNumFaces);
	indices->capacityIncrement = 3;
	for (i = 0; i < aiMesh->mNumFaces; i++)
	{
		const struct aiFace face = aiMesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++)
			array_uint_push(indices, face.mIndices[j]);
	}

	const struct aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];

	array_texture_t_t* diffuseArray = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
	array_texture_t_t* specularArray = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
	array_texture_t_t* normalArray = loadMaterialTextures(model, material, aiTextureType_NORMALS,  "texture_normal");
	array_texture_t_t* heightArray = loadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_height");

	array_texture_t_t* textures = array_texture_t_create(diffuseArray-> size + specularArray-> size + normalArray-> size + heightArray-> size);

	for (i = 0; i < diffuseArray->size; i++)
		array_texture_t_push(textures, diffuseArray->array[i]);
	for (i = 0; i < specularArray->size; i++)
		array_texture_t_push(textures, specularArray->array[i]);
	for (i = 0; i < normalArray->size; i++)
		array_texture_t_push(textures, normalArray->array[i]);
	for (i = 0; i < heightArray->size; i++)
		array_texture_t_push(textures, heightArray->array[i]);

	array_texture_t_delete(diffuseArray);
	array_texture_t_delete(specularArray);
	array_texture_t_delete(normalArray);
	array_texture_t_delete(heightArray);

	mesh_t* mesh = meshCreate(vertices, indices, textures);
	return mesh;
}

array_texture_t_t* loadMaterialTextures(const model_t* model, const struct aiMaterial* material, const enum aiTextureType type, char* typeName)
{
	array_texture_t_t* textures = array_texture_t_create(1);
	textures->capacityIncrement = 1;
	for (uint i = 0; i < aiGetMaterialTextureCount(material, type); i++)
	{
		struct aiString str;
		aiGetMaterialTexture(material, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
		uint skip = 0;
		for (uint j = 0; j < model->texturesLoaded->size; j++)
		{
			if (strcmp(model->texturesLoaded->array[j].path, str.data) == 0)
			{
				array_texture_t_push(textures, model->texturesLoaded->array[j]);
				skip = 1;
				break;
			}
		}
		if (skip == 0)
		{
			char path[1024];
			strcat(path, model->directory);
			const char* slash = "/";
			strcat(path, slash);
			strcat(path, str.data);
			const GLuint id = loadTextureFromFile(path);
			const texture_t texture = {id, typeName, str.data};
			array_texture_t_push(textures, texture);
			array_texture_t_push(model->texturesLoaded, texture);
		}
	}
	return textures;
}
