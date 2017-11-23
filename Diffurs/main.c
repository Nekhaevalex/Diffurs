//
//  main.c
//  Diffurs
//
//  Created by Alexander Nekhaev on 23.11.2017.
//  Copyright © 2017 Alexander Nekhaev. All rights reserved.
//

#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {
	if (argc < 2) {
		printf("Usage: programname <filename>\n");
		return -1;
	}
	FILE* scenary = fopen(argv[1], "rt");
	if (scenary == NULL) {
		exit(-2);
	}
	char* filename = malloc(sizeof(char)*strlen(argv[1]));
	strcpy(filename, argv[1]);
	int calcProcessAmount = 0;
	size_t length = 0;
	char* string;
	while (!feof(scenary)) {
		size_t nread = getline(&string, &length, scenary);
		calcProcessAmount++;
	}
	int createResult = creat("sem", O_CREAT|0777);
	if (createResult<0) {
		printf("Error creating semaphore file\n");
		exit(-4);
	}
	fseek(scenary, 0, SEEK_SET);
	for (int i = 0; i<calcProcessAmount; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			//child
			
		} else if (pid>0) {
			//parent
		}
		else {
			printf("Error while fork\n");
			exit(-2);
		}
	}
	return 0;
}
