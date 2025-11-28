/**
 * @file parser.c
 * @author xmikusm00
 * @brief Stack implementation for expression evaluation
 */
#ifndef EXPR_STACK_PRECEDENCE_H
#define EXPR_STACK_PRECEDENCE_H
#include "ast.h"
#include "expr_ast.h"
#include "scanner.h"
#include <stdbool.h>

ASTNode *main_precedence_parser(Token *token, int *rc);
#endif // EXPR_STACK_PRECEDENCE_H
