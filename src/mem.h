#ifndef MEM_HEADER
#define MEM_HEADER
/******************************************************************************
* File:             mem.h
*
* Author:           Reberti Carvalho Soares
* Created:          09/14/19
* Description:      Handle memory
*****************************************************************************/

#include "parse.h"
#include "jobs.h"

void *
realloc_string(void *old_ptr, size_t new_size);

void
clean_node_list(node_t *list_head);

void
clean_child_list(job_t *list_head);

node_t *
init_node_list(void);

void
clean_everything(void);

#endif
