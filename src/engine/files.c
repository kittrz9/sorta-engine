#include "engine/files.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef NO_BZIP2_SUPPORT
#include <bzlib.h>
#endif

#ifndef NO_GZIP_SUPPORT
#include <zlib.h>
#endif

#include "engine/resourceManager.h"
#include "engine/logging.h"

gameFile readGameFile(const char* filename, bool isText) {
	if(strcmp(filename + (strlen(filename)-3), ".gz") == 0) {
		return readFileGZ(filename, isText);
	} else if (strcmp(filename + (strlen(filename)-4), ".bz2") == 0) {
		return readFileBZ2(filename, isText);
	}
	FILE* filePointer;
	uint32_t fileSize;
	void* buffer;
	char* fullResourcePath = malloc(resDirStrLen + strlen(filename) + 1);
	sprintf(fullResourcePath, "%s%s", resourceDir, filename);

	filePointer = fopen(fullResourcePath, "rb");
	free(fullResourcePath);
	if(!filePointer) {
		debugLog(LOG_ERROR, "could not open file \"%s\"\n", filename);
		return (gameFile){.buffer=NULL,.size=0};
	}

	fseek(filePointer, 0, SEEK_END);
	fileSize = ftell(filePointer);
	rewind(filePointer);

	if(isText) {
		buffer = malloc(fileSize+1);
		((char*)buffer)[fileSize] = '\0';
	} else {
		buffer = malloc(fileSize);
	}
	fread(buffer, fileSize, 1, filePointer);

	fclose(filePointer);

	return (gameFile){.buffer = buffer, .size = fileSize+1};
}


#ifndef NO_GZIP_SUPPORT
// TODO: figure out how to do this in a way that doesn't seem as weird
gameFile readFileGZ(const char* filename, bool isText) {
	gzFile file;
	char* fullResourcePath = malloc(resDirStrLen + strlen(filename) + 1);
	uint32_t fileSize;
	int readBytes;
	int error;

	sprintf(fullResourcePath, "%s%s", resourceDir, filename);
	file = gzopen(fullResourcePath, "rb");
	if(!file) {
		debugLog(LOG_ERROR, "could not open gzip file \"%s\"\n", filename);
		return (gameFile){.buffer=NULL,.size=0};
	}

	uint32_t bufferSize = 1024;
	uint8_t* buffer = malloc(bufferSize); 
	if(!buffer) {
		debugLog(LOG_ERROR, "got a malloc error when trying to alloc 1KiB for gzip decompression, probably ran out of memory\n");
		return (gameFile){.buffer=NULL,.size=0};
	}

	readBytes = gzread(file, buffer, bufferSize);
	const char* errstr;
	errstr = gzerror(file, &error);
	
	// really janky solution to not have to allocate a ton of space every time I want to decompress something
	while(error == Z_OK) {
		if(error != Z_OK && error != Z_BUF_ERROR) {
			debugLog(LOG_ERROR, "gzip error: \"%s\"\n", errstr);
			return (gameFile){.buffer=NULL,.size=0};
		}

		if(error == Z_BUF_ERROR) {
			break;
		}

		bufferSize *= 2;
		buffer = realloc(buffer, bufferSize);

		if(!buffer) {
			debugLog(LOG_ERROR, "got an error when trying to realloc while decompressing a gzip file, probably ran out of memory\n");
			return (gameFile){.buffer=NULL,.size=0};
		}

		readBytes = gzread(file, buffer+bufferSize/2, bufferSize/2);
		if((uint32_t)readBytes != bufferSize/2) {
			break;
		}
		errstr = gzerror(file, &error);
	}

	gzseek(file, 0, SEEK_CUR);

	fileSize = gztell(file);

	gzclose(file);

	void* allocatedBuf;
	if(isText) {
		allocatedBuf = malloc(fileSize+1);
		((char*)allocatedBuf)[fileSize] = '\0';
	} else {
		allocatedBuf = malloc(fileSize);
	}

	memcpy(allocatedBuf, buffer, fileSize);

	free(buffer);

	return (gameFile){.buffer = allocatedBuf, .size=fileSize};
}
#else
gameFile readFileGZ(UNUSED const char* filename, UNUSED bool isText) {
	debugLog(LOG_ERROR, "gzip decompression is not enabled\n");

	exit(1);
}
#endif

#ifndef NO_BZIP2_SUPPORT
gameFile readFileBZ2(const char* filename, bool isText) {
	FILE* file;
	uint32_t fileSize;
	int bzerror = -1;

	char* fullResourcePath = malloc(resDirStrLen + strlen(filename) + 1);
	sprintf(fullResourcePath, "%s%s", resourceDir, filename);

	file = fopen(fullResourcePath, "rb");
	if(!file) {
		debugLog(LOG_ERROR, "could not open bzip2 file \"%s\"\n", filename);
		return (gameFile){.buffer=NULL,.size=0};
	}

	free(fullResourcePath);

	BZFILE* bzFile;
	bzFile = BZ2_bzReadOpen(&bzerror, file, 0, 0, NULL, 0);
	if(!bzFile) {
		debugLog(LOG_ERROR, "bzip open error, %i\n", bzerror);
	}

	uint32_t bufferSize = 1024;
	uint8_t* buffer = malloc(bufferSize);
	if(!buffer) {
		debugLog(LOG_ERROR, "malloc error when trying to allocate 1KiB for the bzip2 decompression, probably ran out of memory somehow.");
	}

	fileSize = BZ2_bzRead(&bzerror, bzFile, buffer, bufferSize);
	if(bzerror < BZ_OK) {
		debugLog(LOG_ERROR, "bzip read error, %i\n", bzerror);
		return (gameFile){.buffer=NULL,.size=0};
	}
	if(bzerror == BZ_OK) {
		bufferSize *= 2;
		buffer = realloc(buffer, bufferSize);
	}
	while(bzerror != BZ_STREAM_END) {
		fileSize += BZ2_bzRead(&bzerror, bzFile, buffer+bufferSize/2, bufferSize/2);
		if(bzerror < BZ_OK) {
			debugLog(LOG_ERROR, "bzip read error, %i\n", bzerror);
			return (gameFile){.buffer=NULL,.size=0};
		}
		if(bzerror == BZ_OK) {
			bufferSize *= 2;
			buffer = realloc(buffer, bufferSize);
			if(!buffer) {
				debugLog(LOG_ERROR, "got an error when trying to realloc while decompressing a gzip file, probably ran out of memory\n");
			}
		}
	}

	BZ2_bzReadClose(NULL, bzFile);

	fclose(file);

	void* allocatedBuf;
	if(isText) {
		allocatedBuf = malloc(fileSize+1);
		((char*)allocatedBuf)[fileSize] = '\0';
	} else {
		allocatedBuf = malloc(fileSize);
	}

	memcpy(allocatedBuf, buffer, fileSize);

	free(buffer);

	return (gameFile){.buffer=allocatedBuf, .size=fileSize};
}
#else
gameFile readFileBZ2(UNUSED const char* filename, UNUSED bool isText) {
	debugLog(LOG_ERROR, "bzip2 decompression is not enabled\n");

	exit(1);
}
#endif




