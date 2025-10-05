# author: xcernoj00

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror 

TARGET = main

SRCS =  src/main.c \
		src/scanner.c \
		src/dynamic_string.c \
		src/symtable.c \

TEST_SYMTABLE_SRCS = test/test_symtable.c \
			src/dynamic_string.c \
			src/symtable.c \

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test_symtable: $(TEST_SYMTABLE_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) test_symtable

.PHONY: all clean
