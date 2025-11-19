/**
 * @file parser.c
 * @author xmikusm00
 * @brief expression parser implementation
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "expr_stack.h"
#include "error.h"
#include "parser.h"
#include "symtable.h"
#include "expr_ast.h"
#include "expr_parser.h"

bool is_previous_token_operator;
bool is_previous_token_identifier;
void create_operator_node_with_operands(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode **expressionTree,int *rc);

void untilLeftPar(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode *expressionTree,int *rc) {
    while (!token_stack_is_empty(operator_stack)) {
        Token *top = token_stack_top(operator_stack);
        if(top->type == TOKEN_LPAREN) {
            token_stack_pop(operator_stack);
            return;
        }
        create_operator_node_with_operands(number_stack, operator_stack, &expressionTree,rc);
        if (*rc != NO_ERROR) {
            return;
        }
    }
}
int operator_priority(Token *op) {
    switch (op->type)
    {
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
        return 6;
        break;

    case TOKEN_PLUS:
    case TOKEN_MINUS:
        return 5;
        break;
    
    case TOKEN_LESSER:
    case TOKEN_GREATER: 
    case TOKEN_LESSER_EQUAL:
    case TOKEN_GREATER_EQUAL:
        return 4;
        break;
    case KEYWORD_IS:
        return 3;
        break;
    case TOKEN_LOGIC_EQUAL:
    case TOKEN_NEQUAL:
        return 2;
        break;
    case TOKEN_LPAREN:
        return 1;
        break;
    default:
        return -1;
    }
}
void expression_parser( Token *token, ExprTstack *number_stack, TokenStack *operator_stack, int *rc) {
    int number_of_lparen = 0;
    int number_of_rparen = 0;
    while (token->type != TOKEN_EOF && token->type != TOKEN_RCURLY && token->type != TOKEN_EOL && token->type != TOKEN_COMMA) {
        if (token->type == TOKEN_RPAREN) {
        }
        switch (token->type)
        {
        case TOKEN_DOUBLE:// added support for double literals // Do i need them ? 
        {
            
            // push number as ExprNode (operand stack)
            ExprNode *num_node = create_num_literal_node(token->value.decimal);
            expr_stack_push(number_stack, num_node);
            break;
        }
        case TOKEN_INTEGER: 
            if(is_previous_token_operator == false){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = false;
            is_previous_token_identifier = false;
            // push number as ExprNode (operand stack)
            ExprNode *num_node = create_num_literal_node((double)token->value.integer);
            expr_stack_push(number_stack, num_node);
            break;
        case TOKEN_IDENTIFIER:
            if(is_previous_token_operator == false){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = false;
            is_previous_token_identifier = true;
            // push identifier as ExprNode (operand stack)
            ExprNode *id_node = create_identifier_node(token->value.string->str);
            expr_stack_push(number_stack, id_node);
            break;
        case TOKEN_GLOBAL_VAR: //TODO: i dont know this one
            if(is_previous_token_operator == false){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = false;
            is_previous_token_identifier = false;
            printf("GLOBAL VAR in expr parser not implemented\n");
            break;
        case TOKEN_STRING:
            if(is_previous_token_operator == false){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = false;
            is_previous_token_identifier = false;
            // push string as ExprNode (operand stack)
            ExprNode *str_node = create_string_literal_node(token->value.string->str);
            expr_stack_push(number_stack, str_node);
            break;
        case TOKEN_KEYWORD:
            if(token->value.keyword != KEYWORD_IS){
                printf("Unexpected keyword in expression: %d\n", token->value.keyword);
                *rc = SYNTAX_ERROR;
                return ;
            }
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        case TOKEN_LESSER:
        case TOKEN_GREATER:
        case TOKEN_LESSER_EQUAL:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_LOGIC_EQUAL:
        case TOKEN_NEQUAL:
            if(is_previous_token_operator == true){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = true;
            is_previous_token_identifier = false;
            if(!token_stack_is_empty(operator_stack)){
                Token* top = token_stack_top(operator_stack);
                if (operator_priority(top) >= operator_priority(token) ){
                    while (!token_stack_is_empty(operator_stack)) {
                        Token *top = token_stack_top(operator_stack);
                        if (operator_priority(top) >= operator_priority(token)) {
                            create_operator_node_with_operands(number_stack, operator_stack, NULL,rc);
                        } else {
                            break;
                        }
                    }
                }
            }
            token_stack_push(operator_stack, *token);
            break;
        
        case TOKEN_LPAREN:
            if(is_previous_token_operator == false ){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = true;
            is_previous_token_identifier = false;
            (number_of_lparen)++;
            token_stack_push(operator_stack, *token);
            break;
        case TOKEN_RPAREN:
            if(is_previous_token_operator == true){
                printf("Two operands in a row: %d %d\n", token->type, token->type);
                *rc = SYNTAX_ERROR;
                return ;
            }
            is_previous_token_operator = false;
            is_previous_token_identifier = false;
            (number_of_rparen)++;
            if (number_of_rparen > number_of_lparen) {
                untilLeftPar(number_stack, operator_stack, NULL,rc);
                return ;
            }
            untilLeftPar(number_stack, operator_stack, NULL,rc);
            break;

        case TOKEN_EOL:
            untilLeftPar(number_stack, operator_stack, NULL,rc);
            return ;
        default:
            printf("EXPRESSION_PARSER: Unexpected token type: %d\n", token->type);

            *rc = SYNTAX_ERROR;
            return ; 
        }
        get_token(token);
        if (*rc != NO_ERROR) {
            return ;
        }
    }
    return ;
}

ASTNode* expression_parser_main(Token *token, int *rc) {
    ExprTstack number_stack;
    TokenStack operator_stack;
    expr_stack_init(&number_stack);
    token_stack_init(&operator_stack);
    ExprNode *expressionTree = NULL;
    is_previous_token_operator = true;  // Allow first operand
    is_previous_token_identifier = false;

    if (token->type == TOKEN_IDENTIFIER){
        ExprNode *id_node = create_identifier_node( token->value.string->str);
        expr_stack_push(&number_stack, id_node);
        is_previous_token_operator = false;
        is_previous_token_identifier = true;
        get_token(token);
        if (*rc != NO_ERROR) {
            return NULL;
        }
        if (token->type == TOKEN_LPAREN){
            ASTNode* call_node = create_ast_node(AST_FUNC_CALL, id_node->data.identifier_name);
            return call_node;
        }

    }

    expression_parser(token,&number_stack,&operator_stack,rc);
    // Final reduction: apply remaining operators
    while (!token_stack_is_empty(&operator_stack)) {
        create_operator_node_with_operands(&number_stack,&operator_stack,&expressionTree,rc);
    }
    // If no operators (single operand), use it as the tree
    if (!expr_stack_is_empty(&number_stack) && expressionTree == NULL) {
        expressionTree = expr_stack_top(&number_stack);
        expr_stack_pop(&number_stack);
    }
    if (*rc != NO_ERROR) {
        expr_stack_free(&number_stack);
        token_stack_free(&operator_stack);
        return NULL;
    }
    
    expr_stack_free(&number_stack);
    token_stack_free(&operator_stack);

    ASTNode* exprNodeWrapper = create_ast_node(AST_EXPRESSION, NULL);
    exprNodeWrapper->expr = expressionTree;
    return exprNodeWrapper;
}
void create_operator_node_with_operands(ExprTstack *number_stack, TokenStack *operator_stack, ExprNode **expressionTree,int *rc){
    // Ensure there is an operator
    if (token_stack_is_empty(operator_stack)) {
        *rc = SYNTAX_ERROR;
        return;
    }

    // Need two operands for a binary operator. If not available, set error
    // and pop the operator so caller's reduction loop can make progress.
    if (expr_stack_is_empty(number_stack)) {
        *rc = SYNTAX_ERROR;
        // pop the operator to avoid infinite loops in final reduction
        token_stack_pop(operator_stack);
        return;
    }

    ExprNode *right = expr_stack_top(number_stack);
    expr_stack_pop(number_stack);
    if (expr_stack_is_empty(number_stack)) {
        // only one operand available: restore it and remove the operator
        expr_stack_push(number_stack, right);
        token_stack_pop(operator_stack);
        *rc = SYNTAX_ERROR;
        return;
    }
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
        case TOKEN_LESSER: op = OP_LT; break;
        case TOKEN_GREATER: op = OP_GT; break;
        case TOKEN_LESSER_EQUAL: op = OP_LTE; break;
        case TOKEN_GREATER_EQUAL: op = OP_GTE; break;
        case TOKEN_LOGIC_EQUAL: op = OP_EQ; break;
        case TOKEN_NEQUAL: op = OP_NEQ; break;
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
