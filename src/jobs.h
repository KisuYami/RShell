#ifndef JOBS_HEADER
#define JOBS_HEADER

#define CHILD_MAX_NUMBER 10
static int process_child_last;
static int process_child_numb;

struct process_child_list {
	int number;
	char *name;
	pid_t pid;
} process_child_list[CHILD_MAX_NUMBER];

void process_child_register(char *child_name);
void process_child_set_fg();
void signal_sigstop();
void signal_sigcont();
void signal_sigintr();
void signal_sigkill();

#endif /* ifndef JOBS_HEADER */
