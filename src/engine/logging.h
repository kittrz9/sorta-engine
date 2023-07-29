#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>

typedef enum {
	LOG_NORMAL,
	LOG_SUCCESS,
	LOG_WARN,
	LOG_ERROR,
} LOG_TYPE;

// will not print anything lower than logPriority (i.e. will not print LOG_WARN and below when set to LOG_ERROR)
extern LOG_TYPE logPriority;

void initLogFile(void);
void closeLogFile(void);

// naming it this so it doesn't conflict with the log fucntion from math.h
void debugLog(LOG_TYPE type, const char* fmt, ...);

#endif
