#include <stdio.h>
#include <stddef.h>

static char myBlock[4096];

void* mymalloc(int size, char* file, int line) {
	// Metadata is 2 bytes so size cannot be greater than 4094 bytes
	if (size > 4094 || size < 1) {
		printf("Error caused by line %d in %s:\n Invalid allocation size\n", line, file);
		return NULL;
	}

	// The first item in the array must be some metadata
	unsigned short* meta = (unsigned short*)myBlock;
	/* Parity means whether or not the block of memory this metadata
	   is refering to is used or not.*/
	unsigned short parity = *meta >> 12;
	// Current index in the array is 0
	unsigned short currentIndex = 0;
	// Index of next metadata is stored in the current metadata
	unsigned short nextIndex = *meta & 0xfff;
	/* Initialize next metadata pointer to where the next metadata
	   is going to be if this is the first run of malloc */
	unsigned short* nextMeta = (unsigned short*)(myBlock + size + 2);

	/* Static arrays have all their bytes set to zero when declared. This
	   if statement looks for this when checking if this is the first run.
	   The other conditions are there just in case the array isn't zeroed. */
	if ((*meta == 0) || (parity > 1) || (nextIndex > 4093)) {
		// Initializing the array when malloc is first run
		*meta = 0x1000;
		if (size < 4092) {
			/* If size is small enough for another block of memory to be
			   allocated, the index of the next piece of metadata is set */
			*meta = *meta | ((unsigned short)size + 2);
			// The next piece of meta data is unused and it points back to the beginning of the array
			*nextMeta = 0;
		}
		return myBlock + 2;
	}
	else {
		/* This is not the first time malloc is running so you have to check every metadata for
		   an adequate amount of free space. */
		do {
			nextMeta = (unsigned short*)(myBlock + currentIndex + size + 2);
			/* If the block is unused, check if it can accomodate the requested size, otherwise
			   continue to the next index. */
			if (parity == 0) {
				if (nextIndex == 0) {
					// If the last metadata is reached, check if the array can accomodate the requested size
					if (size < (4093 - currentIndex)) {
						*meta = 0x1000 | (currentIndex + (unsigned short)size + 2);
						*nextMeta = 0;
						return myBlock + currentIndex + 2;
					}
					else if (size < (4095 - currentIndex)) {
						*meta = 0x1000;
						return myBlock + currentIndex + 2;
					}
					else {
						/* If the end of the array is reached and there isn't enough space to store the
						   requested size, return a NULL pointer. */
						printf("Error caused by line %d in %s:\n Not enough space for requested pointer\n", line, file);
						return NULL;
					}
				}
				else {
					/* If the current block is not the last one and it is available, check if it can accomodate
					   the requested size */
					if (size < (nextIndex - (currentIndex + 4))) {
						*meta = 0x1000 | (currentIndex + (unsigned short)size + 2);
						*nextMeta = nextIndex;
						return myBlock + currentIndex + 2;
					}
					else if (size < (nextIndex - (currentIndex + 1))) {
						*meta = 0x1000 | nextIndex;
						return myBlock + currentIndex + 2;
					}
				}
			}
			meta = (unsigned short*)(myBlock + nextIndex);
			parity = *meta >> 12;
			currentIndex = nextIndex;
			nextIndex = *meta & 0xfff;
		} while ((char*)meta != myBlock);
	}
	/* The program should only ever get to this point if there is only one element in the array that takes up
	   the entire space of the array. In that case, there is no space available to give the caller.*/
	printf("Error caused by line %d in %s:\n Not enough space for requested pointer\n", line, file);
	return NULL;
}

void myfree(void* ptr, char* file, int line) {
	// Set ptr to point to metadata
	ptr = (char*)ptr - 2;
	// Check if ptr is out of range
	if (((char*)ptr < myBlock) || ((char*)ptr > myBlock + 4093)) {
		printf("Error caused by line %d in %s:\n Memory address outside of malloc's domain\n", line, file);
		return;
	}

	// First item in array is metadata
	unsigned short* meta = (unsigned short*)myBlock;
	/* Parity means whether or not the block of memory this metadata
	   is refering to is used or not.*/
	unsigned short parity = *meta >> 12;
	// Index of next metadata is stored in the current metadata
	unsigned short nextIndex = *meta & 0xfff;
	// The location of previous metadata is initialized to the first metadata
	unsigned short* prevMeta = meta;
	unsigned short prevParity = *prevMeta >> 12;
	// The location of next metadata is initialized to nextIndex
	unsigned short* nextMeta = (unsigned short*)(myBlock + nextIndex);
	unsigned short nextParity = *nextMeta >> 12;

	do {
		if (meta == ptr) {
			if (parity == 1) {
				// If ptr is found and in use, the block can be freed
				*meta = nextIndex;
				if ((nextIndex != 0) && (nextParity == 0)) {
					// If the next block is not in use, combine the blocks
					*meta = *nextMeta & 0xfff;
				}
				if (prevParity == 0) {
					// If the previous block is not in use, combine the blocks
					*prevMeta = *meta;
				}
				return;
			}
			else {
				// If ptr is found but not in use, it has already been freed before
				printf("Error caused by line %d in %s:\n Memory address cannot be freed\n", line, file);
				return;
			}
		}
		prevMeta = meta;
		prevParity = parity;
		meta = (unsigned short*)(myBlock + nextIndex);
		parity = *meta >> 12;
		nextIndex = *meta & 0xfff;
		nextMeta = (unsigned short*)(myBlock + nextIndex);
		nextParity = *nextMeta >> 12;
	} while ((char*)meta != myBlock);

	/* If the program gets to this point, the user tried to free something that was
	   not allocated by malloc */
	printf("Error caused by line %d in %s:\n Memory address cannot be freed\n", line, file);
	return;
}
