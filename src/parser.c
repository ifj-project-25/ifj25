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
        case TOKEN_IDENTIFIER:
                if(expected_value != NULL){
                if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                    return_code = SYNTAX_ERROR;
                }
            }
            return return_code;
        default:
            return return_code;
    }

}
int CLASS(){
    next_token();
    if(return_code != NO_ERROR)return return_code;
    expexpected_keyword = KEYWORD_CLASS;
    return_code = token_control(TOKEN_KEYWORD,&expexpected_keyword);
    if (return_code != NO_ERROR)return return_code;
    
    next_token();
    if (return_code != NO_ERROR)return return_code;
    return_code = (token_control(TOKEN_IDENTIFIER,"Program"));
    if (return_code != NO_ERROR)return return_code;

    next_token();
    if (return_code != NO_ERROR)return return_code;
    return_code = (token_control(TOKEN_LCURLY,NULL));
    if (return_code != NO_ERROR)return return_code;

    return return_code;

}
int PROLOG(){
    next_token();
    if(return_code != NO_ERROR)return return_code;
    expexpected_keyword = KEYWORD_IMPORT;
    return_code = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (return_code != NO_ERROR)return return_code;

    next_token();
    if (return_code != NO_ERROR)return return_code;
    return_code = token_control(TOKEN_STRING,"ifj25");
    if (return_code != NO_ERROR)return return_code;

    next_token();
    if (return_code != NO_ERROR)return return_code;
    expexpected_keyword = KEYWORD_FOR;
    return_code = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (return_code != NO_ERROR)return return_code;

    next_token();
    if (return_code != NO_ERROR)return return_code;
    expexpected_keyword = KEYWORD_IFJ;
    return_code = (token_control(TOKEN_KEYWORD,&expexpected_keyword));
    if (return_code != NO_ERROR)return return_code;

    return return_code;
}


int parser(){
    return_code = PROLOG();
    if(return_code != NO_ERROR)return return_code;

    next_token();
    if(return_code != NO_ERROR)return return_code;
    return_code = token_control(TOKEN_EOL,NULL);
    if(return_code != NO_ERROR)return return_code;


    return_code = CLASS();
    if(return_code != NO_ERROR)return return_code;
    
    return return_code;
}