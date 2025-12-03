/**
 * @file expr_parser.c
 * @author xmikusm00
 * @brief Implementation of operator-precedence based expression parser.
 * @details
 * Converts a linear token stream into an internal expression tree (`ExprNode`)
 * wrapped inside an `AST_EXPRESSION` node (or directly an `AST_FUNC_CALL` if
 * the first token is an identifier followed by '(' ). The parser handles:
 *  - Binary operators: + - * / < > <= >= == != is
 *  - Parenthesis grouping
 *  - Identifiers & global variables
 *  - Numeric, string and null literals
 *  - Type literals (Null, Num, String) for the `is` operator
 * Memory allocation failures set *rc = ERROR_INTERNAL; syntax issues set *rc =
 * SYNTAX_ERROR.
 *
 * Parsing strategy:
 *  1. Treat identifiers followed immediately by '(' as function call shorthand
 * and return an `AST_FUNC_CALL` node to be further processed by the
 * higher-level parser.
 *  2. Otherwise apply operator precedence parsing using a custom stack
 * (`ExprPstack`).
 *  3. Reduce according to rules TERM->E and E op E -> E until a single
 * expression remains.
 */
#include "expr_parser.h"
#include "ast.h"
#include "error.h"
#include "expr_ast.h"
#include "expr_stack.h"
#include "parser.h"
#include "scanner.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Maps a scanner token to a precedence stack symbol.
 * @param token Pointer to token to classify.
 * @return Corresponding `Sym` value used in the precedence table.
 * @details
 * Collapses literal-like tokens (identifiers, numbers, strings, type keywords)
 * into PS_TERM. Unrecognized or unsupported tokens default to PS_TERM;
 * structural/end markers map to PS_DOLLAR.
 */
