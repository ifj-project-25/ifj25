#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "expr_ast.h"   // For ExprNode creation helpers

// Terminal colors
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

int tests_passed = 0;
int tests_total = 0;

void run_test(const char* test_name, int (*test_func)(void)) {
    printf(COLOR_BLUE "=== %s ===\n" COLOR_RESET, test_name);
    int result = test_func();
    tests_total++;

    if (result == NO_ERROR) {
        tests_passed++;
        printf(COLOR_GREEN "✓ PASSED\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "✗ FAILED (Expected %d, got %d)\n" COLOR_RESET, NO_ERROR, result);
    }
    printf("\n");
}

int test_program1_factorial_full() {
    printf("Testing Program 1: Full Factorial Program...\n");

    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_body = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_body;

    // Ifj.write("Zadejte cislo pro vypocet faktorialu\n")
    ASTNode* write_call1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    main_body->left = write_call1;
    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg1_val = create_ast_node(AST_EXPRESSION, NULL);
    arg1_val->expr = create_string_literal_node("Zadejte cislo pro vypocet faktorialu\n");
    arg1->right = arg1_val;
    arg1->left = NULL;
    write_call1->left = arg1;

    // var a
    ASTNode* var_a_decl = create_ast_node(AST_VAR_DECL, NULL);
    write_call1->right = var_a_decl;
    var_a_decl->left = create_ast_node(AST_IDENTIFIER, "a");

    // a = Ifj.read_num()
    ASTNode* assign_a = create_ast_node(AST_ASSIGN, NULL);
    var_a_decl->right = assign_a;

    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign_a->left = equals1;
    equals1->left = create_ast_node(AST_IDENTIFIER, "a");

    // Expression node with function call on left
    ASTNode* expr_read_num = create_ast_node(AST_EXPRESSION, NULL);
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "Ifj.read_num");
    expr_read_num->left = func_call;
    equals1->right = expr_read_num;

    // if (a != null)
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_a->right = if_stmt;
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_binary_op_node(OP_NEQ,
        create_identifier_node("a"),
        create_null_literal_node()
    );

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // inner if (a < 0)
    ASTNode* inner_if = create_ast_node(AST_IF, NULL);
    then_block->left = inner_if;
    ASTNode* inner_if_expr = create_ast_node(AST_EXPRESSION, NULL);
    inner_if->left = inner_if_expr;
    inner_if_expr->expr = create_binary_op_node(OP_LT,
        create_identifier_node("a"),
        create_num_literal_node(0)
    );

    ASTNode* inner_then = create_ast_node(AST_BLOCK, NULL);
    inner_if->right = inner_then;

    // Ifj.write("Faktorial ")
    ASTNode* write_neg1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    inner_then->left = write_neg1;
    ASTNode* arg_neg1 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_neg1_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_neg1_val->expr = create_string_literal_node("Faktorial ");
    arg_neg1->right = arg_neg1_val;
    write_neg1->left = arg_neg1;

    // Ifj.write(a)
    ASTNode* write_neg2 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg1->right = write_neg2;
    ASTNode* arg_neg2 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_neg2_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_neg2_val->expr = create_identifier_node("a");
    arg_neg2->right = arg_neg2_val;
    write_neg2->left = arg_neg2;

    // Ifj.write(" nelze spocitat\n")
    ASTNode* write_neg3 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg2->right = write_neg3;
    ASTNode* arg_neg3 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_neg3_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_neg3_val->expr = create_string_literal_node(" nelze spocitat\n");
    arg_neg3->right = arg_neg3_val;
    write_neg3->left = arg_neg3;

    // else branch (calculate factorial)
    ASTNode* inner_else = create_ast_node(AST_ELSE, NULL);
    then_block->right = inner_else;
    ASTNode* inner_else_block = create_ast_node(AST_BLOCK, NULL);
    inner_else->right = inner_else_block;

    // var vysl
    ASTNode* var_vysl = create_ast_node(AST_VAR_DECL, NULL);
    inner_else_block->left = var_vysl;
    var_vysl->left = create_ast_node(AST_IDENTIFIER, "vysl");

    // vysl = 1
    ASTNode* assign_vysl = create_ast_node(AST_ASSIGN, NULL);
    var_vysl->right = assign_vysl;
    ASTNode* equals_vysl = create_ast_node(AST_EQUALS, NULL);
    assign_vysl->left = equals_vysl;
    equals_vysl->left = create_ast_node(AST_IDENTIFIER, "vysl");

    ASTNode* expr_vysl = create_ast_node(AST_EXPRESSION, NULL);
    expr_vysl->expr = create_num_literal_node(1);
    equals_vysl->right = expr_vysl;

    // while (a > 0)
    ASTNode* while_stmt = create_ast_node(AST_WHILE, NULL);
    assign_vysl->right = while_stmt;
    ASTNode* while_expr = create_ast_node(AST_EXPRESSION, NULL);
    while_stmt->left = while_expr;
    while_expr->expr = create_binary_op_node(OP_GT,
        create_identifier_node("a"),
        create_num_literal_node(0)
    );

ASTNode* while_body = create_ast_node(AST_BLOCK, NULL);
while_stmt->right = while_body;

    // vysl = vysl * a
    ASTNode* assign_mul = create_ast_node(AST_ASSIGN, NULL);
    while_body->left = assign_mul;
    ASTNode* equals_mul = create_ast_node(AST_EQUALS, NULL);
    assign_mul->left = equals_mul;
    equals_mul->left = create_ast_node(AST_IDENTIFIER, "vysl");

    ASTNode* expr_mul = create_ast_node(AST_EXPRESSION, NULL);
    expr_mul->expr = create_binary_op_node(OP_MUL,
        create_identifier_node("vysl"),
        create_identifier_node("a")
    );
    equals_mul->right = expr_mul;

    // a = a - 1
    ASTNode* assign_dec = create_ast_node(AST_ASSIGN, NULL);
    assign_mul->right = assign_dec;
    ASTNode* equals_dec = create_ast_node(AST_EQUALS, NULL);
    assign_dec->left = equals_dec;
    equals_dec->left = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* expr_dec = create_ast_node(AST_EXPRESSION, NULL);
    expr_dec->expr = create_binary_op_node(OP_SUB,
        create_identifier_node("a"),
        create_num_literal_node(1)
    );
    equals_dec->right = expr_dec;

    // vysl = "Vysledek: " + vysl + "\n"
    ASTNode* assign_concat = create_ast_node(AST_ASSIGN, NULL);
    assign_dec->right = assign_concat;
    ASTNode* equals_concat = create_ast_node(AST_EQUALS, NULL);
    assign_concat->left = equals_concat;
    equals_concat->left = create_ast_node(AST_IDENTIFIER, "vysl");

    ASTNode* expr_concat = create_ast_node(AST_EXPRESSION, NULL);
    expr_concat->expr =
        create_binary_op_node(OP_ADD,
            create_binary_op_node(OP_ADD,
                create_string_literal_node("Vysledek: "),
                create_identifier_node("vysl")),
            create_string_literal_node("\n")
        );
    equals_concat->right = expr_concat;

    // Ifj.write(vysl)
    ASTNode* write_result = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    assign_concat->right = write_result;
    ASTNode* write_result_arg = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* write_result_val = create_ast_node(AST_EXPRESSION, NULL);
    write_result_val->expr = create_identifier_node("vysl");
    write_result_arg->right = write_result_val;
    write_result->left = write_result_arg;

    // Outer else (a == null)
    ASTNode* outer_else = create_ast_node(AST_ELSE, NULL);
    then_block->right = outer_else;
    ASTNode* outer_else_block = create_ast_node(AST_BLOCK, NULL);
    outer_else->right = outer_else_block;

    ASTNode* write_null = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    outer_else_block->left = write_null;
    ASTNode* arg_null = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_null_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_null_val->expr = create_string_literal_node("Faktorial pro null nelze spocitat\n");
    arg_null->right = arg_null_val;
    write_null->left = arg_null;

    main_body->right = NULL;

    int result = semantic_analyze(program);
    printf("Expected: %d, got: %d\n", NO_ERROR, result);

    free_ast_tree(program);
    return result;
}

