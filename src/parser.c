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
#include "ast.h"

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

static ASTNode* IF();
static ASTNode* WHILE();
static ASTNode* VAR();
static ASTNode* STML(ASTNode* function);
static ASTNode* STML_LINE(ASTNode* function);
static int STML_LIST(ASTNode* function);
static int eol(void);
static ASTNode* BLOCK();
static int PARAMETER_TAIL();
static int PARAMETER_LIST();
static int DEF_FUN_TAIL(ASTNode* function);
static ASTNode* DEF_FUN();
static ASTNode* DEF_FUN_LIST(ASTNode* current_token);
static int CLASS( ASTNode* PROGRAM);
static int PROLOG();
static int ARGUMENT_TAIL();
static int ARGUMENT_LIST();
static ExprNode* EXPRESSION( char* first_expr_parser_token); 


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
static int ARGUMENT_TAIL(){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        EXPRESSION(NULL);
        if (rc != NO_ERROR)return rc;

        ARGUMENT_TAIL();
        if (rc != NO_ERROR)return rc;
    }
    //dont i need next token here because what it ends on epsilon and the caller will automaticaly make next token then i skipp one token, right ? 
    return NO_ERROR;
}

static int ARGUMENT_LIST(){
    EXPRESSION(NULL);
    if (rc != NO_ERROR)return rc;

    ARGUMENT_TAIL();
    return NO_ERROR;
}
static ExprNode* EXPRESSION( char* first_expr_parser_token){
    int error_code = NO_ERROR;
    ExprNode* expressionTree = expression_parser_main(first_expr_parser_token, &token, &error_code);
    if (expressionTree == NULL || error_code != NO_ERROR){
        printf("Error: Failed to parse expression\n");
        rc = SYNTAX_ERROR;
        return NULL;
    }
    return expressionTree;
}
static ASTNode* IF(){
    ASTNode* node = create_ast_node(AST_IF, NULL);

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    ExprNode* expr = EXPRESSION(NULL);
    if (expr == NULL){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }
    node->left = create_ast_node(AST_EXPRESSION, NULL);
    node->left->expr = expr; // attach expression tree to IF condition

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
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
        rc = SYNTAX_ERROR;
        return NULL;
    }

    expected_keyword = KEYWORD_ELSE;
    rc = token_control(TOKEN_KEYWORD, &expected_keyword);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }
    
    ASTNode* else_node = create_ast_node(AST_ELSE, NULL);
    node->right->right = else_node;

    next_token(&token);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
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
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return NULL;
    while_node -> left = create_ast_node(AST_EXPRESSION, NULL);
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    ExprNode* expr = EXPRESSION(NULL);
    if (rc != NO_ERROR)return NULL;
    while_node -> left -> expr = expr; // attach expression tree to WHILE condition

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    while_node -> right = BLOCK();
    if (rc != NO_ERROR)return NULL;
    return while_node;
}

static ASTNode* VAR(){
    ASTNode* var_node = create_ast_node(AST_VAR_DECL, NULL);
    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return NULL;
    var_node -> left = create_ast_node(AST_IDENTIFIER, token.value.string->str);
    
    return var_node;
}
static ASTNode* IFJ(){
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    rc = token_control(TOKEN_LPAREN,NULL);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;

    rc = ARGUMENT_LIST();
    if (rc != NO_ERROR)return NULL;

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return NULL;

    return NULL;
}

