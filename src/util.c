/*
 * Created by Duncan on 05/04/2025.
 */

#include <glad/glad.h>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

unsigned char* loadImageDataFromFile(const char* path, int* width, int* height, GLenum* format);

char* readFile(const char* filename)
{
	long size = 0;

	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Couldn't open file %s\n", filename);
		exit(-1);
	}
	fseek(file, 0L, SEEK_END);
	size = ftell(file) + 1;
	fclose(file);
	//	printf("%ld\n", size);

	file = fopen(filename, "r");
	char* content = memset(malloc(size), '\0', size);
	fread(content, 1, size - 1, file);
	//	content[size - 1] = '\0';
	fclose(file);

	//	printf("Read file '%s'\n", filename);
	return content;
}

unsigned char* loadImageDataFromFile(const char* path, int* width, int* height, GLenum* format)
{
	int nChannels;
	unsigned char* imageData = stbi_load(path, width, height, &nChannels, 0);
	if (!imageData)
	{
		fprintf(stderr, "Failed to load image: %s\n", path);
		stbi_image_free(imageData);
		return NULL;
	}

	switch (nChannels)
	{
		case 1:
			*format = GL_RED;
			break;
		case 3:
			*format = GL_RGB;
			break;
		default:
			*format = GL_RGBA;
			break;
	}
	return imageData;
}

GLuint loadTextureFromFile(const char* path, const GLint wrapS, const GLint wrapT)
{
	GLuint textureId;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, wrapS);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, wrapT);
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	GLenum format;
	unsigned char* imageData = loadImageDataFromFile(path, &width, &height, &format);
	if (imageData)
	{
		// Look into `internalFormat`
		glTextureStorage2D(textureId, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateTextureMipmap(textureId);
		printf("Texture '%s' loaded\n", path);
	} else
		fprintf(stderr, "Failed to load texture: %s\n", path);
	stbi_image_free(imageData);

	return textureId;
}

GLuint loadCubeMapTextureFromFiles(const char* faces[], const GLint wrapS, const GLint wrapT, const GLint wrapR)
{
	GLuint textureId;
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureId);

	glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, wrapS);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, wrapT);
	glTextureParameteri(textureId, GL_TEXTURE_WRAP_R, wrapR);
	glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	GLenum format;
	unsigned char* imageData = loadImageDataFromFile(faces[0], &width, &height, &format);
	if (imageData)
	{
		glTextureStorage2D(textureId, 1, GL_RGBA8, width, height);
		glTextureSubImage3D(textureId, 0, 0, 0, 0, width, height, 1, format, GL_UNSIGNED_BYTE, imageData);

		for (int i = 1; i < 6; i++)
		{
			imageData = loadImageDataFromFile(faces[i], &width, &height, &format);
			if (imageData)
				glTextureSubImage3D(textureId, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, imageData);
			stbi_image_free(imageData);
		}
		printf("Cube map texture '%d' loaded\n", textureId);
	} else
		fprintf(stderr, "Failed to load cube map texture: %d\n", textureId);
	return textureId;
}
