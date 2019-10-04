#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "jobs.h"
#include "mem.h"

struct TOKEN *init_TOKEN_list()
{
	struct TOKEN *list;

	list = malloc(sizeof(struct TOKEN));

	if(!list) {
		printf("RShell: Failed to allocate memory\n");
		longjmp(prompt_jmp, 0);
	}

	*list = (struct TOKEN) {
		.next  = NULL,
		.size  = 0,
		.flags = 0,
		.command = malloc(sizeof(char) * MAX_INPUT_SIZE * MAX_TOKEN_NUMBER)
	};

	if(!list->command) {
		printf("RShell: Failed to allocate memory\n");
		longjmp(prompt_jmp, 0);
	}

	memset(list->command, 0, MAX_INPUT_SIZE * MAX_TOKEN_NUMBER);

	return list;
}


char *realloc_string(char *old_ptr, size_t new_size)
{
	char *new_ptr = NULL;

	new_ptr = realloc(old_ptr, new_size);

	if(new_ptr == NULL) {
		printf("RShell: Failed to allocate memory\n");
		free(old_ptr);
		longjmp(prompt_jmp, 1);
	}

	return new_ptr;
}

void clean_TOKEN_list(struct TOKEN *list_head) {

	struct TOKEN *list_ptr = NULL, *list_tmp = NULL;
	unsigned int i;

	list_ptr = list_head;

	while(list_ptr != NULL) {

		for(i = 0; i < list_ptr->size; ++i)
				free(list_ptr->command[i]);

		list_tmp = list_ptr->next;

		free(list_ptr->command);
		free(list_ptr);

		list_ptr = list_tmp;

	}
}

void clean_child_list(struct child *list_head) {

	struct child *list_ptr, *list_tmp;

	list_ptr = list_head->next;
	free(list_head->name);

	while(list_ptr != NULL) {

		list_tmp = list_ptr;
		list_ptr = list_ptr->next;

		free(list_tmp->name);
		free(list_tmp);
	}
}
