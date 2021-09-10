#ifndef ENTITY_H
#define ENTITY_H

typedef struct entity {
	// pointer to where other properties of the object are stored
	void* object;
	
	// function pointers to functions to draw and update the entity
	void (*draw)(struct entity*);
	void (*update)(struct entity*, double);
	
	// used when freeing the entity to like free resources that aren't being used anymore
	void (*destructor)();
	
	// having a pointer to the specific node in the entity list the entity is in would probably speed up things a lot since you wouldn't need to have to loop through the list to find it but the entListNode type requires the entity struct to be defined while also having the entity struct need to have the entListNode type defined
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

void destroyEntityList();

#endif
