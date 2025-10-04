/**
 * @file parser.c
 * @author xmikusm00
 * @brief Parser function
 */
#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
static Token token; 
int return_code = NO_ERROR;
int token_output = NO_ERROR;
static void next_token(void){
    token_output = get_token(&token);
    if (token_output != NO_ERROR){
        return_code = token_output;
    } 
}
static int token_control(TokenType expected_type, const void *expected_value){
    if(token.type != expected_type){
        return_code = SYNTAX_ERROR;
        return return_code;
    } 
    switch( expected_type){
        case TOKEN_KEYWORD:
            if(token.value.keyword != *(const Keyword*)expected_value){
                return_code = SYNTAX_ERROR;
            }
            return return_code; 
        case TOKEN_STRING:
            if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                return_code = SYNTAX_ERROR;
            }
            return return_code;
        default:
            return return_code;
    }

}
int CLASS(){
    
    return return_code;

}
int PROLOG(){
    if (token.type != TOKEN_KEYWORD || token.value.keyword != KEYWORD_IMPORT)
    {
        return_code = SYNTAX_ERROR;
    }
    next_token();
    if((token.type != TOKEN_STRING) || token.value.string == NULL || d_string_cmp(token.value.string,"ifj25"))
    {
        return_code = SYNTAX_ERROR;
    }
    next_token();
    if(token.type != TOKEN_KEYWORD || token.value.keyword != KEYWORD_FOR)
    {
        return_code = SYNTAX_ERROR;
    }
    next_token();
    if(token.type != TOKEN_KEYWORD || token.value.keyword != KEYWORD_IFJ)
    {
        return_code = SYNTAX_ERROR;
    } 
    return return_code;
    
}

int PROGRAM(void){
    if (PROLOG() != NO_ERROR)
    {
        return return_code;
    }
    next_token();
    if ( token_control(TOKEN_EOL,NULL) != NO_ERROR)
    {
        return return_code;
    }
    next_token();
    if (CLASS() != NO_ERROR)
    {
        return return_code;
    }

    return return_code;
    
    

}


int parser(){
    next_token();
    if (return_code != NO_ERROR){
        return return_code;
    }
    PROGRAM();
    return return_code;
}