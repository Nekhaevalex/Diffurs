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

#define STEP 0.01

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
	size_t nread = 0;
	while (!feof(scenary)) {
		nread = getline(&string, &length, scenary);
		calcProcessAmount++;
	}
	printf("%zu strings in file\n", nread);
	int createResult = creat("sem", O_CREAT|0777);
	if (createResult<0) {
		printf("Error creating semaphore file\n");
		exit(-4);
	}
	fclose(scenary);
	unsigned int globalCounter = 1;
	for (int i = 0; i<calcProcessAmount; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			//child
			int cpid = getpid();
			double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
			FILE* scenFile = fopen(filename, "rt");
			for (int i = 0; i<globalCounter; i++) {
				fscanf(scenFile, "%lf %lf %lf %lf", &a, &b, &c, &d);
			}
			fclose(scenFile);
			char* calcFileName = NULL;
			sprintf(calcFileName, "Calculation report %d (%d)", globalCounter, cpid);
			FILE* calculationReport = fopen(calcFileName, "wt");
			fprintf(calculationReport, "Calculation report\nTaks: dy/dt=A*t+B\nThread info: A: %f\tB: %f\tC: %f\tD: %f\n", a, b, c, d );
			double y = c;
			for (double t = 0; t< (float)d; t+= STEP) {
				y = y+(a*t+b)*t;
				fprintf(calculationReport, "%lf\t%lf", t, y);
			}
			fclose(calculationReport);
		} else if (pid>0) {
			//parent
			globalCounter++;
		}
		else {
			printf("Error while fork\n");
			exit(-2);
		}
	}
	return 0;
}
