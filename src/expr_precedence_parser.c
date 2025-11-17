/**
 * @file expr_precedence_parser.c
 * @author xmikusm00
 * @brief precedence-based expression parser implementation
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "parser.h"
#include "symtable.h"
#include "expr_ast.h"
#include "expr_precedence_parser.h"
#include "expr_precedence_stack.h"
#include "expr_ast.h"
#include "ast.h"

static Sym token_to_sym(const Token* token){
    switch (token->type)
    {
    case TOKEN_PLUS:
        return PS_PLUS;
    case TOKEN_MINUS:
        return PS_MINUS;
    case TOKEN_MULTIPLY:
        return PS_MUL;
    case TOKEN_DIVIDE:
        return PS_DIV;
    case TOKEN_LESSER:
        return PS_LT;
    case TOKEN_GREATER:
        return PS_GT;
    case TOKEN_LESSER_EQUAL:
        return PS_LTE;
    case TOKEN_GREATER_EQUAL:
        return PS_GTE;
    case TOKEN_LOGIC_EQUAL:
        return PS_EQ;
    case TOKEN_NEQUAL:
        return PS_NEQ;
    case TOKEN_LPAREN:
        return PS_LPAREN;
    case TOKEN_RPAREN:
        return PS_RPAREN;
    case TOKEN_IDENTIFIER:
    case TOKEN_GLOBAL_VAR:
    case TOKEN_INTEGER:
    case TOKEN_DOUBLE:
    case TOKEN_STRING:
        return PS_TERM;
    case TOKEN_DOLLAR:
    case TOKEN_EOL:
    case TOKEN_COMMA:
    case TOKEN_EOF:
        return PS_DOLLAR;
    case TOKEN_KEYWORD:
        switch (token->value.keyword)
        {
        case KEYWORD_IS:
            return PS_IS;
        case KEYWORD_NULL_C:
        case KEYWORD_NULL_L:
        case KEYWORD_NUM:
        case KEYWORD_STRING:
            return PS_TERM;
        default:
            return PS_TERM; // Other keywords shouldn't appear
        }
    default:
        return PS_TERM;
    }
}

ExprNode* reduce_term_to_node(ExprPstack* stack){
    ExprNode* node = NULL;
    switch (stack->top->type)
    {
    case SYM_TERM:
        switch (stack->top->token.type)
        {
        case TOKEN_IDENTIFIER:
        case TOKEN_GLOBAL_VAR:
            node = create_identifier_node(stack->top->token.value.string->str);
            break;
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE:
            node = create_num_literal_node(stack->top->token.value.integer);
            break;
        case TOKEN_STRING:
            node = create_string_literal_node(stack->top->token.value.string->str);
            break;  
            
        case TOKEN_KEYWORD:
            if (stack->top->token.value.keyword == KEYWORD_NULL_C ||
                stack->top->token.value.keyword == KEYWORD_NULL_L) {
                node = create_type_node("Null");
            }
            else if (stack->top->token.value.keyword == KEYWORD_NUM) {
                node = create_type_node("Num");
            }
            else if (stack->top->token.value.keyword == KEYWORD_STRING) {
                node = create_type_node("String");
            }
            break;
        default:
            return NULL;
        }
        if (node){
            expr_Pstack_pop(stack);
            expr_Pstack_push_nonterm(stack, node);
        }
        break;
    
    default:
        return NULL;
    }
    return node;
}
static const char prec_table[15][15] = {
//         TERM   +   -   *   /   (   )   <   >  <=  >=  is  ==  !=   $
/* Term */ {' ', '>','>','>','>',' ','>','>','>','>','>','>','>','>','>'},
/* + */    {'<', '>','>','<','<','<','>','>','>','>','>','>','>','>','>'},
/* - */    {'<', '>','>','<','<','<','>','>','>','>','>','>','>','>','>'},
/* * */    {'<', '>','>','>','>','<','>','>','>','>','>','>','>','>','>'},
/* / */    {'<', '>','>','>','>','<','>','>','>','>','>','>','>','>','>'},
/* ( */    {'<', '<','<','<','<','<','=',' ',' ',' ',' ',' ',' ',' ',' '},
/* ) */    {' ', '>','>','>','>',' ','>','>','>','>','>','>','>','>','>'},
/* < */    {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* > */    {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* <= */   {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* >= */   {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* is */   {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* == */   {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* != */   {'<', '<','<','<','<','<','>','>','>','>','>','>','>','>','>'},
/* $ */    {'<', '<','<','<','<','<','T','<','<','<','<','<','<','<',' '}
};

// Debug helper: print stack contents
static void print_stack(ExprPstack* stack) {
    printf("=== STACK (top to bottom) ===\n");
    ExprPstackNode* current = stack->top;
    int depth = 0;
    while (current && depth < 10) {
        printf("  [%d] ", depth);
        if (current->type == SYM_TERM) {
            printf("TERM sym=%d ", current->sym);
            switch (current->token.type) {
                case TOKEN_IDENTIFIER:
                    printf("ID(%s)", current->token.value.string ? current->token.value.string->str : "?");
                    break;
                case TOKEN_INTEGER:
                    printf("INT(%d)", current->token.value.integer);
                    break;
                case TOKEN_DOUBLE:
                    printf("DBL(%.2f)", current->token.value.decimal);
                    break;
                case TOKEN_STRING:
                    printf("STR(\"%s\")", current->token.value.string ? current->token.value.string->str : "?");
                    break;
                case TOKEN_PLUS: printf("+"); break;
                case TOKEN_MINUS: printf("-"); break;
                case TOKEN_MULTIPLY: printf("*"); break;
                case TOKEN_DIVIDE: printf("/"); break;
                case TOKEN_DOLLAR: printf("$"); break;
                case TOKEN_LPAREN: printf("("); break;
                case TOKEN_RPAREN: printf(")"); break;
                default:
                    printf("tok=%d", current->token.type);
                    break;
            }
        } else if (current->type == SYM_NONTERM) {
            printf("NONTERM sym=%d ", current->sym);
            if (current->node) {
                switch (current->node->type) {
                    case EXPR_NUM_LITERAL:
                        printf("E(%.2f)", current->node->data.num_literal);
                        break;
                    case EXPR_IDENTIFIER:
                        printf("E(%s)", current->node->data.identifier_name);
                        break;
                    case EXPR_BINARY_OP:
                        printf("E(binop)");
                        break;
                    case EXPR_STRING_LITERAL:
                        printf("E(\"%s\")", current->node->data.string_literal);
                        break;
                    case EXPR_NULL_LITERAL:
                        printf("E(NULL)");
                        break;
                    default:
                        printf("E(?)");
                        break;
                }
            } else {
                printf("E(NULL)");
            }
        } else {
            printf("UNKNOWN type=%d", current->type);
        }
        printf("\n");
        current = current->next;
        depth++;
    }
    if (current) printf("  ... (more)\n");
    printf("=============================\n");
}

//Rules:
// TERM -> E
// E op E -> E  
int reduce_expr_op_expr(ExprPstack* stack){
    if(expr_Pstack_is_empty(stack)){
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_NONTERM) return SYNTAX_ERROR;
    ExprNode* right = stack->top->node;
    expr_Pstack_pop(stack);
    
    if(expr_Pstack_is_empty(stack)){
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_TERM) return SYNTAX_ERROR;
    
    BinaryOpType op;
    switch (stack->top->token.type) {
        case TOKEN_PLUS: op = OP_ADD; break;
        case TOKEN_MINUS: op = OP_SUB; break;
        case TOKEN_MULTIPLY: op = OP_MUL; break;
        case TOKEN_DIVIDE: op = OP_DIV; break;
        case TOKEN_LESSER: op = OP_LT; break;
        case TOKEN_GREATER: op = OP_GT; break;
        case TOKEN_LESSER_EQUAL: op = OP_LTE; break;
        case TOKEN_GREATER_EQUAL: op = OP_GTE; break;
        case TOKEN_LOGIC_EQUAL: op = OP_EQ; break;
        case TOKEN_NEQUAL: op = OP_NEQ; break;
        case TOKEN_KEYWORD:
            if (stack->top->token.value.keyword == KEYWORD_IS){
                op = OP_IS;
                break;
            }
            else{
                return SYNTAX_ERROR;
            }

        default:
            return SYNTAX_ERROR;
    }
    
    // Extract left operand
    expr_Pstack_pop(stack);
    
    if(expr_Pstack_is_empty(stack)){
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_NONTERM) return SYNTAX_ERROR;
    ExprNode* left = stack->top->node;
    expr_Pstack_pop(stack);
    

    // Create binary operation node
    ExprNode* new_node = create_binary_op_node(op, left, right);
    if (!new_node) {
        return SYNTAX_ERROR;
    }
    expr_Pstack_push_nonterm(stack, new_node);
    return NO_ERROR;
}

int reduce(ExprPstack* stack){
    print_stack(stack);
    if(expr_Pstack_is_empty(stack)){
        return SYNTAX_ERROR;
    }
    // Reduce ( E ) -> E
    if (stack->top->type == SYM_TERM && 
        stack->top->token.type == TOKEN_RPAREN &&
        stack->top->next != NULL &&
        stack->top->next->type == SYM_NONTERM &&
        stack->top->next->next != NULL &&
        stack->top->next->next->type == SYM_TERM &&
        stack->top->next->next->token.type == TOKEN_LPAREN){
        
        expr_Pstack_pop(stack); // Pop )
        
        if(expr_Pstack_is_empty(stack)){
            return SYNTAX_ERROR;
        }
        if (stack->top->type != SYM_NONTERM){
            return SYNTAX_ERROR;
        }
        ExprNode* node = stack->top->node;
        expr_Pstack_pop(stack); // Pop E
        
        if(expr_Pstack_is_empty(stack)){
            return SYNTAX_ERROR;
        }
        if (stack->top->sym != PS_LPAREN){
            return SYNTAX_ERROR;
        }
        expr_Pstack_pop(stack); // Pop (
        
        expr_Pstack_push_nonterm(stack, node); // Push E
    }
    // Reduce TERM -> E
    else if (stack->top->type == SYM_TERM){
        if (stack->top->sym == PS_PLUS || stack->top->sym == PS_MINUS ||
            stack->top->sym == PS_MUL || stack->top->sym == PS_DIV ||
            stack->top->sym == PS_LT || stack->top->sym == PS_GT ||
            stack->top->sym == PS_LTE || stack->top->sym == PS_GTE ||
            stack->top->sym == PS_EQ || stack->top->sym == PS_NEQ ||
            stack->top->sym == PS_IS){
            return SYNTAX_ERROR;
        }
        ExprNode* node = reduce_term_to_node(stack);
        if (node == NULL){
            return SYNTAX_ERROR;
        }
    }
    // Reduce E op E -> E
    else if(stack->top->type == SYM_NONTERM){
        int rc = reduce_expr_op_expr(stack);
        if (rc != NO_ERROR){
            return SYNTAX_ERROR;
        }
    }
    else{
        return SYNTAX_ERROR;
    }
    
    
    return NO_ERROR;
}

ASTNode* main_precedence_parser(Token* token, int* rc) {
    ExprPstack stack;
    ASTNode *ast_expr = NULL;

    expr_Pstack_init(&stack);

    if (token->type == TOKEN_IDENTIFIER){
        Token id_token = *token;
        get_token(token);
        if (*rc != NO_ERROR) {
            expr_Pstack_free(&stack);
            return NULL;
        }
        if (token->type == TOKEN_LPAREN){
            ASTNode* call_node = create_ast_node(AST_FUNC_CALL, id_token.value.string->str);
            if (call_node == NULL) {
                *rc = ERROR_INTERNAL;
                expr_Pstack_free(&stack);
                return NULL;
            }
            expr_Pstack_free(&stack);
            return call_node;
        }
        
        expr_Pstack_push_term(&stack, &id_token, PS_TERM);
    }
    do{ 
        ExprPstackNode* scan = stack.top;
        while (scan && scan->type == SYM_NONTERM) {
            scan = scan->next;  // Skip the NONTERM
        }
        Sym stack_sym = scan ? token_to_sym(&scan->token) : PS_DOLLAR;
        Sym current_sym = token_to_sym(token);
        print_stack(&stack);
        if (prec_table[stack_sym][current_sym] == '<'){
            expr_Pstack_push_term(&stack, token, current_sym);
            get_token(token);
            if (*rc != NO_ERROR) return NULL;
        }
        else if (prec_table[stack_sym][current_sym] == '>'){
            *rc = reduce(&stack);
            if (*rc != NO_ERROR) return NULL;
        }
        else if(prec_table[stack_sym][current_sym] == '='){
            expr_Pstack_push_term(&stack, token, current_sym);
            get_token(token);
            if (*rc != NO_ERROR) return NULL;
            *rc = reduce(&stack);
            if (*rc != NO_ERROR) return NULL;
        }
        else if (prec_table[stack_sym][current_sym] == 'T')break;
        else {
            *rc = SYNTAX_ERROR;
            return NULL;
        }
        
    }while (token->type != TOKEN_EOF && token->type != TOKEN_EOL && token->type != TOKEN_COMMA );
    while (true){
        if (stack.top->type == SYM_NONTERM &&
        stack.top->next != NULL &&
        stack.top->next->sym == PS_DOLLAR) {
        break;
    }
        Sym stack_sym = token_to_sym(&stack.top->token);
        Sym current_sym = PS_DOLLAR; // End marker
        
        if (prec_table[stack_sym][current_sym] == '<'){
            // Should not happen
            *rc = SYNTAX_ERROR;
            return NULL;
        }
        else if (prec_table[stack_sym][current_sym] == '>'){
            *rc = reduce(&stack);
            if (*rc != NO_ERROR) return NULL;
        }
        else if(prec_table[stack_sym][current_sym] == '='){
            break; // Finished
        }
        
        else {
            *rc = SYNTAX_ERROR;
            return NULL;
        }
    }
    // Wrap final expression in AST node if we have one
    ExprNode* final_expr = expr_Pstack_top(&stack);
    if (final_expr) {
        ast_expr = create_ast_node(AST_EXPRESSION, NULL);
        ast_expr->expr = final_expr;
    }
    
    expr_Pstack_free(&stack);
    return ast_expr;
}
