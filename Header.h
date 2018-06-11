typedef struct free_memory{
	void *free_address;
	int no_of_free_bytes;
	free_memory *next;
}free_memory;

typedef struct memoryblock{
	void *memory;
	int max_free_memory_size;
	int free_memory_available;
	free_memory *max_free_block;
	free_memory *free_blocks;
	memoryblock *next;
}memoryblock;
