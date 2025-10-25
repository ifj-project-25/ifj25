/**
 * @file expr_ast.c
 * @author xmikusm00
 * @brief Expression Abstract Syntax Tree (AST) implementations
 */

#include "expr_ast.h"
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "symtable.h"

ExprNode* create_num_literal_node(double value){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return ERROR_INTERNAL;
    }
    node->type = EXPR_NUM_LITERAL;
    node->data.num_literal = value;
    return node;
}

ExprNode* create_string_literal_node(const char* value){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return ERROR_INTERNAL;
    }
    node->type = EXPR_STRING_LITERAL;
    node->data.string_literal = strdup(value);
    return node;
}

ExprNode* create_null_literal_node(){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return ERROR_INTERNAL;
    }
    node->type = EXPR_NULL_LITERAL;
    return node;
}

ExprNode* create_identifier_node(const char* name){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return ERROR_INTERNAL;
    }
    node->type = EXPR_IDENTIFIER;
    node->data.identifier_name = strdup(name);
    return node;
}

ExprNode* create_binary_op_node(BinaryOpType op, ExprNode* left, ExprNode* right){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return ERROR_INTERNAL;
    }
    node->type = EXPR_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

void free_expr_node(ExprNode* node){
    if (!node) return;

    switch (node->type) {
        case EXPR_STRING_LITERAL:
            free(node->data.string_literal);
            break;
        case EXPR_IDENTIFIER:
            free(node->data.identifier_name);
            break;
        case EXPR_BINARY_OP:
            free_expr_node(node->data.binary.left);
            free_expr_node(node->data.binary.right);
            break;
        default:
            break;
    }
    free(node);
}