static Sym token_to_sym(const Token *token) {
    switch (token->type) {
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
        switch (token->value.keyword) {
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

/**
 * @brief Reduces the top TERM symbol on the stack into an expression node.
 * @param stack Pointer to precedence stack (top must be SYM_TERM).
 * @param rc Pointer to error code (set on allocation failure).
 * @return Newly created `ExprNode` pointer, or NULL on error.
 * @details
 * Handles identifiers, literals (number, string, null) and type keywords mapped
 * for the `is` operator. On success replaces the TERM with a NONTERM containing
 * the created node.
 */
ExprNode *reduce_term_to_node(ExprPstack *stack, int *error_code) {
    ExprNode *node = NULL;
    switch (stack->top->type) {
    case SYM_TERM:
        switch (stack->top->token.type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_GLOBAL_VAR:
            node = create_identifier_node(stack->top->token.value.string->str);
            break;
        case TOKEN_INTEGER:
        case TOKEN_DOUBLE:
            node = create_num_literal_node(stack->top->token.value.integer);
            break;
        case TOKEN_STRING:
            node =
                create_string_literal_node(stack->top->token.value.string->str);
            break;

        case TOKEN_KEYWORD:
            if (stack->top->token.value.keyword == KEYWORD_NULL_L) {
                // Lowercase 'null' is a null literal value
                node = create_null_literal_node();
            } else if (stack->top->token.value.keyword == KEYWORD_NULL_C) {
                // Uppercase 'Null' is a type (for 'is' operator)
                node = create_type_node("Null");
            } else if (stack->top->token.value.keyword == KEYWORD_NUM) {
                node = create_type_node("Num");
            } else if (stack->top->token.value.keyword == KEYWORD_STRING) {
                node = create_type_node("String");
            }
            break;
        default:
            return NULL;
        }
        if (node) {
            expr_Pstack_pop(stack);
            int return_value = expr_Pstack_push_nonterm(stack, node);
            if (return_value != NO_ERROR) {
                *error_code = ERROR_INTERNAL;
                return NULL;
            }
        }
        break;

    default:
        return NULL;
    }
    return node;
}
/**
 * @brief Operator precedence and associativity table.
 * @details
 * Each cell encodes relation of stack symbol (row) to incoming symbol (column):
 *  '<' shift (push incoming)
 *  '>' reduce
 *  '=' shift then reduce grouping (parentheses)
 *  'T' terminate expression parsing (return control to parent parser to
 * validate context of outside logic of parser ) ' ' invalid / not applicable
 */
static const char prec_table[15][15] = {
    //         TERM   +   -   *   /   (   )   <   >  <=  >=  is  ==  !=   $
    /* Term */ {' ', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>',
                '>', '>'},
    /* + */
    {'<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* - */
    {'<', '>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* * */
    {'<', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* / */
    {'<', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* ( */
    {'<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '<', ' '},
    /* ) */
    {' ', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* < */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* > */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* <= */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* >= */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* is */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* == */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* != */
    {'<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>'},
    /* $ */
    {'<', '<', '<', '<', '<', '<', 'T', '<', '<', '<', '<', '<', '<', '<',
     ' '}};

/**
 * @brief Reduces pattern E op E into a single expression node.
 * @param stack Precedence stack (top  must be NONTERM, below operator TERM,
 * below NONTERM).
 * @param rc Pointer to error code (set on allocation failure).
 * @return NO_ERROR on success, SYNTAX_ERROR on structural mismatch or failure.
 * @details
 * Pops right expression, operator token, left expression and pushes a binary
 * operator NONTERM.
 */
int reduce_expr_op_expr(ExprPstack *stack) {
    if (expr_Pstack_is_empty(stack)) {
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_NONTERM)
        return SYNTAX_ERROR;
    // Extract right operand E
    ExprNode *right = stack->top->node;
    expr_Pstack_pop(stack);

    if (expr_Pstack_is_empty(stack)) {
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_TERM)
        return SYNTAX_ERROR;

    BinaryOpType op;
    switch (stack->top->token.type) {
    case TOKEN_PLUS:
        op = OP_ADD;
        break;
    case TOKEN_MINUS:
        op = OP_SUB;
        break;
    case TOKEN_MULTIPLY:
        op = OP_MUL;
        break;
    case TOKEN_DIVIDE:
        op = OP_DIV;
        break;
    case TOKEN_LESSER:
        op = OP_LT;
        break;
    case TOKEN_GREATER:
        op = OP_GT;
        break;
    case TOKEN_LESSER_EQUAL:
        op = OP_LTE;
        break;
    case TOKEN_GREATER_EQUAL:
        op = OP_GTE;
        break;
    case TOKEN_LOGIC_EQUAL:
        op = OP_EQ;
        break;
    case TOKEN_NEQUAL:
        op = OP_NEQ;
        break;
    case TOKEN_KEYWORD:
        if (stack->top->token.value.keyword == KEYWORD_IS) {
            op = OP_IS;
            break;
        } else {
            return SYNTAX_ERROR;
        }

    default:
        return SYNTAX_ERROR;
    }

    // Extract left operand E
    expr_Pstack_pop(stack);

    if (expr_Pstack_is_empty(stack)) {
        return SYNTAX_ERROR;
    }
    if (stack->top->type != SYM_NONTERM)
        return SYNTAX_ERROR;
    ExprNode *left = stack->top->node;
    // Extract operator
    expr_Pstack_pop(stack);

    // Create binary operation node
    ExprNode *new_node = create_binary_op_node(op, left, right);
    if (!new_node) {
        return ERROR_INTERNAL;
    }
    expr_Pstack_push_nonterm(stack, new_node);

    return NO_ERROR;
}

/**
 * @brief Performs one reduction step based on current stack top pattern.
 * @param stack Precedence stack.
 * @param rc Pointer to error code.
 * @return NO_ERROR if a reduction succeeded, SYNTAX_ERROR if pattern invalid.
 * @details
 * Reduction forms:
 *  1. ( E )  -> E
 *  2. TERM   -> E
 *  3. E op E -> E
 */
int reduce(ExprPstack *stack) {
    if (expr_Pstack_is_empty(stack)) {
        return SYNTAX_ERROR;
    }
    // Reduce ( E ) -> E
    if (stack->top->type == SYM_TERM &&
        stack->top->token.type == TOKEN_RPAREN && stack->top->next != NULL &&
        stack->top->next->type == SYM_NONTERM &&
        stack->top->next->next != NULL &&
        stack->top->next->next->type == SYM_TERM &&
        stack->top->next->next->token.type == TOKEN_LPAREN) {

        expr_Pstack_pop(stack); // Pop )

        if (expr_Pstack_is_empty(stack)) {
            return SYNTAX_ERROR;
        }
        if (stack->top->type != SYM_NONTERM) {
            return SYNTAX_ERROR;
        }
        ExprNode *node = stack->top->node;
        expr_Pstack_pop(stack); // Pop E

        if (expr_Pstack_is_empty(stack)) {
            return SYNTAX_ERROR;
        }
        if (stack->top->sym != PS_LPAREN) {
            return SYNTAX_ERROR;
        }
        expr_Pstack_pop(stack); // Pop (

        expr_Pstack_push_nonterm(stack, node); // Push E

    }
    // Reduce TERM -> E
    else if (stack->top->type == SYM_TERM) {
        if (stack->top->sym == PS_PLUS || stack->top->sym == PS_MINUS ||
            stack->top->sym == PS_MUL || stack->top->sym == PS_DIV ||
            stack->top->sym == PS_LT || stack->top->sym == PS_GT ||
            stack->top->sym == PS_LTE || stack->top->sym == PS_GTE ||
            stack->top->sym == PS_EQ || stack->top->sym == PS_NEQ ||
            stack->top->sym == PS_IS) {
            return SYNTAX_ERROR;
        }
        int *tmp_err = NO_ERROR;
        ExprNode *node = reduce_term_to_node(stack, tmp_err);
        if (tmp_err != NO_ERROR) {
            return ERROR_INTERNAL;
        }
        if (node == NULL) {
            return SYNTAX_ERROR;
        }
    }
    // Reduce E op E -> E
    else if (stack->top->type == SYM_NONTERM) {
        int rc = reduce_expr_op_expr(stack);
        if (rc != NO_ERROR) {
            return SYNTAX_ERROR;
        }
    } else {
        return SYNTAX_ERROR;
    }

    return NO_ERROR;
}

/**
 * @brief Entry point for precedence expression parsing.
 * @param token Pointer to current token; advanced past parsed expression.
 * @param rc Pointer to error code (NO_ERROR on success).
 * @return `AST_EXPRESSION` wrapping internal expression tree or `AST_FUNC_CALL`
 * shortcut.
 * @details
 * Drives the shift/reduce loop using `prec_table`. Stops on EOL, COMMA, EOF.
 * After completion, converts final `ExprNode` stack content into an AST node.
 */
ASTNode *main_precedence_parser(Token *token, int *rc) {
    ExprPstack stack;
    ASTNode *ast_expr = NULL;

    expr_Pstack_init(&stack);
    if (*rc != NO_ERROR) {
        expr_Pstack_free(&stack);
        return NULL;
    }
    // Check for function call shortcut: ID (
    if (token->type == TOKEN_IDENTIFIER) {
        Token id_token = *token;
        get_token(token);
        if (*rc != NO_ERROR) {
            expr_Pstack_free(&stack);
            return NULL;
        }
        // Function call shortcut detected
        if (token->type == TOKEN_LPAREN) {
            ASTNode *call_node =
                create_ast_node(AST_FUNC_CALL, id_token.value.string->str);
            if (call_node == NULL) {
                *rc = ERROR_INTERNAL;
                expr_Pstack_free(&stack);
                return NULL;
            }
            expr_Pstack_free(&stack);
            return call_node; // return AST_FUNC_CALL directly, handeled in
                              // parser - EXPRESSION
        }

        int return_value = expr_Pstack_push_term(&stack, &id_token, PS_TERM);
        if (return_value != NO_ERROR) {
            expr_Pstack_free(&stack);
            *rc = return_value;
            return NULL;
        }
    }
    do {
        ExprPstackNode *scan = stack.top;
        while (scan && scan->type == SYM_NONTERM) {
            scan = scan->next; // Skip the NONTERM
        }
        Sym stack_sym = scan ? token_to_sym(&scan->token) : PS_DOLLAR;
        Sym current_sym = token_to_sym(token);

        // Logic based on precedence table
        if (prec_table[stack_sym][current_sym] == '<') { // Shift
            int return_value =
                expr_Pstack_push_term(&stack, token, current_sym);
            if (return_value != NO_ERROR) {
                expr_Pstack_free(&stack);
                *rc = return_value;
                return NULL;
            }
            get_token(token);
            if (*rc != NO_ERROR)
                return NULL;

        } else if (prec_table[stack_sym][current_sym] == '>') { // Reduce
            *rc = reduce(&stack);
            if (*rc != NO_ERROR)
                return NULL;
        } else if (prec_table[stack_sym][current_sym] ==
                   '=') { // Shift then reduce
            int return_value =
                expr_Pstack_push_term(&stack, token, current_sym);
            if (return_value != NO_ERROR) {
                expr_Pstack_free(&stack);
                *rc = return_value;
                return NULL;
            }
            get_token(token);
            if (*rc != NO_ERROR)
                return NULL;
            *rc = reduce(&stack);
            if (*rc != NO_ERROR)
                return NULL;
        } else if (prec_table[stack_sym][current_sym] == 'T') // Terminate
            break;
        else {
            *rc = SYNTAX_ERROR;
            return NULL;
        }

        // Stop parsing on EOL, COMMA, EOF
    } while (token->type != TOKEN_EOF && token->type != TOKEN_EOL &&
             token->type != TOKEN_COMMA);
    // Final reductions
    while (true) { // Reduce until only single expression remains
        if (stack.top->type == SYM_NONTERM && stack.top->next != NULL &&
            stack.top->next->sym == PS_DOLLAR) {
            break;
        }
        Sym stack_sym = token_to_sym(&stack.top->token);
        Sym current_sym = PS_DOLLAR; // End marker

        if (prec_table[stack_sym][current_sym] == '<') {
            // Should not happen
            *rc = SYNTAX_ERROR;
            return NULL;
        } else if (prec_table[stack_sym][current_sym] == '>') {
            *rc = reduce(&stack);
            if (*rc != NO_ERROR)
                return NULL;
        } else if (prec_table[stack_sym][current_sym] == '=') {
            break; // Finished
        }

        else {
            *rc = SYNTAX_ERROR;
            return NULL;
        }
    }
    // Final expression node wrapper
    // wraps ExprNode into AST_EXPRESSION
    ExprNode *final_expr = expr_Pstack_top(&stack);
    if (final_expr) {
        ast_expr = create_ast_node(AST_EXPRESSION, NULL);
        ast_expr->expr = final_expr;
    }

    expr_Pstack_free(&stack);
    return ast_expr;
}
