/**
 * @file parser.h
 * @author xmikusm00
 * @brief Header file for syntactic parser of IFJ25 language
 * @details
 * Provides the main parser entry point and utility functions for parsing
 * the IFJ25 source code and constructing the abstract syntax tree.
 */
#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "scanner.h"
#include "symtable.h"

/**
 * @brief Main parser entry point that performs syntactic analysis.
 * @param PROGRAM Pointer to the root AST node representing the program.
 * @return Error code (NO_ERROR on success, SYNTAX_ERROR or ERROR_INTERNAL on
 * failure).
 * @details
 * Parses the IFJ25 source code using recursive descent parsing, validates
 * syntax according to the language grammar, and constructs the abstract syntax
 * tree.
 */
int parser(ASTNode *PROGRAM);

#endif
