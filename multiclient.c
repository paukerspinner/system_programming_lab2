#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
	char hosts[5][20] = {"google.com", "fb.com", "vk.com", "goog", "youtube.com"};
	for (int i = 0; i < 1000; i++) {
		int index = rand() % 4;
		char *command = (char*) calloc(50, sizeof(char));
		sprintf(command, "./client %s", hosts[index]);
		system(command);
	}
	return 0;
}
