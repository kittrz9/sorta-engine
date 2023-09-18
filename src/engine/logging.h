#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>

typedef enum {
	LOG_NORMAL,
	LOG_SUCCESS,
	LOG_WARN,
	LOG_ERROR,
} LOG_TYPE;

void initLogFile(void);
void closeLogFile(void);

#ifdef DEBUG_EXTRA
void debugLogFunc(const char* file, const char* func, const int line, LOG_TYPE type, const char* fmt, ...);

// TODO: figure out a way to do this without clang giving tons of warnings about that last part of the macro
#define debugLog(type, fmt, ...) debugLogFunc(__FILE__, __func__, __LINE__, type, fmt, ##__VA_ARGS__);

#else
// naming it this so it doesn't conflict with the log fucntion from math.h
void debugLog(LOG_TYPE type, const char* fmt, ...);

#endif

#endif
