/*
 * Created by Duncan on 05/04/2025.
 */

#include <stdlib.h>

#include "camera.h"
#include "util.h"

camera_t* cameraCreate(const vec3 pos, const float yaw, const float pitch, const float pitchConstraint)
{
	camera_t* camera = (camera_t*) malloc(sizeof(camera_t));
	memcpy(camera->position, pos, sizeof(float) * 3); // vec3 is a float array of 3
	camera->yaw = yaw;
	camera->pitch = pitch;
	camera->pitchConstraint = pitchConstraint;

	cameraUpdateVectors(camera);
	return camera;
}

void cameraDelete(camera_t* camera)
{
	free(camera);
}

void cameraGetViewMatrix(camera_t* camera, mat4x4* view)
{
	vec3 center;
	vec3_add(center, camera->position, camera->front);
	mat4x4_look_at(*view, camera->position, center, camera->up);
}

void cameraMoveForward(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->front, delta);
	vec3_add(camera->position, camera->position, step);
}

void cameraMoveBackward(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->front, delta);
	vec3_sub(camera->position, camera->position, step);
}

void cameraMoveLeft(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->right, delta);
	vec3_sub(camera->position, camera->position, step);
}

void cameraMoveRight(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->right, delta);
	vec3_add(camera->position, camera->position, step);
}

void cameraMoveUp(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->up, delta);
	vec3_add(camera->position, camera->position, step);
}

void cameraMoveDown(camera_t* camera, float delta)
{
	vec3 step;
	vec3_scale(step, camera->up, delta);
	vec3_sub(camera->position, camera->position, step);
}

void cameraProcessMouse(camera_t* camera, float xOffset, float yOffset)
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
	vec3_norm(camera->front, front);
	vec3_mul_cross(camera->right, camera->front, (vec3)GLOBAL_UP);
	vec3_norm(camera->right, camera->right);
	vec3_mul_cross(camera->up, camera->right, camera->front);
	vec3_norm(camera->up, camera->up);
}
