/*
 * Created by Duncan on 05/04/2025.
 */

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"

typedef struct model_t
{
	array_texture_t_t* texturesLoaded;
	array_mesh_t_t* meshes;
	char* directory;
	int gammaCorrection;
} model_t;

model_t* modelCreate(const char* path, int gammaCorrection);
void modelDestroy(model_t* model);
void modelDraw(const model_t* model, const GLuint* shader);

#endif //MODEL_H
