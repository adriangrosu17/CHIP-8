TARGET_EXEC ?= chip8.elf

CC = gcc
CXX = g++
BUILD_DIR ?= build
SRC_DIRS ?= src
DEBUG ?= n
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS += inc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CFLAGS ?= -Wall -Wextra -Werror -pedantic -std=c11
ifeq ($(DEBUG), y)
	CFLAGS += -g
endif
LDFLAGS += -Xlinker -Map=$(BUILD_DIR)/$(TARGET_EXEC).map
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
		$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run:
	./$(BUILD_DIR)/$(TARGET_EXEC) $(filter-out $@,$(MAKECMDGOALS))
%:
	@:

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
-include $(DEPS)

MKDIR_P ?= mkdir -p
