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
	install -Dm644 $(BUILD_DIR)/$(TARGET_EXEC) ${DESTDIR}/usr/bin/
	install -Dm644 man/rshell.1 ${DESTDIR}/usr/share/man/man1/
	install -Dm644 man/pt_BR/rshell.1 ${DESTDIR}/usr/share/man/pt_BR/man1/
	chmod +x ${DESTDIR}/usr/bin/${TARGET_EXEC}

pkg:
	makepkg

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r pkg
	$(RM) -r RShell
	$(RM) -r src/RShell
	$(RM) -r *.pkg.*
