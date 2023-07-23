#ifndef ENTITY_H
#define ENTITY_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_PROPERTIES 128

// entity property stuff so that you don't need to do weird struct type casting stuff to get other entities properties
// should probably not be in this file since you'd need to change it for game specific things but whatever
typedef enum {
	// not actual properties, used when processing properties
	EMPTY_PROPERTY_SLOT = 0, // not used anymore, but probably could be used later when removing properties (idk if you would even want to remove properties though lmao)
	// vec2f
	PROPERTY_POSITION,
	PROPERTY_VELOCITY,
	PROPERTY_SIZE,
	LAST_PROPERTY_SLOT = 0xffff, // also not actually a property, used when looping through properties to not go through every single slot
} ENTITY_PROPERTY;

typedef struct {
	ENTITY_PROPERTY id; 
	void* data; // dynamically allocated data, no type checking lmao
} entityProperty;

typedef struct entity {
	entityProperty properties[MAX_PROPERTIES]; // for all normal properties that might be used by other properties, arbitrary size
	
	// could probably use the old method for properties that are only used for this method so you don't have to worry about having 2000 different property ids or whatever
	void* object;
	
	// function pointers to functions to draw and update the entity
	void (*draw)(struct entity*);
	void (*update)(struct entity*, double);
	
	// used when freeing the entity to like free resources that aren't being used anymore
	void (*destructor)();
	
	// having a pointer to the specific node in the entity list the entity is in would probably speed up things a lot since you wouldn't need to have to loop through the list to find it but the entListNode type requires the entity struct to be defined while also having the entity struct need to have the entListNode type defined
	

	bool markedForDeletion;
} entity;

typedef struct entListNode {
	entity* ent;
	struct entListNode* next;
	struct entListNode* prev;
} entListNode;

extern entListNode* entListHead;
extern entListNode* entListTail;
extern entListNode* entListCurrent;

void pushToEntityList(entity* ent);
void removeEntity(entity* ent);

void processEntities(double deltaTime);

void destroyEntityList();

void initializeEntityPropertyList(entity* ent);
entityProperty* createEntityProperty(entity* ent, ENTITY_PROPERTY property, size_t bytes);
void setEntityPropertyAddress(entity* ent, ENTITY_PROPERTY property, void* address);
entityProperty* getEntityPropertySlot(entity* ent, ENTITY_PROPERTY property);
// might not make a function to destroy entity properties since some might just point to something allocated in the object thing

// used to avoid needing to do a bunch of pointer typecasting
#define getEntityProperty(ent, property, type) ((type*)getEntityPropertySlot(ent, property)->data)

#endif
