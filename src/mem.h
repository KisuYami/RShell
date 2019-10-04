#ifndef MEM_HEADER
#define MEM_HEADER
/******************************************************************************
* File:             mem.h
*
* Author:           Reberti Carvalho Soares
* Created:          09/14/19
* Description:      Handle memory
*****************************************************************************/

#include "jobs.h"

void clean_TOKEN_list(struct TOKEN *list_head);
void clean_child_list(struct child *list_head);

char *realloc_string(char *old_ptr, size_t new_size);
struct TOKEN *init_TOKEN_list();
#endif
