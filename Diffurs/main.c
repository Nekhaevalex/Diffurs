//
//  main.c
//  Diffurs
//
//  Created by Alexander Nekhaev on 23.11.2017.
//  Copyright © 2017 Alexander Nekhaev. All rights reserved.
//

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define STEP 0.01

void reset() {
	remove("sem");
}

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
	//printf("%d strings in file\n", calcProcessAmount);
	int createResult = creat("sem", O_CREAT|0777);
	if (createResult<0) {
		printf("Error creating semaphore file\n");
		exit(-4);
	}
	fclose(scenary);
	unsigned int globalCounter = 1;
	key_t semKey = ftok("sem", 1);
	for (int i = 0; i<calcProcessAmount; i++) {
		pid_t pid = fork();
		if (pid == 0) {
			//child
			int cpid = getpid();
			if (semKey < 0) {
				printf("Error creating semaphores\n");
				reset();
				exit(-5);
			}
			struct sembuf sem[1];
			sem[0].sem_num = 0;
			sem[0].sem_op = 1;
			sem[0].sem_flg = 0;
			double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
			FILE* scenFile = fopen(filename, "rt");
			for (int i = 0; i<globalCounter; i++) {
				fscanf(scenFile, "%lf %lf %lf %lf", &a, &b, &c, &d);
			}
			//printf("My params: %lf, %lf, %lf, %lf\n", a, b, c, d);
			fclose(scenFile);
			int semIdC = semget(semKey, 1, IPC_R|0777);
			int result = semop(semIdC, sem, 0);
			if (result<0) {
				printf("Error on semaphor\n");
				reset();
				exit(-5);
			}
			char calcFileName[256];
			sprintf(calcFileName, "Calculation report %d (%d).txt", globalCounter, cpid);
			//printf("Will be saved in %s\n", calcFileName);
			FILE* calculationReport = fopen(calcFileName, "wt");
			fprintf(calculationReport, "Calculation report on process %d\nTaks: dy/dt=A*t+B\nThread info: A: %f\tB: %f\tC: %f\tD: %f\n\nResults:\n", cpid,a, b, c, d );
			double y = c;
			for (double t = 0; t<=(float)d; t+= STEP) {
				y = y+(a*t+b)*t;
				fprintf(calculationReport, "%lf\t%lf\n", t, y);
			}
			close(semIdC);
			fclose(calculationReport);
			if (globalCounter == calcProcessAmount)
				reset();
			exit(0);
		} else if (pid>0) {
			//parent
			if (semKey < 0) {
				printf("Error creating semaphores\n");
				reset();
				exit(-5);
			}
			globalCounter++;
			int semIdP = semget(semKey, 1, IPC_CREAT|0777);
			struct sembuf sem[1];
			sem[0].sem_num = 0;
			sem[0].sem_op = -1;
			sem[0].sem_flg = 0;
			int result = semop(semIdP, sem, 0);
			if (result<0) {
				printf("Error on posting semaphore\n");
				reset();
				exit(-5);
			}
		}
		else {
			printf("Error while fork\n");
			exit(-2);
		}
	}
	sleep(1);
	reset();
	return 0;
}
