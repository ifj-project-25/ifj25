/**
 * @file parser.c
 * @author xmikusm00
 * @brief Parser function
 */
#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "symtable.h"
#include "expr_parser.h"

//TODO:: add global variable support

static void next_token(Token *token);
static int token_control(TokenType expected_type, const void *expected_value);
// Debug helpers
static const char *token_type_name(TokenType t) {
    switch (t) {
        case TOKEN_UNDEFINED: return "UNDEFINED";
        case TOKEN_EOF: return "EOF";
        case TOKEN_EOL: return "EOL";
        case TOKEN_GLOBAL_VAR: return "GLOBAL_VAR";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_DOUBLE: return "DOUBLE";
        case TOKEN_STRING: return "STRING";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NEQUAL: return "NEQUAL";
        case TOKEN_LESSER: return "LESSER";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESSER_EQUAL: return "LESSER_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_NOT: return "NOT";
        case TOKEN_LOGIC_EQUAL: return "LOGIC_EQUAL";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LCURLY: return "LCURLY";
        case TOKEN_RCURLY: return "RCURLY";
        case TOKEN_DOT: return "DOT";
        case TOKEN_COMMA: return "COMMA";
        default: return "UNKNOWN";
    }
}

static void debug_print_token(const char *prefix, const Token *t) {
    if (!t) return;
    const char *name = token_type_name(t->type);
    if (t->type == TOKEN_IDENTIFIER || t->type == TOKEN_STRING || t->type == TOKEN_GLOBAL_VAR) {
        printf("%s %s (%s)\n", prefix, name, t->value.string ? t->value.string->str : "(null)");
    } else if (t->type == TOKEN_KEYWORD) {
        printf("%s %s (kw=%d)\n", prefix, name, t->value.keyword);
    } else if (t->type == TOKEN_INTEGER) {
        printf("%s %s (int=%d)\n", prefix, name, t->value.integer);
    } else if (t->type == TOKEN_DOUBLE) {
        printf("%s %s (dbl=%f)\n", prefix, name, t->value.decimal);
    } else {
        printf("%s %s\n", prefix, name);
    }
}

static int IF(SymTable *AST);
static int WHILE(SymTable *AST);
static int VAR(SymTable *AST);
static int STML(SymTable *AST);
static int STML_LINE(SymTable *AST);
static int STML_LIST(SymTable *AST);
static int eol(void);
static int BLOCK(SymTable *AST);
static int PARAMETER_TAIL(SymTable *AST);
static int PARAMETER_LIST(SymTable *AST);
static int DEF_FUN_TAIL(SymTable *AST);
static int DEF_FUN(SymTable *AST);
static int DEF_FUN_LIST(SymTable *AST);
static int CLASS(SymTable *AST);
static int PROLOG(SymTable *AST);
static int ARGUMENT_TAIL(SymTable *AST);
static int ARGUMENT_LIST(SymTable *AST);
static int EXPRESSION(void);


Keyword expected_keyword;
Token token;
int rc = NO_ERROR;
int token_output = NO_ERROR;

static int skip_eol(void) {
    while (token.type == TOKEN_EOL) {
        next_token(&token);
        if (rc != NO_ERROR) return rc;
    }
    return NO_ERROR;
}

static void next_token(Token *token){
    token_output = get_token(token);
    debug_print_token("token ->", token);
    if (token_output != NO_ERROR){
        rc = token_output;
        

    } 
}

static int token_control(TokenType expected_type, const void *expected_value){
    if(token.type != expected_type){
        printf("token_control mismatch: expected=%s(%d) got=%s(%d)\n", token_type_name(expected_type), expected_type, token_type_name(token.type), token.type);
        debug_print_token("  current", &token);
        rc = SYNTAX_ERROR;

        return rc;
    } 
    switch( expected_type){
        case TOKEN_KEYWORD:
            if(token.value.keyword != *(const Keyword*)expected_value){
                return SYNTAX_ERROR;
            }
        return NO_ERROR;
        case TOKEN_STRING:
            if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                return SYNTAX_ERROR;
            }
            return NO_ERROR;
        case TOKEN_IDENTIFIER:
                if(expected_value != NULL){
                if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                    return SYNTAX_ERROR;
                }
            }
            return NO_ERROR;
        default:
            return NO_ERROR;
    }

}
static int ARGUMENT_TAIL(SymTable *AST){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        rc = EXPRESSION();
        if (rc != NO_ERROR)return rc;

        ARGUMENT_TAIL(AST);
        if (rc != NO_ERROR)return rc;
    }
    //dont i need next token here because what it ends on epsilon and the caller will automaticaly make next token then i skipp one token, right ? 
    return NO_ERROR;
}

