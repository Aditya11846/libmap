###############################################################################
# Configuration
###############################################################################

CC       := gcc
SRC_DIR  := src
INC_DIR  := include
TEST_DIR := tests
BUILD_DIR:= build
BIN_DIR  := bin

# Automatically add all subdirectories under include/
INC_DIRS := $(shell find $(INC_DIR) -type d)
CFLAGS   := -std=c99 -O3 -Wall -Wextra -Werror -pedantic $(addprefix -I, $(INC_DIRS))

AR       := ar
ARFLAGS  := rcs

###############################################################################
# Sources and Targets
###############################################################################

# Recursively find all C source files in src/ and tests/
SRC_FILES  := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_FILES  := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

LIB_NAME   := libmap.a
LIB_TARGET := $(BUILD_DIR)/$(LIB_NAME)

TEST_FILES := $(shell find $(TEST_DIR) -type f -name '*.c')
TEST_OBJS  := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_FILES))
TEST_BINS  := $(patsubst $(BUILD_DIR)/%.o,$(BIN_DIR)/%,$(TEST_OBJS))

###############################################################################
# Primary targets
###############################################################################

# 'all' builds the library and all test executables
all: $(LIB_TARGET) $(TEST_BINS)

# Build the static library from object files
$(LIB_TARGET): $(OBJ_FILES)
	@echo "  AR      $@"
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

# Build each test executable by linking the test object and the library
$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(LIB_TARGET)
	@echo "  LINK    $@"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

###############################################################################
# Pattern rules for building object files
###############################################################################

# Compile .c files from src/ into object files, preserving subdirectory paths
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile .c files from tests/ into object files, preserving subdirectory paths
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@echo "  CC      $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Housekeeping
###############################################################################

.PHONY: clean

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(BIN_DIR)
