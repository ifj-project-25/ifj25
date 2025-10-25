/**
 * @file parser.c
 * @author xmikusm00
 * @brief expression parser implementation
 */
#include "expr_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include "scanner.h"
#include "expr_stack.h"
#include "error.h"
#include "parser.h"
#include "symtable.h"
#include "expr_ast.h"

int expression_parser(const Token *token, Tstack *stack, int *number_of_lparen, int *number_of_rparen, int *rc, SymTable *AVL) {
     while (token->type != TOKEN_EOF && token->type != TOKEN_RCURLY) {
        if (token->type == TOKEN_RPAREN) {
            (*number_of_rparen)++;
            if (*number_of_rparen > *number_of_lparen) return 0;
        }
        expr_stack_push(stack, *token);
        parser_next_token();
        token = parser_current_token();
        if (token->type == TOKEN_INTEGER)
        {
            ExprNode* node = create_num_literal_node(token->value.integer);
            if (node == NULL) {
                return ERROR_INTERNAL;
            }
            symtable_insert(AVL, node);
            printf("Token value: %d\n", token->value.integer);
            
        }
        else if (token->type == TOKEN_PLUS)
        {
            printf("Token value: +\n");
        }
        else if (token->type == TOKEN_MINUS)
        {
            printf("Token value: -\n");
        }
        else if (token->type == TOKEN_MULTIPLY)
        {
            printf("Token value: *\n");
        }
        else if (token->type == TOKEN_DIVIDE)
        {
            printf("Token value: /\n");
        }
        else if (token->type == TOKEN_LPAREN)
        {
            printf("Token value: (\n");
            (*number_of_lparen)++;
        }
        else if (token->type == TOKEN_RPAREN)
        {
            printf("Token value: )\n");
        }
        else
        {
            /* code */
        }
        
        
    }
    return NO_ERROR;
}

int expression_parser_main(SymTable *AVL) {
    Tstack stack;
    const Token *token = parser_current_token();
    int rc = NO_ERROR;
    int number_of_lparen = 0;
    int number_of_rparen = 0;
    expr_stack_init(&stack);
    expression_parser(token, &stack, &number_of_lparen, &number_of_rparen, &rc, AVL);

    
    return NO_ERROR; // Success
}
