#include <stdio.h> 
#include <sys/time.h> 

#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)

#include "mymalloc.h"

int main(int argc, char** argv) {

	clock_t t; 
	char *ptrs[100]; 
	int i, j; 

	// Test case A 
	t = clock();

	for(i = 0; i < 150; i++)
	{
		ptrs[i] = (char *) malloc('a');		
	}  
	for(i = 0; i < 150; i++)
	{	
		free(ptrs[i]);
		ptrs[i] = NULL; 	
	}
	t = clock() - t;
	double elapsed_time = ((double)t)/CLOCKS_PER_SEC; // in seconds 
	printf("Time elapsed: %f\n", elapsed_time); 	

	// Test case B
	t = clock();
	for(i = 0; i < 150; i++)
	{
		ptrs[i] = (char *) malloc('a');  
	}
	for(i = 0; i < 50; i++)
	{
		free(ptrs[i]); 
		ptrs[i] = NULL; 
	}

	t = clock() - t;
	double elapsed_time = ((double)t)/CLOCKS_PER_SEC; // in seconds 
	printf("Time elapsed: %f\n", elapsed_time); 	
	
	// Test case C

	// Test case D 
	
	return 0; 
}
