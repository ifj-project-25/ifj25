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
#include "parser.h"
#include "expr_ast.h"
#include "expr_precedence_parser.h"
#include "expr_precedence_stack.h"
#include "scanner.h"


 void expr_Pstack_init(ExprPstack *s) {
    ExprPstackNode* bottom = (ExprPstackNode*)malloc(sizeof(ExprPstackNode));
    if (!bottom) {
        fprintf(stderr, "Memory allocation error in expr_Pstack_init\n");
        exit(ERROR_INTERNAL);
    }
    bottom->next = NULL;
    bottom->type = SYM_TERM;
    bottom->sym = PS_DOLLAR;
    bottom->token.type = TOKEN_DOLLAR;
    s->top = bottom;
}

void expr_Pstack_free(ExprPstack* stack) {
    ExprPstackNode* current = stack->top;
    ExprPstackNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}

void expr_Pstack_push_term(ExprPstack* stack, Token* token, Sym sym) {
    ExprPstackNode *new_node = (ExprPstackNode*)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error in expr_Pstack_push\n");
        exit(ERROR_INTERNAL);
    }
    new_node->token = *token;
    new_node->type = SYM_TERM;
    new_node->sym = sym;
    new_node->next = stack->top;
    stack->top = new_node;
}
void expr_Pstack_push_nonterm(ExprPstack* stack, ExprNode* node) {
    ExprPstackNode *new_node = (ExprPstackNode*)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        fprintf(stderr, "Memory allocation error in expr_Pstack_push\n");
        exit(ERROR_INTERNAL);
    }
    new_node->node = node;
    new_node->type = SYM_NONTERM;
    new_node->sym = PS_TERM; 
    new_node->token.type = TOKEN_UNDEFINED;
    new_node->next = stack->top;
    stack->top = new_node;
}
void expr_Pstack_pop(ExprPstack* stack) {
    if (stack->top != NULL) {
        ExprPstackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}

ExprNode* expr_Pstack_top(ExprPstack* stack) {
    return (stack->top->node);
}

bool expr_Pstack_is_empty(ExprPstack* stack) {
    return (stack->top == NULL);
}
