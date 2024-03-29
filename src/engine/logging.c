#include "engine/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

FILE* logFile;

LOG_TYPE logMode = LOG_NORMAL;

const char* logAnsiColorStrings[] = {
	[LOG_NORMAL]  = "\x1b[97m", // LOG_NORMAL
	[LOG_ERROR]   = "\x1b[91m", // LOG_ERROR
	[LOG_SUCCESS] = "\x1b[92m", // LOG_SUCCESS
	[LOG_WARN]    = "\x1b[93m", // LOG_WARN
};

const char* logTypeStrings[] = {
	[LOG_NORMAL]  = "LOG: ",
	[LOG_ERROR]   = "ERROR: ",
	[LOG_SUCCESS] = "SUCCESS: ",
	[LOG_WARN]    = "WARNING: ",
};

void initLogFile(void){
	logFile = fopen("log-file.txt", "w");
	
	if(!logFile){
		printf("could not open log file\n");
	}
	
	return;
}

void closeLogFile(void){
	debugLog(LOG_NORMAL, "closing log file\n");
	fclose(logFile);
	
	return;
}

// variadic functions are weird
#ifdef DEBUG_EXTRA
void debugLogFunc(const char* file, const char* func, const int line, LOG_TYPE type, const char* fmt, ...) {
#else
void debugLog(LOG_TYPE type, const char* fmt, ...) {
#endif
	if(type < logMode) { return; }
	va_list args;
	va_start(args,fmt);
	va_list args2;
	va_copy(args2, args);
	
	// https://stackoverflow.com/questions/26910479/find-the-length-of-a-formatted-string-in-c
	int length = vsnprintf(NULL, 0, fmt, args);
	
	char* str = malloc(sizeof(char) * (strlen(logTypeStrings[type]) + length + 1));
	str[0] = '\0';
	
#if !defined(WIN32) && !defined(_WIN32)
	printf("%s", logAnsiColorStrings[type]);
#endif
	
	char* tempStr = malloc(sizeof(char)*(length+1));
	vsprintf(tempStr, fmt, args2);
	va_end(args);
	va_end(args2);
	strcat(str, logTypeStrings[type]);
	strcat(str, tempStr);
	printf("%s", str);
	if(logFile){
		fprintf(logFile, "%s", str);
	}
	if(type == LOG_ERROR) {
		fflush(logFile);
#if defined(WIN32) || defined(_WIN32)
		MessageBox(NULL, str, "Sorta Engine Error", MB_OK | MB_ICONERROR);
#endif
	}
	free(tempStr);
	free(str);

#ifdef DEBUG_EXTRA
	if(type > LOG_SUCCESS) { // only print out extra info if a warning or error
		printf("FILE \"%s\", FUNCTION \"%s\", LINE %i\n", file, func, line);
		if(logFile) {
			fprintf(logFile, "FILE \"%s\", FUNCTION \"%s\", LINE %i\n", file, func, line);
		}
	}
#endif

#if !defined(WIN32) && !defined(_WIN32)
	printf("%s", logAnsiColorStrings[LOG_NORMAL]);
#endif
	
	return;
}
