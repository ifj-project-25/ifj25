# author: xcernoj00

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror 

TARGET = main

SRCS =  src/main.c \
		src/scanner.c \
		src/dynamic_string.c \
		src/symtable.c \
		src/semantic.c \
		

TEST_SYMTABLE_SRCS = test/test_symtable.c \
			src/dynamic_string.c \
			src/symtable.c \

TEST_SEMANTIC_SRCS = test/test_semantic.c \
			src/symtable.c \
			src/semantic.c
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test_symtable: $(TEST_SYMTABLE_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test_semantic: $(TEST_SEMANTIC_SRCS)
	@echo "Building semantic tests..."
	$(CC) $(CFLAGS) -Isrc -o $@ $^
	@echo "Running semantic tests..."
	./test_semantic

clean:
	rm -f $(TARGET) test_symtable test_semantic

.PHONY: all clean 