int test_program3_basic_strings() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var str1
    ASTNode* var_str1 = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_str1;
    var_str1->left = create_ast_node(AST_IDENTIFIER, "str1");

    // str1 = "Toto je text v programu jazyka IFJ25"
    ASTNode* assign_str1 = create_ast_node(AST_ASSIGN, NULL);
    var_str1->right = assign_str1;

    ASTNode* equals_str1 = create_ast_node(AST_EQUALS, NULL);
    assign_str1->left = equals_str1;
    equals_str1->left = create_ast_node(AST_IDENTIFIER, "str1");
    
    ASTNode* expr_str1 = create_ast_node(AST_EXPRESSION, NULL);
    equals_str1->right = expr_str1;
    expr_str1->expr = create_string_literal_node("Toto je text v programu jazyka IFJ25");

    // var str2
    ASTNode* var_str2 = create_ast_node(AST_VAR_DECL, NULL);
    assign_str1->right = var_str2;
    var_str2->left = create_ast_node(AST_IDENTIFIER, "str2");

    // str2 = ", ktery jeste trochu obohatime"
    ASTNode* assign_str2 = create_ast_node(AST_ASSIGN, NULL);
    var_str2->right = assign_str2;

    ASTNode* equals_str2 = create_ast_node(AST_EQUALS, NULL);
    assign_str2->left = equals_str2;
    equals_str2->left = create_ast_node(AST_IDENTIFIER, "str2");
    
    ASTNode* expr_str2 = create_ast_node(AST_EXPRESSION, NULL);
    equals_str2->right = expr_str2;
    expr_str2->expr = create_string_literal_node(", ktery jeste trochu obohatime");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_string_operations() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var str1 = "Hello"
    ASTNode* var_str1 = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_str1;
    var_str1->left = create_ast_node(AST_IDENTIFIER, "str1");

    ASTNode* assign_str1 = create_ast_node(AST_ASSIGN, NULL);
    var_str1->right = assign_str1;

    ASTNode* equals_str1 = create_ast_node(AST_EQUALS, NULL);
    assign_str1->left = equals_str1;
    equals_str1->left = create_ast_node(AST_IDENTIFIER, "str1");
    
    ASTNode* expr_str1 = create_ast_node(AST_EXPRESSION, NULL);
    equals_str1->right = expr_str1;
    expr_str1->expr = create_string_literal_node("Hello");

    // var str2 = "World"
    ASTNode* var_str2 = create_ast_node(AST_VAR_DECL, NULL);
    assign_str1->right = var_str2;
    var_str2->left = create_ast_node(AST_IDENTIFIER, "str2");

    ASTNode* assign_str2 = create_ast_node(AST_ASSIGN, NULL);
    var_str2->right = assign_str2;

    ASTNode* equals_str2 = create_ast_node(AST_EQUALS, NULL);
    assign_str2->left = equals_str2;
    equals_str2->left = create_ast_node(AST_IDENTIFIER, "str2");
    
    ASTNode* expr_str2 = create_ast_node(AST_EXPRESSION, NULL);
    equals_str2->right = expr_str2;
    expr_str2->expr = create_string_literal_node("World");

    // str2 = str1 + (str2 * 3)
    ASTNode* assign_combined = create_ast_node(AST_ASSIGN, NULL);
    assign_str2->right = assign_combined;

    ASTNode* equals_combined = create_ast_node(AST_EQUALS, NULL);
    assign_combined->left = equals_combined;
    equals_combined->left = create_ast_node(AST_IDENTIFIER, "str2");
    
    ASTNode* expr_combined = create_ast_node(AST_EXPRESSION, NULL);
    equals_combined->right = expr_combined;
    
    // Vytvorenie výrazu: str1 + (str2 * 3)
    ExprNode* multiply_expr = create_binary_op_node(OP_MUL,
        create_identifier_node("str2"),
        create_num_literal_node(3.0));
    
    ExprNode* add_expr = create_binary_op_node(OP_ADD,
        create_identifier_node("str1"),
        multiply_expr);
    
    expr_combined->expr = add_expr;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_builtin_write() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // Ifj.write(str1)
    ASTNode* call_write1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    main_block->left = call_write1;

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    call_write1->left = arg1;
    
    ASTNode* expr_arg1 = create_ast_node(AST_EXPRESSION, NULL);
    arg1->right = expr_arg1;
    expr_arg1->expr = create_identifier_node("str1");

    // Ifj.write("\n")
    ASTNode* call_write2 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    call_write1->right = call_write2;

    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    call_write2->left = arg2;
    
    ASTNode* expr_arg2 = create_ast_node(AST_EXPRESSION, NULL);
    arg2->right = expr_arg2;
    expr_arg2->expr = create_string_literal_node("\n");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_read_and_null() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var input
    ASTNode* var_input = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_input;
    var_input->left = create_ast_node(AST_IDENTIFIER, "input");

    // input = Ifj.read_str()
    ASTNode* assign_input = create_ast_node(AST_ASSIGN, NULL);
    var_input->right = assign_input;

    ASTNode* equals_input = create_ast_node(AST_EQUALS, NULL);
    assign_input->left = equals_input;
    equals_input->left = create_ast_node(AST_IDENTIFIER, "input");
    
    ASTNode* expr_read = create_ast_node(AST_EXPRESSION, NULL);
    equals_input->right = expr_read;
    
    // Ifj.read_str() call
    ASTNode* call_read = create_ast_node(AST_FUNC_CALL, "Ifj.read_str");
    expr_read->left = call_read;

    // var all = ""
    ASTNode* var_all = create_ast_node(AST_VAR_DECL, NULL);
    assign_input->right = var_all;
    var_all->left = create_ast_node(AST_IDENTIFIER, "all");

    ASTNode* assign_all = create_ast_node(AST_ASSIGN, NULL);
    var_all->right = assign_all;

    ASTNode* equals_all = create_ast_node(AST_EQUALS, NULL);
    assign_all->left = equals_all;
    equals_all->left = create_ast_node(AST_IDENTIFIER, "all");
    
    ASTNode* expr_empty = create_ast_node(AST_EXPRESSION, NULL);
    equals_all->right = expr_empty;
    expr_empty->expr = create_string_literal_node("");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_while_and_strcmp() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // Declare and initialize input
    ASTNode* var_input = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_input;
    var_input->left = create_ast_node(AST_IDENTIFIER, "input");

    ASTNode* assign_input = create_ast_node(AST_ASSIGN, NULL);
    var_input->right = assign_input;

    ASTNode* equals_input = create_ast_node(AST_EQUALS, NULL);
    assign_input->left = equals_input;
    equals_input->left = create_ast_node(AST_IDENTIFIER, "input");
    
    ASTNode* expr_null = create_ast_node(AST_EXPRESSION, NULL);
    equals_input->right = expr_null;
    expr_null->expr = create_null_literal_node();

    // while (input != null) { }
    ASTNode* while_loop = create_ast_node(AST_WHILE, NULL);
    assign_input->right = while_loop;

    // Condition: input != null
    ASTNode* while_cond = create_ast_node(AST_EXPRESSION, NULL);
    while_loop->left = while_cond;
    while_cond->expr = create_binary_op_node(OP_NEQ,
        create_identifier_node("input"),
        create_null_literal_node());

    ASTNode* while_body = create_ast_node(AST_BLOCK, NULL);
    while_loop->right = while_body;

    // var strcmpResult
    ASTNode* var_strcmp = create_ast_node(AST_VAR_DECL, NULL);
    while_body->left = var_strcmp;
    var_strcmp->left = create_ast_node(AST_IDENTIFIER, "strcmpResult");

    // strcmpResult = Ifj.strcmp(input, "abcdefgh")
    ASTNode* assign_strcmp = create_ast_node(AST_ASSIGN, NULL);
    var_strcmp->right = assign_strcmp;

    ASTNode* equals_strcmp = create_ast_node(AST_EQUALS, NULL);
    assign_strcmp->left = equals_strcmp;
    equals_strcmp->left = create_ast_node(AST_IDENTIFIER, "strcmpResult");
    
    ASTNode* expr_strcmp = create_ast_node(AST_EXPRESSION, NULL);
    equals_strcmp->right = expr_strcmp;
    
    // Ifj.strcmp(input, "abcdefgh") call
    ASTNode* call_strcmp = create_ast_node(AST_FUNC_CALL, "Ifj.strcmp");
    expr_strcmp->left = call_strcmp;

    // First argument: input
    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    call_strcmp->left = arg1;
    
    ASTNode* expr_arg1 = create_ast_node(AST_EXPRESSION, NULL);
    arg1->right = expr_arg1;
    expr_arg1->expr = create_identifier_node("input");
    expr_arg1->expr->type = EXPR_STRING_LITERAL;

    // Second argument: "abcdefgh"
    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = arg2;
    
    ASTNode* expr_arg2 = create_ast_node(AST_EXPRESSION, NULL);
    arg2->right = expr_arg2;
    expr_arg2->expr = create_string_literal_node("abcdefgh");
    expr_arg2->expr->type = EXPR_STRING_LITERAL;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_if_else_branching() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // Declare variables
    ASTNode* var_strcmp = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_strcmp;
    var_strcmp->left = create_ast_node(AST_IDENTIFIER, "strcmpResult");

    ASTNode* assign_strcmp = create_ast_node(AST_ASSIGN, NULL);
    var_strcmp->right = assign_strcmp;

    ASTNode* equals_strcmp = create_ast_node(AST_EQUALS, NULL);
    assign_strcmp->left = equals_strcmp;
    equals_strcmp->left = create_ast_node(AST_IDENTIFIER, "strcmpResult");
    
    ASTNode* expr_zero = create_ast_node(AST_EXPRESSION, NULL);
    equals_strcmp->right = expr_zero;
    expr_zero->expr = create_num_literal_node(0.0);

    // if (strcmpResult == 0) { } else { }
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_strcmp->right = if_stmt;

    // Condition: strcmpResult == 0
    ASTNode* if_cond = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_cond;
    if_cond->expr = create_binary_op_node(OP_EQ,
        create_identifier_node("strcmpResult"),
        create_num_literal_node(0.0));

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // Ifj.write("Spravne zadano! Predchozi pokusy:\n")
    ASTNode* call_write1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    then_block->left = call_write1;

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    call_write1->left = arg1;
    
    ASTNode* expr_arg1 = create_ast_node(AST_EXPRESSION, NULL);
    arg1->right = expr_arg1;
    expr_arg1->expr = create_string_literal_node("Spravne zadano! Predchozi pokusy:\n");

    ASTNode* else_stmt = create_ast_node(AST_ELSE, NULL);
    then_block->right = else_stmt;

    ASTNode* else_block = create_ast_node(AST_BLOCK, NULL);
    else_stmt->right = else_block;

    // Ifj.write("Spatne zadana posloupnost, zkuste znovu:\n")
    ASTNode* call_write2 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    else_block->left = call_write2;

    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    call_write2->left = arg2;
    
    ASTNode* expr_arg2 = create_ast_node(AST_EXPRESSION, NULL);
    arg2->right = expr_arg2;
    expr_arg2->expr = create_string_literal_node("Spatne zadana posloupnost, zkuste znovu:\n");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

