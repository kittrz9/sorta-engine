#include "audio.h" 

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "logging.h"

#include "portaudio.h"

#define SAMPLE_RATE 44100
#define AUDIO_CHANNELS 8

// a lot of this is just based off the tutorial for portaudio lmao
// http://files.portaudio.com/docs/v19-doxydocs/writing_a_callback.html

// the synthesizer part of this is based on the synth code I made for the other thing
// https://github.com/kittrz9/SDL_ProjectBase/blob/main/src/engine/audio.c

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

void initAudio() {
	// initialize the list of active synths
	for(int i = 0; i < AUDIO_CHANNELS; i++){
		activeSynths[i].data = NULL;
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
	for(int i = 0; i < numDevices; i++){
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("Device %i: %i, %s, %i, %i\n", i, deviceInfo->structVersion, deviceInfo->name, deviceInfo->maxInputChannels, deviceInfo->maxOutputChannels);
		// uses the pulseaudio device because using the default device just doesn't work well
		if(strcmp(deviceInfo->name, "pulse") == 0){
			outputParameters.device = i;
		}
	}
	if(outputParameters.device == -1){
		debugLog(LOG_ERROR, "could not find pulseaudio device\n");
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

void uninitAudio() {
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


bool playSynth(synthData* data){
	// check for free audio channels
	int freeChannel = -1;
	for(int i = 0; i < AUDIO_CHANNELS; i++){
		if(activeSynths[i].data == NULL) {
			freeChannel = i;
			break;
		}
	}
	if(freeChannel == -1) {
		return false;
	}
	
	activeSynths[freeChannel].data = data;
	activeSynths[freeChannel].timeRemaining = data->length + data->instrument->envelope.release;
	activeSynths[freeChannel].currentFreq= data->startFreq;
	
	return true;
}

#if defined(WIN32) || defined(_WIN32)
#define _USE_MATH_DEFINES
#endif

#define PI  M_PI
#define PI2 (PI*2)

int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	paTestData* audioData = (paTestData*)userData;
	float* output = (float*)outputBuffer;
	(void)inputBuffer;
	
#define envelope (data->instrument->envelope)
#define time2 ((data->length+envelope.release) - activeSynths[i].timeRemaining)
	float amplitude = 0.1f;
	for(unsigned int i = 0; i < frameCount; i++){
		*output++ = audioData->leftPhase;
		*output++ = audioData->rightPhase;
		
		audioData->leftPhase = 0.0;
		for(int i = 0; i < AUDIO_CHANNELS; i++){
			if(activeSynths[i].data == NULL) { continue; }
			synthData* data = activeSynths[i].data;
			float freq = activeSynths[i].currentFreq;
			
			if(data->endFreq != 0 && time2 < data->length){
				activeSynths[i].currentFreq += (data->endFreq - data->startFreq)/(data->length * SAMPLE_RATE*1.5);
			} else if(data->endFreq != 0){
				activeSynths[i].currentFreq = data->endFreq;
			}
			
			if(time2 <= envelope.attack){
				amplitude = time2/envelope.attack;
			} else if(time2 <= envelope.attack + envelope.decay){
				amplitude = (((envelope.sustain-1.0)*(time2 - envelope.attack)))/(envelope.decay) + 1.0;
			} else if(time2 <= data->length){
				amplitude = envelope.sustain;
			} else {
				amplitude = (((0 - envelope.sustain)/(envelope.release))*(time2 - data->length)) + envelope.sustain;
			}
			
			audioData->leftPhase += data->instrument->synth(activeSynths[i].functionTime) * amplitude * data->volume * (1.0/AUDIO_CHANNELS);
			// I don't remember why these values work, I messed around with the values until they sounded right. Though 27.5 seems close to what SAMPLE_RATE/(frameCount * AUDIO_CHANNELS) would be (21.5) so idk maybe that's close
			activeSynths[i].timeRemaining -= 1.0/(frameCount*frameCount);
			activeSynths[i].functionTime += activeSynths[i].currentFreq/(frameCount*27.5);
			// probably unnecessary to have it always stay between 0 to tau since most sounds wont be played long enough for like weird floating point shenanigans 
			if(activeSynths[i].functionTime > PI2) { activeSynths[i].functionTime -= PI2; }
			if(activeSynths[i].timeRemaining < 0) {
				activeSynths[i].data = NULL;
				activeSynths[i].currentFreq = 0;
				continue;
			}
		}
		audioData->rightPhase = audioData->leftPhase;
	}
	
	return 0;
}

float synthSine(double time) {
	return sin(time);
}

float synthSquare(double time) {
	return (time < PI ? -1.0 : 1.0);
}
float synthNoise(double time) {
	return ((float)rand()/RAND_MAX);
}
float synthSaw(double time) {
	return (time/PI) - 1; // input is never above 2pi
}
float synthTriangle(double time){
	return (time < PI ? ((2*time)/PI) - 1.0 : 3.0 - ((2*time)/PI));
}
