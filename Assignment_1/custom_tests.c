#include <stdio.h>
#include <sys/time.h>

#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)

// Comment out this define for a different random seed for every run
#define time(NULL) 0

int saturateMemory(int numberOfLoops) {
	if (numberOfLoops < 1) {
		return -1;
	}
	// The myBlock array can fit at most 1365 pointers of size 1 based on our metadata size
	char* pointers[1365];
	int i = 0;
	int j = 0;
	// Need a variable to keep track of how many pointers have been allocated
	int pointersIndex = 0;
	int randInt = rand();
	for (i; i < numberOfLoops; i++) {
		// There are four random cases that could happen
		switch (randInt % 4) {
		// In case 0, two pointers that each fill up half of myBlock are allocated
		case 0:
			pointers[0] = malloc(2046);
			pointers[1] = malloc(2046);
			pointersIndex += 2;
			break;
		/* In case 1, the first half of myBlock is allocated to one pointer and the rest
		   of it is full of 1 byte pointers */
		case 1:
			pointers[pointersIndex] = malloc(2046);
			pointersIndex++;
			for (j = 0; j < 682; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			break;
		/* In case 2, the second half of myBlock is allocated to one pointer and the first
		   half of it is full of 1 byte pointers */
		case 2:
			for (j = 0; j < 682; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			pointers[pointersIndex] = malloc(2046);
			pointersIndex++;
			break;
		// In the last case, the entire array is allocated to 1 byte pointers
		default:
			for (j = 0; j < 1365; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			break;
		}
		for (j = 0; j < pointersIndex; j++) {
			// All the pointers are freed
			free(pointers[j]);
		}
		// Index keeping track of pointers is reset and a new random integer is generated
		pointersIndex = 0;
		randInt = rand();
	}
	return 1;
}

int checkAllocations(int numberOfLoops, char* file, int line) {
	if (numberOfLoops < 1) {
		return -1;
	}
	// The myBlock array can fit at most 1365 pointers of size 1 based on our metadata size
	char* pointers[1365];
	int i = 0;
	int j = 0;
	for (i; i < numberOfLoops; i++) {
		for (j = 0; j < 1365; j++) {
			// Fill the myBlock array with 1 byte pointers and assign them an arbitrary value
			pointers[j] = malloc(1);
			*pointers[j] = 'a';
		}
		for (j = 0; j < 1365; j++) {
			if (*pointers[j] != 'a') {
				/* If any of the pointers are not equal to the arbitrary value, there is a
				   problem with either the malloc function or the previous call to free */
				printf("Error caused by line %d in %s:\n Test Failed\n", line, file);
				return -1;
			}
			// Free the pointer after it's been checked
			free(pointers[j]);
		}
	}
	return 1;
}

int main(int argc, char** argv) {
	printf("start0\n");
	srand(time(NULL));
	struct timeval start, end;
	gettimeofday(&start, NULL);
	checkAllocations(1000, __FILE__, __LINE__);
	gettimeofday(&end, NULL);
	long seconds = end.tv_sec - start.tv_sec;
	long milliseconds = (((seconds * 1000000) + end.tv_usec) - start.tv_usec)/1000;
	printf("Took %d milliseconds\n", milliseconds);
	return 0;
}
