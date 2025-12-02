/**
 * @file expr_precedence_stack.h
 * @author xmikusm00
 * @brief Expression precedence parser stack header file
 * @details
 * Defines the stack data structure and operations for the bottom-up
 * precedence parser used in expression evaluation.
 */
#ifndef EXPR_PARSER_PRECEDENCE_H
#define EXPR_PARSER_PRECEDENCE_H
#include "expr_ast.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Symbol types for precedence parser stack.
 * @details Represents terminal symbols from the precedence table.
 */
typedef enum {
    PS_TERM,

    PS_PLUS,
    PS_MINUS,
    PS_MUL,
    PS_DIV,

    PS_LPAREN,
    PS_RPAREN,

    PS_LT,
    PS_GT,
    PS_LTE,
    PS_GTE,

    PS_IS,

    PS_EQ,
    PS_NEQ,

    PS_DOLLAR,

} Sym;

/**
 * @brief Type of symbol on the stack.
 * @details Distinguishes between terminals (tokens) and non-terminals (reduced
 * expressions).
 */
typedef enum {
    SYM_TERM,
    SYM_NONTERM,
} SymType;

/**
 * @brief Node in the expression precedence stack.
 * @details Contains either a terminal (token) or non-terminal (expression AST
 * node).
 */
typedef struct ExprPstackNode {
    SymType type;   ///< Type of symbol (terminal or non-terminal)
    Sym sym;        ///< Symbol type from precedence table
    Token token;    ///< Token data (valid if type == SYM_TERM)
    ExprNode *node; ///< Expression AST node (valid if type == SYM_NONTERM)
    struct ExprPstackNode *next; ///< Pointer to next node in stack
} ExprPstackNode;

/**
 * @brief Expression precedence parser stack structure.
 * @details Linked list-based stack for bottom-up expression parsing.
 */
typedef struct ExprPstack {
    ExprPstackNode *top; ///< Pointer to the top node of the stack
} ExprPstack;

// ==================== Function ====================

/**
 * @brief Initializes the expression precedence stack with a bottom marker.
 * @param stack Pointer to the stack structure to initialize.
 * @param rc Pointer to error code variable (set to ERROR_INTERNAL on allocation
 * failure).
 */
void expr_Pstack_init(ExprPstack *stack, int *rc);

/**
 * @brief Frees all nodes in the stack and resets it to empty state.
 * @param stack Pointer to the stack to free.
 */
void expr_Pstack_free(ExprPstack *stack);

/**
 * @brief Pushes a terminal symbol (token) onto the stack.
 * @param stack Pointer to the stack.
 * @param token Pointer to the token to push (copied into new node).
 * @param sym Symbol type from precedence table.
 * @param rc Pointer to error code (set to ERROR_INTERNAL on allocation
 * failure).
 */
void expr_Pstack_push_term(ExprPstack *stack, Token *token, Sym sym, int *rc);

/**
 * @brief Pushes a non-terminal symbol (expression AST node) onto the stack.
 * @param stack Pointer to the stack.
 * @param node Pointer to the expression AST node to push.
 * @param rc Pointer to error code (set to ERROR_INTERNAL on allocation
 * failure).
 */
void expr_Pstack_push_nonterm(ExprPstack *stack, ExprNode *node, int *rc);

/**
 * @brief Removes the top element from the stack.
 * @param stack Pointer to the stack.
 */
void expr_Pstack_pop(ExprPstack *stack);

/**
 * @brief Returns the expression node from the top of the stack.
 * @param stack Pointer to the stack.
 * @return Pointer to the ExprNode at the top.
 * @warning Does not check if top is actually a non-terminal node.
 */
ExprNode *expr_Pstack_top(ExprPstack *stack);

/**
 * @brief Checks if the stack is empty.
 * @param stack Pointer to the stack.
 * @return true if stack is empty (top is NULL), false otherwise.
 */
bool expr_Pstack_is_empty(ExprPstack *stack);

#endif // EXPR_PARSER_PRECEDENCE_H
