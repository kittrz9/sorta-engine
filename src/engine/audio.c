#include "engine/audio.h" 

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "engine/logging.h"
#include "engine/resourceManager.h"

#include "portaudio.h"


// a lot of this is just based off the tutorial for portaudio lmao
// http://files.portaudio.com/docs/v19-doxydocs/writing_a_callback.html

// the synthesizer part of this is based on the synth code I made for the other thing
// https://github.com/kittrz9/SDL_ProjectBase/blob/main/src/engine/audio.c

bool audioEnabled = true;

PaStream* stream;

typedef struct {
	float leftPhase;
	float rightPhase;
} paTestData;

static paTestData audioData;

typedef struct {
	synthData* data;
	float timeRemaining; // how much time is remaining for this synth
	float currentFreq; // the current frequency for this synth. starts at the start frequency given in the synthData and sweeps to the end frequency
	float functionTime; // what will be passed as the "time" parameter to the synth function 
} synthListEntry;

synthListEntry activeSynths[AUDIO_CHANNELS];

typedef struct {
	audioSample* sample;
	double offset;
	float volume;
	double resampleFactor;
} sampleListEntry;
sampleListEntry activeSamples[AUDIO_CHANNELS];

void initAudio(void) {
	if(!audioEnabled) {
		debugLog(LOG_NORMAL, "audio is currently disabled\n");
		return;
	}
	// initialize the list of active synths and samples
	for(int i = 0; i < AUDIO_CHANNELS; i++){
		activeSynths[i].data = NULL;
		activeSamples[i].sample = NULL;
	}
	
	PaError error;
	
	// initialize portaudio
	debugLog(LOG_NORMAL, "initializing portaudio\n");
	error = Pa_Initialize();
	if(error != paNoError) { 
		debugLog(LOG_ERROR, "could not initialize portaudio: \"%s\"\n", Pa_GetErrorText(error));
		return;
	}
	
	// get output device
	PaDeviceIndex outputDevice;
	PaStreamParameters outputParameters;
	int numDevices = Pa_GetDeviceCount();
	if(numDevices < 0){
		debugLog(LOG_ERROR, "Pa_CountDevices returned 0x%x: \"%s\"\n", numDevices, Pa_GetErrorText(numDevices));
		return;
	}
	
	outputParameters.device = -1;
	const PaDeviceInfo* deviceInfo;
#if !defined(_WIN32) && !defined(WIN32)
	for(uint8_t i = 0; i < numDevices; ++i) {
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("Device %i: %i, %s, %i, %i\n", i, deviceInfo->structVersion, deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
		if(strncmp(deviceInfo->name, "default", 7) == 0) {
			outputParameters.device = i;
			break;
		}
	}
#else
	const char* audioDevices[] = {
		"Speakers (High Definition Audio)", // can't test this again since my windows vm just decided it can't use opengl anymore lmao
		"Wine Sound Mapper - Output",
	};
	for(uint8_t d = 0; d < sizeof(audioDevices)/sizeof(audioDevices[0]); ++d) {
		for(int i = 0; i < numDevices; i++){
			deviceInfo = Pa_GetDeviceInfo(i);
			printf("Device %i: %i, %s, %i, %i\n", i, deviceInfo->structVersion, deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
			// pick default device
			if(strcmp(deviceInfo->name, audioDevices[d]) == 0) {
				outputParameters.device = i;
				break;
			}
		}
		if(outputParameters.device != -1) {
			break;
		}
	}
#endif
	if(outputParameters.device == -1){
		debugLog(LOG_ERROR, "could not find default audio device, disabling audio\n");
		Pa_Terminate();
		audioEnabled = false;
		return;
	}
	
	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;
	
	outputDevice = outputParameters.device;
	
	// open audio stream
	debugLog(LOG_NORMAL, "opening audio stream\n");
	error = Pa_OpenStream(&stream, NULL, &outputParameters, SAMPLE_RATE, 256, paNoFlag, audioCallback, &audioData);
	if(error != paNoError){
		debugLog(LOG_ERROR, "could not open audio stream\"%s\"", Pa_GetErrorText(error));
		return;
	}
	
	deviceInfo = Pa_GetDeviceInfo(outputDevice);
	printf("Using device %i: %i, %s, %i, %i\n", outputDevice, deviceInfo->structVersion, deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
	
	// start audio stream
	debugLog(LOG_NORMAL, "starting audio stream\n");
	if(Pa_StartStream(stream) != paNoError) {
		debugLog(LOG_ERROR, "could not start audio stream\n");
		return;
	}
	
	debugLog(LOG_SUCCESS, "audio successfully initialized\n");
	
	return;
}

void uninitAudio(void) {
	if(!audioEnabled) { return; }
	PaError error;
	
	debugLog(LOG_NORMAL, "uninitializing audio\n");
	
	debugLog(LOG_NORMAL, "stopping audio stream\n");
	error = Pa_StopStream(stream);
	if(error != paNoError) {
		debugLog(LOG_ERROR, "could not stop stream: \"%s\"\n", Pa_GetErrorText(error));
		return;
	}
	
	debugLog(LOG_NORMAL, "closing audio stream\n");
	error = Pa_CloseStream(stream);
	if(error != paNoError) {
		debugLog(LOG_ERROR, "could not close stream: \"%s\"\n", Pa_GetErrorText(error));
		return;
	}
	
	debugLog(LOG_NORMAL, "terminating portaudio\n");
	error = Pa_Terminate();
	if(error != paNoError) {
		debugLog(LOG_ERROR, "could not terminate portaudio: \"%s\"\n", Pa_GetErrorText(error));
		return;
	}
	
	debugLog(LOG_SUCCESS, "audio uninitialized\n");
	
	return;
}

bool playSample(resource* sample, float volume, float resampleFactor) {
	if(!audioEnabled) { return false; }
	int freeChannel = -1;
	for(int i = 0; i < AUDIO_CHANNELS; ++i) {
		if(activeSamples[i].sample == NULL) {
			freeChannel = i;
			break;
		}
	}
	if(freeChannel == -1) {
		return false;
	}
	activeSamples[freeChannel].sample = sample->pointer;
	activeSamples[freeChannel].offset = 0;
	activeSamples[freeChannel].volume = volume;
	activeSamples[freeChannel].resampleFactor = resampleFactor;
	return true;
}

bool stopSample(resource* sample) {
	if(!audioEnabled) { return false; }
	for(int i = 0; i < AUDIO_CHANNELS; ++i) {
		if(activeSamples[i].sample == sample->pointer) {
			activeSamples[i].sample = NULL;
			return true;
		}
	}
	return false;
}

bool playSynth(synthData* data){
	if(!audioEnabled) { return false; }
	int freeChannel = -1;
	if(data->channel < 0){
		// check for free audio channels
		for(int i = 0; i < AUDIO_CHANNELS; i++){
			if(activeSynths[i].data == NULL) {
				freeChannel = i;
				break;
			}
		}
		if(freeChannel == -1) {
			return false;
		}
	} else if(data->channel > AUDIO_CHANNELS){
		return false;
	} else if(activeSynths[data->channel].data != NULL){
		freeChannel = data->channel;
	}
	
	activeSynths[freeChannel].data = data;
	activeSynths[freeChannel].timeRemaining = data->length + data->instrument->envelope.release;
	activeSynths[freeChannel].currentFreq= data->startFreq;
	
	return true;
}

#if defined(WIN32) || defined(_WIN32)
#define _USE_MATH_DEFINES
#endif

// M_PI isn't standard apparently 
// https://en.wikibooks.org/wiki/C_Programming/math.h#Mathematical_constants_(not_standard)
#ifndef M_PI
#define PI 3.14159265358979323846f
#else
#define PI  M_PI
#endif

#define PI2 (PI*2)

int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, UNUSED const PaStreamCallbackTimeInfo* timeInfo, UNUSED PaStreamCallbackFlags statusFlags, void* userData) {
	paTestData* audioData = (paTestData*)userData;
	float* output = (float*)outputBuffer;
	(void)inputBuffer;
	
#define envelope (data->instrument->envelope)
// time synth is currently at
#define synthTime ((data->length+envelope.release) - activeSynths[i].timeRemaining)
	float amplitude = 0.1f;
	for(unsigned int i = 0; i < frameCount; i++){
		*output++ = audioData->leftPhase;
		*output++ = audioData->rightPhase;
		
		audioData->leftPhase = 0.0;
		audioData->rightPhase = 0.0;
		for(int i = 0; i < AUDIO_CHANNELS; i++){
			// process synths
			if(activeSynths[i].data != NULL) {
				synthData* data = activeSynths[i].data;
				
				if(data->endFreq != 0 && synthTime < data->length){
					activeSynths[i].currentFreq += (data->endFreq - data->startFreq)/(data->length * SAMPLE_RATE*1.5);
				} else if(data->endFreq != 0){
					activeSynths[i].currentFreq = data->endFreq;
				}
				
				if(synthTime <= envelope.attack){
					amplitude = synthTime/envelope.attack;
				} else if(synthTime <= envelope.attack + envelope.decay){
					amplitude = (((envelope.sustain-1.0)*(synthTime - envelope.attack)))/(envelope.decay) + 1.0;
				} else if(synthTime<= data->length){
					amplitude = envelope.sustain;
				} else {
					amplitude = (((0 - envelope.sustain)/(envelope.release))*(synthTime - data->length)) + envelope.sustain;
				}
				// this variable is here now so that it can be reused for both the left and right audio channels without recalculating, and also maybe later add in panning
				float synthOutput = data->instrument->synth(activeSynths[i].functionTime) * amplitude * data->volume * (1.0/AUDIO_CHANNELS);
				audioData->leftPhase += synthOutput;
				audioData->rightPhase += synthOutput;
				// I don't remember why these values work, I messed around with the values until they sounded right. Though 27.5 seems close to what SAMPLE_RATE/(frameCount * AUDIO_CHANNELS) would be (21.5) so idk maybe that's close
				activeSynths[i].timeRemaining -= 1.0/(frameCount*frameCount);
				activeSynths[i].functionTime += activeSynths[i].currentFreq/(frameCount*27.5);
				// probably unnecessary to have it always stay between 0 to tau since most sounds wont be played long enough for like weird floating point shenanigans 
				if(activeSynths[i].functionTime > PI2) { activeSynths[i].functionTime -= PI2; }
				if(activeSynths[i].timeRemaining < 0) {
					activeSynths[i].data = NULL;
					activeSynths[i].currentFreq = 0;
				}
			}
			
			// process samples
			if(activeSamples[i].sample != NULL) {
				// a lot of this stuff feels really janky and I'm not sure if everything works fine yet, and resampling could also probably be done with linear interpolation stuff to not be as weird
				// there was one bug that seemed to have to do with floating point imprecision and changing things to doubles *seemed* to have worked but I don't know for sure and it'll probably happen again with a long enough sample
				// I have no clue what I'm doing lmao
				uint64_t realOffset = (double)activeSamples[i].sample->sampleRate * activeSamples[i].sample->dataLength * activeSamples[i].offset;
				if(realOffset >= activeSamples[i].sample->dataLength) {
					activeSamples[i].sample = NULL;
					continue;
				}
				audioData->leftPhase += *(activeSamples[i].sample->data + realOffset) * activeSamples[i].volume;
				audioData->rightPhase += *(activeSamples[i].sample->data + realOffset + 1) * activeSamples[i].volume;
				activeSamples[i].offset += 2.0/SAMPLE_RATE/activeSamples[i].sample->dataLength * activeSamples[i].resampleFactor;
				//audioData->leftPhase += *(activeSamples[i]->data + sampleOffsets[i]++) * sampleVolumes[i];
				//audioData->rightPhase += *(activeSamples[i]->data + sampleOffsets[i]++) * sampleVolumes[i];
				if(activeSamples[i].offset >= 1.0f) {
					activeSamples[i].sample = NULL;
				}
			}
		}
	}
	
	return 0;
}

float synthSine(double time) {
	return sin(time);
}

float synthSquare(double time) {
	return (time < PI ? -1.0 : 1.0);
}
float synthNoise(UNUSED double time) {
	return ((float)rand()/(float)RAND_MAX);
}
float synthSaw(double time) {
	return (time/PI) - 1; // input is never above 2pi
}
float synthTriangle(double time){
	return (time < PI ? ((2*time)/PI) - 1.0 : 3.0 - ((2*time)/PI));
}
