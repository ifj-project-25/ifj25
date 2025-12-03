# author: xcernoj00,xmikusm00,xmalikm00

CC = gcc
CFLAGS = -g -std=c11 -Wall -Werror -Wextra

TARGET = main

# Support both src/ directory structure and flattened structure
SRC_DIR = $(if $(wildcard src/),src/,)

SRCS =  $(SRC_DIR)main.c \
        $(SRC_DIR)scanner.c \
        $(SRC_DIR)dynamic_string.c \
        $(SRC_DIR)parser.c \
        $(SRC_DIR)symtable.c \
        $(SRC_DIR)expr_ast.c \
        $(SRC_DIR)ast.c \
		$(SRC_DIR)semantic.c \
		$(SRC_DIR)expr_parser.c \
		$(SRC_DIR)expr_stack.c \
		$(SRC_DIR)generator.c

TEST_SYMTABLE_SRCS = test/test_symtable.c \
			$(SRC_DIR)dynamic_string.c \
			$(SRC_DIR)symtable.c \

TEST_SEMANTIC_SRCS = test/test_semantic.c \
			$(SRC_DIR)expr_ast.c \
			$(SRC_DIR)ast.c \
			$(SRC_DIR)symtable.c \
			$(SRC_DIR)semantic.c
TEST_SEMANTIC_BASIC_SRCS = test/test_semantic_basic.c \
			$(SRC_DIR)expr_ast.c \
			$(SRC_DIR)ast.c \
			$(SRC_DIR)symtable.c \
			$(SRC_DIR)semantic.c

TEST_PARSER_SRCS = test/test_parser_runner.c \
			$(SRC_DIR)scanner.c \
			$(SRC_DIR)dynamic_string.c \
			$(SRC_DIR)parser.c \
			$(SRC_DIR)symtable.c \
			$(SRC_DIR)expr_parser.c \
			$(SRC_DIR)expr_stack.c \
			$(SRC_DIR)expr_ast.c \
			$(SRC_DIR)ast.c \
			$(SRC_DIR)semantic.c \
			$(SRC_DIR)expr_precedence_parser.c \
			$(SRC_DIR)expr_precedence_stack.c

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

test_complet: $(TARGET)
	@chmod +x test/test_complet.sh
	@./test/test_complet.sh $(FILE)

clean:
	rm -f $(TARGET) test_symtable test_semantic test_semantic_basic test_parsem
	rm -f *.exe log.txt *.ifj25
	rm -f $(ZIP_NAME).zip

count_lines:
	@chmod +x count_lines.sh
	@./count_lines.sh

.PHONY: all clean zip test_complet count_lines 

ZIP_NAME = xklusaa00
zip:
	zip -j $(ZIP_NAME).zip \
		src/*.c \
		src/*.h \
		rozdeleni \
		dokumentace.pdf \
		rozsireni \
		Makefile
