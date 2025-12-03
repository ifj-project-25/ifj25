/**
 * @file ast.h
 * @brief Abstract Syntax Tree node types and structures for IFJ25 language
 * @author xmalikm00 xklusaa00
 *
 * This file defines the Abstract Syntax Tree (AST) data structures used
 * throughout the IFJ25 compiler. The AST represents the hierarchical structure
 * of the source program after parsing, and is used by both semantic analysis
 * and code generation phases.
 */

#ifndef AST_H
#define AST_H

#include "expr_ast.h"
#include "symtable.h"

// Forward declaration of Scope structure (defined in semantic.h)
typedef struct Scope Scope;

/**
 * @enum ASTNodeType
 * @brief All possible node types in the Abstract Syntax Tree
 *
 * This enumeration defines every kind of syntactic construct that can appear
 * in the parsed IFJ25 program. Each node type has a specific structure defined
 * by how its left, right, expr, and name fields are used.
 *
 * @note The tree structure uses left/right pointers for linking nodes.
 *       The 'expr' field holds expression trees (from expr_ast.h).
 */
typedef enum {
    // ========== Program Structure ==========

    /**
     * @brief Root node of the entire program
     * - left:  First function definition (linked list via right)
     * - right: NULL
     * - name:  NULL
     */
    AST_PROGRAM,

    // ========== Function-Related Nodes ==========

    /**
     * @brief Function call expression
     * - left:  First argument (AST_FUNC_ARG chain)
     * - right: Next code statements
     * - name:  Function name with overload suffix (e.g., "foo$2" for 2 params)
     */
    AST_FUNC_CALL,

    /**
     * @brief Function argument (used in both calls and definitions)
     * - left:  Next argument in chain (or NULL if last)
     * - right: Argument value (AST_EXPRESSION for calls, AST_IDENTIFIER for
     * definitions)
     * - name:  NULL
     */
    AST_FUNC_ARG,

    /**
     * @brief Getter call (moved to Expression nodes)
     * - left:  NULL
     * - right: NULL
     * - name:  Getter name
     */
    AST_GETTER_CALL,

    /**
     * @brief Setter call statement
     * - left:  Expression to assign (AST_EXPRESSION)
     * - right: Next statement in sequence
     * - name:  Setter name
     */
    AST_SETTER_CALL,

    /**
     * @brief Block of statements ({ ... })
     * - left:  First statement in block
     * - right: Next statement after block (or else branch for if statements)
     * - current_table: Symbol table for this block's scope
     */
    AST_BLOCK,

    // ========== Variables ==========

    /**
     * @brief Variable declaration (var x)
     * - left:  AST_IDENTIFIER node with variable name
     * - right: Next statement in sequence
     * - var_next: Next variable in declaration list
     */
    AST_VAR_DECL,

    /**
     * @brief Variable or parameter identifier
     * - left:  NULL
     * - right: NULL
     * - name:  Identifier name
     * - current_scope: Scope where this identifier is defined
     */
    AST_IDENTIFIER,

    // ========== Function/Getter/Setter Definitions ==========

    /**
     * @brief Main function definition
     * - left:  Parameter list (AST_FUNC_ARG chain)
     * - right: Function body (AST_BLOCK)
     * - name:  "main"
     * - var_next: Linked list of variable declarations in this function
     */
    AST_MAIN_DEF,

    /**
     * @brief User-defined function definition
     * - left:  Parameter list (AST_FUNC_ARG chain)
     * - right: Function body (AST_BLOCK)
     * - name:  Function name with overload suffix (e.g., "foo$2")
     * - var_next: Linked list of variable declarations in this function
     */
    AST_FUNC_DEF,

    /**
     * @brief Static getter definition
     * - left:  NULL (getters have no parameters)
     * - right: Getter body (AST_BLOCK)
     * - name:  Getter name
     */
    AST_GETTER_DEF,

    /**
     * @brief Static setter definition
     * - left:  Parameter (single AST_IDENTIFIER)
     * - right: Setter body (AST_BLOCK)
     * - name:  Setter name
     */
    AST_SETTER_DEF,

    // ========== Control Flow Statements ==========

    /**
     * @brief If statement
     * - left:  Condition (AST_EXPRESSION)
     * - right: Then branch (AST_BLOCK), may be followed by AST_ELSE
     * - name:  NULL
     */
    AST_IF,

    /**
     * @brief Else statement
     * - left:  NULL
     * - right: Else branch (AST_BLOCK)
     * - name:  NULL
     */
    AST_ELSE,

    /**
     * @brief While loop statement
     * - left:  Condition (AST_EXPRESSION)
     * - right: Loop body (AST_BLOCK)
     * - name:  NULL
     */
    AST_WHILE,

    /**
     * @brief Return statement
     * - left:  May contain AST_FUNC_CALL if returning function result
     * - right: NULL
     * - expr:  Return value expression (or NULL for void return)
     * - data_type: Type of returned value
     */
    AST_RETURN,

    // ========== Assignment Operations ==========

    /**
     * @brief Assignment statement
     * - left:  AST_EQUALS node
     * - right: Next statement in sequence
     * - name:  NULL
     */
    AST_ASSIGN,

    /**
     * @brief Assignment operator (=)
     * - left:  Target variable (AST_IDENTIFIER)
     * - right: Value to assign (AST_EXPRESSION)
     * - name:  NULL
     */
    AST_EQUALS,

    /**
     * @brief Expression wrapper node
     * - left:  May contain AST_FUNC_CALL if expression is just a call
     * - right: Next statement in sequence
     * - expr:  Expression tree (ExprNode from expr_ast.h)
     * - data_type: Inferred type of the expression
     */
    AST_EXPRESSION,

} ASTNodeType;

