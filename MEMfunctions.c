#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MEMfunctions.h"

//initialize ML
void init_memory_list(mem_list *ML)
{
	ML->head = NULL;
	ML->size = 0;
	ML->num_frag = 0;
	ML->num_frees = 0;
	ML->num_mallocs = 0;
}

//free ML
void free_memory_list(mem_list *ML)
{
	mem *current = ML->head;
	mem *next;

	while (current) {
		next = current->next;
		//free the content only if it was allocated
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}

	ML->head = NULL;
	ML->size = 0;
}

//adding new block to ML at the correct
//position so ML remains sorted ASC on address
void mark_allocated(mem_list *ML, unsigned long start_address,
					unsigned long bytes, int index)
{
	mem *new_block = (mem *)malloc(sizeof(mem));
	if (!new_block) {
		printf("Error\n");
		exit(EXIT_FAILURE);
	}
	// index = index from father + SET
	new_block->index = index;
	new_block->start_address = start_address;
	new_block->bytes = bytes;
	new_block->next = NULL;
	new_block->content = NULL;
	if (!ML->head || start_address < ML->head->start_address) {
		// adding the head
		new_block->next = ML->head;
		ML->head = new_block;
	} else {
		// searching the right poition so ML remains sorted
		mem *current = ML->head;
		while (current->next && current->next->start_address
			   < start_address) {
			current = current->next;
		}
		//inserting new block
		new_block->next = current->next;
		current->next = new_block;
	}
	ML->size++;
}

// returns the total free memory in SFL
unsigned long total_free_memory(sfl_t *SFL)
{
	unsigned long total = 0;

	for (int i = 0; i < SFL->size; i++) {
		ll_node_t *current = SFL->vector[i].head;
		while (current) {
			total += current->bytes;
			current = current->next;
		}
	}

	return total;
}

// returns total allocated memory in ML
unsigned long total_allocated_memory(mem_list *ML)
{
	unsigned long total = 0;
	mem *current = ML->head;

	while (current) {
		total += current->bytes;
		current = current->next;
	}

	return total;
}

//calculates the number of free blocks in SFL
unsigned long count_free_blocks(sfl_t *SFL)
{
	unsigned long total_free_blocks = 0;

	for (int i = 0; i < SFL->size; i++)
		total_free_blocks += SFL->vector[i].size;
	return total_free_blocks;
}
