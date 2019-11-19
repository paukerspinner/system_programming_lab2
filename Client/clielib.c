#include<stdio.h>
#include "clielib.h"

#define VERSION "1.1.1"

// Get version
char* getVersion() {
	char *version = VERSION;
	return version;
}

// Print manual
void printManual() {
	printf("Usage: client [OPTIONS, VALUES...] [HOSTNAME]\n");
	printf("\n");
	printf("  -a\tThe address on which the server listens and to which the client connects.\n");
	printf("  -p\tThe port on which the server listens and to which the client connects.\n");
	printf("  -v\tDisplay version of the program.\n");
	printf("  -h\tDisplay manual.\n");
}
