#ifndef SHELL_HEADER
#define SHELL_HEADER

// define: MAX_PATH
// Posix std of max path chars.
#define MAX_PATH 1024

// define: MAX_COMMAND_SIZE
// Max command size, this will be used in malloc.
#define MAX_COMMAND_SIZE 1024

// define: COMMAND_BUF_SIZE
// Starting buf size, this will be used in malloc and the buf should be 
//resized on demand.
#define COMMAND_BUF_SIZE 1024

// define: TOKEN_BUFSIZE
// Max ammount of parsed arguments.
//
// Se Also:
// <shell_command_parser>
#define TOKEN_BUFSIZE 64

// define: TOKEN_DELIM
// What is considered a delimiter between arguments.
//
// Se Also:
// <shell_command_parser>
#define TOKEN_DELIM "\t\r\n\a\b "

// define: SHELL_CLEAN_VALUE
// The ammount of newlines when using clean builtin.
//
// See Also:
// <shell_builtin_clean>
#define SHELL_CLEAN_VALUE 50

// Group: Shell Routines
// ---------------------

/* 
Function: shell_command_input

   Print prompt and get input from user.

Parameters:

   command - Return the user input value via pointer

User Input is Read:

   === C ===
	while((command[count++] = getchar()) != '\n') {
		if(count > MAX_COMMAND_LINE) break;
	}
   =========

*/
void shell_command_input(char *command);

/*
Function: shell_command_fork

	Fork shell and wait return from child.

Parameters:
	
	command_argv - Parsed user input

See Also:
	
	<shell_command_parser>
*/
void shell_command_fork(char **command_argv);

/*
Function: shell_command_parser

	Parser user input in to null termined array of pointers.

Parameters:

	command_string - Unparsed string
	argv_buf - Parsed array

See Also:

	<shell_command_input>
*/
void shell_command_parser(char *command_string, char **argv_buf);

/*
Function: shell_command_builtin

	Check for built-in commands in user input.

Parameters:

	command_argv - Parsed user input
*/
int  shell_command_builtin(char **command_argv);

// Group: Shell Built-in Commands
// -----------------------------

// define: BUILTIN_NUMBER
// Size of shell_builtin_list
#define BUILTIN_NUMBER 4

// Function: shell_builtin_cd
// Change working dir.
static void shell_builtin_cd(const char **argv);

// Function: shell_builtin_bg
// Show background process.
static void shell_builtin_bg(const char **argv);

// Function: shell_builtin_fg
// Return to last process.
static void shell_builtin_fg(const char **argv);

// Function: shell_builtin_pwd
// Print the working path.
static void shell_builtin_pwd(const char **argv);

// Function: shell_builtin_exit
// Exit shell.
static void shell_builtin_exit(const char **argv);

// Function: shell_builtin_kill
// Kill a process.
static void shell_builtin_kill(const char **argv);

// Function: shell_builtin_clean
// Clean terminal screen.
static void shell_builtin_clean(const char **argv);

// Function: shell_builtin_printf
// Print organized string.
static void shell_builtin_printf(const char **argv);

/* 
struct: shell_list

   Containg the built in funcions.

Members:
	size - Size of the option
	option - String cotaining compared text
	func - Function executed when option matches
*/
struct shell_list {
	int size;
	char *option;
	void (*func)();
};

#endif /* ifndef SHELL_HEADER */
