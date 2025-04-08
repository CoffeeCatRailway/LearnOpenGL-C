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

GLuint loadTextureFromFile(const char* path, const GLint wrapS, const GLint wrapT)
{
	GLuint textureId;
	glGenTextures(1, &textureId);

	int width, height, nChannels;
	unsigned char* imageData = stbi_load(path, &width, &height, &nChannels, 0);
	if (imageData)
	{
		GLenum format;
		switch (nChannels)
		{
			case 1:
				format = GL_RED;
			break;
			case 3:
				format = GL_RGB;
			break;
			default:
				format = GL_RGBA;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, textureId); // All upcoming GL_TEXTURE_2D operations now effect the texture object
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		// Set filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		printf("Texture '%s' loaded\n", path);
	} else
	{
		fprintf(stderr, "Failed to load texture: %s\n", path);
	}
	stbi_image_free(imageData);

	return textureId;
}
