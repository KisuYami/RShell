TARGET_EXEC = rshell
VERSION = v0.01

SRC_DIRS = ./src
BUILD_DIR = ./build

CC= clang
CLIBS= -L/usr/local/musl/lib

SRCS = $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS = $(shell find $(SRC_DIRS) -type d)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

CPPFLAGS = $(INC_FLAGS) -MMD -MP -Wall -pedantic -g

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P = mkdir -p
