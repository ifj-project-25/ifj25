/**
 * @file ast.h
 * @brief Abstract Syntax Tree node types for IFJ25 language
 * @author xmalikm00
 */

#ifndef AST_H
#define AST_H

#include "symtable.h"


// ===== All possible node types in the AST =====
//
// This enum includes every kind of syntactic construct that
// can appear in the parsed program — derived from all tokens
// and keywords recognized by the scanner.
//
typedef enum {

    // ----- Program structure 
    AST_PROGRAM,         // root node of the entire program
    AST_CLASS_DEF,       // 'class' definition
    AST_FUNC_DEF,        // function definition (possibly static)
    AST_FUNC_CALL,       // function call (identifier + arguments)
    AST_BLOCK,           // block of statements { ... }

    // ----- Declarations 
    AST_VAR_DECL,        // 'var' declaration inside a block or class
    AST_GLOBAL_VAR_DECL, // global variable (__var)
    AST_PARAM,           // function parameter

    // ----- Statements 
    AST_ASSIGN,          // assignment '='
    AST_RETURN,          // 'return' statement
    AST_IF,              // 'if' statement
    AST_ELSE,            // 'else' branch
    AST_WHILE,           // 'while' loop
    AST_FOR,             // 'for' loop
    AST_EXPR_STMT,       // expression used as a statement (e.g., function call)

    // ----- Expressions 
    AST_BINOP,           // binary operation (+, -, *, /, <, >, <=, >=, ==, !=)
    AST_UNOP,            // unary operation (!, -)
    AST_LITERAL_INT,     // integer literal
    AST_LITERAL_DOUBLE,  // double literal
    AST_LITERAL_STRING,  // string literal
    AST_LITERAL_NULL,    // 'null' literal
    AST_IDENTIFIER,      // variable or function name
    AST_MEMBER_ACCESS,   // '.' operator (obj.member)
    AST_GROUP_EXPR,      // expression in parentheses ( ... )

    // ----- Types 
    AST_TYPE_NUM,        // 'Num' keyword (type)
    AST_TYPE_STRING,     // 'String' keyword (type)
    AST_TYPE_NULL,       // 'Null' keyword (type)
    AST_TYPE_UNDEF,      // untyped (used before inference)

    // ----- Miscellaneous 
    AST_IMPORT,          // 'import' statement
    AST_STATIC,          // 'static' modifier
    AST_IS,              // 'is' keyword (type comparison)
    AST_UNKNOWN,         // fallback for invalid or unrecognized nodes

} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;      // type of the AST node
    char *name;            // name of the identifier or operator, if it exists

    struct ASTNode *left;  // left operand / subnode
    struct ASTNode *right; // right operand / subnode
    struct ASTNode *next;  // for linking nodes in a list (e.g., function body)
    struct ASTNode *body;  // body of the function, if, while, etc.

    // Literal values
    int int_val;
    double double_val;
    char *string_val;

    // Type of the variable / expression (e.g., TYPE_NUM, TYPE_STRING, ...) - used from symtable.h
    DataType data_type;

} ASTNode;


#endif // AST_H
