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


static void next_token(void);
static int token_control(TokenType expected_type, const void *expected_value);

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


Keyword expexpected_keyword;
static Token token; 
int rc = NO_ERROR;
int token_output = NO_ERROR;
static void next_token(void){
    token_output = get_token(&token);
    if (token_output != NO_ERROR){
        rc = token_output;
    } 
}

static int token_control(TokenType expected_type, const void *expected_value){
    if(token.type != expected_type){
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
static int IF(SymTable *AST){
    next_token();
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    //expression parsing here

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    expexpected_keyword = KEYWORD_ELSE;
    rc = token_control(TOKEN_KEYWORD, &expexpected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;
            
}
static int WHILE(SymTable *AST){
    next_token();
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    //expression parsing here

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    rc = BLOCK(AST);
    if (rc != NO_ERROR)return rc;
    return NO_ERROR;
}
static int VAR(SymTable *AST){
    next_token();
    if (rc != NO_ERROR)return rc;

    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc;
    
    return NO_ERROR;
}
static int STML(SymTable *AST){
    switch (token.type)
    {
    case TOKEN_KEYWORD :
        switch (token.value.keyword)
        {
        case KEYWORD_VAR:
            VAR(AST);
            if (rc != NO_ERROR)return rc;
            break;

        case KEYWORD_IF: //if statement
            IF(AST);
            if (rc != NO_ERROR)return rc;
        break;
        
        case KEYWORD_WHILE:
            WHILE(AST);
            if (rc != NO_ERROR)return rc;
        break;
        
        case KEYWORD_RETURN:
            //return statement parsing here
            break;

        default:
            return SYNTAX_ERROR;
        }
        break;
    case TOKEN_IDENTIFIER:
        next_token();
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_EQUAL,NULL);
        if (rc != NO_ERROR)return rc;

        //expression parsing here
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

        next_token();
        if (rc != NO_ERROR)return rc;

        rc = STML_LIST(AST);
        if (rc != NO_ERROR)return rc;
    }
    
    return NO_ERROR;
}


static int eol(){
    next_token();
    if(rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_EOL,NULL);
    return rc;
}

static int BLOCK(SymTable *AST){
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    eol();
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    rc = STML_LIST(AST);
    if (rc != NO_ERROR)return rc;

    rc = (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;

    return NO_ERROR;

}
static int PARAMETER_TAIL(SymTable *AST){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token();
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;
        
        next_token();
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
        
        next_token();
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL(AST);
        if (rc != NO_ERROR)return rc;


    }
    return NO_ERROR;
}
static int DEF_FUN_TAIL(SymTable *AST){
    if(!((token.type == TOKEN_RPAREN))){
        next_token();
        if (rc != NO_ERROR)return rc;
        PARAMETER_LIST(AST);

        rc = (token_control(TOKEN_RPAREN,NULL));
        if (rc != NO_ERROR)return rc;

        next_token();
        if (rc != NO_ERROR)return rc;
        BLOCK(AST);
        if (rc != NO_ERROR)return rc;
        
        return NO_ERROR;

    }
    else if(token_control(TOKEN_LCURLY,NULL)==NO_ERROR){
        BLOCK(AST);
        if (rc != NO_ERROR)return rc;

        next_token();
        if (rc != NO_ERROR)return rc;

        eol();
        if (rc != NO_ERROR)return rc;
        return NO_ERROR;

    }
    else if(token_control(TOKEN_EQUAL,NULL)){

        next_token();
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_LPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token();
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;       

        next_token();
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_RPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token();
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
    expexpected_keyword = KEYWORD_STATIC;
    printf("token type %d\n",token.type);
    rc =token_control(TOKEN_KEYWORD,&expexpected_keyword);
    if (rc != NO_ERROR)return rc;
    printf("it is fine \n");
    
    next_token();
    if (rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc; 
    next_token();
    if (rc != NO_ERROR)return rc;
    rc = DEF_FUN_TAIL(AST);
    if (rc != NO_ERROR)return rc;  

    return rc;
}
static int DEF_FUN_LIST(SymTable *AST){
    if (!((token.type == TOKEN_RCURLY))){
       
        rc = DEF_FUN(AST);
        if (rc != NO_ERROR)return rc;

        next_token();
        if(rc != NO_ERROR)return rc;

        rc = DEF_FUN_LIST(AST);
        if (rc != NO_ERROR)return rc; 
        
    }
    return rc;
}
static int CLASS(SymTable *AST){

    expexpected_keyword = KEYWORD_CLASS;
    rc = token_control(TOKEN_KEYWORD,&expexpected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;
    rc = (token_control(TOKEN_IDENTIFIER,"Program"));
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;
 
    eol();
    if(rc != NO_ERROR)return rc;

    next_token();
    if(rc != NO_ERROR)return rc;
    rc = DEF_FUN_LIST(AST);
    if(rc != NO_ERROR)return rc; 
    
    next_token();
    if (rc != NO_ERROR)return rc;
    (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    return rc;

}
static int PROLOG(SymTable *AST){
    
    expexpected_keyword = KEYWORD_IMPORT;
    rc = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_STRING,"ifj25");
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;
    expexpected_keyword = KEYWORD_FOR;
    rc = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (rc != NO_ERROR)return rc;
    next_token();
    if (rc != NO_ERROR)return rc;
    expexpected_keyword = KEYWORD_IFJ;
    rc = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (rc != NO_ERROR)return rc;

    return rc;
}


int parser(SymTable *AST){
    next_token();
    if(rc != NO_ERROR)return rc;
    rc = PROLOG(AST);
    if(rc != NO_ERROR)return rc;
    eol();
    if(rc != NO_ERROR)return rc;

    next_token();
    if(rc != NO_ERROR)return rc;
    rc = CLASS(AST);
    if(rc != NO_ERROR)return rc;
    return rc;
    
}
