#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

static char myBlock[4096];

void* mymalloc(int size, char* file, char* line);
void* myfree(void* ptr, char* file, char* line);