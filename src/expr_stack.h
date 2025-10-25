/**
 * @file parser.c
 * @author xmikusm00
 * @brief Stack implementation for expression evaluation
 */
#ifndef EXPR_STACK_H
#define EXPR_STACK_H
#include "scanner.h"
#include <stdbool.h>

typedef struct TstackNode {
    Token token;
    struct TstackNode* next;
} TstackNode;

typedef struct Tstack {
    TstackNode* top;
} Tstack;

void expr_stack_init(Tstack* stack);
void expr_stack_free(Tstack* stack);
void expr_stack_push(Tstack* stack, Token token);
void expr_stack_pop(Tstack* stack);
Token expr_stack_top(Tstack* stack);
bool expr_stack_is_empty(Tstack* stack);




#endif //EXPR_STACK_H
