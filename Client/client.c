#include <stdio.h> 
#include <strings.h>
#include <string.h>
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include "clielib.h"
#include <errno.h>

int main(int argc, char **argv) {
	// get options
	int opt;
	int PORT = getenv("L2PORT") != NULL? atoi(getenv("L2PORT")) : -1;
	char *ip = getenv("L2ADDR") != NULL? getenv("L2ADDR") : "127.0.0.1";
	while ((opt = getopt(argc, argv, "p:hva:")) != -1) {
		switch(opt) {
			case 'p':
				PORT = atoi(optarg);
				break;
			case 'v':
				printf("Version: %s\n", getVersion());
				exit(0);
			case 'h':
				printManual();
				exit(0);
			case 'a':
				ip = optarg;
				break;
		}
	}
	// remind set PORT
	if (PORT == -1) {
		printf("[!!!]PORT is not set\nUse command: export L2PORT=[port]\n");
		exit(1);	
	}

	char buffer[1024]; 
	char *required_host = argv[optind];
	int sockfd; 
	struct sockaddr_in servaddr;
	
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr(ip);
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
	
	// create datagram socket 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("ERROR %d: %s\n", errno, strerror(errno));
		exit(1);
	}; 

	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
		printf("ERROR %d: %s\n", errno, strerror(errno));
		exit(1); 
	} 

	// request to send datagram 
	// no need to specify server address in sendto 
	// connect stores the peers IP and port 
	send(sockfd, required_host, strlen(required_host), 0);
	
	// waiting for response 
	bzero(buffer, sizeof(buffer));
	read(sockfd, buffer, 1024);
	printf("List IP of %s:\n%s\n", required_host, buffer); 

	// close the descriptor 
	close(sockfd);
} 

