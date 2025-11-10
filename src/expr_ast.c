/**
 * @file expr_ast.c
 * @author xmikusm00
 * @brief Expression Abstract Syntax Tree (AST) implementations
 */

#include "expr_ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"
#include "symtable.h"

static char *my_strdup(const char *s) {//ASK TEAM 
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len);
    if (!copy) return NULL; // have to be change to INTERNAL ERROR from error.h
    memcpy(copy, s, len);
    return copy;
}

ExprNode* create_num_literal_node(double value){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_NUM_LITERAL;
    node->data.num_literal = value;
    return node;
}

ExprNode* create_string_literal_node(const char* value){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_STRING_LITERAL;
    node->data.string_literal = my_strdup(value);
    if (!node->data.string_literal) {
        free(node);
        return NULL;
    }
    return node;
}

ExprNode* create_null_literal_node(){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_NULL_LITERAL;
    return node;
}

ExprNode* create_identifier_node(const char* name){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_IDENTIFIER;
    node->data.identifier_name = my_strdup(name);
    if (!node->data.identifier_name) {
        free(node);
        return NULL;
    }
    return node;
}

ExprNode* create_binary_op_node(BinaryOpType op, ExprNode* left, ExprNode* right){
    ExprNode* node = (ExprNode*)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
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

// Helper to get operator string
static const char* get_op_string(BinaryOpType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_EQ:  return "==";
        case OP_NEQ: return "!=";
        case OP_LT:  return "<";
        case OP_GT:  return ">";
        case OP_LTE: return "<=";
        case OP_GTE: return ">=";
        case OP_IS:  return "is";
        default:     return "???";
    }
}

// Print AST in tree format with indentation
void print_expr_ast(ExprNode* node, int indent) {
    if (!node) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("(null)\n");
        return;
    }

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case EXPR_NUM_LITERAL:
            printf("NUM: %.2f\n", node->data.num_literal);
            break;
        case EXPR_STRING_LITERAL:
            printf("STRING: \"%s\"\n", node->data.string_literal);
            break;
        case EXPR_NULL_LITERAL:
            printf("NULL\n");
            break;
        case EXPR_IDENTIFIER:
            printf("ID: %s\n", node->data.identifier_name);
            break;
        case EXPR_BINARY_OP:
            printf("BINARY_OP: %s\n", get_op_string(node->data.binary.op));
            print_expr_ast(node->data.binary.left, indent + 1);
            print_expr_ast(node->data.binary.right, indent + 1);
            break;
    }
}
