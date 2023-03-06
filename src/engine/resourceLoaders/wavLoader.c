#include "wavLoader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "resourceManager.h"
#include "audio.h"

typedef struct {
	char chunkID[4]; // "RIFF"
	uint32_t chunkSize;
	char waveID[4]; // "WAVE'
} RIFFHeader;

typedef struct {
	// assumes PCM fmt chunk
	char chunkID[4]; // "fmt "
	uint32_t chunkSize;
	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplesPerSec;
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;
} fmtHeader;

typedef struct {
	char chunkID[4]; // "data"
	uint32_t chunkSize;
} dataHeader;

typedef struct {
	char chunkID[4]; // "data"
	uint32_t chunkSize;
	char listType[4];
} listHeader;

typedef enum {
	CHUNK_RIFF,
	CHUNK_FMT,
	CHUNK_DATA,
	CHUNK_LIST,
	CHUNK_INFO,
	CHUNK_IDS_LENGTH,
	CHUNK_UNKNOWN,
} CHUNK_ID;

char* chunkIDStrings[] = {
	[CHUNK_RIFF] = "RIFF",
	[CHUNK_FMT]  = "fmt ",
	[CHUNK_DATA] = "data",
	[CHUNK_LIST] = "LIST",
	[CHUNK_INFO] = "INFO",
};

size_t chunkHeaderSizes[] = {
	sizeof(RIFFHeader),
	sizeof(fmtHeader),
	sizeof(dataHeader),
	0,
	0,
	0,
};

// returns the size of the chunk
size_t parseChunk(void* chunk, CHUNK_ID* outID) {
	CHUNK_ID id = CHUNK_UNKNOWN;
	size_t size;
	for(uint8_t i = 0; i < CHUNK_IDS_LENGTH; ++i) {
		if(strncmp(chunkIDStrings[i], chunk, 4) == 0) {
			id = i;
			break;
		}
	}
	if(id == CHUNK_UNKNOWN) {
		//printf("unknown chunk \"%.4s\"\n", (char*)chunk);
	}

	*outID = id;

	switch(id) {
		case CHUNK_LIST:
			size = ((listHeader*)chunk)->chunkSize;
			//printf("%.4s\n", ((listHeader*)chunk)->listType);
			size += *(int16_t*)((uint8_t*)chunk + 16) - 6;
			break;
		case CHUNK_INFO:
		case CHUNK_DATA:
			size = ((dataHeader*)chunk)->chunkSize;
			if(size%2==1) ++size; 
			break;
		default:
			size = chunkHeaderSizes[id];
			break;
	}

	//printf("chunk: %.4s\nsize: %li\n", chunkIDStrings[id], size);

	return size;
}

resource* loadWav(const char* filename) {
	resource* tempRes = checkIfAlreadyLoaded(filename);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));

	char* fullResourcePath = malloc(resDirStrLen + strlen(filename));
	sprintf(fullResourcePath, "%s%s", resourceDir, filename);

	FILE* fp;
	fp = fopen(fullResourcePath, "rb");
	if(!fp) {
		debugLog(LOG_ERROR, "could not open wav file \"%s\"\n", filename);
		return 0;
	}
	free(fullResourcePath);

	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);

	uint8_t* wavFile = malloc(size);
	fread(wavFile, size, 1, fp);


	// parse wav
	size_t offset = 0;
	CHUNK_ID id;
	// initializing these values so the compiler doesn't yell about it
	fmtHeader formatH = {.nSamplesPerSec = 0, .wBitsPerSample = 0};
	RIFFHeader riffh;
	dataHeader dataH;
	int16_t* audioData = NULL;
	uint32_t dataLength = 0;
	do {
		size_t chunkSize = parseChunk(wavFile+offset, &id);
		switch(id) {
			case CHUNK_RIFF:
				riffh = *(RIFFHeader*)(wavFile+offset);
				if(strncmp(riffh.waveID, "WAVE",4) != 0) {
					debugLog(LOG_ERROR, "wave file doesn't have a normal header\n");
					return 0;
				}
				break;
			case CHUNK_FMT:
				formatH = *(fmtHeader*)(wavFile+offset);
				break;
			case CHUNK_DATA:
				dataH = *(dataHeader*)(wavFile+offset);
				audioData = malloc(dataH.chunkSize - sizeof(dataHeader));
				memcpy(audioData, wavFile+offset+sizeof(dataHeader), chunkSize-sizeof(dataHeader));
				dataLength = (chunkSize-sizeof(dataHeader))/sizeof(int16_t);
				break;
			default:
				break;
		}
		offset += chunkSize;
	} while(id != CHUNK_UNKNOWN);
	free(wavFile);
	fclose(fp);

	if(audioData == NULL) {
		debugLog(LOG_ERROR, "could not load audio data from \"%s\"\n", filename);
		return 0;
	}

	if(formatH.nChannels != 2) {
		debugLog(LOG_ERROR, "support for non-stereo samples is not added yet\n");
		return 0;
	}

	if(formatH.nSamplesPerSec != SAMPLE_RATE) {
		debugLog(LOG_WARN, "sample rate %i is not the same as the current playback rate %i, expect the sample to be mangled during playback\n", formatH.nSamplesPerSec, SAMPLE_RATE);
	}

	if(formatH.wBitsPerSample != 16) {
		debugLog(LOG_ERROR, "wavs with formats other than int16 are not supported\n");
		return 0;
	}

	// load sample data and convert to floats
	audioSample* sample = malloc(sizeof(audioSample));
	sample->dataLength = dataLength;
	sample->data = malloc(sizeof(float) * dataLength);
	sample->sampleRate = formatH.nSamplesPerSec;
	for(uint32_t i = 0; i < dataLength; ++i) {
		sample->data[i] = ((float)audioData[i]/(float)INT16_MAX * 2.0);
	}

	res->pointer = sample;
	

	addResourceToList(RES_TYPE_SAMPLE, filename, res);
	free(audioData);

	return res;
}

void destroyWav(resource* res) {
	free(((audioSample*)res->pointer)->data);
	free(res->pointer);

	return;
}
