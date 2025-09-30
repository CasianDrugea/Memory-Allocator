#include "structs.h"
#include <stdio.h>

void init_memory_list(mem_list *);
void free_memory_list(mem_list *ML);
void mark_allocated(mem_list *ML, unsigned long start_address,
					unsigned long bytes, int index);
uint64_t total_free_memory(sfl_t *SFL);
uint64_t total_allocated_memory(mem_list *ML);
uint64_t count_free_blocks(sfl_t *SFL);
