/**
 * @file ast.c
 * @brief Abstract Syntax Tree implementation for IFJ25 language
 * @author xmalikm00
 * 
 * This file implements the core AST (Abstract Syntax Tree) functionality for
 * the IFJ25 compiler. It provides functions for creating and destroying AST nodes,
 * which represent the hierarchical structure of the parsed source code.
 * 
 * The AST is a tree-based representation of the source program's syntactic
 * structure, built by the parser and used by semantic analysis and code
 * generation phases.
 */

#include "ast.h"
#include "symtable.h"
#include "expr_ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declaration for my_strdup from symtable.c
// (Used to duplicate strings safely)
char *my_strdup(const char *s);

/**
 * @brief Creates and initializes a new AST node
 * 
 * This function allocates memory for a new AST node and initializes all
 * its fields to safe default values. The node's type is set according to
 * the parameter, and if a name is provided, it is duplicated and stored.
 * 
 * Memory initialization details:
 * - type: Set to provided value
 * - name: Duplicated from parameter (or NULL)
 * - left: NULL
 * - right: NULL
 * - expr: NULL
 * - data_type: TYPE_UNDEF
 * - current_table: NULL
 * - current_scope: NULL
 * - var_next: NULL
 * 
 * @param type The type of AST node to create (from ASTNodeType enum)
 * @param name Optional name/identifier for the node (will be duplicated using my_strdup)
 *             May be NULL for nodes that don't require names
 * 
 * @return Pointer to the newly created and initialized AST node
 * @retval NULL if memory allocation fails
 * 
 * @note The returned node must be freed using free_ast_tree() when no longer needed
 * @note The name parameter is copied, so the caller retains ownership of the original
 * 
 * @warning Always check for NULL return value before using the node
 * 
 * Example usage:
 * @code
 * ASTNode *var_node = create_ast_node(AST_VAR_DECL, "myVariable");
 * if (!var_node) {
 *     // Handle allocation error
 * }
 * @endcode
 */
ASTNode* create_ast_node(ASTNodeType type, const char* name) {
    // Allocate memory for the new node
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;  // Allocation failed
    
    // Initialize node type
    node->type = type;
    
    // Duplicate name if provided, otherwise set to NULL
    node->name = name ? my_strdup(name) : NULL;
    
    // Initialize tree structure pointers to NULL
    node->left = NULL;
    node->right = NULL;
    node->expr = NULL;
    
    // Initialize type information
    node->data_type = TYPE_UNDEF;  // Undefined until semantic analysis
    
    // Initialize scope and symbol table pointers
    node->current_table = NULL;
    node->current_scope = NULL;
    
    // Initialize variable declaration list pointer
    node->var_next = NULL;
    
    return node;
}

/**
 * @brief Recursively frees an entire AST tree and all associated resources
 * 
 * This function performs a post-order traversal of the AST tree, freeing all
 * child nodes before freeing the parent. It also deallocates all associated
 * resources including:
 * - Left and right subtrees (recursively)
 * - Expression trees (expr field)
 * - Node names (dynamically allocated strings)
 * - Symbol tables (current_table)
 * - The node structure itself
 * 
 * The function uses post-order traversal to ensure that parent nodes are only
 * freed after all their children have been freed, preventing access to freed
 * memory.
 * 
 * @param node Pointer to the root node of the tree to free
 *             May be NULL (function safely handles NULL pointers)
 * 
 * @note This function does NOT free:
 *       - The current_scope field (scopes are managed by semantic analysis)
 *       - The var_next chain (caller's responsibility if needed)
 * 
 * @warning After calling this function:
 *          - Do not access the node or any of its descendants
 *          - Any pointers to nodes in the freed tree become invalid
 *          - Using freed nodes results in undefined behavior
 * 
 * @note Safe to call with NULL pointer (no-op)
 * 
 * Example usage:
 * @code
 * ASTNode *tree = parse_program();
 * // Use the tree...
 * free_ast_tree(tree);  // Clean up when done
 * tree = NULL;          // Good practice to prevent dangling pointer
 * @endcode
 */
void free_ast_tree(ASTNode* node) {
    // Base case: NULL pointer - nothing to free
    if (!node) return;
    
    // Recursively free left subtree (post-order traversal)
    free_ast_tree(node->left);
    
    // Recursively free right subtree (post-order traversal)
    free_ast_tree(node->right);
    
    // Free expression tree if present
    if(node->expr) free_expr_node(node->expr);
    
    // Free node name (dynamically allocated string)
    if (node->name) free(node->name);
    
    // Free symbol table if present
    if (node->current_table) symtable_free(node->current_table);
    
    // Finally, free the node itself
    free(node);
}


