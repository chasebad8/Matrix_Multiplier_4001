#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "matrix_def.h"


#define MICRO_SEC_IN_SEC 1000000
#define NUM_CHILDREN 4
#define ROW_COLUMN 4

/**
Chase Badalato
101072570

2021-02-11

Calculate the resulting matrix when multiplying
matrix M*N.  The parent will fork 4 child processes
to calculated 1 row of the resulting matrix Q
**/
int main(void){

	pid_t pid; //returned process ID from forked child
	void *memory_location; //pointer to shared memory
	struct matrix_struct *matrices; //structure of data that will be in shared memory
	int shmid; //the ID of the shared memory
	key_t key; //The key to access the shared memory
	int i; //counter variable
	struct timeval start, end; //variables for calculating execution time 

	key = 2570;

	//Matrix M
    int M[4][4] = {{ 20, 20, 50, 40 },
                   { 10, 6,  70, 8  },
                   { 40, 3,  2,  10 },
                   { 8,  7,  6,  5 }};
 	
 	//Matrix N
    int N[4][4] = {{ 10, 30, 50, 70 },
                   { 1,  3,  6,  8  },
                   { 9,  5,  5,  7  },
                   { 8,  6,  4,  2  }};

	//attempt to allocated some shared memory of size matrix_struct
	shmid = shmget(key, sizeof(struct matrix_struct), 0666 | IPC_CREAT);
	if (shmid == -1) {
		int error = errno;
		fprintf(stderr,"Failed to allocate shared memory: %d\n", error);
		exit(EXIT_FAILURE);
	}

	//attempt to attach the shared memory to the program
	memory_location = shmat(shmid, (void *) 0, 0);
	if (memory_location == (void *) -1) {
		fprintf(stderr,"Failed to attach to the memory location\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at %X\n", memory_location);

	//copy the inputted arrays into shared memory
	matrices = (struct matrix_struct *) memory_location;
	memcpy(matrices->M, M, sizeof(M));
	memcpy(matrices->N, N, sizeof(N));

	//start execution duration timer
	//for 4 children and if they are children break out of the loop
	gettimeofday(&start, NULL);
	for (i = 0; i < NUM_CHILDREN; i++){
		pid = fork();

		if (pid == -1){
			perror("fork failed");
			exit(1);
		}
		else if (pid == 0){
			break;
		}
	}

	/*****************************************************
					  	    PARENT
	******************************************************/
	if (pid != 0) {
		//wait until all children complete execution
		int n = NUM_CHILDREN;
		while(n > 0){
			wait(NULL);
			n --;
		}
		gettimeofday(&end, NULL);
		printf("\nMatrix Multiplication completed in: %ld microseconds\n\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) 
				- (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));

		//Output the 3 matrices
		printf("Matrix M: \n");
		for(int j = 0; j < ROW_COLUMN; j++){
			for(int k = 0; k < ROW_COLUMN; k ++){
				printf(" %5d ", matrices->M[j][k]);
			}
			printf("\n");
		}
		printf("\nMatrix N: \n");
		for(int j = 0; j < ROW_COLUMN; j++){
			for(int k = 0; k < ROW_COLUMN; k ++){
				printf(" %5d ", matrices->N[j][k]);
			}
			printf("\n");
		}
		printf("\nThe resulting matrix Q is: \n");
		for(int j = 0; j < ROW_COLUMN; j++){
			for(int k = 0; k < ROW_COLUMN; k ++){
				printf(" %5d ", matrices->Q[j][k]);
			}
			printf("\n");
		}
		printf("\n");

		//find the largest value in the matrix and output largest value per row
		int largestValue = matrices->largestNum[0];
		for(int j = 0; j < ROW_COLUMN; j++){
			printf("Largest value in row %d: %d\n", j, matrices->largestNum[j]);
			if(largestValue < matrices->largestNum[j]){
				largestValue = matrices->largestNum[j];
			}
		}
		printf("The largest value in the matrix is: %d\n\n", largestValue);
		printf("Detaching memory from program\n");

		//detach from the shared memory
		if (shmdt(memory_location) == -1) {
			fprintf(stderr, "Failed to detach memory from program\n");
			exit(EXIT_FAILURE);
		}		
		if (shmctl(shmid,  IPC_RMID, 0) == -1) {
			fprintf(stderr, "shmctl(IPC_RMID) failed\n");
			exit(EXIT_FAILURE);
		}
	}

	/*****************************************************
					  	    CHILD
	******************************************************/
	else {
		//the matrices pointer points to the values in shared memory
		matrices = (struct matrix_struct *) memory_location;
		printf("[CHILD %d] Working with row : %d\n", i, i);

		//use the matrix multiplication formula to find the Q value
		for(int j = 0; j < ROW_COLUMN; j++){
			for(int k = 0; k < ROW_COLUMN; k ++){
				matrices->Q[i][j] += matrices->M[i][k] * matrices->N[k][j];
			}

			//find the largest value in the row
			if(j == 0){
				matrices->largestNum[i] = matrices->Q[i][j];
			}
			else{
				if(matrices->largestNum[i] < matrices->Q[i][j]){
					matrices->largestNum[i] = matrices->Q[i][j];
				}
			}			
		}
		exit(0);
	}
}


/*****************
Alternative Tests
******************/
/**
TEST ONE

	//Matrix M
    int M[4][4] = {{ 1, 1, 1, 1 },
                   { 2, 2, 2, 2 },
                   { 3, 3, 3, 3 },
                   { 4, 4, 4, 4 }};
 	
 	//Matrix N
    int N[4][4] = {{ 1, 1, 1, 1 },
                   { 2, 2, 2, 2 },
                   { 3, 3, 3, 3 },
                   { 4, 4, 4, 4 }};


TEST TWO

	//Matrix M
    int M[4][4] = {{ 5,   2, 12, 40 },
                   { 70, 64,  4, 18 },
                   { 8,  33,  9, 12 },
                   { 2,   9, 31, 7 }};
 	
 	//Matrix N
    int N[4][4] = {{ 6,  28, 5,  70 },
                   { 13, 38, 33, 43 },
                   { 7,  25, 2,  23 },
                   { 13, 53, 1,  2  }};

**/