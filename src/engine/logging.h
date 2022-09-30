#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>

typedef enum {
	LOG_NORMAL,
	LOG_ERROR,
	LOG_SUCCESS,
} LOG_TYPE;

void initLogFile();
void closeLogFile();

// naming it this so it doesn't conflict with the log fucntion from math.h
void debugLog(LOG_TYPE type, const char* fmt, ...);

#endif
