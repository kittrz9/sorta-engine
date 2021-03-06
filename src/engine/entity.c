#include "entity.h"

#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

entListNode* entListHead = NULL;
entListNode* entListTail = NULL;
entListNode* entListCurrent = NULL;

void pushToEntityList(entity* ent){
	if(entListHead == NULL) {
		debugLog(LOG_NORMAL, "initializing entity list\n");
		entListHead = malloc(sizeof(entListNode));
		entListTail = entListHead;
		entListCurrent = entListHead;
		entListCurrent->prev = NULL;
	} else {
		// pushes to the end of the list if it is already initialized
		entListTail->next = malloc(sizeof(entListNode));
		entListTail->next->prev = entListTail;
		entListTail = entListTail->next;
		entListCurrent = entListTail;
	}
	
	entListCurrent->ent = ent;
	entListCurrent->next = NULL;
	debugLog(LOG_NORMAL, "entity %p pushed to list at node %p\n", (void*)ent, (void*)entListCurrent);
	
	return;
}

void removeEntity(entity* ent){
	if(entListHead == NULL){
		debugLog(LOG_ERROR, "entity list is empty, cannot remove entity %p", (void*)ent);
		return;
	}
	
	// if there is only one entity in the list
	if(entListHead == entListTail){
		free(entListHead);
		entListHead = NULL;
		entListTail = NULL;
		entListCurrent = NULL;
		
		return;
	}
	
	// loop through the entire list to find the entity to be removed
	for(entListCurrent = entListHead; entListCurrent != NULL; entListCurrent = entListHead){
		if(entListCurrent->ent == ent){
			// free entity
			(*ent->destructor)();
			free(ent->object);
			free(ent);
			
			// free entity list node
			entListNode* temp;
			// handling entities at the ends of the list specifically because they're weird
			if(entListCurrent == entListHead){
				entListHead->next->prev = NULL;
				temp = entListHead->next;
				free(entListHead);
				entListHead = temp;
				return;
			}
			if(entListCurrent == entListTail){
				entListTail->prev->next = NULL;
				temp = entListTail->prev;
				free(entListTail);
				entListTail = temp;
				return;
			}
			
			entListCurrent->prev->next = entListCurrent->next;
			entListCurrent->next->prev = entListCurrent->prev;
			free(entListCurrent);
			
			return;
		}
	}
	
	debugLog(LOG_ERROR, "could not find entity at %p in the list\n", (void*)ent);
}

void destroyEntityList(){
	entListNode* temp;
	
	for(entListCurrent = entListHead; entListCurrent != NULL;  ){
		if(entListHead == entListTail){
			free(entListHead);
			return;
		}
		temp = entListCurrent->next;
		free(entListCurrent);
		entListCurrent = temp;
	}
	return;
}
