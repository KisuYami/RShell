#ifndef BUILTIN_HEADER
#define BUILTIN_HEADER

/******************************************************************************
* File:             builtin.h
* Author:           Reberti Carvalho Soares
* Created:          09/05/19
* Description:      Shell built-in commands.
*****************************************************************************/

#include "shell.h"

#define MAX_PATH_SIZE 1024

int exec_builtin(struct TOKEN *head);
int is_file(char *path);

void builtin_exit(struct TOKEN *head);
void builtin_bg(struct TOKEN *head);
void builtin_fg(struct TOKEN *head);
void builtin_cd(struct TOKEN *head);
void builtin_ls(struct TOKEN *head);
void builtin_pwd(struct TOKEN *head);
void builtin_calc(struct TOKEN *head);
void builtin_clean(struct TOKEN *head);
void builtin_set_env(struct TOKEN *head);
void builtin_show_env(struct TOKEN *head);
#endif // BUILTIN_HEADER //
