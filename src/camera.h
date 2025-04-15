/*
 * Created by Duncan on 05/04/2025.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#define GLOBAL_UP {0.f, 1.f, 0.f}

typedef struct camera_t
{
	vec3 position;
	vec3 front;
	vec3 right;
	vec3 up;
	float yaw;
	float pitch;
	float pitchConstraint;
} camera_t;

camera_t* cameraCreate(const vec3 pos, float yaw, float pitch, float pitchConstraint);
void cameraDelete(camera_t* camera);

void cameraGetViewMatrix(camera_t* camera, mat4* view);

void cameraMoveForward(camera_t* camera, float delta);
void cameraMoveBackward(camera_t* camera, float delta);

void cameraMoveLeft(camera_t* camera, float delta);
void cameraMoveRight(camera_t* camera, float delta);

void cameraMoveUp(camera_t* camera, float delta);
void cameraMoveDown(camera_t* camera, float delta);

void cameraProcessMouse(camera_t* camera, float xOffset, float yOffset);

void cameraUpdateVectors(camera_t* camera);

#endif //CAMERA_H
