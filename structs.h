#include <stdio.h>
#include <stdint.h>

typedef struct ll_node_t {
	int index;
	long long start_addr;
	long long bytes;
	struct ll_node_t *next;
	struct ll_node_t *prev;

} ll_node_t;

//struct for a list in SFL
typedef struct dlinked_list_t {
	long long bytes;
	ll_node_t *head;
	ll_node_t *tail;
	unsigned int data_size;
	unsigned int size;
	void *content;
} dlinked_list_t;

//struct for SFL
typedef struct sfl_t {
	//used for bonus
	int cnt;
	//reconstraction type
	int rec;
	unsigned long bytes_per_list;
	int size;
	int const_size;
	dlinked_list_t *vector;
	unsigned long free_b;
} sfl_t;

// SLL for allocated memory
typedef struct mem {
	int index;
	void *content;
	unsigned long start_address;
	unsigned long bytes;
	struct mem *next;
} mem;

//SLL node
typedef struct mem_list {
	unsigned long num_mallocs, num_frees, num_frag;
	mem *head;
	//allocated blocks
	int size;
} mem_list;
