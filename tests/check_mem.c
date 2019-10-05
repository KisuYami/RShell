#include <criterion/criterion.h>
#include "../src/shell.h"
#include "../src/jobs.h"
#include "../src/mem.h"

Test(memory, TOKEN_tests)
{
	struct TOKEN *test;

	test = init_TOKEN_list();
	cr_assert(test != NULL,
			"Failed to initialize TOKEN with init_TOKEN_list()");

	clean_TOKEN_list(test);
	cr_assert(test != NULL,
			"Failed to free TOKEN with clean_TOKEN_list()");
}


Test(memory, realloc_string)
{
	char *test = NULL;

	test = cr_malloc(sizeof(char) * 256);
	test = realloc_string(test, sizeof(char) * 50);

	cr_expect(test != NULL,
			"Failed to initialize TOKEN with init_TOKEN_list()");

	cr_free(test);
}
