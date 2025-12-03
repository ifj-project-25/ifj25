/**
 * @file semantic.h
 * @brief Header for semantic analysis phase of IFJ25 compiler
 * @author xmalikm00
 * 
 * This file defines the interface for semantic analysis, which validates
 * the AST (Abstract Syntax Tree) for semantic correctness and preparing for code generation, including:
 * - Type checking and type inference
 * - Symbol table management and scope resolution
 * - Function overloading validation
 * - Variable initialization checking
 * - Built-in function validation
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symtable.h"
#include "ast.h"
#include "error.h"

// ========== Scope Structure ==========

/**
 * @struct Scope
 * @brief Represents a lexical scope for symbol resolution
 * 
 * Scopes form a hierarchical structure (tree) where each scope has a parent
 * (except the global scope). This allows for proper symbol resolution that
 * searches from inner to outer scopes.
 */
typedef struct Scope {
    /** @brief Symbol table for this scope (AVL tree) */
    SymTable symbols;
    
    /** @brief Parent scope (outer scope), NULL for global scope */
    struct Scope *parent;
} Scope;

// ========== Scope Management Functions ==========

/**
 * @brief Initializes a new scope with an empty symbol table
 * 
 * Creates and initializes a new Scope structure with an empty symbol table.
 * The parent pointer is set to NULL and should be assigned by the caller
 * to establish the scope hierarchy.
 * 
 * @return Pointer to newly created scope
 */
Scope* init_scope();

// ========== Symbol Table Utility Functions ==========

/**
 * @brief Looks up a symbol in current and parent scopes
 * 
 * Searches for a symbol by name, starting from the given scope and
 * traversing up through parent scopes until found or global scope is reached.
 * This implements the standard scoping rules where inner scopes shadow
 * outer scopes.
 * 
 * @param scope Starting scope for the search (typically current scope)
 * @param name Name of the symbol to find
 * 
 * @return Pointer to symbol data if found
 * @retval NULL if symbol not found in any scope
 * 
 */
SymTableData* lookup_symbol(Scope *scope, const char *name);

/**
 * @brief Preloads all built-in functions into the global scope
 * 
 * Inserts symbol table entries for all IFJ25 built-in functions:
 * - Ifj.read_str() -> String
 * - Ifj.read_num() -> Num
 * - Ifj.write(term) -> Null
 * - Ifj.floor(term: Num) -> Num
 * - Ifj.str(term) -> String
 * - Ifj.length(s: String) -> Num
 * - Ifj.substring(s: String, i: Num, j: Num) -> String
 * - Ifj.strcmp(s1: String, s2: String) -> Num
 * - Ifj.ord(s: String, i: Num) -> Num
 * - Ifj.chr(i: Num) -> String
 * 
 * @param global_scope The global scope to populate with built-in functions
 * 
 * @note Must be called before semantic analysis begins
 * @note Each function is stored with overload key format "name$argc"
 */
void preload_builtins(Scope *global_scope);

// ========== Type Inference Functions ==========

/**
 * @brief Infers the data type of an expression node
 * 
 * Recursively analyzes an expression tree to determine its result type.
 * Handles literals, identifiers, binary operations, and getter calls.
 * Performs type compatibility checking for operators.
 * 
 * @param expr Expression node to analyze (from expr_ast.h)
 * @param scope Current scope for identifier resolution
 * @param out_type Output parameter for the inferred type
 * 
 * @return Error code
 * @retval NO_ERROR if type inference succeeded
 * @retval SEM_ERROR_UNDEFINED if identifier not found
 * @retval SEM_ERROR_TYPE_COMPATIBILITY if type mismatch in operation
 * @retval ERROR_INTERNAL if invalid parameters
 * 
 * @note Sets *out_type to TYPE_UNDEF if type cannot be determined
 * @note For binary operations, checks type compatibility of operands
 */
int infer_expr_node_type(ExprNode *expr, Scope *scope, DataType *out_type);

// ========== Function Call Validation ==========

/**
 * @brief Counts the number of arguments in a function call
 * 
 * Traverses the AST_FUNC_ARG linked list to count arguments.
 * 
 * @param arg_list Head of the argument list (AST_FUNC_ARG chain)
 * 
 * @return Number of arguments in the list
 * @retval 0 if arg_list is NULL (no arguments)
 */
int count_arguments(ASTNode *arg_list);


/**
 * @brief Validates a user-defined function call
 * 
 * Checks that a call to a user-defined function matches the function's
 * signature (parameter count and types). Sets the return type on the
 * function call node.
 * 
 * @param node Function call AST node
 * @param scope Current scope for type inference
 * @param func_symbol Symbol table entry for the function
 * 
 * @return Error code
 * @retval NO_ERROR if call is valid
 * @retval SEM_ERROR_WRONG_PARAMS if parameter count or types don't match
 * @retval SEM_ERROR_OTHER if symbol is not a function
 */
int check_user_function_call(ASTNode *node, Scope *scope, SymTableData *func_symbol);

// ========== Helper Functions ==========

/**
 * @brief Adds a variable declaration node to the function's variable list
 * 
 * Links a variable declaration node into the function's var_next chain.
 * Used by the code generator to emit variable definitions at function start.
 * 
 * @param node Variable declaration node to add (AST_VAR_DECL)
 * @param func_node Function definition node (AST_FUNC_DEF or AST_MAIN_DEF)
 * 
 * @note Modifies func_node->var_next to include the new variable
 */
void add_node_to_func_node(ASTNode *node, ASTNode *func_node);

// ========== Main Semantic Analysis Functions ==========

/**
 * @brief First pass: Collects all function, getter, and setter definitions
 * 
 * Traverses the AST to register all function, getter, and setter definitions
 * in the symbol table before the main semantic analysis pass. This allows
 * forward references and function overloading to work correctly.
 * 
 * @param node Program AST node (AST_PROGRAM)
 * @param current_scope Global scope for definitions
 * 
 * @return NO_ERROR on success, error code on failure
 */
int semantic_definition(ASTNode *node, Scope *current_scope);

/**
 * @brief Main semantic analysis function - entry point
 * 
 * Performs complete semantic analysis on the parsed AST:
 * 1. Initializes global scope
 * 2. Preloads built-in functions
 * 3. First pass: Collects function/getter/setter definitions
 * 4. Second pass: Validates all statements and expressions
 * 5. Verifies main() with 0 parameters exists
 * 
 * Validation includes:
 * - Type checking and inference
 * - Variable initialization tracking
 * - Function call validation
 * - Scope resolution
 * - Control flow validation
 * 
 * @param root Root node of the AST (AST_PROGRAM)
 * 
 * @return Error code
 * 
 * @note Sets root->current_scope to global scope for code generator
 * @note The global scope contains all symbols and must be freed after use
 */
int semantic_analyze(ASTNode *root);

/**
 * @brief Recursive AST visitor for semantic analysis
 * 
 * Traverses and validates an AST node and its children. Different node
 * types are handled differently
 * 
 * 
 * @param node Current AST node to visit
 * @param current_scope Current scope for symbol resolution
 * 
 * @return Error code
 * @retval NO_ERROR if node and children are valid else Error code
 * 
 * @note Recursively visits children nodes as appropriate
 */
int semantic_visit(ASTNode *node, Scope *current_scope);

#endif // SEMANTIC_H