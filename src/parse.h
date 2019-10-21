#ifndef PARSE_H
#define PARSE_H

struct TOKEN *
parse_input(char *);

void
parse_string(struct TOKEN *, char *, size_t *);

#endif /* PARSE_H */
