/**
 * @file semantic.h
 * @brief Header for semantic analysis phase
 * @author xmalikm00
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H


#include "symtable.h"
#include "ast.h"
#include "error.h"

//---------- Scope structure for symbol tables ----------
// Scope structure for symbol tables
typedef struct Scope {
    SymTable symbols;        // symboly v tomto scope
    struct Scope *parent;    // nadradený scope
} Scope;

// Function to initialize a new scope
Scope* init_scope();
// Function to free a scope and its symbol table
void free_scope(Scope* scope);

//---------- Utility functions ----------

// strdup replacement (not in C standard)
char* my_strdup(const char* s);
//print all symbols in the symbol table (for debugging)
void print_all_symbols(ASTNode *node);

//---------- Semantic analysis functions ----------



/**
 * @brief Performs semantic analysis on the given symbol tree.
 *
 * @param root Root node of the symbol table or parsed program structure.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_analyze(ASTNode *root);

/*
 * @brief Visits a node in the symbol tree for semantic analysis.
 *
 * @param node The current node to visit.
 * @param current_scope The current symbol table scope.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_visit(ASTNode *node, Scope *current_scope);
#endif // SEMANTIC_H
