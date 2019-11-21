#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "servlib.h"
#include <signal.h>
#include <errno.h>
#include <netdb.h>

int main(int argc, char **argv) {
	char buffer[100];
	char *message; 
	int listenfd, newSocket;
	int socketOption = 1;
	struct sockaddr_in servaddr; 
	int servAddrLen = sizeof(servaddr);
	int daemonMode = 0;
	bzero(&servaddr, sizeof(servaddr));
	
	// Get options
	int opt;
	int PORT = getenv("L2PORT") != NULL? atoi(getenv("L2PORT")) : -1;
	int waitTime = 0;
	char* logfile = (char *) calloc(20, sizeof(char));
	logfile = getenv("L2LOGFILE") != NULL ? getenv("L2LOGFILE") : "/tmp/lab2.log";
	char *ip = getenv("L2ADDR") != NULL? getenv("L2ADDR") : "127.0.0.1";
	while ((opt = getopt(argc, argv, "p:w:vhl:da:")) != -1) {
		switch(opt) {
			case 'p':
				PORT = atoi(optarg);
				break;
			case 'w':
				waitTime = atoi(optarg);
				break;
			case 'v':
				printf("Version: %s\n", getVersion());
				exit(0);
			case 'h':
				printManual();
				exit(0);
			case 'l':
				logfile = optarg;
				break;
			case 'd':
				daemonMode = 1;
				break;
			case 'a':
				ip = optarg;
				break;
		}
	}
	
	// Create log file
	createLogfile(logfile);
	
	// remind set PORT
	if (PORT == -1) {
		printf("[!!!]PORT is not set\nUse command: export L2PORT=[port]\n");
		exit(1);
	}
	printf("[+]Listening from PORT %d...\n", PORT);

	// Daemonize
	if (daemonMode == 1) {
		daemonize();
	}

	// Create a TCP Socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		showMessage(messageOfError(errno));
		exit(1);
	};
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(PORT);
	servaddr.sin_family = AF_INET;

	// Set socket options
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socketOption, sizeof(socketOption))) {
		showMessage(messageOfError(errno));
		exit(1);
	};
	 
	// bind server address to socket descriptor 
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		showMessage(messageOfError(h_errno));	
		exit(1);
	};
	setTimeStartListen();

	// Put the server socket in a passive mode
	if (listen(listenfd, 3) < 0) {
		showMessage(messageOfError(errno));
		exit(1);
	};
	
	// handle signals with sigaction
	struct sigaction act;
	bzero(&act, sizeof(act));
	act.sa_handler = signalHandler;
	act.sa_flags = SA_RESTART;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);
	pid_t pid;
	while(1) {
		// Extract the first connection request on the queue, at this, connection client-server is established
		newSocket = accept(listenfd, (struct sockaddr*)&servaddr, (socklen_t*)&servAddrLen);

		//receive message from server
		memset(&buffer, '\0', sizeof(buffer));
		read(newSocket, buffer, 1024);
		countHostname();
		
		// Create a new proccess
		pid = fork();
		if (pid < 0) {
			showMessage(messageOfError(errno));
			exit(1);
		} else if (pid == 0) {			
			message = getIP(buffer);
			sleep(waitTime);
			// send the response
			send(newSocket, message, strlen(message), 0);
			showMessage(createMessageResponse(buffer));
			exit(1);
		}
	}
	close(listenfd);
	return 0;
} 

