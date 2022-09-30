#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

typedef enum {
	RES_TYPE_TEXTURE,
	RES_TYPE_SHADER,
	RES_TYPE_FONT,
	RES_TYPE_ENUM_LENGTH,
} RESOURCE_TYPE;

void setResourceDir(char* path);
extern char* resourceDir;
extern unsigned int resDirStrLen;

typedef struct {
	RESOURCE_TYPE type;
	void* pointer; // pointer to where the actual loaded in thing is in memory
	char* name;
} resource;

resource* checkIfAlreadyLoaded(const char* filename);
void addResourceToList(RESOURCE_TYPE type, const char* name, resource* res);

// destroys every resource
void clearResourceList();

// include the header for every resource loader here
#include "resourceLoaders/textureLoader.h"
#include "resourceLoaders/shaderLoader.h"
#include "resourceLoaders/fontLoader.h"

#endif
