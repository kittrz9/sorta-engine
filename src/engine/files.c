#include "files.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resourceManager.h"
#include "logging.h"

gameFile readGameFile(const char* filename) {
	FILE* filePointer;
	uint32_t fileSize;
	void* buffer;
	char* fullResourcePath = malloc(resDirStrLen + strlen(filename) + 1);
	sprintf(fullResourcePath, "%s%s", resourceDir, filename);

	filePointer = fopen(fullResourcePath, "r"); // should probably have some way to choose to read it as text
	free(fullResourcePath);
	if(!filePointer) {
		debugLog(LOG_ERROR, "could not open file \"%s\"\n", filename);
		return (gameFile){.buffer=NULL,.size=0};
	}

	fseek(filePointer, 0, SEEK_END);
	fileSize = ftell(filePointer);
	rewind(filePointer);

	buffer = malloc(fileSize);
	fread(buffer, fileSize, 1, filePointer);

	return (gameFile){.buffer = buffer, .size = fileSize};
}
