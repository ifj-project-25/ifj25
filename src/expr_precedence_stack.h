/**
 * @file parser.c
 * @author xmikusm00
 * @brief expression parser header file
 */
#ifndef EXPR_PARSER_PRECEDENCE_H
#define EXPR_PARSER_PRECEDENCE_H
#include "scanner.h"
#include "symtable.h"
#include "expr_stack.h"
#include "expr_ast.h"

typedef enum {
    PS_TERM,      

    PS_PLUS,
    PS_MINUS,
    PS_MUL,
    PS_DIV,

    PS_LPAREN,
    PS_RPAREN,

    PS_LT,
    PS_GT,
    PS_LTE,
    PS_GTE,

    PS_IS,      

    PS_EQ,      
    PS_NEQ,

    PS_DOLLAR,

} Sym;

typedef enum {
    SYM_TERM,   
    SYM_NONTERM, 
} SymType;  
typedef struct ExprPstackNode {
    SymType type;
    Sym sym;
    Token token; 
    ExprNode* node;
    struct ExprPstackNode* next;
} ExprPstackNode;

typedef struct ExprPstack {
    ExprPstackNode* top;
} ExprPstack;
    
    
void expr_Pstack_init(ExprPstack* stack);
void expr_Pstack_free(ExprPstack* stack);
void expr_Pstack_push_term(ExprPstack* stack, Token* token, Sym sym);
void expr_Pstack_push_nonterm(ExprPstack* stack, ExprNode* node);
void expr_Pstack_pop(ExprPstack* stack);
ExprNode* expr_Pstack_top(ExprPstack* stack);
bool expr_Pstack_is_empty(ExprPstack* stack);



struct ASTNode* expression_parser_main(Token *token, int *error_code);



#endif //EXPR_PARSER_PRECEDENCE_H
