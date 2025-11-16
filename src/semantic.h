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

extern int semantic_visit_count;

//---------- Scope structure for symbol tables ----------
// Scope structure for symbol tables
typedef struct Scope {
    SymTable symbols;        // symboly v tomto scope
    struct Scope *parent;    // nadradený scope
} Scope;

//---------- Function declarations ----------

// Function to initialize a new scope
Scope* init_scope();

// Function to free a scope and its symbol table
void free_scope(Scope* scope);

//---------- Utility functions ----------

// strdup replacement (not in C standard)
char* my_strdup(const char* s);

// print all symbols in the symbol table (for debugging)
void print_all_symbols(ASTNode *node);

// Convert AST node type to string for debugging
const char* ast_node_type_to_string(ASTNodeType type);

// Look up symbol in current and parent scopes
SymTableData* lookup_symbol(Scope *scope, const char *name);

// Preload built-in functions into global scope
void preload_builtins(Scope *global_scope);

// Check of uninitialized variables
int check_uninitialized_usage(ExprNode* expr, Scope* scope);

// Infer data type of an expression
int infer_expression_type(ASTNode *expr, Scope *scope, DataType *out_type);

// Count arguments in function call
int count_arguments(ASTNode *arg_list);

// Check built-in function call
int check_builtin_function_call(ASTNode *node, Scope *scope, const char *func_name);

// Check user function call
int check_user_function_call(ASTNode *node, Scope *scope, SymTableData *func_symbol);

//---------- Semantic analysis functions ----------

/**
 * @brief Performs semantic analysis on the given symbol tree.
 *
 * @param root Root node of the symbol table or parsed program structure.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_analyze(ASTNode *root);

/**
 * @brief Visits a node in the symbol tree for semantic analysis.
 *
 * @param node The current node to visit.
 * @param current_scope The current symbol table scope.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_visit(ASTNode *node, Scope *current_scope);

#endif // SEMANTIC_H