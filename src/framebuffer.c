/*
 * Created by Duncan on 15/04/2025.
 */

#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "framebuffer.h"

void destroyTexturesAndFBO(const framebuffer_t* framebuffer);

framebuffer_t* framebufferCreate(const int width, const int height)
{
	framebuffer_t* framebuffer = (framebuffer_t*) malloc(sizeof(framebuffer_t));
	framebuffer->width = width;
	framebuffer->height = height;
	return framebuffer;
}

void destroyTexturesAndFBO(const framebuffer_t* framebuffer)
{
	glDeleteTextures(1, &framebuffer->colorTex);
	glDeleteTextures(1, &framebuffer->depthTex);

	glDeleteFramebuffers(1, &framebuffer->fbo);
}

void framebufferDestroy(framebuffer_t* framebuffer)
{
	destroyTexturesAndFBO(framebuffer);

	free(framebuffer);
}

bool framebufferInit(framebuffer_t* framebuffer)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer->colorTex);
	glTextureStorage2D(framebuffer->colorTex, 1, GL_RGBA8, framebuffer->width, framebuffer->height);

	glCreateTextures(GL_TEXTURE_2D, 1, &framebuffer->depthTex);
	glTextureStorage2D(framebuffer->depthTex, 1, GL_DEPTH_COMPONENT32F, framebuffer->width, framebuffer->height);

	glCreateFramebuffers(1, &framebuffer->fbo);
	glNamedFramebufferTexture(framebuffer->fbo, GL_COLOR_ATTACHMENT0, framebuffer->colorTex, 0);
	glNamedFramebufferTexture(framebuffer->fbo, GL_DEPTH_ATTACHMENT, framebuffer->depthTex, 0);

	if (glCheckNamedFramebufferStatus(framebuffer->fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void framebufferResize(framebuffer_t* framebuffer, const int width, const int height)
{
	framebuffer->width = width;
	framebuffer->height = height;

	destroyTexturesAndFBO(framebuffer);
	if (!framebufferInit(framebuffer))
	{
		fprintf(stderr, "Failed to initialize framebuffer\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}

void framebufferClear(const framebuffer_t* framebuffer)
{
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClearNamedFramebufferfv(framebuffer->fbo, GL_COLOR, 0, clearColor);
	const float clearDepth = 1.f;
	glClearNamedFramebufferfv(framebuffer->fbo, GL_DEPTH, 0, &clearDepth);
}

void framebufferBindToDraw(const framebuffer_t* framebuffer)
{
	const GLenum bufs[] = {GL_COLOR_ATTACHMENT0};
	glNamedFramebufferDrawBuffers(framebuffer->fbo, 1, bufs);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer->fbo);
}

void framebufferCopyToDefault(const framebuffer_t* framebuffer)
{
	// Read from framebuffer's output to the default framebuffer
	// This could be done once at init, doing here just to be safe
	glNamedFramebufferReadBuffer(framebuffer->fbo, GL_COLOR_ATTACHMENT0);
	glNamedFramebufferDrawBuffer(0, GL_BACK);

	// Copy contents to default framebuffer
	glBlitNamedFramebuffer(framebuffer->fbo, 0, 0, 0, framebuffer->width, framebuffer->height, 0, 0, framebuffer->width, framebuffer->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
