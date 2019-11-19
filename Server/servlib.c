#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <signal.h>
#include "servlib.h"
#include <time.h>
#include <math.h>
#include <fcntl.h>

#define VERSION "1.1.1"

char* logfile;
time_t timeStartListen;
int hostnameCount = 0;
int daemonMode = 0;

void daemonize() {
	// Our process ID and Session ID
	pid_t pid, sid;

	// Fork off the parent process
	pid = fork();
	if (pid < 0) {
		exit(1);
	}
	if (pid > 0) { // child can continue to run even after the parent has finished executing
		exit(1);
	}
	// Change the file mode mask
	/* Its used to set file permissions. umask(0) means you have full read, write, execute access.. */
	//umask(0);

	sid = setsid();
	if (sid < 0) {
		exit(1);
	}
	daemonMode = 1;
}

// Set time, when server start listening
void setTimeStartListen() {
	timeStartListen = time(NULL);
}

// Get format of current time
char* getFormatOfTime(time_t time) {
	char* formatOfTime = (char*) calloc(300, sizeof(char));
	struct tm *timeinfo = localtime(&time);
	sprintf(formatOfTime, "%02d.%02d.%04d %02d:%02d:%02d ", 
				timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year + 1900,
				timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	return formatOfTime;
}

// add time to message
char* addTimeToMessage(char* message) {
	char* timeInString = getFormatOfTime(time(NULL));
	return strcat(timeInString, message);
}

// Add a new string to the file
void addToLogfile(char* logfile, char* content){
	FILE *fp = fopen(logfile, "r+");
	fseek(fp, 0, SEEK_END);
	fputs(content, fp);
	fclose(fp);
}

// Output messages to the logfile and the standard stream
void showMessage(char* message) {
	if (daemonMode == 0)	
		printf("%s", message);
	if (logfile != NULL) {
		char* messageWithTime = addTimeToMessage(message);
		addToLogfile(logfile, messageWithTime);
	}
}

// Check host entry
char* messageOfError(int errornum) {
	char* message = (char*) calloc(100, sizeof(char));
	sprintf(message, "ERROR %d: %s\n", errornum, hstrerror(errornum));
	return message;
};

// Count required hostname
void countHostname() {
	hostnameCount++;
}

// Get list of IP addresses of hostname
char* getIP(char* host) {
	char *listIP = (char*) calloc(1024, sizeof(char*));
	struct hostent *hostEntry;
	
	// To retrieve host information
	hostEntry = gethostbyname(host);
	if (hostEntry == NULL) {
		showMessage(messageOfError(h_errno));
		return "Invalid hostname\n";
	}

	// Add all of IP addresses to list
	char **paddrlist = hostEntry->h_addr_list;
	while (*paddrlist != NULL) {
		char* buffIP = inet_ntoa(*((struct in_addr*) *paddrlist));
		strcat(listIP, buffIP);
		strcat(listIP, "\n");
		paddrlist++;
	}

	return listIP;
}

// Get version
char* getVersion() {
	char *version = VERSION;
	return version;
}

// Print manual
void printManual() {
	printf("Usage: server [OPTIONS, VALUES...]\n");
	printf("\n");
	printf("  -w\tSimulate the work by suspending the process/thread serving the request for N seconds(default: 0).\n");
	printf("  -d\tWork in demon mode.\n");
	printf("  -l\tdirectory to log file (default: /tmp/lab2.log).\n");
	printf("  -a\tThe address on which the server listens and to which the client connects.\n");
	printf("  -p\tThe port on which the server listens and to which the client connects.\n");
	printf("  -v\tDisplay version of the program.\n");
	printf("  -h\tDisplay manual.\n");
}

// Format time hh:mm:ss
char* format_hh_mm_ss(int second) {
	char* hh_mm_ss = (char*) calloc(10, sizeof(char));
	int hour = floor(second / 3600);
	int min = floor((second % 3600) / 60);
	int sec = second - 3600 * hour - 60 * min;
	sprintf(hh_mm_ss, "%02dh%02dm%02ds", hour, min, sec);
	return hh_mm_ss;
}

// Create message of working time
char* createMessageWokingTime() {
	char* message = (char*) calloc(100, sizeof(char));
	float workingSeconds = difftime(time(NULL), timeStartListen);
	sprintf(message, "Time working: %s\n", format_hh_mm_ss(workingSeconds));
	return message;
}

// Create message of counting required hostname
char* createMessageCountHost() {
	char* message = (char*) calloc(100, sizeof(char));
	sprintf(message, "Number of required hostname: %d\n", hostnameCount);
	return message;
}

// Handler signal
void signalHandler(int sig) {
	switch(sig) {
		case SIGINT:
			showMessage("Received SIGINT\n");
			exit(0);
		case SIGQUIT:
			showMessage("Received SIGQUIT\n");
			exit(0);
		case SIGTERM:
			showMessage("Received SIGTERM\n");
			exit(0);
		case SIGUSR1:
			showMessage(createMessageWokingTime());
			showMessage(createMessageCountHost());
			break;
	}
}

// Create logfile and save name of logfile to logfilename.txt
void createLogfile(char* logfileName) {
	FILE* fp = fopen(logfileName, "w");
	fclose(fp);
	logfile = logfileName;
}
