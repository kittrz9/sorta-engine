#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* logFile;

LOG_TYPE logMode = LOG_NORMAL;

const char* logAnsiColorStrings[] = {
	[LOG_NORMAL]  = "\033[97m", // LOG_NORMAL
	[LOG_ERROR]   = "\033[91m", // LOG_ERROR
	[LOG_SUCCESS] = "\033[92m", // LOG_SUCCESS
	[LOG_WARN]    = "\033[93m", // LOG_WARN
};

const char* logTypeStrings[] = {
	[LOG_NORMAL]  = "LOG: ",
	[LOG_ERROR]   ="ERROR: ",
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
void debugLog(LOG_TYPE type, const char* fmt, ...){
	if(type < logMode) { return; }
	va_list args;
	va_start(args,fmt);
	va_list args2;
	va_copy(args2, args);
	
	// https://stackoverflow.com/questions/26910479/find-the-length-of-a-formatted-string-in-c
	int length = vsnprintf(NULL, 0, fmt, args);
	
	char* str = malloc(sizeof(char) * (strlen(logTypeStrings[type]) + length + 1));
	str[0] = '\0';
	
	printf("%s", logAnsiColorStrings[type]);
	
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
	free(tempStr);
	free(str);
	
	printf("%s", logAnsiColorStrings[LOG_NORMAL]);

	if(type == LOG_ERROR) {
		fflush(logFile);
	}
	
	return;
}
