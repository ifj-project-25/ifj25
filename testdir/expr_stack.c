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

void expr_stack_init(ExprTstack* stack) {
    stack->top = NULL;
}

void expr_stack_free(ExprTstack* stack) {
    ExprTstackNode* current = stack->top;
    ExprTstackNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}
void expr_stack_push(ExprTstack* stack, ExprNode* node) {
    ExprTstackNode *new_node = (ExprTstackNode*)malloc(sizeof(ExprTstackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error in expr_stack_push\n");
        exit(ERROR_INTERNAL);
    }
    new_node->node = node;
    new_node->next = stack->top;
    stack->top = new_node;
}

void expr_stack_pop(ExprTstack* stack) {
    if (stack->top != NULL) {
        ExprTstackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}
//You must ensure the stack is not empty before calling this function
ExprNode* expr_stack_top(ExprTstack* stack) {
    return (stack->top->node);
}

bool expr_stack_is_empty(ExprTstack* stack) {
    return (stack->top == NULL);
}

void token_stack_init(TokenStack* stack) {
    stack->top = NULL;
}

void token_stack_free(TokenStack* stack) {
    TokenStackNode* current = stack->top;
    TokenStackNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}

void token_stack_push(TokenStack* stack, Token token) {
    TokenStackNode *new_node = (TokenStackNode*)malloc(sizeof(TokenStackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error in token_stack_push\n");
        exit(ERROR_INTERNAL);
    }
    new_node->token = token;
    new_node->next = stack->top;
    stack->top = new_node;
}

void token_stack_pop(TokenStack* stack) {
    if (stack->top != NULL) {
        TokenStackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}

Token *token_stack_top(TokenStack* stack) {
    return &(stack->top->token);
}

bool token_stack_is_empty(TokenStack* stack) {
    return (stack->top == NULL);
}
