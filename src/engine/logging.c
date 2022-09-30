#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* logFile;

// in order of the LOG_TYPE enum
const char* logAnsiColorStrings[] = {
	"\033[97m", // LOG_NORMAL
	"\033[91m", // LOG_ERROR
	"\033[92m", // LOG_SUCCESS
};

const char* logTypeStrings[] = {
	"LOG: ",
	"ERROR: ",
	"SUCCESS: ",
};

void initLogFile(){
	logFile = fopen("log-file.txt", "w");
	
	if(!logFile){
		printf("could not open log file\n");
	}
	
	return;
}

void closeLogFile(){
	debugLog(LOG_NORMAL, "closing log file\n");
	fclose(logFile);
	
	return;
}

// variadic functions are weird
void debugLog(LOG_TYPE type, const char* fmt, ...){
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
	
	return;
}
