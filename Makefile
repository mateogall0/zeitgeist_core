CC := gcc

SAN_PROFILE ?= none

ifeq ($(SAN_PROFILE),asan)
  SAN_FLAGS := -fsanitize=address,undefined
endif

ifeq ($(SAN_PROFILE),ubsan)
  SAN_FLAGS := -fsanitize=undefined
endif

ifeq ($(SAN_PROFILE),tsan)
  SAN_FLAGS := -fsanitize=thread
endif

ifeq ($(SAN_PROFILE),lsan)
  SAN_FLAGS := -fsanitize=leak,undefined
endif

ifeq ($(SAN_PROFILE),none)
  SAN_FLAGS :=
endif


CFLAGS := -Iinclude \
	  -Itests/include \
	  -Wall \
	  -Wextra \
	  -std=gnu11 \
	  -DTEST_BUILD \
	  -g \
	  -pthread \
	  $(SAN_FLAGS)

SRC_DIR := src
TESTS_DIR := tests
BUILD_DIR := build
BIN_DIR := bin
TEST_TARGET := protocol_tests

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

COMMON_SRC := $(shell find $(SRC_DIR)/common -name '*.c')
SERVER_SRC := $(shell find $(SRC_DIR)/server -name '*.c')
CLIENT_SRC := $(shell find $(SRC_DIR)/client -name '*.c')

COMMON_OBJ := $(patsubst $(SRC_DIR)/common/%.c,$(BUILD_DIR)/common/%.o,$(COMMON_SRC))
SERVER_OBJ := $(patsubst $(SRC_DIR)/server/%.c,$(BUILD_DIR)/server/%.o,$(SERVER_SRC))
CLIENT_OBJ := $(patsubst $(SRC_DIR)/client/%.c,$(BUILD_DIR)/client/%.o,$(CLIENT_SRC))

TEST_SRC := $(shell find $(TESTS_DIR) -name '*.c')
TEST_OBJ := $(patsubst $(TESTS_DIR)/%.c,$(BUILD_DIR)/tests/%.o,$(TEST_SRC))

.PHONY: clean tests

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

# Build only common + server code (no link)
server_build: $(COMMON_OBJ) $(SERVER_OBJ)

# Build only common + client code (no link)
client_build: $(COMMON_OBJ) $(CLIENT_OBJ)


tests: $(BIN_DIR)/$(TEST_TARGET)
