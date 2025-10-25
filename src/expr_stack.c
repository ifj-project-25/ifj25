/**
 * @file parser.c
 * @author xmikusm00
 * @brief Stack implementation for expression evaluation
 */
#include "expr_stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "error.h"

void expr_stack_init(Tstack* stack) {
    stack->top = NULL;
}

void expr_stack_free(Tstack* stack) {
    TstackNode* current = stack->top;
    TstackNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}

void expr_stack_push(Tstack* stack, Token token) {
    TstackNode *new_node = (TstackNode*)malloc(sizeof(TstackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error in expr_stack_push\n");
        exit(ERROR_INTERNAL);
    }
    new_node->token = token;
    new_node->next = stack->top;
    stack->top = new_node;
}

void expr_stack_pop(Tstack* stack) {
    if (stack->top != NULL) {
        TstackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}

Token expr_stack_top(Tstack* stack) {
    return stack->top->token;
}

bool expr_stack_is_empty(Tstack* stack) {
    return (stack->top == NULL);
}