static int ARGUMENT_LIST(SymTable *AST){
    rc = EXPRESSION();
    if (rc != NO_ERROR)return rc;

    ARGUMENT_TAIL(AST);
    return NO_ERROR;
}
static int EXPRESSION(){
    int error_code = NO_ERROR;
    
    ExprNode* expressionTree = expression_parser_main(&token, &error_code);
    if (expressionTree == NULL || error_code != NO_ERROR){
        printf("Error: Failed to parse expression\n");
    }
    else{
        printf("Expression AST:\n");
        print_expr_ast(expressionTree, 0);
        free_expr_node(expressionTree);
    }
    return error_code;
}
static int IF(SymTable *AST){
    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = EXPRESSION();
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    expected_keyword = KEYWORD_ELSE;
    rc = token_control(TOKEN_KEYWORD, &expected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;
            
}
static int WHILE(SymTable *AST){
    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = EXPRESSION();
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;
    return NO_ERROR;
}

static int VAR(SymTable *AST){
    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc;
    
    return NO_ERROR;
}
static int IFJ(SymTable *AST){
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    rc = ARGUMENT_LIST(AST);
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;
}

static int STML(SymTable *AST){
    printf("STML processing token type: %s (%d)\n", token_type_name(token.type), token.type);
    switch (token.type)
    {
    case TOKEN_KEYWORD :
        switch (token.value.keyword)
        {
        case KEYWORD_VAR:
            VAR(AST);
            if (rc != NO_ERROR)return rc;

            next_token(&token);
            if (rc != NO_ERROR)return rc;
            
            break;

        case KEYWORD_IF: //if statement
            IF(AST);
            if (rc != NO_ERROR)return rc;

            next_token(&token);
            if (rc != NO_ERROR)return rc;
        break;
        
        case KEYWORD_WHILE:
            WHILE(AST);
            if (rc != NO_ERROR)return rc;

            next_token(&token);
            if (rc != NO_ERROR)return rc;
        break;
        
        case KEYWORD_RETURN:
            next_token(&token);
            if (rc != NO_ERROR)return rc;
            

            rc = EXPRESSION();
            if (rc != NO_ERROR)return rc;
            break;
        case KEYWORD_IFJ:
            next_token(&token);
            if (rc != NO_ERROR)return rc;

            IFJ(AST);
            if (rc != NO_ERROR)return rc;

            next_token(&token);
            if (rc != NO_ERROR)return rc;
            break;

        default:
            return SYNTAX_ERROR;
        }
        break;

    case TOKEN_LPAREN:
        next_token(&token);
        if (rc != NO_ERROR)return rc;

        ARGUMENT_LIST(AST);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_RPAREN,NULL);

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        break;
    case TOKEN_IDENTIFIER:
        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_EQUAL,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = EXPRESSION();
        if (rc != NO_ERROR)return rc;
        break;

    default:
        return SYNTAX_ERROR;
    }
    return NO_ERROR;
}
static int STML_LINE(SymTable *AST){

    rc = STML(AST);
    if (rc != NO_ERROR)return rc;
    
    eol();
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;
}
static int STML_LIST(SymTable *AST){
    if (!((token.type == TOKEN_RCURLY))){
        rc = STML_LINE(AST);
        if (rc != NO_ERROR)return rc;

        rc = STML_LIST(AST);
        if (rc != NO_ERROR)return rc;
    }
    
    return NO_ERROR;
}


static int eol(){
    
    rc = token_control(TOKEN_EOL,NULL);
    skip_eol();
    if(rc != NO_ERROR)return rc;
    return rc;
}

