#ifndef FILES_H
#define FILES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	void* buffer;
	uint32_t size;
} gameFile; // probably shouldn't just name the type "file" to avoid confusing it with "FILE" from stdio

// need to free afterwards
// need to specify if you're reading text to make sure to allocate for the null byte
gameFile readGameFile(const char* filename, bool isText);

gameFile readFileGZ(const char* filename, bool isText);
gameFile readFileBZ2(const char* filename, bool isText);

#endif
