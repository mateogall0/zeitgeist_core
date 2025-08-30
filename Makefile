CC := gcc
CFLAGS := -Iinclude -Itests/include -Wall -Wextra -std=gnu11 -DTEST_BUILD -g -fsanitize=address       # general flags

SRC_DIR := src
TESTS_DIR := tests
BUILD_DIR := build
BIN_DIR := bin
TEST_TARGET := protocol_tests

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

TEST_SRC := $(shell find $(TESTS_DIR) -name '*.c')
TEST_OBJ := $(patsubst $(TESTS_DIR)/%.c,$(BUILD_DIR)/tests/%.o,$(TEST_SRC))

.PHONY: clean tests run-tests

# Main test binary
$(BIN_DIR)/$(TEST_TARGET): $(OBJ) $(TEST_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/tests/%.o: $(TESTS_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure bin folder exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

tests: $(BIN_DIR)/$(TEST_TARGET)

run-tests: tests
	./$(BIN_DIR)/$(TEST_TARGET)
