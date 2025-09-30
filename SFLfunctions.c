#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "SFLfunctions.h"
#include <errno.h>
#include <inttypes.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#define SET 1000

unsigned long min(unsigned long a, unsigned long b)
{
	if (a < b)
		return a;
	return b;
}

//initialize the vector of lists(SFL)
void case_init(sfl_t *SFL, unsigned long start_address, int nr_lists,
			   unsigned long bytes_per_list, int rec)
{
	SFL->cnt = 0;
	SFL->vector = malloc(nr_lists * sizeof(dlinked_list_t));

	if (!SFL->vector)
		DIE(!SFL->vector, "EROARE SFL");

	//current butes starts from 8
	long long currb = 8;
	//current address
	long long curradr = start_address;
	SFL->size = 0;
	//reconstruction type
	SFL->rec = rec;
	SFL->bytes_per_list = bytes_per_list;

	for (int i = 0; i < nr_lists; i++) {
		SFL->size++;
		SFL->vector[i].bytes = currb;
		SFL->vector[i].size = 0;

		ll_node_t *pre = NULL;

		for (unsigned long j = 0; j < bytes_per_list / currb; j++) {
			SFL->free_b++;
			// nr of free blocks increases

			ll_node_t *node = malloc(sizeof(ll_node_t));
			if (!node)
				DIE(!node, "Error at malloc");
			//index is for bonus task
			node->index = ++(SFL->cnt);
			node->bytes = currb;
			node->start_addr = curradr;
			node->prev = pre;
			node->next = NULL;

			//inserting the block
			if (pre) {
				pre->next = node;
				SFL->vector[i].size++;
			} else {
				// head of list
				SFL->vector[i].head = node;
				SFL->vector[i].size++;
			}

			pre = node;
			curradr += currb;
		}

		currb *= 2;
	}
	//memorating the initial size
	SFL->const_size = SFL->size;
}

// determinates if there is already a list of bytes_r bytes in SFL
int list_exist(sfl_t *SFL, long long bytes_r)
{
	for (int i = 0; i < SFL->size; i++) {
		if (bytes_r == SFL->vector[i].bytes)
			return 1;
	}
	return 0;
}

//creates a new list of bytes_r bytes and add it' s head with it' s fields
void create_list(sfl_t *SFL, long long bytes_r,
				 long long address_r, int index)
{
	//increasing the capacity of SFL
	SFL->vector = realloc(SFL->vector, (SFL->size + 1) *
						  sizeof(dlinked_list_t));

	dlinked_list_t *new_list = malloc(sizeof(dlinked_list_t));
	if (!new_list) {
		DIE(!new_list, "Error at malloc\n");
		return;
	}

	ll_node_t *head = malloc(sizeof(ll_node_t));

	if (!head) {
		free(new_list);
		DIE(!head, "Error at malloc\n");
		return;
	}

	// fixing the list and it' s head
	head->bytes = bytes_r;
	head->index = index;
	head->prev = NULL;
	head->next = NULL;
	head->start_addr = address_r;
	new_list->head = head;
	new_list->tail = head;
	new_list->size = 1;
	// nr of bytes/block
	new_list->bytes = bytes_r;

	// searching the position to insert in SFL, so SFL is sorted
	int insert_position = 0;
	while (insert_position < SFL->size &&
		   SFL->vector[insert_position].bytes < bytes_r)
		insert_position++;

	// shifting the lists in SFL
	for (int i = SFL->size; i > insert_position; i--)
		SFL->vector[i] = SFL->vector[i - 1];

	// inserting new list
	SFL->vector[insert_position] = *new_list;
	SFL->size++;
	free(new_list);
}

