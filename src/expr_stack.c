/**
 * @file expr_precedence_stack.c
 * @author xmikusm00
 * @brief Stack implementation for expression precedence parser
 * @details
 * This module implements a stack data structure used by the precedence parser
 * for expression evaluation. The stack stores both terminal symbols (tokens)
 * and non-terminal symbols (expression AST nodes) during bottom-up parsing.
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
 * @brief Initializes the expression precedence stack with a bottom marker.
 * @param s Pointer to the stack structure to initialize.
 * @param rc Pointer to error code variable (set to ERROR_INTERNAL on allocation
 * failure).
 * @details Creates a bottom-of-stack sentinel node with PS_DOLLAR symbol.
 */
void expr_Pstack_init(ExprPstack *s, int *rc) {
    ExprPstackNode *bottom = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
    if (!bottom) {
        *rc = ERROR_INTERNAL;
        return;
    }
    bottom->next = NULL;
    bottom->type = SYM_TERM;
    bottom->sym = PS_DOLLAR;
    bottom->token.type = TOKEN_DOLLAR;
    s->top = bottom;
}

/**
 * @brief Frees all nodes in the stack and resets it to empty state.
 * @param stack Pointer to the stack to free.
 * @details Iterates through all nodes, deallocating each one. Sets top to NULL.
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
 * @brief Pushes a terminal symbol (token) onto the stack.
 * @param stack Pointer to the stack.
 * @param token Pointer to the token to push (copied into new node).
 * @param sym Symbol type from precedence table.
 * @param rc Pointer to error code (set to ERROR_INTERNAL on allocation
 * failure).
 * @details Creates a new SYM_TERM node and pushes it to the top of the stack.
 */
void expr_Pstack_push_term(ExprPstack *stack, Token *token, Sym sym, int *rc) {
    ExprPstackNode *new_node = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        *rc = ERROR_INTERNAL;
        return;
    }
    new_node->token = *token;
    new_node->type = SYM_TERM;
    new_node->sym = sym;
    new_node->next = stack->top;
    stack->top = new_node;
}

/**
 * @brief Pushes a non-terminal symbol (expression AST node) onto the stack.
 * @param stack Pointer to the stack.
 * @param node Pointer to the expression AST node to push.
 * @param rc Pointer to error code (set to ERROR_INTERNAL on allocation
 * failure).
 * @details Creates a new SYM_NONTERM node representing a reduced expression.
 */
void expr_Pstack_push_nonterm(ExprPstack *stack, ExprNode *node, int *rc) {
    ExprPstackNode *new_node = (ExprPstackNode *)malloc(sizeof(ExprPstackNode));
    if (!new_node) {
        *rc = ERROR_INTERNAL;
        return;
    }
    new_node->node = node;
    new_node->type = SYM_NONTERM;
    new_node->sym = PS_TERM;
    new_node->token.type = TOKEN_UNDEFINED;
    new_node->next = stack->top;
    stack->top = new_node;
}

/**
 * @brief Removes the top element from the stack.
 * @param stack Pointer to the stack.
 * @details Pops and frees the top node. Does nothing if stack is empty.
 */
void expr_Pstack_pop(ExprPstack *stack) {
    if (stack->top != NULL) {
        ExprPstackNode *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
}

/**
 * @brief Returns the expression node from the top of the stack.
 * @param stack Pointer to the stack.
 * @return Pointer to the ExprNode at the top.
 * @warning Does not check if top is actually a non-terminal node.
 */
ExprNode *expr_Pstack_top(ExprPstack *stack) { return (stack->top->node); }

/**
 * @brief Checks if the stack is empty.
 * @param stack Pointer to the stack.
 * @return true if stack is empty (top is NULL), false otherwise.
 */
bool expr_Pstack_is_empty(ExprPstack *stack) { return (stack->top == NULL); }
