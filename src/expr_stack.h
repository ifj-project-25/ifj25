/**
 * @file expr_stack.h
 * @author xmikusm00
 * @brief Expression precedence stack header file
 * @details Defines data structures and functions for the stack used in
 *          precedence-based expression parsing.
 */
#ifndef EXPR_PARSER_PRECEDENCE_H
#define EXPR_PARSER_PRECEDENCE_H
#include "expr_ast.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @enum Sym
 * @brief Precedence symbols for expression parsing
 * @details Defines all terminal symbols used in the precedence table
 *          for expression parsing, including operators, parentheses,
 *          and special markers.
 */
typedef enum {
    PS_TERM, /**< Non-terminal symbol (reduced expression) */

    PS_PLUS,
    /**< Addition operator (+) */ /**< Addition operator (+) */
    PS_MINUS,                     /**< Subtraction operator (-) */
    PS_MUL,                       /**< Multiplication operator (*) */
    PS_DIV,                       /**< Division operator (/) */

    PS_LPAREN, /**< Left parenthesis '(' */
    PS_RPAREN,
    /**< Right parenthesis ')' */ /**< Right parenthesis ')' */

    PS_LT,  /**< Less than operator (<) */
    PS_GT,  /**< Greater than operator (>) */
    PS_LTE, /**< Less than or equal operator (<=) */
    PS_GTE, /**< Greater than or equal operator (>=) */

    PS_IS, /**< Type equality operator (is) */

    PS_EQ,  /**< Equality operator (==) */
    PS_NEQ, /**< Inequality operator (!=) */

    PS_DOLLAR, /**< Bottom of stack marker ($) */

} Sym;

/**
 * @enum SymType
 * @brief Type of symbol on the precedence stack
 */
typedef enum {
    SYM_TERM,    /**< Terminal symbol (token from input) */
    SYM_NONTERM, /**< Non-terminal symbol (reduced AST node) */
} SymType;

/**
 * @struct ExprPstackNode
 * @brief Node in the expression precedence stack
 * @details Each node can hold either a terminal (token) or non-terminal (AST
 * node)
 */
typedef struct ExprPstackNode {
    SymType type;   /**< Type of symbol (terminal or non-terminal) */
    Sym sym;        /**< Precedence symbol type */
    Token token;    /**< Token data (for terminals) */
    ExprNode *node; /**< AST node pointer (for non-terminals) */
    struct ExprPstackNode *next; /**< Pointer to next node in stack */
} ExprPstackNode;

/**
 * @struct ExprPstack
 * @brief Expression precedence stack
 * @details Stack structure used during bottom-up precedence parsing of
 * expressions
 */
typedef struct ExprPstack {
    ExprPstackNode *top; /**< Pointer to the top of the stack */
} ExprPstack;

/**
 * @brief Initializes the expression precedence stack
 * @param stack Pointer to the stack to initialize
 */
void expr_Pstack_init(ExprPstack *stack);

/**
 * @brief Frees all memory allocated for the stack
 * @param stack Pointer to the stack to free
 */
void expr_Pstack_free(ExprPstack *stack);

/**
 * @brief Pushes a terminal symbol onto the stack
 * @param stack Pointer to the stack
 * @param token Pointer to the token to push
 * @param sym The precedence symbol type of the terminal
 * @return NO_ERROR on success, ERROR_INTERNAL on memory allocation failure
 */
int expr_Pstack_push_term(ExprPstack *stack, Token *token, Sym sym);

/**
 * @brief Pushes a non-terminal symbol (AST node) onto the stack
 * @param stack Pointer to the stack
 * @param node Pointer to the expression AST node to push
 * @return NO_ERROR on success, ERROR_INTERNAL on memory allocation failure
 */
int expr_Pstack_push_nonterm(ExprPstack *stack, ExprNode *node);

/**
 * @brief Removes the top element from the stack
 * @param stack Pointer to the stack
 */
void expr_Pstack_pop(ExprPstack *stack);

/**
 * @brief Returns the expression node from the top of the stack
 * @param stack Pointer to the stack
 * @return Pointer to the expression node at the top of the stack
 */
ExprNode *expr_Pstack_top(ExprPstack *stack);

/**
 * @brief Checks if the stack is empty
 * @param stack Pointer to the stack
 * @return true if the stack is empty, false otherwise
 */
bool expr_Pstack_is_empty(ExprPstack *stack);

/**
 * @brief Main entry point for expression parsing
 * @param token Pointer to the first token of the expression
 * @param error_code Pointer to store error code if parsing fails
 * @return Pointer to the root AST node of the parsed expression, or NULL on
 * error
 */
struct ASTNode *expression_parser_main(Token *token, int *error_code);

#endif // EXPR_PARSER_PRECEDENCE_H