// adds a new block to a list form SFL after searching it
void add_to_list(sfl_t *SFL, long long bytes_r, long long address_r, int index)
{
	// searching the proper list
	for (int i = 0; i < SFL->size; i++) {
		if (SFL->vector[i].bytes == bytes_r) {
			// found it

			if (!SFL->vector[i].head) {
				// adding the head
				ll_node_t *new_node = malloc(sizeof(ll_node_t));

				if (!new_node)
					DIE(!new_node, "Error at malloc");

				new_node->index = index;
				new_node->bytes = bytes_r;
				new_node->start_addr = address_r;
				new_node->prev = NULL;
				new_node->next = NULL;
				SFL->vector[i].head = new_node;
				SFL->vector[i].size++;
				return;
			}
			//searching the correct position to insert
			ll_node_t *current = SFL->vector[i].head;
			while (current) {
				if (current->start_addr > address_r) {
					// found the position to insert
					ll_node_t *new_node = malloc(sizeof(ll_node_t));
					new_node->index = index;
					new_node->bytes = bytes_r;
					new_node->start_addr = address_r;
					new_node->prev = current->prev;
					new_node->next = current;
					if (current->prev) {
						current->prev->next = new_node;
					} else {
						// add the head
						SFL->vector[i].head = new_node;
					}
					current->prev = new_node;
					SFL->vector[i].size++;
					return;
				}
				if (!current->next) {
					// add to end of list
					ll_node_t *new_node = malloc(sizeof(ll_node_t));
					new_node->index = index;
					new_node->bytes = bytes_r;
					new_node->start_addr = address_r;
					new_node->prev = current;
					new_node->next = NULL;
					current->next = new_node;
					SFL->vector[i].size++;
					return;
				}
				current = current->next;
			}
		}
	}
}

void case_malloc(sfl_t *SFL, long long mbytes, mem_list *ML)
{
	long long addres_r; // address for fragmentation
	int found = 0;//searching for block>=mbytes
	ll_node_t *caught = NULL;
	for (int i = 0; i < SFL->size; i++) {
		ll_node_t *current = SFL->vector[i].head;
		while (current) {
			if (current->bytes >= mbytes) {
				SFL->vector[i].size--;
				// found a block
				//nr of  allocated blocks
				ML->num_mallocs++;
				//nr of free blocks
				SFL->free_b--;

				found = 1;
				// pop the block
				if (current->prev)
					current->prev->next = current->next;
				else
					SFL->vector[i].head = current->next;

				if (current->next)
					current->next->prev = current->prev;

				caught = current;
				if (mbytes != current->bytes)
					//frag means i modify the index
					mark_allocated(ML, current->start_addr, mbytes,
								   current->index + SET);
					// adding new block to ML
				else
					// not a frag ,means i don t modify the index
					mark_allocated(ML, current->start_addr, mbytes,
								   current->index);

				if (current->bytes > mbytes) {
					//num of fragmentations
					ML->num_frag++;
					//splitting the block
					long long bytes_r = current->bytes - mbytes;
					addres_r = current->start_addr + mbytes;
					//adding block in ML (memory list)
					if (!list_exist(SFL, bytes_r))
						//list of bytes_r does not exist
						create_list(SFL, bytes_r, addres_r,
									current->index + SET);
					else
						//list does exits
						add_to_list(SFL, bytes_r, addres_r,
									current->index + SET);
				}
			}
			current = current->next;
			//found a block
			if (found == 1) {
				free(caught);
				break;
			}
		}
		if (found == 1)
			break;
	}
	if (!found) {
		//couldn't malloc
		printf("Out of memory\n");
	}
}

