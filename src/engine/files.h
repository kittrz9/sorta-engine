#ifndef FILES_H
#define FILES_H

#include <stdint.h>

typedef struct {
	void* buffer;
	uint32_t size;
} gameFile; // probably shouldn't just name the type "file" to avoid confusing it with "FILE" from stdio

// need to free afterwards
gameFile readGameFile(const char* filename);

gameFile readFileGZ(const char* filename);
gameFile readFileBZ2(const char* filename);

#endif
