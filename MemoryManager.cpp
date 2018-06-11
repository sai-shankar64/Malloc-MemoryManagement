// MemoryManager.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "Header.h"
#define PAGESIZE 1024
memoryblock *memoryblocks = NULL;
int no_of_memory_blocks = 0;

memoryblock * create_new_memoryblock(){
	memoryblock *temp = (memoryblock *)malloc(sizeof(memoryblock));
	temp->memory = malloc(PAGESIZE * sizeof(char));
	temp->next = NULL;
	temp->max_free_memory_size = PAGESIZE;
	temp->free_memory_available = PAGESIZE;
	temp->free_blocks = (free_memory *)malloc(sizeof(free_memory));
	temp->free_blocks->free_address = temp->memory;
	temp->free_blocks->no_of_free_bytes = PAGESIZE;
	temp->free_blocks->next = NULL;
	temp->max_free_block = temp->free_blocks;
	no_of_memory_blocks + 1;
	return temp;
}

memoryblock* find_memory_block(size_t no_of_bytes){
	memoryblock *start = memoryblocks;
	while (start){
		if (start->max_free_memory_size >= no_of_bytes + 2)
			return start;
		start = start->next;
	}
	return NULL;
}

/*Creates new memoryblock and adds at end*/
memoryblock *create_memory_block(){
	memoryblock *new_memory_block = create_new_memoryblock();
	if (!memoryblocks) {
		memoryblocks = new_memory_block;
		return new_memory_block;
	}
	memoryblock *temp = memoryblocks;
	while (temp->next)
		temp = temp->next;
	temp->next = new_memory_block;
	return new_memory_block;
}

/*Allocates memory for given number of bytes*/
void *ss_malloc(size_t no_of_bytes){
	memoryblock *memory_block = find_memory_block(no_of_bytes);
	if (!memory_block){
		if(no_of_memory_blocks == 12) return NULL;
		else
		memory_block = create_memory_block();
	} 
	
	free_memory *block = memory_block->free_blocks;
	while (block && !(block->no_of_free_bytes >= no_of_bytes + 2))
		block = block->next;

	*(short int *)(block->free_address) = no_of_bytes;
	block->no_of_free_bytes -= (no_of_bytes + sizeof(short int));
	void *return_address = (char *)(block->free_address) + 2;
	(block->free_address) = ((char *)(block->free_address) + (no_of_bytes + sizeof(short int)));
	memory_block->free_memory_available -= (no_of_bytes + sizeof(short int));

	if (memory_block->max_free_block == block){
		memory_block->max_free_memory_size -= (no_of_bytes + sizeof(short int));
		free_memory *temp = memory_block->free_blocks;
		while (temp){
			if (temp->no_of_free_bytes >= memory_block->max_free_memory_size){
				memory_block->max_free_memory_size = temp->no_of_free_bytes;
				memory_block->max_free_block = temp;
			}
			temp = temp->next;
		}
	}
	return return_address;
}

free_memory *create_free_block(void *add,int bytes){
	free_memory *temp = (free_memory *)malloc(sizeof(free_memory));
	temp->next = NULL;
	temp->free_address = (void *)((char *)add - sizeof(short int));
	temp->no_of_free_bytes = bytes + sizeof(short int);
	return temp;
}

free_memory *insert_in_free_blocks(free_memory *start_block, void *add, int bytes,memoryblock *memblock){
	int address = int(add);
	free_memory *prevblock, *block = prevblock = start_block;
	while (block && address + bytes > (int)(block->free_address)){
		prevblock = block;
		block = block->next;
	}
	if (address + bytes == (int)(block->free_address)){
		block->no_of_free_bytes +=(bytes + sizeof(short int));
		block->free_address = (char *)(block->free_address) - (bytes + sizeof(short int));
		if (memblock->max_free_memory_size < block->no_of_free_bytes){
			memblock->max_free_memory_size = block->no_of_free_bytes;
			memblock->max_free_block = block;
		}
	}
	else if (address + bytes < (int)(block->free_address)){
		free_memory *temp = create_free_block(add, bytes);
		if (memblock->max_free_memory_size < block->no_of_free_bytes){
			memblock->max_free_memory_size = block->no_of_free_bytes;
			memblock->max_free_block = block;
		}
		if (start_block == block){
			temp->next = block;
			return temp;
		}
		else{
			prevblock->next = temp;
			temp->next = block;
		}
	}
	return start_block;
}

void ss_free(void *address){
	memoryblock *memory_block=memoryblocks;
	while (memory_block && !((int)(memory_block->memory)<=(int)address && (int)address <= (int)(memory_block->memory) + PAGESIZE)){
		memory_block = memory_block->next;
	}
	if (memory_block){
		short int no_of_bytes = *(short int *)((char *)(address)-2);
		memory_block->free_blocks=insert_in_free_blocks(memory_block->free_blocks,address,no_of_bytes,memory_block);
		memory_block->free_memory_available += no_of_bytes + sizeof(short int);
		/*if (memory_block->max_free_memory_size < check_with_max_size)
			memory_block->max_free_memory_size = check_with_max_size;*/
	}
	return;
}

void ss_debug(){
	memoryblock *block = memoryblocks;
	int i = 1;
	while (block){
		printf("\n%d Memoryblock : Memory Available %d\n",i,block->free_memory_available);
		free_memory *temp = block->free_blocks;
		while (temp){
			printf("%d     %d\n",(int)(temp->free_address),temp->no_of_free_bytes);
			temp = temp->next;
		}
		block = block->next;
		i++;
	}
	return;
}

int main(int argc, _TCHAR* argv[])
{

	void *a=ss_malloc(10);
	printf("\nAllocated at %d. 10 Bytes\n",int(a));
	ss_debug();

	void *b = ss_malloc(5);
	printf("\nAllocated at %d. 5 Bytes\n", int(b));
	ss_debug();

	a = ss_malloc(10);
	printf("\nAllocated at %d. 10 Bytes\n", int(a));
	ss_debug();

	void *c = ss_malloc(1022);
	printf("\nAllocated at %d. 1022 Bytes\n", int(c));
	ss_debug();

	ss_free(b);
	printf("\nFreed memory at %d. 5 bytes\n",int(b));
	ss_debug();

	void *d = ss_malloc(10);
	printf("\nAllocated at %d. 10 Bytes\n", int(d));
	ss_debug();

	void *e = ss_malloc(4);
	printf("\nAllocated at %d. 4 Bytes\n", int(e));
	ss_debug();

	getchar();
	return 0;
}

