# Author: xmikusm00
# Build configuration for IFJ25 project

CC = gcc
CFLAGS = -std=c11 -Wall -Werror -g -O0

TARGET = main
TEST_PARSER = test_parser

SRCS =  src/main.c \
        src/scanner.c \
        src/dynamic_string.c \
        src/parser.c \
        src/symtable.c \
        src/expr_parser.c \
        src/expr_stack.c \
        src/expr_ast.c \
        src/ast.c 
         

# Default target – builds both binaries
all: $(TARGET) $(TEST_PARSER)

# Build main program
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Clean build artifacts
clean:
	rm -f $(TARGET) $(TEST_PARSER) src/*.o *.o

.PHONY: all clean
