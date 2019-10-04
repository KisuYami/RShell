TARGET_EXEC = rshell
VERSION = v1.1

SRC_DIRS = ./src
BUILD_DIR = ./build

CC= gcc
CLIBS= -lreadline -lncurses -g

SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS = $(shell find $(SRC_DIRS) -type d)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

CPPFLAGS = $(INC_FLAGS) -O3 -Wall -Werror

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

install: $(BUILD_DIR)/$(TARGET_EXEC)
	install  -g 0 -o 0 -m 0644 $(BUILD_DIR)/$(TARGET_EXEC) /usr/bin/
	chmod +x /usr/bin/rshell

	install -g 0 -o 0 -m 0644 man/rshell.1 /usr/share/man/man1/
	install -g 0 -o 0 -m 0644 man/pt_BR/rshell.1 /usr/share/man/pt_BR/man1/

	gzip -f /usr/share/man/man1/rshell.1
	gzip -f /usr/share/man/pt_BR/man1/rshell.1

docs:
	mkdir -p ./Documentation
	naturaldocs2 ./.ND-Config

clean:
	$(RM) -r $(BUILD_DIR)
