/*
 * Created by Duncan on 05/04/2025.
 */

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <cglm/cglm.h>

void shaderCompile(GLuint* shader, GLenum shaderType, const char* shaderFile);

GLuint shaderCreate(const char* vertexFile, const char* fragmentFile, const char* geometryFile);

void setUniform1i(const GLuint* shader, const char* name, GLint x);
void setUniform1ui(const GLuint* shader, const char* name, GLuint x);

void setUniform1f(const GLuint* shader, const char* name, GLfloat x);
void setUniform1fv(const GLuint* shader, const char* name, const GLfloat* value);

void setUniform2f(const GLuint* shader, const char* name, GLfloat x, GLfloat y);
void setUniform2fv(const GLuint* shader, const char* name, vec2 value);

void setUniform3f(const GLuint* shader, const char* name, GLfloat x, GLfloat y, GLfloat z);
void setUniform3fv(const GLuint* shader, const char* name, vec3 value);

void setUniform4f(const GLuint* shader, const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void setUniform4fv(const GLuint* shader, const char* name, vec4 value);

void setUniformMatrix4fv(const GLuint* shader, const char* name, const GLfloat* value);

#endif //SHADER_H
