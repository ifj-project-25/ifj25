/**
* @file expr_ast.h
* @author xmikusm00
* @brief Expression Abstract Syntax Tree (AST) definitions
*/
#ifndef EXPR_AST_H
#define EXPR_AST_H

typedef enum {
    EXPR_NUM_LITERAL,
    EXPR_STRING_LITERAL,
    EXPR_NULL_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_BINARY_OP,
} ExprNodeType;

typedef enum{
    OP_ADD, // +
    OP_SUB, // -
    OP_MUL, // *
    OP_DIV, // /
    OP_EQ,  // ==
    OP_NEQ, // !=
    OP_LT,  // <
    OP_GT,  // >
    OP_LTE, // <=
    OP_GTE, // >=
    OP_IS   // IS
} BinaryOpType;

typedef struct ExprNode {
    ExprNodeType type;
    union 
    {
        double num_literal; // For EXPR_NUM_LITERAL
        char* string_literal; // For EXPR_STRING_LITERAL
        char* identifier_name                                                                                                                                                                                                                                                                                                                                                                                      ; // For EXPR_IDENTIFIER
        struct 
        {
            BinaryOpType op;
            struct ExprNode* left;
            struct ExprNode* right;
        } binary;
    } data;
} ExprNode;

ExprNode* create_num_literal_node(double value);
ExprNode* create_string_literal_node(const char* value);
ExprNode* create_null_literal_node();
ExprNode* create_identifier_node(const char* name);
ExprNode* create_binary_op_node(BinaryOpType op, ExprNode* left, ExprNode* right);
void free_expr_node(ExprNode* node);
void print_expr_ast(ExprNode* node, int indent);

#endif //EXPR_AST_H
