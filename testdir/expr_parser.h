/**
 * @file expr_parser.h
 * @author xmikusm00
 * @brief Precedence-based expression parser public interface
 * @details
 * Declares the entry point for the precedence (operator-precedence) expression
 * parser. The parser consumes tokens starting at the current token and
 * constructs an `AST_EXPRESSION` node containing a linked internal expression
 * tree (`ExprNode`). It supports binary operators (+,-,*,/,<,>,<=,>=,==,!=,is)
 * grouping with parentheses, literals (numbers, strings, null), identifiers,
 * global variables, and type literals used with the `is` operator.
 */
#ifndef EXPR_STACK_PRECEDENCE_H
#define EXPR_STACK_PRECEDENCE_H
#include "ast.h"
#include "expr_ast.h"
#include "scanner.h"
#include <stdbool.h>

/**
 * @brief Parses an expression using operator precedence parsing.
 * @param token Pointer to the current token (the function advances it past the
 * expression).
 * @param rc Pointer to error code; set to SYNTAX_ERROR or ERROR_INTERNAL on
 * failure.
 * @return Pointer to an `AST_EXPRESSION` node wrapping the parsed expression
 * subtree, or a function call node (`AST_FUNC_CALL`) when the first token is an
 * identifier followed immediately by `(`. Returns NULL on error (and sets *rc
 * accordingly).
 * @details
 * Grammar (simplified):
 *    EXPR -> TERM ( op TERM )*
 *    TERM -> identifier | literal | ( EXPR )
 * Supported binary operators: + - * / < > <= >= == != is
 * The `is` operator compares against type literals: `Null`, `Num`, `String`.
 * Function-call short form: if the first token is identifier followed by '(',
 * the parser returns an `AST_FUNC_CALL` node (argument list parsed by
 * higher-level parser).
 */
ASTNode *main_precedence_parser(Token *token, int *rc);
#endif // EXPR_PARSER_H
