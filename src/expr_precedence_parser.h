/**
 * @file parser.c
 * @author xmikusm00
 * @brief Stack implementation for expression evaluation
 */
#ifndef EXPR_STACK_PRECEDENCE_H
#define EXPR_STACK_PRECEDENCE_H
#include "scanner.h"
#include <stdbool.h>
#include "expr_ast.h"


ASTNode* main_precedence_parser();
#endif //EXPR_STACK_PRECEDENCE_H