static ASTNode* STML(ASTNode* function){
    printf("STML processing token type: %s (%d)\n", token_type_name(token.type), token.type);
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
            statement->right = create_ast_node(AST_EXPRESSION, NULL);
            statement->right->expr = EXPRESSION(NULL);
            if (rc != NO_ERROR)return NULL;
            break;
        case KEYWORD_IFJ:
            next_token(&token);
            if (rc != NO_ERROR)return NULL;

            IFJ();
            if (rc != NO_ERROR)return NULL;

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

        ARGUMENT_LIST();
        if (rc != NO_ERROR)return NULL;

        rc = token_control(TOKEN_RPAREN,NULL);

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        break;
    case TOKEN_IDENTIFIER:
        ASTNode* id_node = create_ast_node(AST_IDENTIFIER, token.value.string->str);
        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        if (token.type == TOKEN_EQUAL){//ASSIGNMENT
            ASTNode* assign_node = create_ast_node(AST_ASSIGN, NULL);
            statement = assign_node;  // Fix: assign to statement, not statement->left
            assign_node->left = create_ast_node(AST_EQUALS, NULL);
            assign_node->left->left = id_node;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            assign_node ->left -> right = create_ast_node(AST_EXPRESSION, NULL); //TO_BE_ASKED(MICHAL): it isnt expression but ask Michal if he needs it like this 
            if (token.type == TOKEN_IDENTIFIER){
                ASTNode* first_expr_parser_token = create_ast_node(AST_IDENTIFIER, token.value.string->str);
                next_token(&token);
                if (rc != NO_ERROR)return NULL;
                if (token.type == TOKEN_LPAREN) { // CALL
                    next_token(&token);
                    if (rc != NO_ERROR)return NULL;
                    ASTNode* call_node = create_ast_node(AST_FUNC_CALL, first_expr_parser_token->name);
                    //ARGUMENT_LIST();
                    //if (rc != NO_ERROR)return NULL;

                    rc = token_control(TOKEN_RPAREN,NULL);
                    if (rc != NO_ERROR)return NULL;
                    debug_print_token("After processing function call in assignment, next token is:", &token);
                    next_token(&token);
                    if (rc != NO_ERROR)return NULL;
                    assign_node->left->right = call_node;
                    break;
                }
                else {
                    ExprNode* expr = EXPRESSION(first_expr_parser_token->name);
                    if (rc != NO_ERROR)return NULL;
                    assign_node->left->right->expr = expr; // attach expression tree to assignment

                }}
                else{
                    ExprNode* expr = EXPRESSION(NULL);
                    if (rc != NO_ERROR)return NULL;
                    assign_node -> right -> expr = expr; // attach expression tree to assignment
                    
                }
            
            break;
        }
        else if (token.type == TOKEN_LPAREN){ // CALL
            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            ASTNode* call_node = create_ast_node(AST_FUNC_CALL, id_node->name);
            statement = call_node;
            
            ARGUMENT_LIST();
            if (rc != NO_ERROR)return NULL;

            rc = token_control(TOKEN_RPAREN,NULL);
            if (rc != NO_ERROR)return NULL;
            break;
        }

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        EXPRESSION(NULL);
        if (rc != NO_ERROR)return NULL;
        break;

    default:
        rc = SYNTAX_ERROR;
        return NULL;
    }
    return statement;
}
static ASTNode* STML_LINE(ASTNode* function){

    ASTNode* current_function = STML(function);
    if (rc != NO_ERROR)return NULL;
    
    eol();
    if (rc != NO_ERROR)return NULL;

    return current_function;
}
static int STML_LIST(ASTNode* function){
    if (!((token.type == TOKEN_RCURLY))){
        ASTNode* current_statement = STML_LINE(function);
        if (rc != NO_ERROR)return rc;
        
        // Link statements as siblings using 'right' pointer
        if (function->left == NULL) {
            // First statement goes to left
            function->left = current_statement;
        } else {
            // Subsequent statements chain via right
            ASTNode* last = function->left;
            while (last->right != NULL) {
                last = last->right;
            }
            last->right = current_statement;
        }

        // Recursively call with BLOCK (function), not the statement
        rc = STML_LIST(function);
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

static ASTNode* BLOCK(){
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    rc = (token_control(TOKEN_LCURLY,NULL));
    if (rc != NO_ERROR){
        rc = SYNTAX_ERROR;
        return NULL;
    }
    

    next_token(&token);
        if (rc != NO_ERROR){
        rc = SYNTAX_ERROR;
        return NULL;
    }
    
    
    eol();
    if (rc != NO_ERROR){
        rc = SYNTAX_ERROR;
        return NULL;
    }
    


    rc = STML_LIST(block);
    if (rc != NO_ERROR){
        rc = SYNTAX_ERROR;
        return block;
    }
    

    rc = (token_control(TOKEN_RCURLY,NULL));
    if (rc != NO_ERROR){
        rc = SYNTAX_ERROR;
        return NULL;
    }
    

    return block;

}
static int PARAMETER_TAIL(){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return rc;
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL();
        if (rc != NO_ERROR)return rc;
    }

    return NO_ERROR;
}
static int PARAMETER_LIST(){
    if (!((token.type == TOKEN_RPAREN))){
        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_TAIL();
        if (rc != NO_ERROR)return rc;


    }
    return NO_ERROR;
}
static int DEF_FUN_TAIL(ASTNode* function){
    if(!((token.type == TOKEN_RPAREN))){
        next_token(&token);
        if (rc != NO_ERROR)return rc;
        PARAMETER_LIST();

        rc = (token_control(TOKEN_RPAREN,NULL));
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
    else if(token_control(TOKEN_LCURLY,NULL)==NO_ERROR){
        function->right = BLOCK();
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

        BLOCK();
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
static ASTNode* DEF_FUN(){
    expected_keyword = KEYWORD_STATIC;
    rc =token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    rc = token_control(TOKEN_IDENTIFIER,NULL);
    if (rc != NO_ERROR)return NULL; 

    ASTNode* new_function;
    if(strcmp(token.value.string->str, "main") == 0){
        new_function = create_ast_node(AST_MAIN_DEF, "main");
    } else {
        new_function = create_ast_node(AST_FUNC_DEF, token.value.string->str);
    }

    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    rc = DEF_FUN_TAIL(new_function);
    if (rc != NO_ERROR)return NULL;
    

    return new_function;
}
static ASTNode* DEF_FUN_LIST(ASTNode* current_token){
    if (((token.type == TOKEN_KEYWORD) && (token.value.keyword == KEYWORD_STATIC))){
        ASTNode* new_function = DEF_FUN();
        if (new_function == NULL){
            rc = SYNTAX_ERROR;
            return NULL;
        }

        // For PROGRAM node, assign to right directly
        // For FUNC_DEF nodes, assign to right->right (BLOCK's right)
        if (current_token->type == AST_PROGRAM){
            current_token->right = new_function;
        } else if (current_token->right != NULL && current_token->right->right == NULL){
            current_token->right->right = new_function;
        }

        ASTNode* tail = DEF_FUN_LIST(new_function->right->right);
        if (tail != NULL && rc != NO_ERROR){
            rc = SYNTAX_ERROR;
            return NULL;
        }
        
    }
    return NULL;
}
static int CLASS(ASTNode* PROGRAM){

    expected_keyword = KEYWORD_CLASS;
    rc = token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return rc;

    next_token(&token);
    if (rc != NO_ERROR)return rc;
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
    DEF_FUN_LIST(PROGRAM);
    if(rc != NO_ERROR)return rc;

    
     /* The current token after returning from DEF_FUN_LIST should be
         the closing RCURLY for the class. Check it before advancing.
         Calling next_token() here prematurely moved past the RCURLY and
         caused the token_control mismatch (expected RCURLY but got EOL). */
     rc = token_control(TOKEN_RCURLY,NULL);
     if (rc != NO_ERROR) return rc;

    return rc;

}
static int PROLOG(){//CORRECT
    
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


int parser(ASTNode** PROGRAM){
    next_token(&token);
    skip_eol();
    if (rc != NO_ERROR)return rc;
    //TODO:: add Global function support
    if(rc != NO_ERROR)return rc;
    rc = PROLOG();
    // Create root node and return it to caller
    if (rc == NO_ERROR) {
        *PROGRAM = create_ast_node(AST_PROGRAM, NULL);
    }
    (*PROGRAM)->left = create_ast_node(AST_VAR_DECL, "To_Be_Created");
    if(rc != NO_ERROR)return rc;

    next_token(&token);
    if(rc != NO_ERROR)return rc;
    eol();
    if(rc != NO_ERROR)return rc;
    
    rc = CLASS(*PROGRAM);
    if(rc != NO_ERROR)return rc;
    return rc;
    
}
