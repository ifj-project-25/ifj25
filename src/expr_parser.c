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


void create_operator_node_with_operands(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode **expressionTree);

void untilLeftPar(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode *expressionTree) {
    while (!token_stack_is_empty(operator_stack)) {
        Token *top = token_stack_top(operator_stack);
        if(top->type == TOKEN_LPAREN) {
            token_stack_pop(operator_stack);
            return;
        }
        create_operator_node_with_operands(number_stack, operator_stack, &expressionTree);
    }
}

int expression_parser( Token *token, ExprTstack *number_stack, TokenStack *operator_stack, int *number_of_lparen, int *number_of_rparen, int *rc) {
    while (token->type != TOKEN_EOF && token->type != TOKEN_RCURLY) {
        if (token->type == TOKEN_RPAREN) {
            if (*number_of_rparen > *number_of_lparen) printf("leftP %d rightP %d\n", *number_of_lparen, *number_of_rparen);
        }
        switch (token->type)
        {
        case TOKEN_DOUBLE:// added support for double literals
        {
            // push number as ExprNode (operand stack)
            ExprNode *num_node = create_num_literal_node(token->value.decimal);
            expr_stack_push(number_stack, num_node);
            break;
        }
        case TOKEN_INTEGER: {
            // push number as ExprNode (operand stack)
            ExprNode *num_node = create_num_literal_node((double)token->value.integer);
            expr_stack_push(number_stack, num_node);
            break;
        }
        case TOKEN_IDENTIFIER: {
            // push identifier as ExprNode (operand stack)
            ExprNode *id_node = create_identifier_node(token->value.string->str);
            expr_stack_push(number_stack, id_node);
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            if(!token_stack_is_empty(operator_stack)){
                Token* top = token_stack_top(operator_stack);
                if (top->type == TOKEN_MULTIPLY || top->type == TOKEN_DIVIDE || top->type == TOKEN_MINUS) {
                    // TODO: reduce while higher/equal precedence
                    while (!token_stack_is_empty(operator_stack)) {
                        Token *top = token_stack_top(operator_stack);
                        if (top->type == TOKEN_PLUS || top->type == TOKEN_MINUS || top->type == TOKEN_MULTIPLY || top->type == TOKEN_DIVIDE) {
                            create_operator_node_with_operands(number_stack, operator_stack, NULL);
                        } else {
                            break;
                        }
                    }
                }
            }
            token_stack_push(operator_stack, *token);
            break;
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
            if(!token_stack_is_empty(operator_stack)){
                Token* top = token_stack_top(operator_stack);
                if (top->type == TOKEN_MULTIPLY || top->type == TOKEN_DIVIDE ){
                    // TODO: reduce while higher/equal precedence
                    while (!token_stack_is_empty(operator_stack)) {
                        Token *top = token_stack_top(operator_stack);
                        if (top->type == TOKEN_MULTIPLY || top->type == TOKEN_DIVIDE) {
                            create_operator_node_with_operands(number_stack, operator_stack, NULL);
                        } else {
                            break;
                        }
                    }
                }
            }
            token_stack_push(operator_stack, *token);
            break;
        case TOKEN_LPAREN:
            (*number_of_lparen)++;
            token_stack_push(operator_stack, *token);
            break;
        case TOKEN_RPAREN:
            (*number_of_rparen)++;
            if (*number_of_rparen > *number_of_lparen) printf("leftP %d rightP %d\n", *number_of_lparen, *number_of_rparen);
            untilLeftPar(number_stack, operator_stack, NULL);
            break;
        case TOKEN_EOL:
            break;
        default:
            printf("Unexpected token type: %d\n", token->type);

            *rc = SYNTAX_ERROR;
            return SYNTAX_ERROR; 
        }
        get_token(token);
    }
    return NO_ERROR;
}

ExprNode* expression_parser_main(int *error_code) {
    ExprTstack number_stack;
    TokenStack operator_stack;
    expr_stack_init(&number_stack);
    token_stack_init(&operator_stack);
    ExprNode *expressionTree = NULL;
    Token token;
    get_token(&token);
    int rc = NO_ERROR;
    int number_of_lparen = 0;
    int number_of_rparen = 0;

    expression_parser(&token,&number_stack,&operator_stack,&number_of_lparen,&number_of_rparen,&rc);
    // Final reduction: apply remaining operators
    while (!token_stack_is_empty(&operator_stack)) {
        create_operator_node_with_operands(&number_stack,&operator_stack,&expressionTree);
    }
    // If no operators (single operand), use it as the tree
    if (!expr_stack_is_empty(&number_stack) && expressionTree == NULL) {
        expressionTree = expr_stack_top(&number_stack);
    }
    if (rc != NO_ERROR) {
        expr_stack_free(&number_stack);
        token_stack_free(&operator_stack);
        return NULL;
    }

    expr_stack_free(&number_stack);
    token_stack_free(&operator_stack);
    return expressionTree;
}
void create_operator_node_with_operands(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode **expressionTree){
    // Ensure there are enough operands and an operator
    if (expr_stack_is_empty(number_stack) || token_stack_is_empty(operator_stack)) return;
    ExprNode *right = expr_stack_top(number_stack);
    expr_stack_pop(number_stack);
    if (expr_stack_is_empty(number_stack)) return;
    ExprNode *left = expr_stack_top(number_stack);
    expr_stack_pop(number_stack);

    Token *op_tok = token_stack_top(operator_stack);
    if (op_tok == NULL) return;
    BinaryOpType op;
    switch (op_tok->type) {
        case TOKEN_PLUS: op = OP_ADD; break;
        case TOKEN_MINUS: op = OP_SUB; break;
        case TOKEN_MULTIPLY: op = OP_MUL; break;
        case TOKEN_DIVIDE: op = OP_DIV; break;
        case TOKEN_LPAREN:
            // Should not happen, just return
            return;
        
        default:
            /* unknown operator: pop it and bail out to avoid using an uninitialized `op` */
            printf("Unsupported operator: %d\n", op_tok->type);
            token_stack_pop(operator_stack);
            return;
    }
    token_stack_pop(operator_stack);

    ExprNode* node = create_binary_op_node(op, left, right);
    // Push result back to operand stack for further reductions
    expr_stack_push(number_stack, node);
    // Also update the output pointer to the latest tree (only if caller provided one)
    if (expressionTree) {
        *expressionTree = node;
    }
}
