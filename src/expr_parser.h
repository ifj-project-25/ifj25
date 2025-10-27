/**
 * @file parser.c
 * @author xmikusm00
 * @brief expression parser header file
 */
#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H
#include "scanner.h"
#include "symtable.h"
#include "expr_stack.h"
#include "expr_ast.h"


ExprNode*  expression_parser_main(int *error_code);



#endif //EXPR_PARSER_H
