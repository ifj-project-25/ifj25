
/**
 * @file expr_stack.c
 * @author xmikusm00
 * @brief Stack implementation for expression precedence parser
 * @details Implements a stack data structure used during precedence analysis
 *          of expressions. The stack stores both terminals (tokens) and non-terminals
 *          (AST nodes) during the bottom-up parsing process.
 */
#include "expr_stack.h"
#include "error.h"
#include "expr_ast.h"
#include "expr_parser.h"
#include "parser.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Initializes an empty expression precedence stack.
 * @param s Pointer to the stack to initialize.
 */
void expr_Pstack_init(ExprPstack *s) {
    ExprPstackNode *bottom = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
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
/**
 * @brief Frees all nodes in the expression precedence stack.
 * @param stack Pointer to the stack to free.
 */
void expr_Pstack_free(ExprPstack *stack) {
    ExprPstackNode *current = stack->top;
    ExprPstackNode *temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = NULL;
}
/**
 * @brief Pushes a terminal symbol onto the expression precedence stack.
 * @param stack Pointer to the stack.
 * @param token Pointer to the token to push.
 * @param sym The symbol type corresponding to the token.
 * @return int NO_ERROR on success, ERROR_INTERNAL on allocation failure.
 */
int expr_Pstack_push_term(ExprPstack *stack, Token *token, Sym sym) {
    ExprPstackNode *new_node = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        return ERROR_INTERNAL;
    }
    new_node->token = *token;
    new_node->type = SYM_TERM;
    new_node->sym = sym;
    new_node->next = stack->top;
    stack->top = new_node;
    return NO_ERROR;
}
/**
 * @brief Pushes a non-terminal symbol (expression node) onto the expression
 * precedence stack.
 * @param stack Pointer to the stack.
 * @param node Pointer to the expression node to push.
 * @return int NO_ERROR on success, ERROR_INTERNAL on allocation failure.
 */
int expr_Pstack_push_nonterm(ExprPstack *stack, ExprNode *node) {
    ExprPstackNode *new_node = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        return ERROR_INTERNAL;
    }
    new_node->node = node;
    new_node->type = SYM_NONTERM;
    new_node->sym = PS_TERM;
    new_node->token.type = TOKEN_UNDEFINED;
    new_node->next = stack->top;
    stack->top = new_node;
    return NO_ERROR;
}
/**
 * @brief Pops the top node from the expression precedence stack.
 * @param stack Pointer to the stack.
     */
void expr_Pstack_pop(ExprPstack *stack) {
    if (stack->top != NULL) {
        ExprPstackNode *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}

/**
 * @brief Returns the expression node from the top of the stack
 * @param stack Pointer to the stack
 * @return Pointer to the expression node at the top of the stack
 * @warning Does not check if the top element is actually a non-terminal.
 *          Caller must ensure the top contains a valid expression node.
 */
ExprNode *expr_Pstack_top(ExprPstack *stack) { return (stack->top->node); }

/**
 * @brief Checks if the stack is empty
 * @param stack Pointer to the stack
 * @return true if the stack is empty, false otherwise
 */
bool expr_Pstack_is_empty(ExprPstack *stack) { return (stack->top == NULL); }
