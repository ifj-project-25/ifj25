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
    if (token.type != TOKEN_EOL)
    {
        return_code = SYNTAX_ERROR;
    }
    next_token{};

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