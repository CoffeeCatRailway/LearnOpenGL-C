/*
 * Created by Duncan on 05/04/2025.
 */

#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "util.h"

camera_t* cameraCreate(const vec3 pos, const float yaw, const float pitch, const float pitchConstraint, const float fov, const float near, const float far)
{
	camera_t* camera = (camera_t*) malloc(sizeof(camera_t));
	memcpy(camera->position, pos, sizeof(float) * 3); // vec3 is a float array of 3

	camera->yaw = yaw;
	camera->pitch = pitch;
	camera->pitchConstraint = pitchConstraint;

	camera->fov = fov;
	camera->near = near;
	camera->far = far;

	cameraUpdateVectors(camera);
	return camera;
}

void cameraDelete(camera_t* camera)
{
	free(camera);
}

void cameraGetViewMatrix(camera_t* camera, mat4* view)
{
	vec3 center;
	glm_vec3_add(camera->position, camera->front, center);
	glm_lookat(camera->position, center, camera->up, *view);
}

void cameraMoveForward(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->front, delta, step);
	glm_vec3_add(camera->position, step, camera->position);
}

void cameraMoveBackward(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->front, delta, step);
	glm_vec3_sub(camera->position, step, camera->position);
}

void cameraMoveLeft(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->right, delta, step);
	glm_vec3_sub(camera->position, step, camera->position);
}

void cameraMoveRight(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->right, delta, step);
	glm_vec3_add(camera->position, step, camera->position);
}

void cameraMoveUp(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->up, delta, step);
	glm_vec3_add(camera->position, step, camera->position);
}

void cameraMoveDown(camera_t* camera, const float delta)
{
	vec3 step;
	glm_vec3_scale(camera->up, delta, step);
	glm_vec3_sub(camera->position, step, camera->position);
}

void cameraProcessMouse(camera_t* camera, const float xOffset, const float yOffset)
{
	camera->yaw += xOffset;
	camera->pitch += yOffset;

	if (camera->pitchConstraint > 0.f)
	{
		if (camera->pitch > camera->pitchConstraint)
			camera->pitch = camera->pitchConstraint;
		if (camera->pitch < -camera->pitchConstraint)
			camera->pitch = -camera->pitchConstraint;
	}

	cameraUpdateVectors(camera);
}

void cameraUpdateVectors(camera_t* camera)
{
	vec3 front;
	front[0] = cosf(RAD(camera->yaw)) * cosf(RAD(camera->pitch));
	front[1] = sinf(RAD(camera->pitch));
	front[2] = sinf(RAD(camera->yaw)) * cosf(RAD(camera->pitch));
	glm_vec3_normalize_to(front, camera->front);
	glm_vec3_cross(camera->front, GLOBAL_UP, camera->right);
	glm_vec3_normalize(camera->right);
	glm_vec3_cross(camera->right, camera->front, camera->up);
	glm_vec3_normalize(camera->up);
}
