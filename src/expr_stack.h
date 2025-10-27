/**
 * @file parser.c
 * @author xmikusm00
 * @brief Stack implementation for expression evaluation
 */
#ifndef EXPR_STACK_H
#define EXPR_STACK_H
#include "scanner.h"
#include <stdbool.h>
#include "expr_ast.h"

typedef struct ExprTstackNode {
    ExprNode* node;
    struct ExprTstackNode* next;
} ExprTstackNode;

typedef struct ExprTstack {
    ExprTstackNode* top;
} ExprTstack;
    
void expr_stack_init(ExprTstack* stack);
void expr_stack_free(ExprTstack* stack);
void expr_stack_push(ExprTstack* stack, ExprNode* node);
void expr_stack_pop(ExprTstack* stack);
ExprNode* expr_stack_top(ExprTstack* stack);
bool expr_stack_is_empty(ExprTstack* stack);

// Stack for Token (operator stack)
typedef struct TokenStackNode {
    Token token;
    struct TokenStackNode* next;
} TokenStackNode;

typedef struct TokenStack {
    TokenStackNode* top;
} TokenStack;

void token_stack_init(TokenStack* stack);
void token_stack_free(TokenStack* stack);
void token_stack_push(TokenStack* stack, Token token);
void token_stack_pop(TokenStack* stack);
Token *token_stack_top(TokenStack* stack);
bool token_stack_is_empty(TokenStack* stack);

#endif //EXPR_STACK_H
