#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "SFLfunctions.h"
#include <errno.h>
#include <inttypes.h>

#define MAX 601

//command analysis
void p_cmd(char *cmd, sfl_t *SFL, mem_list *ML)
{
	int nr_lists, rec;
	char freeadd[100], start_address[100];
	long long mbytes, nr_bytes;
	char word[601];
	if (sscanf(cmd, "%s", word) != 1) {
		// Error handling if sscanf fails to read the expected item
		fprintf(stderr, "Failed to parse command.\n");
		return;
	}

	if (strcmp("INIT_HEAP", word) == 0) {
		long long bytes_per_lists;
		//INIT_HEAP start_address  nr_lists bytes rec
		if (sscanf(cmd, "%*s %s %d %llu %d", start_address,
				   &nr_lists, &bytes_per_lists, &rec) != 4) {
			fprintf(stderr, "Failed to parse INIT_HEAP command.\n");
			return;
		}
		// eliminate the 0x
		char *address_str1 = start_address + 2;
		//char* to long long
		long long address1 = strtoll(address_str1, NULL, 16);
		case_init(SFL, address1, nr_lists, bytes_per_lists, rec);

	} else if (strcmp("MALLOC", word) == 0) {
		if (sscanf(cmd, "%*s %lld", &mbytes) != 1) {
			fprintf(stderr, "Failed to parse MALLOC command.\n");
			return;
		}
		case_malloc(SFL, mbytes, ML);

	} else if (strcmp("FREE", word) == 0) {
		// addres to erase from ML
		if (sscanf(cmd, "%*s %s", freeadd) != 1) {
			fprintf(stderr, "Failed to parse FREE command.\n");
			return;
		}
		// eliminate the 0x
		char *address_str = freeadd + 2;
		long long address = strtoll(address_str, NULL, 16);
		free_address(ML, SFL, address);

	} else if (strcmp("DESTROY_HEAP", word) == 0) {
		//freeing all memory and end the program
		destroy(SFL, ML);

	} else if (strcmp("DUMP_MEMORY", word) == 0) {
		dump_memory(SFL, ML);
	} else if (strcmp("WRITE", word) == 0) {
		char data[MAX];
		//extracting the parameters and eliminating the quotes
		if (sscanf(cmd, "WRITE %s \"%[^\"]\" %lld",
				   start_address, data, &nr_bytes) != 3) {
			fprintf(stderr, "Failed to parse WRITE command.\n");
			return;
		}
		// eliminate the 0x
		char *address_str = start_address + 2;
		long long address = strtoll(address_str, NULL, 16);
		write_to_memory(SFL, ML, address, data, nr_bytes);

	} else if (strcmp("READ", word) == 0) {
		if (sscanf(cmd, "%*s %s %lld", start_address, &nr_bytes) != 2) {
			fprintf(stderr, "Failed to parse READ command.\n");
			return;
		}
		// eliminate the 0x
		char *address_str = start_address + 2;
		unsigned long address = strtoll(address_str, NULL, 16);
		read_memory(SFL, ML, address, nr_bytes);
	}
}

int main(void)
{
	char buffer[600];
	// init segregated free lists
	sfl_t *SFL = malloc(sizeof(sfl_t));
	// nr of free blocks
	SFL->free_b = 0;
	//SLL chose for allocated memory
	mem_list *ML = malloc(sizeof(mem_list));
	init_memory_list(ML);
	while (1) {
		fgets(buffer, 600, stdin);
		p_cmd(buffer, SFL, ML);
	}
}