int test_program3_complete() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // TODO: Vytvoriť kompletnú AST štruktúru pre celý Program 3
    // Toto by bolo veľmi rozsiahle, takže vytvorím zjednodušenú verziu

    // var str1 = "Hello"
    ASTNode* var_str1 = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_str1;
    var_str1->left = create_ast_node(AST_IDENTIFIER, "str1");

    ASTNode* assign_str1 = create_ast_node(AST_ASSIGN, NULL);
    var_str1->right = assign_str1;

    ASTNode* equals_str1 = create_ast_node(AST_EQUALS, NULL);
    assign_str1->left = equals_str1;
    equals_str1->left = create_ast_node(AST_IDENTIFIER, "str1");
    
    ASTNode* expr_str1 = create_ast_node(AST_EXPRESSION, NULL);
    equals_str1->right = expr_str1;
    expr_str1->expr = create_string_literal_node("Hello");

    // Ifj.write(str1)
    ASTNode* call_write = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    assign_str1->right = call_write;

    ASTNode* arg = create_ast_node(AST_FUNC_ARG, NULL);
    call_write->left = arg;
    
    ASTNode* expr_arg = create_ast_node(AST_EXPRESSION, NULL);
    arg->right = expr_arg;
    expr_arg->expr = create_identifier_node("str1");

    // var input = Ifj.read_str()
    ASTNode* var_input = create_ast_node(AST_VAR_DECL, NULL);
    call_write->right = var_input;
    var_input->left = create_ast_node(AST_IDENTIFIER, "input");

    ASTNode* assign_input = create_ast_node(AST_ASSIGN, NULL);
    var_input->right = assign_input;

    ASTNode* equals_input = create_ast_node(AST_EQUALS, NULL);
    assign_input->left = equals_input;
    equals_input->left = create_ast_node(AST_IDENTIFIER, "input");
    
    ASTNode* expr_read = create_ast_node(AST_EXPRESSION, NULL);
    equals_input->right = expr_read;
    
    ASTNode* call_read = create_ast_node(AST_FUNC_CALL, "Ifj.read_str");
    expr_read->left = call_read;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

void print_summary() {
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
    printf(COLOR_YELLOW "           TEST SUMMARY\n" COLOR_RESET);
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
    int percentage = (tests_total > 0) ? (tests_passed * 100) / tests_total : 0;
    printf("Tests passed: " COLOR_GREEN "%d/%d\n" COLOR_RESET, tests_passed, tests_total);
    printf("Success rate: %d%%\n", percentage);
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
}

int main() {
    printf(COLOR_BLUE "🧪 Running semantic analysis tests...\n\n" COLOR_RESET);
    
    run_test("ex1-faktorial-iterativne", test_program1_factorial_full);
    
    // Program 3 tests
    run_test("Program3 - Basic strings", test_program3_basic_strings);
    run_test("Program3 - String operations", test_program3_string_operations);
    run_test("Program3 - Builtin write", test_program3_builtin_write);
    run_test("Program3 - Read and null", test_program3_read_and_null);
    run_test("Program3 - While and strcmp", test_program3_while_and_strcmp);
    run_test("Program3 - If-else branching", test_program3_if_else_branching);
    run_test("Program3 - Complete simplified", test_program3_complete);
    
    print_summary();
    return (tests_passed == tests_total) ? 0 : 1;
}