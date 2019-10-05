#include <criterion/criterion.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "../src/shell.h"
#include "../src/builtin.h"
#include "../src/mem.h"

Test(shell, prompt)
{
	char *string = NULL;

	string = print_prompt();
	cr_expect(string != NULL,
			"Failed to get prompt string using string");

	cr_free(string);
}

Test(shell, parse_input)
{ struct TOKEN *test_head = NULL;
	char string[] = "ls | sort -R > test";

	test_head = parse_input(string);

	cr_assert(test_head != NULL,
			"Failed to get parsed imput from string using parse_input()");

	// Size testing
	cr_expect(test_head->size != 2,
			"Wrong size in parsed imput from string using parse_input()");

	// Pipe testing
	cr_expect(test_head->flags & PIPE,
			"Wrong flag set in parsed imput from string using parse_input()");

	cr_expect(test_head->next->flags & REDIRECTION,
			"Wrong flag set in parsed imput from string using parse_input()");

	// String testing
	cr_expect(strncmp(test_head->command[0], "ls", 2) == 0,
			"Wrong string in parsed imput from string using parse_input()");

	cr_expect(strncmp(test_head->next->command[0], "sort", 4) == 0,
			"Wrong string in parsed imput from string using parse_input()");
	cr_expect(strncmp(test_head->next->command[1], "-R", 2) == 0,
			"Wrong string in parsed imput from string using parse_input()");


	cr_expect(strncmp(test_head->next->next->command[0], "test", 4) == 0,
			"Wrong string in parsed imput from string using parse_input()");

	clean_TOKEN_list(test_head);
}
