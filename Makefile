CC := gcc
CFLAGS := -Iinclude -Wall -Wextra -g
SRC_DIR := src
BIN_DIR := bin
TARGET := protocol

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)

.PHONY: all clean run

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

run: all
	./$(BIN_DIR)/$(TARGET)
