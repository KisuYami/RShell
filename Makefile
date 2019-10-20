TARGET_EXEC = rshell

CC 		= gcc
CLIBS 	= -lreadline -lncurses
CFLAGS 	= -O3
DEBUG_CFLAGS = -Wall -Werror -pedantic -ggdb3 -Wno-error=unknown-pragmas

SRC_DIRS 	= ./src
BUILD_DIR 	= ./build

SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

# Program binary
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
ifdef DEBUG
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

install: $(BUILD_DIR)/$(TARGET_EXEC)
	install  -g 0 -o 0 -m 0644 $(BUILD_DIR)/$(TARGET_EXEC) /usr/bin/
	chmod +x /usr/bin/rshell

	install -g 0 -o 0 -m 0644 ./Packages/man/rshell.1 /usr/share/man/man1/
	install -g 0 -o 0 -m 0644 ./Packages/man/pt_BR/rshell.1 /usr/share/man/pt_BR/man1/

	gzip -f /usr/share/man/man1/rshell.1
	gzip -f /usr/share/man/pt_BR/man1/rshell.1

clear:
	$(RM) -rf $(BUILD_DIR)
