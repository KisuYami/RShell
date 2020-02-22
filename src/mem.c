#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "parse.h"
#include "jobs.h"
#include "mem.h"

node_t *
init_node_list()
{
	node_t *list = malloc(sizeof(node_t));

	if(!list)
	{
		printf("RShell: Failed to allocate memory\n");
		return NULL;
	}

	*list = (node_t) {
		.next  = NULL,
		.size  = 0,
		.flags = 0,
	};

	return list;
}

void *
clean_node_list(node_t *list_head)
{
	node_t *list_ptr = NULL, *list_tmp = NULL;
	unsigned int i;

	list_ptr = list_head;

	while(list_ptr != NULL)
	{
		for(i = 0; i < list_ptr->size; ++i)
			free(list_ptr->command[i]);

		list_tmp = list_ptr->next;
		free(list_ptr->command);
		free(list_ptr);
		list_ptr = list_tmp;
	}

	return NULL;
}

void
clean_child_list(job_t *list_head)
{
	job_t *list_ptr, *list_tmp;

	list_ptr = list_head->next;

	while(list_ptr)
	{
		list_tmp = list_ptr;
		list_ptr = list_ptr->next;
		free(list_tmp);
	}
}


void
clean_everything(void)
{
	clean_child_list(&list_child);
	clean_child_list(&running_child);
	write_history(rshell_hist_file);
	return;
}
