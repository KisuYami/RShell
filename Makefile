TARGET_EXEC = rshell

CC= gcc
CLIBS= -lreadline -lncurses
CFLAGS = -O3

SRC_DIRS = ./src
BUILD_DIR = ./build
SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BUILD_DIR)/$(TARGET_EXEC)
	mkdir -p ${DESTDIR}/usr/bin/
	mkdir -p ${DESTDIR}/usr/share/man/man1/
	mkdir -p ${DESTDIR}/usr/share/man/pt_BR/man1/

	cp $(BUILD_DIR)/$(TARGET_EXEC) ${DESTDIR}/usr/bin/
	cp Packages/man/rshell.1 ${DESTDIR}/usr/share/man/man1/
	cp Packages/man/pt_BR/rshell.1 ${DESTDIR}/usr/share/man/pt_BR/man1/

clean:
	$(RM) -r $(BUILD_DIR)
