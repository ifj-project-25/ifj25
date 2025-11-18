# author: xcernoj00,xmikusm00,xmalikm00

CC = gcc
CFLAGS = -g -std=c11 -Wall  

TARGET = main

SRCS =  src/main.c \
        src/scanner.c \
        src/dynamic_string.c \
        src/parser.c \
        src/symtable.c \
        src/expr_parser.c \
        src/expr_stack.c \
        src/expr_ast.c \
        src/ast.c \
		src/semantic.c \
		src/expr_precedence_parser.c \
		src/expr_precedence_stack.c \
		src/generator.c \
		

TEST_SYMTABLE_SRCS = test/test_symtable.c \
			src/dynamic_string.c \
			src/symtable.c \

TEST_SEMANTIC_SRCS = test/test_semantic.c \
			src/expr_ast.c \
			src/ast.c \
			src/symtable.c \
			src/semantic.c
TEST_SEMANTIC_BASIC_SRCS = test/test_semantic_basic.c \
			src/expr_ast.c \
			src/ast.c \
			src/symtable.c \
			src/semantic.c

TEST_PARSER_SRCS = test/test_parser_runner.c \
			src/scanner.c \
			src/dynamic_string.c \
			src/parser.c \
			src/symtable.c \
			src/expr_parser.c \
			src/expr_stack.c \
			src/expr_ast.c \
			src/ast.c \
			src/semantic.c \
			src/expr_precedence_parser.c \
			src/expr_precedence_stack.c

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
test_semantic_basic: $(TEST_SEMANTIC_BASIC_SRCS)
	@echo "Building basic semantic tests..."
	$(CC) $(CFLAGS) -Isrc -o $@ $^
	@echo "Running basic semantic tests..."
	./test_semantic_basic
test_parsem: $(SRCS)
	$(CC) $(CFLAGS) -Isrc -o main $^
	@./test/test_parsem.sh
clean:
	rm -f $(TARGET) test_symtable test_semantic test_semantic_basic test_parsem
	rm -f *.exe log.txt *.ifj25

.PHONY: all clean 
