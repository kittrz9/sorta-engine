#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/resourceManager.h"

#include "portaudio.h"

#define SAMPLE_RATE 44100
#define AUDIO_CHANNELS 8

// most of this is literally copied and pasted from the SDL Project Base thing I made that also had a synthesizer
// probably should work on making a better synthesizer or maybe even just emulating an old sound chip lmao
// emulating something like the SPC700 or whatever for this could be fun and I probably wouldn't need to work on making a sequencer or tracker for this thing

typedef float (*synthFunc)(double);

// Where the envelope and synth function to be used with the synthesizer is defined
// idk if this is actually better or worse than having all of this just be in the data struct but I think it might make making a sequencer easier since you wont need to redefine the envelope and stuff every time you want a note played
typedef struct {
	// attack decay and release are all in seconds, sustain is between 0 and 1
	// attack decay and release were changed to take seconds because it seemed weird to have the values change with longer notes after thinking about it a bit
	struct { float attack, decay, sustain, release; } envelope;
	synthFunc synth; // Could probably set this by having an enum to represent all the synth functions and assign this function with that in the sequencer file format or whatever
} synthInstrument;

typedef struct {
	float startFreq, endFreq; // Should probably make it so that if endFreq is like 0 or something it would just not do the sweep thing
	float length; // Length of the sound in seconds
	float volume;
	
	int channel; // negative makes it play in the first open audio channel, if greater than the amount of audio channels doesn't play
	
	synthInstrument* instrument; // seperate struct becuase I think it might make making a sequencer a lot less bad since you wont need to constantly redefine the envelope and stuff for every note making the file extremely large
} synthData;

bool playSynth(synthData* data);

typedef struct {
	float* data;
	uint32_t dataLength;
	uint32_t sampleRate;
} audioSample;

bool playSample(resource* sample, float volume, float resampleFactor);
bool stopSample(resource* sample);

// portaudio callback function
int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);

void initAudio(void);
void uninitAudio(void);

float synthSine(double time);
float synthSquare(double time);
float synthNoise(double time);
float synthSaw(double time);
float synthTriangle(double time);

#endif
