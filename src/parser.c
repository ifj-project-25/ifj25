/**
 * @file parser.c
 * @author xmikusm00
 * @brief Parser function
 */
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "symtable.h"
#include "expr_parser.h"
#include "expr_precedence_parser.h"
#include "ast.h"

static void next_token(Token *token);
static void token_control(TokenType expected_type, const void *expected_value);

static ASTNode* IF();
static ASTNode* WHILE();
static ASTNode* VAR();
static ASTNode* STML();
static ASTNode* STML_LINE();
static int STML_LIST(ASTNode* current_function);
static int eol(void);
static ASTNode* BLOCK();
ASTNode* PARAMETER_TAIL(ASTNode* node);
ASTNode* PARAMETER_LIST();
static ASTNode* DEF_FUN_TAIL(char* id);
static ASTNode* DEF_FUN();
static ASTNode* DEF_FUN_LIST(ASTNode* current_token);
static int CLASS( ASTNode* PROGRAM);
static int PROLOG();
//static int ARGUMENT_TAIL();
//static int ARGUMENT_LIST();
static ASTNode* EXPRESSION(); 


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
    if (rc != NO_ERROR) return;  
    token_output = get_token(token);
    if (token_output != NO_ERROR){
        rc = token_output;
        

    } 
}

static void token_control(TokenType expected_type, const void *expected_value){
    if (rc != NO_ERROR) return;  
    if(token.type != expected_type){
        rc = SYNTAX_ERROR;

        return ;
    } 
    switch( expected_type){
        case TOKEN_KEYWORD:
            if(token.value.keyword != *(const Keyword*)expected_value){
                rc = SYNTAX_ERROR;
                return ;
            }
        return ;
        case TOKEN_STRING:
            if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                rc = SYNTAX_ERROR;
                return ;
            }
            return ;
        case TOKEN_IDENTIFIER:
                if(expected_value != NULL){
                if(token.value.string == NULL || d_string_cmp(token.value.string,expected_value)){
                    rc = SYNTAX_ERROR;
                    return ;
                }
            }
            return ;
        default:
            return ;
    }

}
static ASTNode* FUNC_CALL(ASTNode* id_node){
    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    ASTNode* call_node = create_ast_node(AST_FUNC_CALL, id_node->name);
    if(call_node == NULL){
        rc = ERROR_INTERNAL;
        return NULL;
    }

    call_node->left = PARAMETER_LIST();
    if (rc != NO_ERROR)return NULL;

    token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return NULL;
    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    return call_node;
}
static ASTNode* EXPRESSION( ){
    int error_code = NO_ERROR;
    ASTNode* expressionTree = main_precedence_parser( &token, &error_code);
    if (expressionTree == NULL || error_code != NO_ERROR){
        rc = SYNTAX_ERROR;
        return NULL;
    }
    if (expressionTree->type == AST_FUNC_CALL){
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        expressionTree->left = PARAMETER_LIST();
        if (rc != NO_ERROR)return NULL;
        token_control(TOKEN_RPAREN,NULL);
        if (rc != NO_ERROR)return NULL;
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        ASTNode* expressionWrpaper = create_ast_node(AST_EXPRESSION, NULL);
        if (expressionWrpaper == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        expressionWrpaper->left = expressionTree;
        expressionTree = expressionWrpaper;
    }
    
    return expressionTree;
}
static ASTNode* IF(){
    ASTNode* node = create_ast_node(AST_IF, NULL);
    if (node == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    node->left = EXPRESSION();
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    node->right = BLOCK();
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }
    ;

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    expected_keyword = KEYWORD_ELSE;
    token_control(TOKEN_KEYWORD, &expected_keyword);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }
    
    ASTNode* else_node = create_ast_node(AST_ELSE, NULL);
    if (node == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    node->right->right = else_node;

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        return NULL;
    }

    else_node->right = BLOCK();
    else_node->left = NULL;
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    return node;
            
}
static ASTNode* WHILE(){
    ASTNode* while_node = create_ast_node(AST_WHILE, NULL);
    if (while_node == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return NULL;
    while_node -> left = create_ast_node(AST_EXPRESSION, NULL);
    if (while_node->left == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    while_node -> left = EXPRESSION();
    if (rc != NO_ERROR)return NULL;

    token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    while_node -> right = BLOCK();
    if (rc != NO_ERROR)return NULL;
    return while_node;
}

static ASTNode* VAR(){
    ASTNode* var_node = create_ast_node(AST_VAR_DECL, NULL);
    if (var_node == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return NULL;
    var_node -> left = create_ast_node(AST_IDENTIFIER, token.value.string->str);
    if (var_node->left == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    
    return var_node;
}
static ASTNode* IFJ(){
    // Expect to be called when current token is DOT (STML consumed KEYWORD_IFJ and advanced)
    token_control(TOKEN_DOT, NULL);
    if (rc != NO_ERROR) return NULL;

    // Move to method identifier
    next_token(&token);
    if (rc != NO_ERROR) return NULL;

    // Method name identifier
    token_control(TOKEN_IDENTIFIER, NULL);
    if (rc != NO_ERROR) return NULL;

    // Create call node with method name (e.g., write)
    ASTNode* node = create_ast_node(AST_FUNC_CALL, token.value.string->str);
    if (node == NULL) {
        rc = ERROR_INTERNAL;
        return NULL;
    }

    // Expect '('
    next_token(&token);
    if (rc != NO_ERROR) return NULL;
    token_control(TOKEN_LPAREN, NULL);
    if (rc != NO_ERROR) return NULL;

    // Move to first argument or RPAREN
    next_token(&token);
    if (rc != NO_ERROR) return NULL;

    // Parse argument list
    node->left = PARAMETER_LIST();
    if (rc != NO_ERROR) return NULL;

    // Expect ')'
    token_control(TOKEN_RPAREN, NULL);
    if (rc != NO_ERROR) return NULL;

    return node;
}

static ASTNode* STML(){
    ASTNode* statement = NULL;
    
    switch (token.type)
    {
    case TOKEN_KEYWORD :
        switch (token.value.keyword)
        {
        case KEYWORD_VAR:
            statement = VAR();
            if (rc != NO_ERROR)return NULL;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            
            break;

        case KEYWORD_IF: //if statement
            statement = IF();
            if (rc != NO_ERROR)return NULL;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
        break;
        
        case KEYWORD_WHILE:
            statement = WHILE();
            if (rc != NO_ERROR)return NULL;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
        break;
        
        case KEYWORD_RETURN:
            
            next_token(&token);
            if (rc != NO_ERROR)return NULL;

            statement = create_ast_node(AST_RETURN, NULL);
            if (statement == NULL){
                rc = ERROR_INTERNAL;
                return NULL; 
            }
            if (token.type == TOKEN_EOL) {
                
            } else {
            statement->right = EXPRESSION();
            if (rc != NO_ERROR)return NULL;
            }
            break;
        case KEYWORD_IFJ:
            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            
            statement = IFJ();
            if (rc != NO_ERROR)return NULL;

            // Advance past ')', next should be EOL or other separator
            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            break;

        default:
            rc = SYNTAX_ERROR;
            return NULL;
        }
        break;

    case TOKEN_LPAREN:
        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        PARAMETER_LIST();
        if (rc != NO_ERROR)return NULL;

        token_control(TOKEN_RPAREN,NULL);

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        break;

    case TOKEN_LCURLY:
        statement = BLOCK();
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        break;

        
    case TOKEN_IDENTIFIER:
    case TOKEN_GLOBAL_VAR:
        ASTNode* id_node = create_ast_node(AST_IDENTIFIER, token.value.string->str);
        if (id_node == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        if (token.type == TOKEN_LPAREN) { // CALL
            ASTNode* call_node = FUNC_CALL(id_node);
            if (rc != NO_ERROR)return NULL;
            statement = call_node;
            break;
        }

        if (token.type == TOKEN_EQUAL){//ASSIGNMENT
            ASTNode* assign_node = create_ast_node(AST_ASSIGN, NULL);
            if (assign_node == NULL){
                rc = ERROR_INTERNAL;
                return NULL; 
            }
            statement = assign_node;  
            assign_node->left = create_ast_node(AST_EQUALS, NULL);
            if (assign_node->left == NULL){
                rc = ERROR_INTERNAL;
                return NULL; 
            }
            assign_node->left->left = id_node;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            if (token.type == TOKEN_KEYWORD && token.value.keyword == KEYWORD_IFJ) {
                next_token(&token);
                if (rc != NO_ERROR) return NULL;
                assign_node->left->right = IFJ();
                if (rc != NO_ERROR) return NULL;
                next_token(&token);
                if (rc != NO_ERROR) return NULL;
            } else {
                assign_node->left->right = EXPRESSION();
                if (rc != NO_ERROR)return NULL;
            }
            break;
        }

        rc = SYNTAX_ERROR;
        return NULL;
        break;

    default:
        rc = SYNTAX_ERROR;
        return NULL;
    }
    return statement;
}
static ASTNode* STML_LINE(){

    ASTNode* current_function = STML();
    if (rc != NO_ERROR)return NULL;
    
    eol();
    if (rc != NO_ERROR)return NULL;

    return current_function;
}
static int STML_LIST(ASTNode* block){
    if (!((token.type == TOKEN_RCURLY))){
        ASTNode* current_statement = STML_LINE();
        if (rc != NO_ERROR)return rc;
        
        // Link statements as siblings using 'right' pointer
        if (block->left == NULL) {
            // First statement goes to left
            block->left = current_statement;
        } else {
            // Subsequent statements chain via right
            ASTNode* last = block->left;
            while (last->right != NULL) {
                last = last->right;
            }
            last->right = current_statement;
        }

        rc = STML_LIST(block);
        if (rc != NO_ERROR)return rc;
    }
    
    return NO_ERROR;
}


static int eol(){
    
    token_control(TOKEN_EOL,NULL);
    skip_eol();
    return rc;
}

static ASTNode* BLOCK(){
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    if (block == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
    (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR){
        return NULL;
    }
    

    next_token(&token);
        if (rc != NO_ERROR){
        return NULL;
    }
    
    
    eol();
    if (rc != NO_ERROR){
        return NULL;
    }
    


    rc = STML_LIST(block);
    if (rc != NO_ERROR){
        return block;
    }
    

    (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR){
        return NULL;
    }
    

    return block;

}

ASTNode* PARAMETER_TAIL(ASTNode* argument_node){
    if(token.type != TOKEN_RPAREN){
        argument_node->left=create_ast_node(AST_FUNC_ARG,NULL);
        if (argument_node->left == NULL){
        rc = ERROR_INTERNAL;
        return NULL; 
    }
        token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        argument_node->left->right = EXPRESSION();
        if (rc != NO_ERROR || argument_node->right == NULL || argument_node->right->type == AST_FUNC_CALL){
            rc = SYNTAX_ERROR;
            return NULL;
        }

        PARAMETER_TAIL(argument_node->left);
        if (rc != NO_ERROR)return NULL;
        return argument_node;
    }
    return NULL;
}

ASTNode* PARAMETER_LIST(){
    if(token.type != TOKEN_RPAREN){
        ASTNode* argument_node = create_ast_node(AST_FUNC_ARG,NULL);
        if (argument_node == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        } 
        argument_node->right = EXPRESSION();
        if (rc != NO_ERROR || argument_node->right == NULL || argument_node->right->type == AST_FUNC_CALL){
            rc = SYNTAX_ERROR;
            return NULL;
        }
        if (rc != NO_ERROR)return NULL;
        PARAMETER_TAIL(argument_node);
        if (rc != NO_ERROR)return NULL;
        return argument_node;
    }
    return NULL;
}

static int SETTER(ASTNode* function){
    next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        token_control(TOKEN_LPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc; 
        function->left = create_ast_node(AST_IDENTIFIER, token.value.string->str);
        if (function->left == NULL){
            rc = ERROR_INTERNAL;
            return rc; 
    }      

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        token_control(TOKEN_RPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        function->right = BLOCK();
        if (rc != NO_ERROR)return rc;
        
    next_token(&token);
    if (rc != NO_ERROR)return rc;

        eol();
        if (rc != NO_ERROR)return rc;

        return NO_ERROR;
}
static ASTNode* ARGUMENT_TAIL(ASTNode* node){
    if(token.type != TOKEN_RPAREN){
        node->left=create_ast_node(AST_FUNC_ARG,NULL);
        if (node->left == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        token_control(TOKEN_IDENTIFIER, NULL);
        if (rc != NO_ERROR)return NULL;

        node->left->right = create_ast_node(AST_IDENTIFIER, token.value.string->str);
        if (node->left->right == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        ARGUMENT_TAIL(node->left);
        if (rc != NO_ERROR)return NULL;
        return node;
    }
    return NULL;
}

static ASTNode* ARGUMENT_LIST(){
    if(token.type != TOKEN_RPAREN){
        token_control(TOKEN_IDENTIFIER, NULL);
        if (rc != NO_ERROR)return NULL;

        ASTNode* argument_node = create_ast_node(AST_FUNC_ARG,NULL);
        if (argument_node == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        } 
        argument_node->right = create_ast_node(AST_IDENTIFIER, token.value.string->str);
        if (argument_node->right == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        ARGUMENT_TAIL(argument_node);
        if (rc != NO_ERROR)return NULL;
        return argument_node;
    }
    return NULL;
}
static ASTNode* DEF_FUN_TAIL(char* id_name){
    ASTNode* function = NULL;
    
    // Getter: static identifier {
    if(token.type == TOKEN_LCURLY){
        function = create_ast_node(AST_GETTER_DEF, id_name);
        if (function == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }

        function->right = BLOCK();
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        eol();
        if (rc != NO_ERROR)return NULL;
        
        return function;
    }
    
    // Setter: static identifier=(param) {
    else if(token.type == TOKEN_EQUAL){
        function = create_ast_node(AST_SETTER_DEF, id_name);
        if (function == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        SETTER(function);
        if (rc != NO_ERROR)return NULL;
        return function;
    }
    
    // Function: static identifier(...) {
    else if(token.type == TOKEN_LPAREN){
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        
        function = create_ast_node(AST_FUNC_DEF, id_name);
        if (function == NULL){
            rc = ERROR_INTERNAL;
            return NULL; 
        }
        
        if(token.type != TOKEN_RPAREN){
            function->left = ARGUMENT_LIST();
        }
        if (rc != NO_ERROR)return NULL;

        (token_control(TOKEN_RPAREN,NULL));
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        
        function->right = BLOCK();
        if (rc != NO_ERROR)return NULL;
        
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        
        eol();
        if (rc != NO_ERROR)return NULL;
        
        return function;
    }
    
    else{
        rc = SYNTAX_ERROR;
        return NULL;
    }
}
static ASTNode* DEF_FUN(){
    expected_keyword = KEYWORD_STATIC;
    token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return NULL; 
    char* id_name = token.value.string->str;
    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    ASTNode* new_function = DEF_FUN_TAIL(id_name);
    if (rc != NO_ERROR)return NULL;
    

    return new_function;
}
static ASTNode* DEF_FUN_LIST(ASTNode* current_node){
    if (((token.type == TOKEN_KEYWORD) && (token.value.keyword == KEYWORD_STATIC))){
        ASTNode* new_function = DEF_FUN();
        if (new_function == NULL){
            return NULL;
        }

        if (current_node->right == NULL){
        if (current_node->type == AST_PROGRAM){
            current_node->left = new_function;
    }
    }
    else if (current_node -> right ->right == NULL){
        current_node->right->right = new_function;
    }
    
    

        ASTNode* tail = DEF_FUN_LIST(new_function);
        if (tail != NULL && rc != NO_ERROR){
            rc = SYNTAX_ERROR;
            return NULL;
        }
        
    }
    return NULL;
}
static int CLASS(ASTNode* PROGRAM){

    expected_keyword = KEYWORD_CLASS;
    token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;

    token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return rc;
    if (strcmp(token.value.string->str, "Program") != 0){
        rc = SYNTAX_ERROR;
        return rc;
    }

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR)return rc;
 
    next_token(&token);
    if(rc != NO_ERROR)return rc;

    eol();
    if(rc != NO_ERROR)return rc;

    DEF_FUN_LIST(PROGRAM);
    if(rc != NO_ERROR)return rc;

     token_control(TOKEN_RCURLY,NULL);
     if (rc != NO_ERROR) return rc;

    return rc;

}
static int PROLOG(){//CORRECT
    
    expected_keyword = KEYWORD_IMPORT;
    (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    token_control(TOKEN_STRING,"ifj25");
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
    expected_keyword = KEYWORD_FOR;
    (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;
    next_token(&token);
    if (rc != NO_ERROR)return rc;
    expected_keyword = KEYWORD_IFJ;
    (token_control(TOKEN_KEYWORD,&expected_keyword));
    if (rc != NO_ERROR)return rc;

    

    return rc;
}


int parser(ASTNode* PROGRAM){
    next_token(&token);
    if (rc != NO_ERROR)return rc;
    skip_eol();
    if (rc != NO_ERROR)return rc;
    rc = PROLOG();
    if(rc != NO_ERROR)return rc;
    
    if(rc != NO_ERROR)return rc;

    next_token(&token);
    if(rc != NO_ERROR)return rc;
    eol();
    if(rc != NO_ERROR)return rc;
    
    rc = CLASS(PROGRAM);
    if(rc != NO_ERROR)return rc;
    return rc;
    
}
