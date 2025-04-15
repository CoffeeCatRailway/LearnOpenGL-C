/*
 * Created by Duncan on 15/04/2025.
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdbool.h>

typedef struct framebuffer_t
{
	GLsizei width;
	GLsizei height;
	GLuint fbo;
	GLuint colorTex;
	GLuint depthTex;
} framebuffer_t;

framebuffer_t* framebufferCreate(int width, int height);
void framebufferDestroy(framebuffer_t* framebuffer);
bool framebufferInit(framebuffer_t* framebuffer);

void framebufferResize(framebuffer_t* framebuffer, int width, int height);

void framebufferClear(const framebuffer_t* framebuffer);
void framebufferBindToDraw(const framebuffer_t* framebuffer);
void framebufferCopyToDefault(const framebuffer_t* framebuffer);

#endif //FRAMEBUFFER_H
