/**
 * @file expr_ast.h
 * @author xmikusm00
 * @brief Expression Abstract Syntax Tree (AST) definitions
 */
#ifndef EXPR_AST_H
#define EXPR_AST_H

typedef struct Scope Scope;

/**
 * @brief Types of expression nodes in the AST
 */
typedef enum {
    EXPR_NUM_LITERAL,    ///< Numeric literal (e.g., 42, 3.14)
    EXPR_STRING_LITERAL, ///< String literal (e.g., "hello")
    EXPR_NULL_LITERAL,   ///< Null literal
    EXPR_IDENTIFIER,     ///< Variable identifier
    EXPR_GETTER_CALL,    ///< Getter method call
    EXPR_BINARY_OP,      ///< Binary operation (e.g., +, -, *, /)
    EXPR_TYPE_LITERAL    ///< Type literal
} ExprNodeType;

/**
 * @brief Types of binary operations
 */
typedef enum {
    OP_ADD, ///< Addition (+)
    OP_SUB, ///< Subtraction (-)
    OP_MUL, ///< Multiplication (*)
    OP_DIV, ///< Division (/)
    OP_EQ,  ///< Equality (==)
    OP_NEQ, ///< Inequality (!=)
    OP_LT,  ///< Less than (<)
    OP_GT,  ///< Greater than (>)
    OP_LTE, ///< Less than or equal (<=)
    OP_GTE, ///< Greater than or equal (>=)
    OP_IS   ///< Type checking (IS)
} BinaryOpType;

/**
 * @brief Expression node in the Abstract Syntax Tree
 *
 * Represents a node in the expression AST. Each node has a type
 * and associated data depending on that type.
 */
typedef struct ExprNode {
    ExprNodeType type;    ///< Type of the expression node
    Scope *current_scope; ///< Current scope (for EXPR_IDENTIFIER and
                          ///< EXPR_GETTER_CALL)
    union {
        double num_literal;    ///< Numeric value (for EXPR_NUM_LITERAL)
        char *string_literal;  ///< String value (for EXPR_STRING_LITERAL)
        char *identifier_name; ///< Identifier name (for EXPR_IDENTIFIER)
        char *getter_name;     ///< Getter name (for EXPR_GETTER_CALL)
        struct {
            BinaryOpType op;        ///< Binary operator type
            struct ExprNode *left;  ///< Left operand
            struct ExprNode *right; ///< Right operand
        } binary;                   ///< Binary operation data
    } data;                         ///< Node data union
} ExprNode;

/**
 * @brief Creates a numeric literal expression node
 * @param value The numeric value
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_num_literal_node(double value);

/**
 * @brief Creates a string literal expression node
 * @param value The string value
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_string_literal_node(const char *value);

/**
 * @brief Creates a null literal expression node
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_null_literal_node();

/**
 * @brief Creates a type literal expression node
 * @param name The type name
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_type_node(const char *name);

/**
 * @brief Creates an identifier expression node
 * @param name The identifier name
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_identifier_node(const char *name);

/**
 * @brief Creates a getter call expression node
 * @param name The getter name
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_getter_call_node(const char *name);

/**
 * @brief Creates a binary operation expression node
 * @param op The binary operator type
 * @param left The left operand node
 * @param right The right operand node
 * @return Pointer to the created node, or NULL on allocation failure
 */
ExprNode *create_binary_op_node(BinaryOpType op, ExprNode *left,
                                ExprNode *right);

/**
 * @brief Frees an expression node and all its children
 * @param node The node to free (can be NULL)
 */
void free_expr_node(ExprNode *node);

#endif // EXPR_AST_H
