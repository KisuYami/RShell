TARGET_EXEC = rshell

CC 		= gcc
CLIBS 	= -lreadline -lncurses
CFLAGS 	= -Wall -Werror -pedantic -ggdb3

SRC_DIRS 	= ./src
BUILD_DIR 	= ./build

SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

CHECK_SRCS 		= $(shell find ./tests -name *.cpp -or -name *.c -or -name *.s)
CHECK_OBJS 		= $(CHECK_SRCS:%=$(BUILD_DIR)/%.o)
CHECK_DEPENS 	= build/src/mem.c.o build/src/shell.c.o build/src/builtin.c.o build/src/jobs.c.o

# Program binary
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# For source testing
check_all: $(BUILD_DIR)/check_all

$(BUILD_DIR)/check_all: $(CHECK_OBJS) $(CHECK_DEPENS)
	$(CC) $(CHECK_DEPENS) $(CHECK_OBJS) -o $@ $(LDFLAGS) $(CLIBS) -lcriterion

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -lcriterion -c $< -o $@

install: $(BUILD_DIR)/$(TARGET_EXEC)
	install  -g 0 -o 0 -m 0644 $(BUILD_DIR)/$(TARGET_EXEC) /usr/bin/
	chmod +x /usr/bin/rshell

	install -g 0 -o 0 -m 0644 man/rshell.1 /usr/share/man/man1/
	install -g 0 -o 0 -m 0644 man/pt_BR/rshell.1 /usr/share/man/pt_BR/man1/

	gzip -f /usr/share/man/man1/rshell.1
	gzip -f /usr/share/man/pt_BR/man1/rshell.1

clear:
	$(RM) -rf $(BUILD_DIR)
