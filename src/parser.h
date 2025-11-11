/**
 * @file parser.h
 * @author xmikusm00
 * @brief Headder file for parser
 */
#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"
#include "symtable.h"
#include "ast.h"

int parser(ASTNode** PROGRAM);
void parser_next_token(void);
const Token *parser_current_token(void);

#endif
