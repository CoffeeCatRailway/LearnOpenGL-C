/*
 * Created by Duncan on 04/04/2025.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// #include <linmath.h>
#include <cglm/cglm.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include "util.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "framebuffer.h"

#define F_MAT_DIFFUSE 0x001
#define F_MAT_SPECULAR 0x010
#define F_MAT_EMISSION 0x100

#define F_LHT_DIRECT 1
#define F_LHT_POINT 2
#define F_LHT_SPOT 3
#define MAX_LIGHTS 8

typedef struct light_t
{
	bool enable;
	char name[10];

	int mode;
	vec3 position;
	vec3 direction;
	float cutOffInner;
	float cutOffOuter;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float range;
} light_t;

const unsigned int WIDTH = 1600;
const unsigned int HEIGHT = 900;
const unsigned int SEED = 0;

bool firstMouse = true;
float lastX = 0.f;
float lastY = 0.f;

float deltaTime = 0.f;
float lastFrame = 0.f;
bool vsync = false;

vec3 clearColor = {0.f, 0.f, 0.f};
bool postProcessing = false;
framebuffer_t* framebuffer;

camera_t* camera;
bool mouseCaptured = false;
float cameraSpeed = 10.f;
float mouseSensitivity = .1f;

ImGuiContext* imguiCtx;
ImGuiIO* imguiIO;

light_t lights[MAX_LIGHTS];

void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

void guiInit(GLFWwindow* window);
void guiRender();
void guiTerminate();
void guiLightSettings(const char* label, light_t* light);
void guiUpdate();

// float randf()
// {
// 	return (float) (rand() % 101) / 100.f * 2. - 1.f;
// }

int main()
{
	printf("Hello, World!\n");
	srand(SEED);
	printf("Seed: %d\n", SEED);

	glfwSetErrorCallback(errorCallback);

	printf("Initializing OpenGL context...\n");
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
	glfwSwapInterval(vsync);

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

	guiInit(window);

	// glViewport(0, 0, WIDTH, HEIGHT);
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
	const float planeCrossVertices[] = {
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

	const float quadVerticies[] = {
		// pos     uv
		-1.f, 1.f, 0.f, 1.f,
		-1.f, -1.f, 0.f, 0.f,
		1.f, -1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f, 1.f,
		1.f, -1.f, 1.f, 0.f,
		1.f, 1.f, 1.f, 1.f
	};

	const float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	const int instanceAmount = 100;//256000;
	// vec3 instancePositions[] = {
	// 	{0.f, 0.f, 0.f},
	// 	{-1.5f, -2.2f, 2.5f},
	// 	{2.4f, -.4f, -3.5f},
	// 	{-2.7f,  3.f, 2.5f},
	// 	{-3.8f, -2.f, -8.3f},
	// 	{1.3f, -2.f, -2.5f},
	// 	{1.5f,  .2f, -1.5f},
	// 	{1.5f,  2.f, 2.5f},
	// 	{2.f, 5.f, -9.f},
	// 	{-2.3f,  1.f, 1.5f}
	// };
	// vec3 instancePositions[instanceAmount];
	// for (int i = 0; i < instanceAmount; i++)
	// {
	// 	instancePositions[i][0] = randf() * 50.f;
	// 	instancePositions[i][1] = (randf() + .5f + .5f) * 50.f;
	// 	instancePositions[i][2] = randf() * 50.f;
	// }
	// printf("%f %f %f\n", instancePositions[0][0], instancePositions[0][1], instancePositions[0][2]);

	// Build & compile shaders
	const GLuint shaderLighting = shaderCreate("resources/shaders/light.vert", "resources/shaders/light_multi.frag", NULL);
	const GLuint shaderSingleColor = shaderCreate("resources/shaders/single_color.vert", "resources/shaders/single_color.frag", NULL);
	const GLuint shaderQuadTexture = shaderCreate("resources/shaders/quad_texture.vert", "resources/shaders/quad_texture.frag", NULL);
	const GLuint shaderSkybox = shaderCreate("resources/shaders/skybox.vert", "resources/shaders/skybox.frag", NULL);

	const GLuint shaderGeomExplode = shaderCreate("resources/shaders/geom_explode.vert", "resources/shaders/geom_explode.frag", "resources/shaders/geom_explode.geom");
	const GLuint shaderGeomNormals = shaderCreate("resources/shaders/geom_normal_visual.vert", "resources/shaders/geom_normal_visual.frag", "resources/shaders/geom_normal_visual.geom");

	GLuint vaoPlaneCross, vboPlaneCross;
	glCreateVertexArrays(1, &vaoPlaneCross);
	glCreateBuffers(1, &vboPlaneCross);
	glBindVertexArray(vaoPlaneCross);
	glNamedBufferData(vboPlaneCross, sizeof(planeCrossVertices), planeCrossVertices, GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(vaoPlaneCross, 0, vboPlaneCross, 0, 8 * sizeof(float));

	GLint positionLocation = glGetAttribLocation(shaderLighting, "i_position");
	const GLint normalLocation = glGetAttribLocation(shaderLighting, "i_normal");
	GLint uvLocation = glGetAttribLocation(shaderLighting, "i_uv");

	glVertexArrayAttribFormat(vaoPlaneCross, positionLocation, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoPlaneCross, positionLocation, 0);
	glVertexArrayAttribFormat(vaoPlaneCross, normalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribBinding(vaoPlaneCross, normalLocation, 0);
	glVertexArrayAttribFormat(vaoPlaneCross, uvLocation, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexArrayAttribBinding(vaoPlaneCross, uvLocation, 0);

	glEnableVertexArrayAttrib(vaoPlaneCross, positionLocation);
	glEnableVertexArrayAttrib(vaoPlaneCross, normalLocation);
	glEnableVertexArrayAttrib(vaoPlaneCross, uvLocation);

	GLuint vaoQuad, vboQuad;
	glCreateVertexArrays(1, &vaoQuad);
	glCreateBuffers(1, &vboQuad);
	glBindVertexArray(vaoQuad);
	glNamedBufferData(vboQuad, sizeof(quadVerticies), quadVerticies, GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(vaoQuad, 0, vboQuad, 0, 4 * sizeof(float));

	positionLocation = glGetAttribLocation(shaderQuadTexture, "i_position");
	uvLocation = glGetAttribLocation(shaderQuadTexture, "i_uv");

	glVertexArrayAttribFormat(vaoQuad, positionLocation, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoQuad, positionLocation, 0);
	glVertexArrayAttribFormat(vaoQuad, uvLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
	glVertexArrayAttribBinding(vaoQuad, uvLocation, 0);

	glEnableVertexArrayAttrib(vaoQuad, positionLocation);
	glEnableVertexArrayAttrib(vaoQuad, uvLocation);

	GLuint vaoSkybox, vboSkybox;
	glCreateVertexArrays(1, &vaoSkybox);
	glCreateBuffers(1, &vboSkybox);
	glBindVertexArray(vaoSkybox);
	glNamedBufferData(vaoSkybox, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(vaoSkybox, 0, vboSkybox, 0, 3 * sizeof(float));

	positionLocation = glGetAttribLocation(shaderSkybox, "i_position");

	glVertexArrayAttribFormat(vaoSkybox, positionLocation, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoSkybox, positionLocation, 0);

	glEnableVertexArrayAttrib(vaoSkybox, positionLocation);

	mesh_t* meshMonkey = meshCreate("resources/models/monkey.obj", false);
	mesh_t* meshCube = meshCreate("resources/models/cube_fixed.obj", false);
	mesh_t* meshInstance = meshCreate("resources/models/monkey.obj", false);

	// Load image, create texture & generate mipmaps
	stbi_set_flip_vertically_on_load(1);

	// load textures
	const GLuint diffuseTexture = loadTextureFromFile("resources/textures/brickwall.jpg", GL_REPEAT, GL_REPEAT);
	const GLuint specularTexture = loadTextureFromFile("resources/textures/brickwall_specular.jpg", GL_REPEAT, GL_REPEAT);
	// GLuint emissionMap = loadTextureFromFile("resources/textures/container2_emission.png");
	const GLuint grassTexture = loadTextureFromFile("resources/textures/grass.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	const GLuint grassSpecularTexture = loadTextureFromFile("resources/textures/grass_specular.png", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	stbi_set_flip_vertically_on_load(0);
	const char* faces[] = {
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg",
	};
	const GLuint skyboxTexture = loadCubeMapTextureFromFiles(faces, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(1);

	// Set shader uniforms
	glUseProgram(shaderLighting);
	// setUniform1i(&shaderLighting, "u_material.flags", F_MAT_DIFFUSE | F_MAT_SPECULAR);
	setUniform1i(&shaderLighting, "u_material.diffuseTex", 0);
	setUniform1i(&shaderLighting, "u_material.specularTex", 1);
	setUniform1f(&shaderLighting, "u_material.shininess", 32.f);
	setUniform1i(&shaderLighting, "u_skybox", 2);

	glUseProgram(shaderQuadTexture);
	setUniform1i(&shaderQuadTexture, "u_texture", 0);

	glUseProgram(shaderSkybox);
	setUniform1i(&shaderSkybox, "u_texture", 0);

	glUseProgram(shaderGeomExplode);
	setUniform1i(&shaderGeomExplode, "u_texture", 0);

	// generate list of transforms
	mat4* modelMatrices = malloc(sizeof(mat4) * instanceAmount);
	if (modelMatrices == NULL)
	{
		fprintf(stderr, "Out of memory! Failed to allocate model matrices!\n");
		free(modelMatrices);
		exit(EXIT_FAILURE);
	}
	const float radius = 10.f;
	const float offset = 10.f;
	for (int i = 0; i < instanceAmount; i++)
	{
		mat4 model;
		glm_mat4_identity(model);
		// translate along circle with 'radius' in range [-offset, offset]
		const float angle = (float) i / (float) instanceAmount * 360.f;
		float displacement = (float) (rand() % (int)(2 * offset * 100.f)) / 100.f - offset;
		const float x = sinf(angle) * radius + displacement;
		displacement = (float) (rand() % (int)(2 * offset * 100.f)) / 100.f - offset;
		const float y = displacement * .5f;
		displacement = (float) (rand() % (int)(2 * offset * 100.f)) / 100.f - offset;
		const float z = cosf(angle) * radius + displacement;
		glm_translate(model, (vec3){x, y, z});

		// scale between .05 & .75
		const float scale = (float) (rand() % 75) / 100.f + .25f;
		glm_scale(model, (vec3){scale, scale, scale});

		// rotate around a semi-random axis vector
		const float rotAngle = (float) (rand() % 360);
		glm_rotate(model, rotAngle, (vec3){.4f, .6f, .8f});

		// modelMatrices[i] = model;
		glm_mat4_copy(model, modelMatrices[i]);
		// memcpy(&modelMatrices[i], &model, sizeof(mat4));
	}
	printf("Generate model matrices\n");

	// configure instanced array
	GLuint instanceBuffer;
	glBindVertexArray(meshInstance->vao);
	glCreateBuffers(1, &instanceBuffer);
	glNamedBufferData(instanceBuffer, instanceAmount * sizeof(mat4), &modelMatrices[0], GL_STATIC_DRAW);
	// glNamedBufferData(instanceBuffer, sizeof(modelMatrices), modelMatrices, GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(meshInstance->vao, 1, instanceBuffer, 0, sizeof(mat4));

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	// ^^ This comment was copied from learnopengl.com ^^
	glVertexArrayAttribFormat(meshInstance->vao, 3, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(meshInstance->vao, 3, 1);
	glVertexArrayAttribFormat(meshInstance->vao, 4, 4, GL_FLOAT, GL_FALSE, sizeof(vec4));
	glVertexArrayAttribBinding(meshInstance->vao, 4, 1);
	glVertexArrayAttribFormat(meshInstance->vao, 5, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(vec4));
	glVertexArrayAttribBinding(meshInstance->vao, 5, 1);
	glVertexArrayAttribFormat(meshInstance->vao, 6, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(vec4));
	glVertexArrayAttribBinding(meshInstance->vao, 6, 1);

	glVertexArrayBindingDivisor(meshInstance->vao, 1, 1);

	glEnableVertexArrayAttrib(meshInstance->vao, 3);
	glEnableVertexArrayAttrib(meshInstance->vao, 4);
	glEnableVertexArrayAttrib(meshInstance->vao, 5);
	glEnableVertexArrayAttrib(meshInstance->vao, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	printf("Model instance vbo\n");

	// Framebuffer
	framebuffer = framebufferCreate(WIDTH, HEIGHT);
	if (!framebufferInit(framebuffer))
	{
		fprintf(stderr, "Failed to initialize framebuffer\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Setup camera
	// Initialize yaw to -90 since 0 results in a direction vector pointing to the right
	camera = cameraCreate((vec3){0.f, 0.f, 10.f}, -90.f, 0.f, 89.f, 45.f, .1f, 500.f);

	// Sun Light
	lights[0].enable = true;
	strcpy(lights[0].name, "Sun");
	lights[0].mode = F_LHT_DIRECT;
	lights[0].position[0] = -10.f;
	lights[0].position[1] = 10.f;
	lights[0].position[2] = -10.f;
	lights[0].direction[0] = 1.f;
	lights[0].direction[1] = -1.f;
	lights[0].direction[2] = 1.f;
	lights[0].cutOffInner = 30.f;
	lights[0].cutOffOuter = 30.f;
	lights[0].ambient[0] = .25f;
	lights[0].ambient[1] = .25f;
	lights[0].ambient[2] = .25f;
	lights[0].diffuse[0] = .5f;
	lights[0].diffuse[1] = .5f;
	lights[0].diffuse[2] = .5f;
	lights[0].specular[0] = 1.f;
	lights[0].specular[1] = 1.f;
	lights[0].specular[2] = 1.f;
	lights[0].range = 200.f;

	// Camera Light
	lights[1].enable = false;
	strcpy(lights[1].name, "Camera");
	lights[1].mode = F_LHT_SPOT;
	memcpy(&lights[1].position, &camera->position, sizeof(vec3));
	memcpy(&lights[1].direction, &camera->front, sizeof(vec3));
	lights[1].cutOffInner = 15.f;
	lights[1].cutOffOuter = 17.f;
	lights[1].ambient[0] = .75f;
	lights[1].ambient[1] = .75f;
	lights[1].ambient[2] = .75f;
	lights[1].diffuse[0] = .75f;
	lights[1].diffuse[1] = .75f;
	lights[1].diffuse[2] = .75f;
	lights[1].specular[0] = 1.f;
	lights[1].specular[1] = 1.f;
	lights[1].specular[2] = 1.f;
	lights[1].range = 100.f;

	// Spot Light
	lights[2].enable = true;
	strcpy(lights[2].name, "Spot");
	lights[2].mode = F_LHT_SPOT;
	lights[2].position[0] = 0.f;
	lights[2].position[1] = 15.f;
	lights[2].position[2] = 0.f;
	lights[2].direction[0] = 0.f;
	lights[2].direction[1] = -1.f;
	lights[2].direction[2] = 0.f;
	lights[2].cutOffInner = 20.f;
	lights[2].cutOffOuter = 25.f;
	lights[2].ambient[0] = .5f;
	lights[2].ambient[1] = .5f;
	lights[2].ambient[2] = .5f;
	lights[2].diffuse[0] = .75f;
	lights[2].diffuse[1] = .75f;
	lights[2].diffuse[2] = .75f;
	lights[2].specular[0] = 1.f;
	lights[2].specular[1] = 1.f;
	lights[2].specular[2] = 1.f;
	lights[2].range = 200.f;

	mat4 view, projection, identity;

	glm_mat4_identity(identity);
	glm_mat4_identity(view);
	glm_mat4_identity(projection);
	printf("Starting main loop\n");

	while (!glfwWindowShouldClose(window))
	{
		// Update/Input
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		memcpy(&lights[1].position, &camera->position, sizeof(vec3));
		memcpy(&lights[1].direction, &camera->front, sizeof(vec3));

		// Light color
		vec3 lightColor = {1.f, 1.f, 1.f};
		lightColor[0] = sinf(currentFrame * 2.f) * .5f + .5f;
		lightColor[1] = sinf(currentFrame * .7f) * .5f + .5f;
		lightColor[2] = sinf(currentFrame * 1.3f) * .5f + .5f;

		glm_vec3_scale(lightColor, .5f, lights[2].ambient);
		glm_vec3_scale(lightColor, .75f, lights[2].diffuse);
		memcpy(&lights[2].specular, &lightColor, sizeof(vec3));

		//lights[2].position[0] = 1.2f * cosf(currentFrame);
		lights[2].position[0] = sinf(currentFrame) * 4.f;
		lights[2].position[2] = cosf(currentFrame) * 4.f;

		guiUpdate();

		processInput(window);

		// Render
		if (postProcessing)
		{
			framebufferClear(framebuffer);
			framebufferBindToDraw(framebuffer);
			glEnable(GL_DEPTH_TEST);
		} else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_DEPTH_TEST);
		}

		glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm_perspective(RAD(camera->fov), (float) framebuffer->width / (float) framebuffer->height, camera->near, camera->far, projection);
		cameraGetViewMatrix(camera, &view);

		// lights & models affected by lights
		glUseProgram(shaderLighting);
		setUniform1i(&shaderLighting, "u_isInstance", 0);
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			char lightParam[23];
			sprintf(lightParam, "u_lights[%d].enable", i);
			setUniform1i(&shaderLighting, lightParam, lights[i].enable);
			if (!lights[i].enable)
				continue;

			sprintf(lightParam, "u_lights[%d].mode", i);
			setUniform1i(&shaderLighting, lightParam, lights[i].mode);
			sprintf(lightParam, "u_lights[%d].position", i);
			setUniform3fv(&shaderLighting, lightParam, lights[i].position);
			sprintf(lightParam, "u_lights[%d].direction", i);
			setUniform3fv(&shaderLighting, lightParam, lights[i].direction);
			sprintf(lightParam, "u_lights[%d].cutOffInner", i);
			setUniform1f(&shaderLighting, lightParam, cosf(RAD(lights[i].cutOffInner)));
			sprintf(lightParam, "u_lights[%d].cutOffOuter", i);
			setUniform1f(&shaderLighting, lightParam, cosf(RAD(lights[i].cutOffOuter)));

			sprintf(lightParam, "u_lights[%d].ambient", i);
			setUniform3fv(&shaderLighting, lightParam, lights[i].ambient);
			sprintf(lightParam, "u_lights[%d].diffuse", i);
			setUniform3fv(&shaderLighting, lightParam, lights[i].diffuse);
			sprintf(lightParam, "u_lights[%d].specular", i);
			setUniform3fv(&shaderLighting, lightParam, lights[i].specular);

			sprintf(lightParam, "u_lights[%d].constant", i);
			setUniform1f(&shaderLighting, lightParam, 1.f);
			sprintf(lightParam, "u_lights[%d].linear", i);
			setUniform1f(&shaderLighting, lightParam, 4.5f / lights[i].range);
			sprintf(lightParam, "u_lights[%d].quadratic", i);
			setUniform1f(&shaderLighting, lightParam, 75.f / (lights[i].range * lights[i].range));
		}

		setUniform3fv(&shaderLighting, "u_viewPos", camera->position);

		// setUniform1f(&shaderLighting, "u_time", currentFrame);

		setUniformMatrix4fv(&shaderLighting, "u_view", (GLfloat*) view);
		setUniformMatrix4fv(&shaderLighting, "u_projection", (GLfloat*) projection);

		// Cube
		glBindTextureUnit(0, diffuseTexture);
		glBindTextureUnit(1, specularTexture);
		// glBindTextureUnit(2, emissionMap);
		glBindTextureUnit(2, skyboxTexture);

		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, (vec3){0.f, -8.f, 0.f});
		glm_scale(model, (vec3){20.f, .5f, 20.f});
		setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		glBindVertexArray(meshCube->vao);
		glDrawArrays(GL_TRIANGLES, 0, meshCube->numVertices);

		// glBindVertexArray(meshMonkey->vao);
		// for (int i = 0; i < instanceAmount; i++)
		// {
		// 	glm_mat4_identity(model);
		// 	glm_translate(model, instancePositions[i]);
		// 	float angle = 20.f * (float) i;
		// 	if (i % 2 == 0)
		// 		angle += currentFrame * (40.f + (float) i * 40.f);
		// 	glm_rotate(model, RAD(angle), (vec3){1.f, .3f, .5f});
		// 	// glm_mat4_scale(model, .5f);
		// 	glm_scale(model, (vec3){.5f, .5f, .5f});
		// 	setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		// glDrawArrays(GL_TRIANGLES, 0, meshMonkey->numVertices);
		// }

		setUniform1i(&shaderLighting, "u_isInstance", 1);
		glBindVertexArray(meshInstance->vao);
		glDrawArraysInstanced(GL_TRIANGLES, 0, meshInstance->numVertices, instanceAmount);
		setUniform1i(&shaderLighting, "u_isInstance", 0);

		// Exploding monkey
		glUseProgram(shaderGeomExplode);
		glBindTextureUnit(0, diffuseTexture);
		setUniform1f(&shaderGeomExplode, "u_time", currentFrame);

		setUniformMatrix4fv(&shaderGeomExplode, "u_projection", (GLfloat*) projection);
		setUniformMatrix4fv(&shaderGeomExplode, "u_view", (GLfloat*) view);

		glBindVertexArray(meshMonkey->vao);
		glm_mat4_identity(model);
		glm_translate(model, (vec3){-5.f, 10.f, 0.f});
		setUniformMatrix4fv(&shaderGeomExplode, "u_model", (GLfloat*) model);
		glDrawArrays(GL_TRIANGLES, 0, meshMonkey->numVertices);

		// Spiky monkey
		glUseProgram(shaderLighting);
		glBindVertexArray(meshMonkey->vao);
		glm_mat4_identity(model);
		glm_translate(model, (vec3){5.f, 10.f, 0.f});
		glm_rotate(model, currentFrame, (vec3){0.f, 1.f, 0.f});
		setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		glDrawArrays(GL_TRIANGLES, 0, meshMonkey->numVertices);

		glUseProgram(shaderGeomNormals);
		setUniformMatrix4fv(&shaderGeomNormals, "u_projection", (GLfloat*) projection);
		setUniformMatrix4fv(&shaderGeomNormals, "u_view", (GLfloat*) view);
		setUniformMatrix4fv(&shaderGeomNormals, "u_model", (GLfloat*) model);
		glDrawArrays(GL_TRIANGLES, 0, meshMonkey->numVertices);

		// Lamp
		glUseProgram(shaderSingleColor);
		setUniform3fv(&shaderSingleColor, "u_color", lightColor);

		setUniformMatrix4fv(&shaderSingleColor, "u_view", (GLfloat*) view);
		setUniformMatrix4fv(&shaderSingleColor, "u_projection", (GLfloat*) projection);

		glm_mat4_identity(model);
		glm_translate(model, lights[2].position);
		// glm_mat4_scale(model, .2f);
		glm_scale(model, (vec3){.2f, .2f, .2f});
		setUniformMatrix4fv(&shaderSingleColor, "u_model", (GLfloat*) model);
		glBindVertexArray(meshCube->vao);
		glDrawArrays(GL_TRIANGLES, 0, meshCube->numVertices);

		// skybox
		glDepthFunc(GL_LEQUAL);
		glUseProgram(shaderSkybox);
		mat4 skyboxView;
		glm_mat4_copy(view, skyboxView);
		mat3 viewMat3;
		glm_mat4_pick3(skyboxView, viewMat3);
		glm_mat4_identity(skyboxView);
		glm_mat4_ins3(viewMat3, skyboxView);
		setUniformMatrix4fv(&shaderSkybox, "u_view", (GLfloat*) skyboxView);
		setUniformMatrix4fv(&shaderSkybox, "u_projection", (GLfloat*) projection);

		glBindTextureUnit(0, skyboxTexture);
		glBindVertexArray(vaoSkybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glBindVertexArray(meshCube->vao);
		glDrawArrays(GL_TRIANGLES, 0, meshCube->numVertices);

		// grass
		glUseProgram(shaderLighting);
		glBindTextureUnit(0, grassTexture);
		glBindTextureUnit(1, grassSpecularTexture);

		glm_mat4_identity(model);
		glm_translate(model, (vec3){0.f, -6.f, 0.f});
		// glm_mat4_scale(model, 2.f);
		glm_scale(model, (vec3){2.f, 2.f, 2.f});
		setUniformMatrix4fv(&shaderLighting, "u_model", (GLfloat*) model);
		glBindVertexArray(vaoPlaneCross);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (postProcessing)
		{
			framebufferCopyToDefault(framebuffer);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			glClearColor(1.f, 1.f, 1.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(shaderQuadTexture);
			glBindVertexArray(vaoQuad);
			glBindTextureUnit(0, framebuffer->colorTex);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		guiRender();

		// Swap buffers & poll IO
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	printf("Cleaning up\n");
	guiTerminate();
	cameraDelete(camera);

	glDeleteVertexArrays(1, &vaoPlaneCross);
	glDeleteBuffers(1, &vboPlaneCross);

	glDeleteVertexArrays(1, &vaoQuad);
	glDeleteBuffers(1, &vboQuad);

	glDeleteVertexArrays(1, &vaoSkybox);
	glDeleteBuffers(1, &vboSkybox);

	glDeleteBuffers(1, &instanceBuffer);
	free(modelMatrices);

	meshDestroy(meshMonkey);
	meshDestroy(meshCube);
	meshDestroy(meshInstance);

	glDeleteProgram(shaderLighting);
	glDeleteProgram(shaderSingleColor);
	glDeleteProgram(shaderQuadTexture);
	glDeleteProgram(shaderSkybox);

	glDeleteProgram(shaderGeomExplode);
	glDeleteProgram(shaderGeomNormals);

	glDeleteTextures(1, &diffuseTexture);
	glDeleteTextures(1, &specularTexture);

	glDeleteTextures(1, &grassTexture);
	glDeleteTextures(1, &grassSpecularTexture);

	glDeleteTextures(1, &skyboxTexture);

	framebufferDestroy(framebuffer);

	glfwDestroyWindow(window);

	exit(EXIT_SUCCESS);
}

void errorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void framebufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
	framebufferResize(framebuffer, width, height);
	glViewport(0, 0, width, height);
	//	printf("Set viewport size to (%d,%d)\n", width, height);
}

void processInput(GLFWwindow *window)
{
	if (!mouseCaptured)
		return;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraMoveForward(camera, cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraMoveBackward(camera, cameraSpeed * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraMoveLeft(camera, cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraMoveRight(camera, cameraSpeed * deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraMoveUp(camera, cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraMoveDown(camera, cameraSpeed * deltaTime);
}

void keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (mouseCaptured)
		{
			mouseCaptured = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else
		{
			mouseCaptured = true;
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
		firstMouse = false;
	}

	const float xOffset = xPos - lastX;
	const float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	cameraProcessMouse(camera, xOffset * mouseSensitivity, yOffset * mouseSensitivity);
}

void scrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
{
	if (!mouseCaptured)
		return;

	camera->fov -= (float) yOffset;
	if (camera->fov < 1.f)
		camera->fov = 1.f;
	if (camera->fov > 90.f)
		camera->fov = 90.f;
}

void guiInit(GLFWwindow* window)
{
	imguiCtx = igCreateContext(NULL);
	imguiIO = igGetIO_ContextPtr(imguiCtx);

	const char* glslVersion = "#version 330 core";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	igStyleColorsDark(NULL);
}

void guiRender()
{
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}

void guiTerminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(imguiCtx);
}

void guiLightSettings(const char* label, light_t* light)
{
	if (igCollapsingHeader_BoolPtr(label, NULL, 0))
	{
		igCheckbox("Enable", &light->enable);
		if (light->enable)
		{
			igInputText("Name", light->name, sizeof(light->name), 0, NULL, NULL);

			const char* lightModes[] = {"Direct", "Point", "Spot"};
			int lightCurrent = light->mode - 1;
			if (igListBox_Str_arr("Mode", &lightCurrent, lightModes, 3, 3))
				light->mode = lightCurrent + 1;

			igSeparator();
			igInputFloat3("Position", light->position, "%.3f", 0);
			igSliderFloat3("Direction", light->direction, -1.f, 1.f, "%.2f", 0);

			igSliderFloat("Cut Off Inner", &light->cutOffInner, 0.f, 180.f, "%.2f", 0);
			igSliderFloat("Cut Off Outer", &light->cutOffOuter, 0.f, 180.f, "%.2f", 0);

			igSeparator();
			igColorEdit3("Ambient", light->ambient, 0);
			igColorEdit3("Diffuse", light->diffuse, 0);
			igColorEdit3("Specular", light->specular, 0);

			igSeparator();
			igInputFloat("Range", &light->range, 10.f, 100.f, "%.2f", 0);
		}
	}
}

void guiUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	// igShowDemoWindow(NULL);

	if (!igBegin("Info", NULL, 0))
	{
		igEnd();
		return;
	}

	igText("FPS: %f", imguiIO->Framerate);
	igText("Delta: %f", deltaTime);
	if (igCheckbox("Vsync", &vsync))
		glfwSwapInterval(vsync);

	igSeparator();
	if (igCollapsingHeader_BoolPtr("Camera", NULL, 0))
	{
		igColorEdit3("Clear Color", clearColor, 0);
		igCheckbox("Post Processing", &postProcessing);

		igSeparator();
		igDragFloat("Speed", &cameraSpeed, .1f, .1f, 20.f, "%.2f", 0);
		igDragFloat("Mouse Sensitivity", &mouseSensitivity, .05f, .05f, 1.f, "%.2f", 0);
		igDragFloat("FOV", &camera->fov, 1.f, 1.f, 90.f, "%.1f", 0);
		igDragFloatRange2("Near/Far", &camera->near, &camera->far, 1.f, .1f, 1000.f, "%.1f", "%.1f", 0);
	}

	if (igCollapsingHeader_BoolPtr("Lights", NULL, 0))
	{
		igText("Settings for lights in scene");
		igSeparator();
		// guiLightSettings("Sun", &lightSun);
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			// if (!lights[i].enable)
			// 	continue;
			igPushID_Int(i);
			char label[7];
			sprintf(label, "Light%d", i);
			guiLightSettings(label, &lights[i]);
			igPopID();
		}
	}

	// const float values[10] = {0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f};
	// igPlotLines_FnFloatPtr()
	// igPlotLines_FloatPtr("FPS Plot", &values, 10, 1, "Overlay", 0.f, 10.f, (ImVec2){0.f, 100.f}, 1);

	// igText("Camera pos (%f, %f, %f)", camera->position[0], camera->position[1], camera->position[2]);
	// igButton("Test", (ImVec2){0.f, 0.f});
	// igSeparatorText("Camera");
	// igBulletText("Position (%f, %f, %f)", camera->position[0], camera->position[1], camera->position[2]);

	igEnd();
}