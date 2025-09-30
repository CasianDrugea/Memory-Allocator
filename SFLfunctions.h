#include "MEMfunctions.h"
#include <stdio.h>

unsigned long min(unsigned long a, unsigned long b);
void case_init(sfl_t *SFL, unsigned long start_address, int nr_lists,
			   unsigned long bytes_per_list, int rec);
int list_exist(sfl_t *SFL, long long bytes_r);
void create_list(sfl_t *SFL, long long bytes_r,
				 long long address_r, int index);
void add_to_list(sfl_t *SFL, long long bytes_r, long long address_r,
				 int index);
void case_malloc(sfl_t *SFL, long long mbytes, mem_list *ML);
int merge_blocks_with_same_index(sfl_t *SFL);
int merge_blocks(sfl_t *SFL, int i);
void free_address(mem_list *ML, sfl_t *SFL, unsigned long address);
void ll_free(ll_node_t **pp_head);
void dump_memory(sfl_t *SFL, mem_list *ML);
void destroy(sfl_t *SFL, mem_list *ML);
int can_write(mem_list *ML, unsigned long nr_bytes, unsigned long address);
void write_to_memory(sfl_t *SFL, mem_list *ML, unsigned long address,
					 char *data, unsigned long nr_bytes);
void read_memory(sfl_t *SFL, mem_list *ML, unsigned long address,
				 unsigned long nr_bytes);
