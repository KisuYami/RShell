#ifndef NODE_BUILTIN_HEADER
#define NODE_BUILTIN_HEADER

/******************************************************************************
* File:             builtin.h
* Author:           Reberti Carvalho Soares
* Created:          09/05/19
* Description:      Shell built-in commands.
*****************************************************************************/

#include "parse.h"

int exec_builtin(node_t *head);
int is_file(char *path);

int builtin_exit(node_t *head);
int builtin_bg(node_t *head);
int builtin_fg(node_t *head);
int builtin_fg(node_t *head);
int builtin_cd(node_t *head);
int builtin_ls(node_t *head);
int builtin_pwd(node_t *head);
int builtin_calc(node_t *head);
int builtin_rand(node_t *head);
int builtin_clean(node_t *head);
int builtin_set_env(node_t *head);
int builtin_show_env(node_t *head);
#endif // NODE_BUILTIN_HEADER //
