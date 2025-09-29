# author: xcernoj00

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror 

TARGET = main

SRCS =  src/main.c \
		src/scanner.c \
		src/dynamic_string.c \

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all
