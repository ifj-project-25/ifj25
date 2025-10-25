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

int expression_parser_main(SymTable *AVL);
int expression_parser(const Token *token, Tstack *stack, int *number_of_lparen, int *number_of_rparen, int *rc, SymTable *AVL);



#endif //EXPR_PARSER_H
