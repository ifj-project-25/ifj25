#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "expr_ast.h"   // <-- needed for ExprNode helpers

// Farby pre terminal
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
    // use expression wrapper with expr = string literal
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
    //equals1->right = create_ast_node(AST_FUNC_CALL, "Ifj.read_num"); 
    //docasny fix
    

    // if (a != null)
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_a->right = if_stmt;
    // use node->expr (ExprNode) for condition
    if_stmt->expr = create_binary_op_node(OP_NEQ,
        create_identifier_node("a"),
        create_null_literal_node()
    );

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // inner if (a < 0)
    ASTNode* inner_if = create_ast_node(AST_IF, NULL);
    then_block->left = inner_if;
    inner_if->expr = create_binary_op_node(OP_LT,
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
    arg_neg1->left = NULL;
    write_neg1->left = arg_neg1;

    // Ifj.write(a)
    ASTNode* write_neg2 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg1->right = write_neg2;
    ASTNode* arg_neg2 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_neg2_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_neg2_val->expr = create_identifier_node("a");
    arg_neg2->right = arg_neg2_val;
    arg_neg2->left = NULL;
    write_neg2->left = arg_neg2;

    // Ifj.write(" nelze spocitat\n")
    ASTNode* write_neg3 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg2->right = write_neg3;
    ASTNode* arg_neg3 = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* arg_neg3_val = create_ast_node(AST_EXPRESSION, NULL);
    arg_neg3_val->expr = create_string_literal_node(" nelze spocitat\n");
    arg_neg3->right = arg_neg3_val;
    arg_neg3->left = NULL;
    write_neg3->left = arg_neg3;
    write_neg3->right = NULL; // end of then block

    // else branch (calculate factorial)
    ASTNode* inner_else = create_ast_node(AST_ELSE, NULL);
    then_block->right = inner_else;
    ASTNode* inner_else_block = create_ast_node(AST_BLOCK, NULL);
    inner_else->right = inner_else_block;

    // var vysl
    ASTNode* var_vysl = create_ast_node(AST_VAR_DECL, NULL);
    inner_else_block->left = var_vysl;
    var_vysl->left = create_ast_node(AST_IDENTIFIER, "vysl");

    // vysl = 1   (equals_vysl->expr = num literal)
    ASTNode* assign_vysl = create_ast_node(AST_ASSIGN, NULL);
    var_vysl->right = assign_vysl;
    ASTNode* equals_vysl = create_ast_node(AST_EQUALS, NULL);
    assign_vysl->left = equals_vysl;
    equals_vysl->left = create_ast_node(AST_IDENTIFIER, "vysl");
    // use expr field for RHS literal
    equals_vysl->expr = create_num_literal_node(1);

    // while (a > 0)
    ASTNode* while_stmt = create_ast_node(AST_WHILE, NULL);
    assign_vysl->right = while_stmt;
    while_stmt->expr = create_binary_op_node(OP_GT,
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
    equals_mul->expr = create_binary_op_node(OP_MUL,
        create_identifier_node("vysl"),
        create_identifier_node("a")
    );

    // a = a - 1
    ASTNode* assign_dec = create_ast_node(AST_ASSIGN, NULL);
    assign_mul->right = assign_dec;
    ASTNode* equals_dec = create_ast_node(AST_EQUALS, NULL);
    assign_dec->left = equals_dec;
    equals_dec->left = create_ast_node(AST_IDENTIFIER, "a");
    equals_dec->expr = create_binary_op_node(OP_SUB,
        create_identifier_node("a"),
        create_num_literal_node(1)
    );

    // vysl = "Vysledek: " + vysl + "\n"
    ASTNode* assign_concat = create_ast_node(AST_ASSIGN, NULL);
    assign_dec->right = assign_concat;
    ASTNode* equals_concat = create_ast_node(AST_EQUALS, NULL);
    assign_concat->left = equals_concat;
    equals_concat->left = create_ast_node(AST_IDENTIFIER, "vysl");
    // nested expr (string + id) + string
    equals_concat->expr =
        create_binary_op_node(OP_ADD,
            create_binary_op_node(OP_ADD,
                create_string_literal_node("Vysledek: "),
                create_identifier_node("vysl")),
            create_string_literal_node("\n")
        );

    // Ifj.write(vysl)
    ASTNode* write_result = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    assign_concat->right = write_result;
    ASTNode* write_result_arg = create_ast_node(AST_FUNC_ARG, NULL);
    ASTNode* write_result_val = create_ast_node(AST_EXPRESSION, NULL);
    write_result_val->expr = create_identifier_node("vysl");
    write_result_arg->right = write_result_val;
    write_result_arg->left = NULL;
    write_result->left = write_result_arg;

    inner_else_block->right = NULL;

    // OUTER ELSE: a == null
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
    arg_null->left = NULL;
    write_null->left = arg_null;
    write_null->right = NULL;

    outer_else_block->right = NULL;
    main_body->right = NULL; // end of main body

    // Run semantic analysis
    int result = semantic_analyze(program);
    printf("Expected: %d, got: %d\n", NO_ERROR, result);

    free_ast_tree(program);
    return result;
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
    print_summary();
    return (tests_passed == tests_total) ? 0 : 1;
}