int merge_blocks(sfl_t *SFL, int i)
{
	ll_node_t *current = SFL->vector[i].head;
	while (current) {
		ll_node_t *next_current = current->next;
		for (int j = 0; j < SFL->size; j++) {
			ll_node_t *temp = SFL->vector[j].head;
			while (temp) {
				ll_node_t *next_temp = temp->next;
				if (temp->index == current->index && temp->start_addr !=
					current->start_addr) {
					// merge the blocks
					// eliminates the blocks
					if (current->prev)
						current->prev->next = next_current;
					else
						SFL->vector[i].head = next_current;

					if (next_current)
						next_current->prev = current->prev;

					if (temp->prev)
						temp->prev->next = next_temp;
					else
						SFL->vector[j].head = next_temp;

					if (next_temp)
						next_temp->prev = temp->prev;

					// update sizes
					SFL->vector[i].size--;
					SFL->vector[j].size--;

					//adding new block to the proper list
					long long new_bytes = current->bytes + temp->bytes;
					long long new_address = min(current->start_addr,
												temp->start_addr);
					int new_index = current->index - SET;

					free(current);
					free(temp);
					if (!list_exist(SFL, new_bytes))
						create_list(SFL, new_bytes, new_address, new_index);
					else
						add_to_list(SFL, new_bytes, new_address, new_index);
					return 1;
				}
				temp = next_temp;
			}
		}
		current = next_current;
	}
	return 0;
}

//merges two blocks with the same size
int merge_blocks_with_same_index(sfl_t *SFL)
{
	//compares every node to every node
	for (int i = 0; i < SFL->size; i++) {
		if (merge_blocks(SFL, i) == 1)
			return 1;
	}
	return 0;
}

//frees the node from ML and add it to a list in SFL
void free_address(mem_list *ML, sfl_t *SFL, unsigned long address)
{
	mem *current = ML->head;
	mem *prev = NULL;

	// searching in ML
	while (current) {
		if (current->start_address == address) {
			// found the address
			ML->num_frees++;

			if (prev)
				prev->next = current->next;
			else
				ML->head = current->next;

			// adding to SFL
			if (!list_exist(SFL, current->bytes))
				//list of bytesdoes not exist
				create_list(SFL, current->bytes, current->start_address,
							current->index);
			else
				//list does exits => add to it
				add_to_list(SFL, current->bytes, current->start_address,
							current->index);

			// free the block
			free(current->content);
			free(current);

			ML->size--;

			if (SFL->rec == 1)
				//merging as much as possible
				while (1)
					if (!merge_blocks_with_same_index(SFL))
						break;
			return;
		}
		prev = current;
		current = current->next;
	}
	//no address found
	printf("Invalid free\n");
}

//frees one list from SFL
void ll_free(ll_node_t **pp_head)
{
	if (!(*pp_head))
		return;

	ll_node_t *current = *pp_head;
	ll_node_t *next;

	while (current) {
		next = current->next;
		free(current);
		current = next;
	}
	*pp_head = NULL;
}

//prints memory status
void dump_memory(sfl_t *SFL, mem_list *ML)
{
	printf("+++++DUMP+++++\n");
	unsigned long total_mem = SFL->const_size * SFL->bytes_per_list;
	printf("Total memory: %lu bytes\n", total_mem);

	unsigned long total_allocated = total_allocated_memory(ML);
	printf("Total allocated memory: %lu bytes\n", total_allocated);

	printf("Total free memory: %lu bytes\n", total_mem - total_allocated);

	int num_free_blocks = count_free_blocks(SFL);
	printf("Free blocks: %d\n", num_free_blocks);

	int num_allocated_blocks = ML->size;
	printf("Number of allocated blocks: %d\n", num_allocated_blocks);

	printf("Number of malloc calls: %lu\n", ML->num_mallocs);

	printf("Number of fragmentations: %lu\n", ML->num_frag);

	printf("Number of free calls: %lu\n", ML->num_frees);

	// Blocks with different sizes
	for (int i = 0; i < SFL->size; i++) {
		if (SFL->vector[i].size > 0) {
			printf("Blocks with %lld bytes - %d free block(s) : ",
				   SFL->vector[i].bytes, SFL->vector[i].size);
			ll_node_t *current = SFL->vector[i].head;
			while (current) {
				printf("0x%llx", current->start_addr);
				if (current->next) {
					printf(" ");
					// Add space only if it's not the last element
				}
				current = current->next;
			}
			printf("\n");
		}
	}

	// Allocated blocks
	printf("Allocated blocks :");
	int i = 1;
	mem *current_mem = ML->head;
	while (current_mem) {
		if (i == 1) {
			printf(" ");
			i++;
		}
		printf("(0x%lx - %ld)", current_mem->start_address, current_mem->bytes);
		if (current_mem->next) {
			// Add space only if it's not the last element
			printf(" ");
		}
		current_mem = current_mem->next;
	}
	printf("\n");
	printf("-----DUMP-----\n");
}