/**
 * @struct ASTNode
 * @brief Abstract Syntax Tree node structure
 *
 * This structure represents a single node in the Abstract Syntax Tree.
 * The interpretation of each field depends on the node's type (see
 * ASTNodeType).
 *
 * @note Memory management: All dynamically allocated fields (name, left, right,
 *       expr, current_table) must be freed when the node is destroyed using
 *       free_ast_tree().
 */
typedef struct ASTNode {
    /** @brief Type of this AST node (determines interpretation of other fields)
     */
    ASTNodeType type;

    /** @brief Name/identifier (for variables, functions, getters, setters)
     *  Dynamically allocated string, may be NULL for nodes without names */
    char *name;

    /** @brief Left child node in the tree structure
     *  Usage varies by node type (see ASTNodeType documentation) */
    struct ASTNode *left;

    /** @brief Right child node in the tree structure
     *  Often used for linking sequential statements */
    struct ASTNode *right;

    /** @brief Expression subtree (for AST_EXPRESSION nodes)
     *  Points to ExprNode structure defined in expr_ast.h */
    struct ExprNode *expr;

    /** @brief Linked list of variable declarations
     *  Used in function definitions to track all variables declared in the
     * function */
    struct ASTNode *var_next;

    /** @brief Symbol table for this node's scope (for blocks and functions)
     *  Points to the SymTable structure, may be NULL */
    SymTable *current_table;

    /** @brief Scope information for semantic analysis
     *  Points to the Scope structure, used to resolve variable references */
    Scope *current_scope;

    /** @brief Data type of this node (for variables, expressions, returns)
     *  Used by semantic analysis and code generation */
    DataType data_type;

} ASTNode;

// ========== Public API Functions ==========

/**
 * @brief Creates and initializes a new AST node
 *
 * Allocates memory for a new AST node and initializes all fields to safe
 * default values. The name is copied if provided.
 *
 * @param type The type of AST node to create
 * @param name Optional name/identifier for the node (will be duplicated), may
 * be NULL
 * @return Pointer to newly created AST node, or NULL if allocation fails
 *
 * @note The returned node must be freed using free_ast_tree() when no longer
 * needed
 * @note All pointers (left, right, expr, etc.) are initialized to NULL
 * @note data_type is initialized to TYPE_UNDEF
 */
ASTNode *create_ast_node(ASTNodeType type, const char *name);

/**
 * @brief Recursively frees an entire AST tree
 *
 * Performs a post-order traversal of the tree, freeing all child nodes
 * before freeing the parent. Also frees associated resources like the
 * node name, expression tree, and symbol table.
 *
 * @param node Root node of the tree to free (may be NULL)
 *
 * @note This function safely handles NULL pointers
 * @note Frees:
 *       - Left and right subtrees (recursively)
 *       - Expression tree (if present)
 *       - Node name (if present)
 *       - Symbol table (if present)
 *       - The node itself
 * @warning Do not access the node or any of its children after calling this
 * function
 */
void free_ast_tree(ASTNode *node);

#endif // AST_H
