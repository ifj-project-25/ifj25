/**
 * @file ast.h
 * @brief Abstract Syntax Tree node types for IFJ25 language
 * @author xmalikm00 xklusaa00
 */

#ifndef AST_H
#define AST_H

#include "symtable.h"
#include "expr_ast.h"

typedef struct Scope Scope;

/**  ===== All possible node types in the AST =====
 *
 * This enum includes every kind of syntactic construct that
 * can appear in the parsed program — derived from all tokens
 * and keywords recognized by the scanner.
 */
typedef enum {
    AST_PROGRAM,         // Root node   -> left - global variables, right - main function/ other functions

// functions

    AST_FUNC_CALL,       // function call  ->   left - AST_FUNC_ARG (arguments), right - next program  

    AST_FUNC_ARG,        // function argument    ->   left - another AST_FUNC_ARG or NULL (end of args), right - expression/identifier

    AST_GETTER_CALL,       // getter call  ->   left - null, right - next program  

    AST_SETTER_CALL,       // setter call  ->   left - expression/value, right - next program

    // CANCELLED AST_FUNC_PARAM, // function argument parameter in declaration   ->   left - another AST_FUNC_ARG or NULL (end of args), right - expression/value

    AST_BLOCK,           // { statements }  -> left - inside code, right - next program - in if continues with else, in functiondef another function...

// variables
    AST_VAR_DECL,        // var declaration ->left - AST_IDENTIFIER, right - next program

    AST_IDENTIFIER,      // identifier - variable (or function name?????) -> left - NULL, right - NULL

// definitions
    AST_MAIN_DEF,        // main function definition    ->   left - AST_FUNC_ARG, right - AST_BLOCK

    AST_FUNC_DEF,        // function definition ->   left - AST_FUNC_ARG, right - AST_BLOCK

    AST_GETTER_DEF,      // static getter ->   left - NULL, right - AST_BLOCK

    AST_SETTER_DEF,      // static setter ->   left - NULL, right - AST_BLOCK

// statements
    AST_IF,              // if statement   ->   left - condition AST_EXPRESSION, right - AST_BLOCK

    AST_ELSE,            // else statement  ->  left - NULL, right - AST_BLOCK

    AST_WHILE,           // while loop     ->   left - condition AST_EXPRESSION, right - AST_BLOCK

    AST_RETURN,          // return statement    ->  left - AST_EXPRESSION, right - NULL


// assignments
    AST_ASSIGN,          // assignment  ->  left - AST_EQUALS, right - next program

    AST_EQUALS,          // this should be an assigment THIS IS "=" CHARACTER    ->  left - AST_IDENTIFIER, right - AST_EXPRESSION
                         //actual equals should be handled in expression node
                         //for exammple var a = 15;

    AST_EXPRESSION,      // expression node -> left - AST_FUNC_CALL, expr - ExprNode
// Expressions -TODO Matej had to check and fix/add/remove that
   /*
    AST_OP,             // operator
    
    AST_LITERAL_INT,     // integer literal
    AST_LITERAL_FLOAT,   // float literal
    AST_LITERAL_STRING,  // string literal
    AST_LITERAL_NULL,    // null literal*/
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;      // type of the AST node
    char *name;            // name of the identifier or operator, if it exists

    struct ASTNode *left;  // left / subnode
    struct ASTNode *right; // right / subnode
    struct ExprNode *expr; // expression node (for expressions)

    // Literal values
    /*int int_val;
    double float_val;
    char *string_val;*/

    SymTable *current_table; // current symbol table for the block
    Scope *current_scope;      // current scope for semantic analysis

    // Type of the variable / expression (e.g., TYPE_NUM, TYPE_STRING, ...) - used from symtable.h
    DataType data_type;

} ASTNode;

ASTNode* create_ast_node(ASTNodeType type, const char* name);   // Create a new AST node

void free_ast_tree(ASTNode* node);                          // Free the entire AST tree recursively

void print_ast_tree(ASTNode* node);                         // Print the AST tree with visual connections

#endif //AST_H