//frees al the memory allocated (not the memory from ML)
void destroy(sfl_t *SFL, mem_list *ML)
{
	for (int i = 0; i < SFL->size; i++)
		// frees the every list
		ll_free(&SFL->vector[i].head);

	free(SFL->vector);
	free(SFL);
	free_memory_list(ML);
	free(ML);
	exit(0);
}

//verifies the address exists in ML and returns the block
mem *is_allocated(mem_list *ML, unsigned long address)
{
	mem *current = ML->head;
	while (current) {
		// getting the address
		if (address == current->start_address)
			//found the block
			return current;
		current = current->next;
	}
	//didn't find the block
	return NULL;
}

//checks if nr_bytes can be written
int can_write(mem_list *ML, unsigned long nr_bytes, unsigned long address)
{
	//getting the proper block
	mem *current  = is_allocated(ML, address);
	if (current) {
		unsigned long written = 0;

		while (current && written < nr_bytes) {
			//written bytes increases
			written += current->bytes;
			if (current->next && current->start_address + current->bytes
				== current->next->start_address)
				current = current->next;
			else if (written < nr_bytes)
				//cannot write
				return 0;
		}
		return 1;
	}
	return 0;
}

//writes nr_bytes to address
void write_to_memory(sfl_t *SFL, mem_list *ML, unsigned long address,
					 char *data, unsigned long nr_bytes)
{
	//determinates the nr_bytes
	if (nr_bytes > strlen(data))
		nr_bytes = strlen(data);

	if (can_write(ML, nr_bytes, address)) {
		mem *current  = is_allocated(ML, address); // start node
		unsigned long written = 0;// written bytes
		while (current && written < nr_bytes) {
			//mallocs data field
			if (!current->content)
				current->content = malloc(current->bytes * sizeof(char));

			//copies current->bytes to addres or the rest till nr_bytes
			strncpy((char *)current->content, data + written,
					min(current->bytes, nr_bytes - written));
			written += min(current->bytes, nr_bytes - written);
			current = current->next;
		}

	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(SFL, ML);
		destroy(SFL, ML);
	}
}

// reads nr_bytes starting form address if possible
void read_memory(sfl_t *SFL, mem_list *ML, unsigned long address,
				 unsigned long nr_bytes)
{
	//searching the block
	mem *current = ML->head;
	// number of bytes written
	unsigned long bytes_read = 0;
	if (total_allocated_memory(ML) <= nr_bytes) {
		//not enough memory to read
		printf("Segmentation fault (core dumped)\n");
		dump_memory(SFL, ML);
		destroy(SFL, ML);
	}

	while (current && bytes_read < nr_bytes) {
		if (address >= current->start_address && address <
			current->start_address + current->bytes) {
			unsigned long bytes_to_read = min(nr_bytes - bytes_read,
											  current->start_address +
											  current->bytes - address);
			// checks if there is a byte outside
			// [address, address + bytes_to_read)
			if (!can_write(ML, bytes_to_read, address)) {
				// there is at least one byte outside
				// [address, address + bytes_to_read)
				printf("Segmentation fault (core dumped)\n");
				dump_memory(SFL, ML);
				destroy(SFL, ML);
			}
			for (unsigned long i = 0; i < bytes_to_read; ++i)
				printf("%c", *((char *)current->content +
					   (address - current->start_address) + i));

			// bytes_read increases
			bytes_read += bytes_to_read;
			address += bytes_to_read;
		}
		current = current->next;
	}

	if (bytes_read < nr_bytes) {
		// couldn' t read nr_bytes
		printf("Segmentation fault (core dumped)\n");
		dump_memory(SFL, ML);
		destroy(SFL, ML);
	} else {
		printf("\n");
	}
}
