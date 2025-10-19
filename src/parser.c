/**
 * @file parser.c
 * @author xmikusm00
 * @brief Parser function
 */
#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
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
int eol(){
    next_token();
    if(rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_EOL,NULL);
    return rc;
}

int BLOCK(){
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;

    eol();
    if (rc != NO_ERROR)return rc;
    return NO_ERROR;
}
int PARAMETER_TAIL(){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token();
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;
        
        next_token();
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL();
        if (rc != NO_ERROR)return rc;
    }

    return NO_ERROR;
}
int PARAMETER_LIST(){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;
        
        next_token();
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL();
        if (rc != NO_ERROR)return rc;


    }
    return NO_ERROR;
}
int DEF_FUN_TAIL(){
    if(!((token.type == TOKEN_RPAREN))){
        next_token();
        if (rc != NO_ERROR)return rc;
        PARAMETER_LIST();

        rc = (token_control(TOKEN_RPAREN,NULL));
        if (rc != NO_ERROR)return rc;

        next_token();
        if (rc != NO_ERROR)return rc;
        BLOCK();
        if (rc != NO_ERROR)return rc;
        
        return NO_ERROR;

    }
    else if(token_control(TOKEN_LCURLY,NULL)==NO_ERROR){

        //BLOCK();
        return NO_ERROR;

    }
    else if(token_control(TOKEN_EQUAL,NULL)){
    }
    else{
        return SYNTAX_ERROR;
    }
    return NO_ERROR;
}
int DEF_FUN(){
    
    expexpected_keyword = KEYWORD_STATIC;
    rc =token_control(TOKEN_KEYWORD,&expexpected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token();
    if (rc != NO_ERROR)return rc;
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc; 
    next_token();
    if (rc != NO_ERROR)return rc;
    rc = DEF_FUN_TAIL();
    if (rc != NO_ERROR)return rc;  

    return rc;
}
int DEF_FUN_LIST(){
    if (!((token.type == TOKEN_RCURLY))){
        rc = DEF_FUN();
        if (rc != NO_ERROR)return rc;
        next_token();
        if(rc != NO_ERROR)return rc;
        rc = DEF_FUN_LIST();

        if (rc != NO_ERROR)return rc; 
        
    }
    else{
    }
    return rc;
}
int CLASS(){
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
    rc = DEF_FUN_LIST();
    printf("after def fun list %d\n",rc);
    if(rc != NO_ERROR)return rc; 
    
    /* next_token();
    if (rc != NO_ERROR)return rc;
    (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR)return rc; */

    return rc;

}
int PROLOG(){
    
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


int parser(){
    next_token();
    if(rc != NO_ERROR)return rc;
    rc = PROLOG();
    if(rc != NO_ERROR)return rc;
    eol();
    if(rc != NO_ERROR)return rc;

    next_token();
    if(rc != NO_ERROR)return rc;
    rc = CLASS();
    if(rc != NO_ERROR)return rc;
    
    return rc;
}