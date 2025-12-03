/**
 * @file expr_ast.c
 * @author xmikusm00
 * @brief Expression Abstract Syntax Tree (AST) implementations
 */

#include "expr_ast.h"
#include "error.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a numeric literal expression node
 *
 * Allocates and initializes a new expression node representing a numeric
 * literal.
 *
 * @param value The numeric value to store
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_num_literal_node(double value) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_NUM_LITERAL;
    node->data.num_literal = value;
    return node;
}

/**
 * @brief Creates a string literal expression node
 *
 * Allocates and initializes a new expression node representing a string
 * literal. The string value is duplicated.
 *
 * @param value The string value to store
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_string_literal_node(const char *value) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
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

/**
 * @brief Creates a null literal expression node
 *
 * Allocates and initializes a new expression node representing a null literal.
 *
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_null_literal_node() {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_NULL_LITERAL;
    return node;
}

/**
 * @brief Creates a type literal expression node
 *
 * Allocates and initializes a new expression node representing a type literal.
 * The type name is duplicated.
 *
 * @param name The type name to store
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_type_node(const char *name) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_TYPE_LITERAL;
    node->data.identifier_name = my_strdup(name);
    if (!node->data.identifier_name) {
        free(node);
        return NULL;
    }
    return node;
}

/**
 * @brief Creates an identifier expression node
 *
 * Allocates and initializes a new expression node representing a variable
 * identifier. The identifier name is duplicated and the scope is initialized to
 * NULL.
 *
 * @param name The identifier name to store
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_identifier_node(const char *name) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_IDENTIFIER;
    node->data.identifier_name = my_strdup(name);
    node->current_scope = NULL;
    if (!node->data.identifier_name) {
        free(node);
        return NULL;
    }
    return node;
}

/**
 * @brief Creates a binary operation expression node
 *
 * Allocates and initializes a new expression node representing a binary
 * operation with left and right operands.
 *
 * @param op The binary operator type (e.g., OP_ADD, OP_MUL)
 * @param left Pointer to the left operand expression node
 * @param right Pointer to the right operand expression node
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_binary_op_node(BinaryOpType op, ExprNode *left,
                                ExprNode *right) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node) {
        return NULL;
    }
    node->type = EXPR_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

/**
 * @brief Creates a getter call expression node
 *
 * Allocates and initializes a new expression node representing a getter method
 * call. The getter name is duplicated.
 *
 * @param name The getter method name to store
 * @return Pointer to newly created node, or NULL if allocation fails
 */
ExprNode *create_getter_call_node(const char *name) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (!node)
        return NULL;
    node->type = EXPR_GETTER_CALL;
    node->data.getter_name = my_strdup(name);
    if (!node->data.getter_name) {
        free(node);
        return NULL;
    }
    return node;
}

/**
 * @brief Recursively frees an expression node and all its children
 *
 * Frees all dynamically allocated memory associated with an expression node,
 * including string data and child nodes (for binary operations). Safe to call
 * with NULL pointer.
 *
 * @param node Pointer to the expression node to free (can be NULL)
 */
void free_expr_node(ExprNode *node) {
    if (!node)
        return;

    switch (node->type) {
    case EXPR_STRING_LITERAL:
        free(node->data.string_literal);
        break;
    case EXPR_IDENTIFIER:
        free(node->data.identifier_name);
        break;
    case EXPR_GETTER_CALL:
        free(node->data.getter_name);
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
