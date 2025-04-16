/*
 * Created by Duncan on 05/04/2025.
 */

#ifndef UTIL_H
#define UTIL_H

#include <glad/glad.h>

#define PI 3.14159265358979323846
#define RAD(n) (n * PI / 180.0)

char* readFile(const char* filename);

GLuint loadTextureFromFile(const char* path, GLint wrapS, GLint wrapT);
GLuint loadCubeMapTextureFromFiles(const char* faces[], GLint wrapS, GLint wrapT, GLint wrapR);

#endif //UTIL_H
