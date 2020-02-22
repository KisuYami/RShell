#ifndef ERR_H
#define ERR_H

/* Print a debug message with the function,
   file and line from where it's used */
#define error(from, message)					\
	fprintf(stderr, "[RSHELL-%s]: <%s> <%s:%d>: %s",	\
		from, __func__, __FILE__, __LINE__, message);	\

#endif /* ERR_H */
