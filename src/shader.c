/*
 * Created by Duncan on 05/04/2025.
 */

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
#include "util.h"

void shaderCompile(GLuint* shader, const GLenum shaderType, const char* shaderFile)
{
	*shader = glCreateShader(shaderType);
	if (*shader == 0)
		fprintf(stderr, "Could not load shader: %s\n", shaderFile);

	const char* shaderSource = readFile(shaderFile);
	glShaderSource(*shader, 1, &shaderSource, NULL);
	glCompileShader(*shader);
//	printf("%s\n", shaderSource);
	free((void*) shaderSource);

	GLint isCompiled = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		fprintf(stderr, "Shader compile error: %s\n", shaderFile);

		GLint logSize = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logSize);
		char infoBuffer[logSize];
		glGetShaderInfoLog(*shader, logSize, &logSize, infoBuffer);
		fprintf(stderr, infoBuffer);

		glDeleteShader(*shader);
		exit(-1);
	}
	printf("Shader '%s' compile success\n", shaderFile);
}

//void createShader(GLuint* shaderProgram)
GLuint shaderCreate(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
	GLuint vertexShader, fragmentShader, geometryShader;
	shaderCompile(&vertexShader, GL_VERTEX_SHADER, vertexFile);
	shaderCompile(&fragmentShader, GL_FRAGMENT_SHADER, fragmentFile);
	if (geometryFile)
		shaderCompile(&geometryShader, GL_GEOMETRY_SHADER, geometryFile);

	const GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geometryFile)
		glAttachShader(shaderProgram, geometryShader);

	glBindFragDataLocation(shaderProgram, 0, "FragColor");

	glLinkProgram(shaderProgram);
//	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geometryFile)
		glDeleteShader(geometryShader);

	printf("Shader %d linked\n", shaderProgram);
	return shaderProgram;
}

void setUniform1i(const GLuint* shader, const char* name, const GLint x)
{
	// glUniform1i(glGetUniformLocation(*shader, name), x);
	glProgramUniform1i(*shader, glGetUniformLocation(*shader, name), x);
}

void setUniform1ui(const GLuint* shader, const char* name, const GLuint x)
{
	// glUniform1ui(glGetUniformLocation(*shader, name), x);
	glProgramUniform1ui(*shader, glGetUniformLocation(*shader, name), x);
}

void setUniform1f(const GLuint* shader, const char* name, const GLfloat x)
{
	// glUniform1f(glGetUniformLocation(*shader, name), x);
	glProgramUniform1f(*shader, glGetUniformLocation(*shader, name), x);
}

void setUniform1fv(const GLuint* shader, const char* name, const GLfloat* value)
{
	// glUniform1fv(glGetUniformLocation(*shader, name), 1, value);
	glProgramUniform1fv(*shader, glGetUniformLocation(*shader, name), 1, value);
}

void setUniform2f(const GLuint* shader, const char* name, const GLfloat x, const GLfloat y)
{
	// glUniform2f(glGetUniformLocation(*shader, name), x, y);
	glProgramUniform2f(*shader, glGetUniformLocation(*shader, name), x, y);
}

void setUniform2fv(const GLuint* shader, const char* name, vec2 value)
{
	// glUniform2fv(glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
	glProgramUniform2fv(*shader, glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
}

void setUniform3f(const GLuint* shader, const char* name, const GLfloat x, const GLfloat y, const GLfloat z)
{
	// glUniform3f(glGetUniformLocation(*shader, name), x, y, z);
	glProgramUniform3f(*shader, glGetUniformLocation(*shader, name), x, y, z);
}

void setUniform3fv(const GLuint* shader, const char* name, vec3 value)
{
	// glUniform3fv(glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
	glProgramUniform3fv(*shader, glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
}

void setUniform4f(const GLuint* shader, const char* name, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
{
	// glUniform4f(glGetUniformLocation(*shader, name), x, y, z, w);
	glProgramUniform4f(*shader, glGetUniformLocation(*shader, name), x, y, z, w);
}

void setUniform4fv(const GLuint* shader, const char* name, vec4 value)
{
	// glUniform4fv(glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
	glProgramUniform4fv(*shader, glGetUniformLocation(*shader, name), 1, (const GLfloat*) value);
}

void setUniformMatrix4fv(const GLuint* shader, const char* name, const GLfloat* value)
{
	// glUniformMatrix4fv(glGetUniformLocation(*shader, name), 1, GL_FALSE, value);
	glProgramUniformMatrix4fv(*shader, glGetUniformLocation(*shader, name), 1, GL_FALSE, value);
}
