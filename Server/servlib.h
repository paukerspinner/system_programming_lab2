#ifndef HEADER_FILE
#define HEADER_FILE

#include <signal.h>

char* getIP();
void signalHandler(int);
char* getVersion();
void printManual();
void createLogfile(char*);
void setTimeStartListen();
void daemonize();
void countHostname();
char* messageOfError(int);
char* createMessageResponse(char*);
void showMessage(char*);

#endif
