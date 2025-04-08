/*
 * Created by Duncan on 04/04/2025.
 */

#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>

#include "util.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#define F_MAT_DIFFUSE 0x001
#define F_MAT_SPECULAR 0x010
#define F_MAT_EMISSION 0x100

#define F_LHT_DIRECT 1
#define F_LHT_POINT 2
#define F_LHT_SPOT 3

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const float CAMERA_SPEED = 7.f;
const float MOUSE_SENSITIVITY = .1f;

unsigned int firstMouse = 1;
float lastX = 0.f;
float lastY = 0.f;
float fov = 45.f;

float deltaTime = 0.f;
float lastFrame = 0.f;

camera_t* camera;
unsigned int mouseCaptured = 0;

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

int main()
{
	printf("Hello, World!\n");

	glfwSetErrorCallback(errorCallback);

	printf("Initializing OpenGL context...\n");
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// #ifdef __APPLE__ // Who likes apple anyway?
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// #endif

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World!", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to initialize GLAD\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_ALWAYS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Setup vertex data & buffers
	const float planeVertices[] = {
		// positions          // normals           // texture coords
		// back
		-0.5f, -0.5f,  0.f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f, -0.5f,  0.f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		// front
		-0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		// left
		 0.f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

		// right
		 0.f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f
	};

	const vec3 cubePositions[] = {
		{0.f, 0.f, 0.f},
		{-1.5f, -2.2f, 2.5f},
		{2.4f, -.4f, -3.5f},
		{-2.7f,  3.f, 2.5f},
		{-3.8f, -2.f, -8.3f},
		{1.3f, -2.f, -2.5f},
		{1.5f,  .2f, -1.5f},
		{1.5f,  2.f, 2.5f},
		{2.f, 5.f, -9.f},
		{-2.3f,  1.f, 1.5f}
	};

	// Build & compile shaders
	const GLuint shaderLighting = createShader("resources/shaders/light.vert", "resources/shaders/light_multi.frag");
	const GLuint shaderColorCube = createShader("resources/shaders/single_color.vert", "resources/shaders/single_color.frag");

	GLuint vaoPlane, vboPlane;
	glGenVertexArrays(1, &vaoPlane);
	glGenBuffers(1, &vboPlane);
	glBindVertexArray(vaoPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	const GLint positionLocation = glGetAttribLocation(shaderLighting, "i_position");
	const GLint normalLocation = glGetAttribLocation(shaderLighting, "i_normal");
	const GLint uvLocation = glGetAttribLocation(shaderLighting, "i_uv");

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(normalLocation);
	glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	glEnableVertexAttribArray(uvLocation);

	mesh_t* meshMonkey = meshCreate("resources/models/ico_sphere.obj", false);
	mesh_t* meshCube = meshCreate("resources/models/cube_fixed.obj", false);

	// Load image, create texture & generate mipmaps
	stbi_set_flip_vertically_on_load(1);

	// load textures
	const GLuint diffuseTexture = loadTextureFromFile("resources/textures/container2.png", GL_REPEAT, GL_REPEAT);
	const GLuint specularTexture = loadTextureFromFile("resources/textures/white.png", GL_REPEAT, GL_REPEAT);
	// GLuint emissionMap = loadTextureFromFile("resources/textures/container2_emission.png");
	const GLuint grassTexture = loadTextureFromFile("resources/textures/grass.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	const GLuint grassSpecularTexture = loadTextureFromFile("resources/textures/grass_specular.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	// Set shader uniforms
	glUseProgram(shaderLighting);
	// setUniform1i(&shaderLighting, "u_material.flags", F_MAT_DIFFUSE | F_MAT_SPECULAR);
	setUniform1i(&shaderLighting, "u_material.diffuseTex", 0);
	setUniform1i(&shaderLighting, "u_material.specularTex", 1);
	setUniform1f(&shaderLighting, "u_material.shininess", 64.f);

	// Uncomment this to draw wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Light properties
	vec3 lightPos = {1.2f, 10.5f, 2.f}, lightDir = {0.f, -1.f, 0.f};
	// vec3_norm(lightDir, lightPos);
	float lightRange = 200.f;

	vec3 sunDir = {-1.f, -1.f, -1.f};
	vec3_norm(sunDir, sunDir);
	// printf("%f %f %f\n", sunDir[0], sunDir[1], sunDir[2]);

	// Setup camera
	vec3 p = {0.f, 0.f, 10.f};
	// Initialize yaw to -90 since 0 results in a direction vector pointing to the right
	camera = cameraCreate(p, -90.f, 0.f, 89.f);

	mat4x4 view, projection;

	mat4x4_identity(view);
	mat4x4_identity(projection);

	while (!glfwWindowShouldClose(window))
	{
		// Update/Input
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// Render
		glClearColor(.1f, .1f, .1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		mat4x4_perspective(projection, RAD(fov), (float) width / (float) height, .1f, 100.f);
		cameraGetViewMatrix(camera, &view);

		// Light color
		vec3 lightColor = {1.f, 1.f, 1.f}, ambientColor, diffuseColor;
		lightColor[0] = sinf(currentFrame * 2.f) * .5f + .5f;
		lightColor[1] = sinf(currentFrame * .7f) * .5f + .5f;
		lightColor[2] = sinf(currentFrame * 1.3f) * .5f + .5f;

		vec3_scale(ambientColor, lightColor, .5f);
		vec3_scale(diffuseColor, lightColor, .75f);

		//lightPos[0] = 1.2f * cosf(currentFrame);
		lightPos[0] = sinf(currentFrame) * 2.f;
		lightPos[2] = cosf(currentFrame) * 2.f;

		glUseProgram(shaderLighting);
		// Light 1
		lightRange = 200.f;
		setUniform1i(&shaderLighting, "u_lights[0].mode", F_LHT_SPOT);
		setUniform3fv(&shaderLighting, "u_lights[0].position", lightPos);
		setUniform3fv(&shaderLighting, "u_lights[0].direction", lightDir);
		setUniform1f(&shaderLighting, "u_lights[0].cutOffInner", cosf(RAD(20.f)));
		setUniform1f(&shaderLighting, "u_lights[0].cutOffOuter", cosf(RAD(25.f)));

		setUniform3fv(&shaderLighting, "u_lights[0].ambient", ambientColor);
		setUniform3fv(&shaderLighting, "u_lights[0].diffuse", diffuseColor);
		setUniform3f(&shaderLighting, "u_lights[0].specular", 1.f, 1.f, 1.f);

		setUniform1f(&shaderLighting, "u_lights[0].constant", 1.f);
		setUniform1f(&shaderLighting, "u_lights[0].linear", 4.5f / lightRange);
		setUniform1f(&shaderLighting, "u_lights[0].quadratic", 75.f / (lightRange * lightRange));

		// Light 2 Camera
		lightColor[0] = 1.f;
		lightColor[1] = 1.f;
		lightColor[2] = 1.f;
		vec3_scale(ambientColor, lightColor, .75f);
		vec3_scale(diffuseColor, lightColor, .75f);
		lightRange = 100.f;

		setUniform1i(&shaderLighting, "u_lights[1].mode", F_LHT_SPOT);
		setUniform3fv(&shaderLighting, "u_lights[1].position", camera->position);
		setUniform3fv(&shaderLighting, "u_lights[1].direction", camera->front);
		setUniform1f(&shaderLighting, "u_lights[1].cutOffInner", cosf(RAD(15.f)));
		setUniform1f(&shaderLighting, "u_lights[1].cutOffOuter", cosf(RAD(17.f)));

		setUniform3fv(&shaderLighting, "u_lights[1].ambient", ambientColor);
		setUniform3fv(&shaderLighting, "u_lights[1].diffuse", diffuseColor);
		setUniform3f(&shaderLighting, "u_lights[1].specular", 1.f, 1.f, 1.f);

		setUniform1f(&shaderLighting, "u_lights[1].constant", 1.f);
		setUniform1f(&shaderLighting, "u_lights[1].linear", 4.5f / lightRange);
		setUniform1f(&shaderLighting, "u_lights[1].quadratic", 75.f / (lightRange * lightRange));

		// Light 3 Sun
		lightColor[0] = 1.f;
		lightColor[1] = 1.f;
		lightColor[2] = 1.f;
		vec3_scale(ambientColor, lightColor, .25f);
		vec3_scale(diffuseColor, lightColor, .5f);

		setUniform1i(&shaderLighting, "u_lights[2].mode", F_LHT_DIRECT);
		setUniform3fv(&shaderLighting, "u_lights[2].direction", sunDir);

		setUniform3fv(&shaderLighting, "u_lights[2].ambient", ambientColor);
		setUniform3fv(&shaderLighting, "u_lights[2].diffuse", diffuseColor);
		setUniform3f(&shaderLighting, "u_lights[2].specular", 1.f, 1.f, 1.f);

		setUniform3fv(&shaderLighting, "u_viewPos", camera->position);

		// setUniform1f(&shaderLighting, "u_time", currentFrame);

		setUniformMatrix4fv(&shaderLighting, "u_view", (GLfloat*) view);
		setUniformMatrix4fv(&shaderLighting, "u_projection", (GLfloat*) projection);

		// Render the cube
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);

		// glActiveTexture(GL_TEXTURE2);
		// glBindTexture(GL_TEXTURE_2D, emissionMap);

		mat4x4 model;
		mat4x4_identity(model);
		mat4x4_translate(model, 0.f, -8.f, 0.f);
		mat4x4_scale_aniso(model, model, 10.f, 1.f, 10.f);
		setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		glBindVertexArray(meshCube->vao);
		glDrawArrays(GL_TRIANGLES, 0, meshCube->numVertices);

		glBindVertexArray(meshMonkey->vao);
		for (int i = 0; i < 10; i++)
		{
			mat4x4_identity(model);
			mat4x4_translate(model, cubePositions[i][0], cubePositions[i][1], cubePositions[i][2]);
			float angle = 20.f * (float) i;
			if (i % 2 == 0)
				angle += (float) glfwGetTime() * (40.f + (float) i * 40.f);
			mat4x4_rotate(model, model, 1.f, .3f, .5f, RAD(angle));
			mat4x4_scale_aniso(model, model, .5f, .5f, .5f);
			setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
			// glDrawArrays(GL_TRIANGLES, 0, 36);
			glDrawArrays(GL_TRIANGLES, 0, meshMonkey->numVertices);
		}

		// glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grassSpecularTexture);

		mat4x4_identity(model);
		mat4x4_translate(model, 0.f, -6.f, 0.f);
		mat4x4_scale_aniso(model, model, 2.f, 2.f, 2.f);
		setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		glBindVertexArray(vaoPlane);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		// glEnable(GL_DEPTH_TEST);

		// Draw lamp
		glUseProgram(shaderColorCube);
		setUniform3fv(&shaderColorCube, "u_color", lightColor);

		setUniformMatrix4fv(&shaderColorCube, "u_view", (GLfloat*) view);
		setUniformMatrix4fv(&shaderColorCube, "u_projection", (GLfloat*) projection);

		mat4x4_identity(model);
		mat4x4_translate(model, lightPos[0], lightPos[1], lightPos[2]);
		// mat4x4_scale(model, model, .2f); // Doesn't work?
		mat4x4_scale_aniso(model, model, .2f, .2f, .2f);
		setUniformMatrix4fv(&shaderColorCube, "u_model", (GLfloat*) model);

		glBindVertexArray(meshCube->vao);
		glDrawArrays(GL_TRIANGLES, 0, meshCube->numVertices);

		// Swap buffers & poll IO
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Cleaning up\n");
	cameraDelete(camera);

	glDeleteVertexArrays(1, &vaoPlane);
	glDeleteBuffers(1, &vboPlane);

	meshDestroy(meshMonkey);
	meshDestroy(meshCube);

	glDeleteProgram(shaderLighting);
	glDeleteProgram(shaderColorCube);

	glDeleteTextures(1, &diffuseTexture);
	glDeleteTextures(1, &specularTexture);

	glDeleteTextures(1, &grassTexture);
	glDeleteTextures(1, &grassSpecularTexture);

	glfwDestroyWindow(window);

	exit(EXIT_SUCCESS);
}

void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void framebufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
	//	printf("Set viewport size to (%d,%d)\n", width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraMoveForward(camera, CAMERA_SPEED * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraMoveBackward(camera, CAMERA_SPEED * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraMoveLeft(camera, CAMERA_SPEED * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraMoveRight(camera, CAMERA_SPEED * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraMoveUp(camera, CAMERA_SPEED * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraMoveDown(camera, CAMERA_SPEED * deltaTime);
}

void keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		//		glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (mouseCaptured)
		{
			mouseCaptured = 0;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else
		{
			mouseCaptured = 1;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}

void mouseCallback(GLFWwindow* window, const double xPosIn, const double yPosIn)
{
	if (!mouseCaptured)
		return;

	const float xPos = xPosIn;
	const float yPos = yPosIn;

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = 0;
	}

	const float xOffset = xPos - lastX;
	const float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	cameraProcessMouse(camera, xOffset * MOUSE_SENSITIVITY, yOffset * MOUSE_SENSITIVITY);
}

void scrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
{
	if (!mouseCaptured)
		return;

	fov -= (float) yOffset;
	if (fov < 1.f)
		fov = 1.f;
	if (fov > 45.f)
		fov = 45.f;
}