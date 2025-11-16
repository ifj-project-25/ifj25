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

void debug_print_token(const char *prefix, const Token *t) {
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

    rc = token_control(TOKEN_RPAREN,NULL);
    if (rc != NO_ERROR)return NULL;
    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    return call_node;
}
/* static int ARGUMENT_TAIL(){
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
} */
static ASTNode* EXPRESSION( ){
    int error_code = NO_ERROR;
    ASTNode* expressionTree = main_precedence_parser( &token, &error_code);
    if (expressionTree == NULL || error_code != NO_ERROR){
        printf("Error: Failed to parse expression\n");
        rc = SYNTAX_ERROR;
        return NULL;
    }
    if (expressionTree->type == AST_FUNC_CALL){
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        expressionTree->left = PARAMETER_LIST();
        if (rc != NO_ERROR)return NULL;
        rc = token_control(TOKEN_RPAREN,NULL);
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

    node->left = EXPRESSION(NULL);
    if (rc != NO_ERROR){
        free_ast_tree(node);
        rc = SYNTAX_ERROR;
        return NULL;
    }

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

    while_node -> left = EXPRESSION(NULL); // attach expression tree to WHILE condition
    if (rc != NO_ERROR)return NULL;

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
    // Expect to be called when current token is DOT (STML consumed KEYWORD_IFJ and advanced)
    rc = token_control(TOKEN_DOT, NULL);
    if (rc != NO_ERROR) return NULL;

    // Move to method identifier
    next_token(&token);
    if (rc != NO_ERROR) return NULL;

    // Method name identifier
    rc = token_control(TOKEN_IDENTIFIER, NULL);
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
    rc = token_control(TOKEN_LPAREN, NULL);
    if (rc != NO_ERROR) return NULL;

    // Move to first argument or RPAREN
    next_token(&token);
    if (rc != NO_ERROR) return NULL;

    // Parse argument list
    node->left = PARAMETER_LIST();
    if (rc != NO_ERROR) return NULL;

    // Expect ')'
    rc = token_control(TOKEN_RPAREN, NULL);
    if (rc != NO_ERROR) return NULL;

    return node;
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
            statement->right = EXPRESSION(NULL);
            if (rc != NO_ERROR)return NULL;
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

        rc = token_control(TOKEN_RPAREN,NULL);

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
            statement = assign_node;  // Fix: assign to statement, not statement->left
            assign_node->left = create_ast_node(AST_EQUALS, NULL);
            assign_node->left->left = id_node;

            next_token(&token);
            if (rc != NO_ERROR)return NULL;
            // If RHS starts with Ifj.method(...), parse it via IFJ handler instead of precedence parser
            if (token.type == TOKEN_KEYWORD && token.value.keyword == KEYWORD_IFJ) {
                // Advance to DOT and parse the call
                next_token(&token);
                if (rc != NO_ERROR) return NULL;
                assign_node->left->right = IFJ();
                if (rc != NO_ERROR) return NULL;
                // After IFJ() we're at ')', advance once to continue
                next_token(&token);
                if (rc != NO_ERROR) return NULL;
            } else {
                assign_node->left->right = EXPRESSION(NULL);
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

ASTNode* PARAMETER_TAIL(ASTNode* argument_node){
    if(token.type != TOKEN_RPAREN){
        argument_node->left=create_ast_node(AST_FUNC_ARG,NULL);
        rc = token_control(TOKEN_COMMA,NULL);
        if (rc != NO_ERROR)return NULL;

        next_token(&token);
        if (rc != NO_ERROR)return NULL;

        argument_node->left->right = EXPRESSION(NULL);
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
        argument_node->right = EXPRESSION(NULL);
        if (rc != NO_ERROR || argument_node->right == NULL || argument_node->right->type == AST_FUNC_CALL){
            rc = SYNTAX_ERROR;
            return NULL;
        }
        if (rc != NO_ERROR)return NULL;
        PARAMETER_TAIL(argument_node);
        return argument_node;
    }
    return NULL;
}

static int SETTER(ASTNode* function){
    next_token(&token);
        if (rc != NO_ERROR)return rc;
        
        rc = token_control(TOKEN_LPAREN,NULL);
        if (rc != NO_ERROR)return rc;

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_IDENTIFIER,NULL);
        if (rc != NO_ERROR)return rc; 
        function->left = create_ast_node(AST_IDENTIFIER, token.value.string->str);      

        next_token(&token);
        if (rc != NO_ERROR)return rc;

        rc = token_control(TOKEN_RPAREN,NULL);
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
static ASTNode* DEF_FUN_TAIL(char* id_name){
    ASTNode* function = NULL;
    
    // Getter: static identifier {
    if(token.type == TOKEN_LCURLY){
        function = create_ast_node(AST_GETTER_DEF, id_name);

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
        SETTER(function);
        if (rc != NO_ERROR)return NULL;
        return function;
    }
    
    // Function: static identifier(...) {
    else if(token.type == TOKEN_LPAREN){
        next_token(&token);
        if (rc != NO_ERROR)return NULL;
        
        function = create_ast_node(AST_FUNC_DEF, id_name);
        
        if(token.type != TOKEN_RPAREN){
            function->left = PARAMETER_LIST();
        }

        rc = (token_control(TOKEN_RPAREN,NULL));
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
    rc =token_control(TOKEN_KEYWORD,&expected_keyword);
    if (rc != NO_ERROR)return NULL;

    next_token(&token);
    if (rc != NO_ERROR)return NULL;
    rc = token_control(TOKEN_IDENTIFIER,NULL);
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
            rc = SYNTAX_ERROR;
            return NULL;
        }

        // For PROGRAM node, assign to right directly
        // For FUNC_DEF nodes, assign to right->right (BLOCK's right)

        /* if (current_node->right->right == NULL){
            current_node->right->right = new_function;
        } else {
            current_node->left = new_function;
        } */
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

    DEF_FUN_LIST(PROGRAM);
    if(rc != NO_ERROR)return rc;

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


int parser(ASTNode* PROGRAM){
    next_token(&token);
    skip_eol();
    if (rc != NO_ERROR)return rc;
    //TODO:: add Global function support
    if(rc != NO_ERROR)return rc;
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
