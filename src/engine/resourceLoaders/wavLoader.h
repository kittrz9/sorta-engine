#ifndef WAV_LOADER
#define WAV_LOADER

#include "resourceManager.h"

resource* loadWav(const char* filePath);
void destroyWav(resource* res);

#endif