static int BLOCK(SymTable *AST){
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if(rc != NO_ERROR)return rc;
    
    eol();
    if (rc != NO_ERROR)return rc;


    rc = STML_LIST(AST);
    if (rc != NO_ERROR)return rc;

    rc = (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;

}
static int PARAMETER_TAIL(SymTable *AST){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL(AST);
        if (rc != NO_ERROR)return rc;
    }

    return NO_ERROR;
}
static int PARAMETER_LIST(SymTable *AST){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL(AST);
        if (rc != NO_ERROR)return rc;


    }
    return NO_ERROR;
}
static int DEF_FUN_TAIL(SymTable *AST){
    if(!((token.type == TOKEN_RPAREN))){
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_LIST(AST);

        rc = (token_control(TOKEN_RPAREN,NULL));
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        BLOCK(AST);
        if (rc != NO_ERROR)return rc;
        
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        eol();
        if (rc != NO_ERROR)return rc;
        
        return NO_ERROR;

    }
    else if(token_control(TOKEN_LCURLY,NULL)==NO_ERROR){
        BLOCK(AST);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        eol();
        if (rc != NO_ERROR)return rc;
        return NO_ERROR;

    }
    else if(token_control(TOKEN_EQUAL,NULL)){

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_LPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;       

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_RPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = BLOCK(AST);
        if (rc != NO_ERROR)return rc;

        eol();
        if (rc != NO_ERROR)return rc;

        return NO_ERROR;
    }
    else{
        return SYNTAX_ERROR;
    }
    return NO_ERROR;
}
static int DEF_FUN(SymTable *AST){
    expected_keyword = KEYWORD_STATIC;
    rc =token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return rc;
    
    next_token(&token);
    if (rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc; 

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    rc = DEF_FUN_TAIL(AST);
    if (rc != NO_ERROR)return rc;  

    return rc;
}
static int DEF_FUN_LIST(SymTable *AST){
    if (((token.type == TOKEN_KEYWORD) && (token.value.keyword == KEYWORD_STATIC))){
       
        rc = DEF_FUN(AST);
        if (rc != NO_ERROR)return rc;

        rc = DEF_FUN_LIST(AST);
        if (rc != NO_ERROR)return rc; 
        
    }
    return rc;
}
static int CLASS(SymTable *AST){//CORRECT

    expected_keyword = KEYWORD_CLASS;
    rc = token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    rc = (token_control(TOKEN_IDENTIFIER,"Program"));
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;
 
    next_token(&token);
    if(rc != NO_ERROR)return rc;

    eol();
    if(rc != NO_ERROR)return rc;

    printf("TOKEN before DEF_FUN_LIST: %s\n", token_type_name(token.type));
    rc = DEF_FUN_LIST(AST);
    if(rc != NO_ERROR)return rc; 
    
     /* The current token after returning from DEF_FUN_LIST should be
         the closing RCURLY for the class. Check it before advancing.
         Calling next_token() here prematurely moved past the RCURLY and
         caused the token_control mismatch (expected RCURLY but got EOL). */
     rc = token_control(TOKEN_RCURLY,NULL);
     if (rc != NO_ERROR) return rc;

    return rc;

}
static int PROLOG(SymTable *AST){//CORRECT
    
    expected_keyword = KEYWORD_IMPORT;
    rc = (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_STRING,"ifj25");
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    expected_keyword = KEYWORD_FOR;
    rc = (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;
    next_token(&token);
    if (rc != NO_ERROR)return rc;
    expected_keyword = KEYWORD_IFJ;
    rc = (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;

    return rc;
}


int parser(SymTable *AST){
    next_token(&token);
    skip_eol();
    if (rc != NO_ERROR)return rc;
    //TODO:: add Global function support
    if(rc != NO_ERROR)return rc;
    rc = PROLOG(AST);
    if(rc != NO_ERROR)return rc;

    next_token(&token);
    if(rc != NO_ERROR)return rc;
    eol();
    if(rc != NO_ERROR)return rc;
    
    rc = CLASS(AST);
    if(rc != NO_ERROR)return rc;
    return rc;
    
}
