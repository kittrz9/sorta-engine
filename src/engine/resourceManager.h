#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

typedef enum {
	RES_TYPE_TEXTURE,
	RES_TYPE_ENUM_LENGTH,
} RESOURCE_TYPE;

typedef struct {
	RESOURCE_TYPE type;
	void* pointer; // pointer to where the actual loaded in thing is in memory
} resource;

resource* loadResource(RESOURCE_TYPE type, const char* filePath);
void destroyResource(resource* res);

// destroys every resource
void clearResourceList();

#endif
