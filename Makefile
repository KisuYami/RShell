TARGET_EXEC = rshell

CC= gcc
CLIBS= -lreadline -lncurses
CFLAGS 	= -O3
DEBUG_CFLAGS = -Wall -Werror -pedantic -ggdb3 -Wno-error=unknown-pragmas

SRC_DIRS = ./src
BUILD_DIR = ./build
SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

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
	mkdir -p ${DESTDIR}/usr/bin/
	mkdir -p ${DESTDIR}/usr/share/man/man1/
	mkdir -p ${DESTDIR}/usr/share/man/pt_BR/man1/

	cp $(BUILD_DIR)/$(TARGET_EXEC) ${DESTDIR}/usr/bin/
	cp man/rshell.1 ${DESTDIR}/usr/share/man/man1/
	cp man/pt_BR/rshell.1 ${DESTDIR}/usr/share/man/pt_BR/man1/

clean:
	$(RM) -r $(BUILD_DIR)
