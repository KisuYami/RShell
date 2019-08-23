#ifndef JOBS_HEADER
#define JOBS_HEADER

/******************************************************************************
* define: CHILD_MAX_NUMBER
* Description: Max number of childs
*****************************************************************************/
#define CHILD_MAX_NUMBER 10

static int process_child_last;
static int process_child_numb;

struct process_child_list {
	int number;
	char name[256];
	pid_t pid;
} process_parent, process_child_list;


/******************************************************************************
* Function: process_child_register
* Description: Register child information
*
* See Also: <CHILD_MAX_NUMBER>
*****************************************************************************/
void process_child_register(char *child_name);

/******************************************************************************
* define: signal_sigintr
* Description: Handle signal interrupt. The shell make sure to only kill the 
* child
*****************************************************************************/
void signal_sigintr();

/******************************************************************************
* define: signal_sigstop
* Description: Handle signal stop. The shell make sure to only stop the 
* child
*****************************************************************************/
void signal_sigstop(pid_t child_pgid);
#endif /* ifndef JOBS_HEADER */
