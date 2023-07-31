#include "wavLoader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "logging.h"
#include "resourceManager.h"
#include "audio.h"
#include "files.h"

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
	[CHUNK_RIFF] = sizeof(RIFFHeader),
	[CHUNK_FMT]  = sizeof(fmtHeader),
	[CHUNK_DATA] = sizeof(dataHeader),
	[CHUNK_LIST] = 0,
	[CHUNK_INFO] = 0,
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

	*outID = id;

	switch(id) {
		case CHUNK_LIST:
			size = ((listHeader*)chunk)->chunkSize;
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


	return size;
}

resource* loadWav(const char* filename) {
	resource* tempRes = checkIfAlreadyLoaded(filename);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));


	gameFile file = readGameFile(filename, false);
	uint8_t* wavFile = file.buffer;
	if(wavFile == NULL) {
		exit(1);
	}


	// parse wav
	size_t offset = 0;
	CHUNK_ID id;
	// initializing these values so the compiler doesn't yell about it
	fmtHeader formatH = {.nSamplesPerSec = 0, .wBitsPerSample = 0};
	RIFFHeader riffh;
	int16_t* audioData = NULL; // audio data to be returned
	uint32_t dataLength = 0;
	size_t dataSize = 0;
	int16_t* dataPointer = NULL; // pointer to the data in the file in memory before converted to stereo
	uint8_t stride = 1;
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
				if(formatH.nSamplesPerSec != SAMPLE_RATE) {
					debugLog(LOG_WARN, "sample rate %i is not the same as the current playback rate %i, expect the sample to be mangled during playback\n", formatH.nSamplesPerSec, SAMPLE_RATE);
				}

				if(formatH.wBitsPerSample != 16) {
					debugLog(LOG_ERROR, "wavs with formats other than int16 are not supported\n");
					return 0;
				}

				break;
			case CHUNK_DATA:
				dataPointer = (int16_t*)((uint8_t*)wavFile+offset+sizeof(dataHeader));
				dataSize = chunkSize - sizeof(dataHeader);
				stride = 1;

				// copy each sample point twice if mono to get stereo
				if(formatH.nChannels == 1) { stride = 2; }
				dataSize *= stride;

				audioData = malloc(sizeof(int16_t)*dataSize);
				size_t i = 0;
				while(i < dataSize / sizeof(int16_t)) {
					int16_t input = *(dataPointer + i/stride);
					for(uint8_t j = 0; j < stride; ++j) {
						*(audioData + i + j) = input;
						i += stride;
					}
				}

				//memcpy(audioData, wavFile+offset+sizeof(dataHeader), dataSize);
				dataLength = dataSize/sizeof(int16_t);
				break;
			default:
				break;
		}
		offset += chunkSize;
	} while(id != CHUNK_UNKNOWN);
	free(wavFile);

	if(audioData == NULL) {
		debugLog(LOG_ERROR, "could not load audio data from \"%s\"\n", filename);
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
