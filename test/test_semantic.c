#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"

// Farby pre terminal
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

// Globálne premenné pre sledovanie výsledkov testov
int tests_passed = 0;
int tests_total = 0;

// Funkcia pre spustenie jedného testu
void run_test(const char* test_name, int (*test_func)(void)) {
    printf(COLOR_CYAN "=== %s ===\n" COLOR_RESET, test_name);
    
    int result = test_func();
    tests_total++;
    
    if (result == NO_ERROR) {
        tests_passed++;
        printf(COLOR_GREEN "✓ PASSED\n" COLOR_RESET);
    } else {
        printf(COLOR_RED "✗ FAILED (Expected: %d, Got: %d)\n" COLOR_RESET, NO_ERROR, result);
    }
    printf("\n");
}

int test_program1_factorial_full() {
    printf("Testing Program 1: Full Factorial Program...\n");
    
    // ===== ROOT: AST_PROGRAM =====
    // program->left = main function
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // ===== AST_MAIN_DEF =====
    // main->left = arguments (NULL), main->right = AST_BLOCK (body)
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    main_func->left = NULL; // no arguments
    program->left = main_func;
    
    // ===== MAIN BODY: AST_BLOCK =====
    // block->left = statements, block->right = next (NULL for main)
    ASTNode* main_body = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_body;
    
    // --- Statement 1: Ifj.write("Zadejte cislo pro vypocet faktorialu\n") ---
    // AST_FUNC_CALL->left = AST_FUNC_ARG, right = next statement
    ASTNode* write_call1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    main_body->left = write_call1;
    
    // Argument: string literal
    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = NULL; // end of args
    arg1->right = create_ast_node(AST_LITERAL_STRING, NULL);
    arg1->right->string_val = my_strdup("Zadejte cislo pro vypocet faktorialu\n");
    write_call1->left = arg1;
    
    // --- Statement 2: var a ---
    // AST_VAR_DECL->left = AST_IDENTIFIER, right = next statement
    ASTNode* var_a_decl = create_ast_node(AST_VAR_DECL, NULL);
    write_call1->right = var_a_decl;
    
    ASTNode* identifier_a = create_ast_node(AST_IDENTIFIER, "a");
    identifier_a->data_type = TYPE_UNDEF; // no explicit type
    var_a_decl->left = identifier_a;
    
    // --- Statement 3: a = Ifj.read_num() ---
    // AST_ASSIGN->left = AST_EQUALS, right = next statement
    ASTNode* assign_a = create_ast_node(AST_ASSIGN, NULL);
    var_a_decl->right = assign_a;
    
    // AST_EQUALS->left = identifier, right = expression
    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign_a->left = equals1;
    
    ASTNode* id_a_assign = create_ast_node(AST_IDENTIFIER, "a");
    equals1->left = id_a_assign;
    
    // Right side: Ifj.read_num() call
    ASTNode* read_num_call = create_ast_node(AST_FUNC_CALL, "Ifj.read_num");
    read_num_call->left = NULL; // no arguments
    equals1->right = read_num_call;
    
    // --- Statement 4: if (a != null) { ... } else { ... } ---
    // AST_IF->left = condition, right = AST_BLOCK (then branch)
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_a->right = if_stmt;
    
    // Condition: a != null (expression)
    ASTNode* condition = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = condition;
    
    ASTNode* op_neq = create_ast_node(AST_OP, "!=");
    condition->left = op_neq;
    
    op_neq->left = create_ast_node(AST_IDENTIFIER, "a");
    op_neq->right = create_ast_node(AST_LITERAL_NULL, NULL);
    
    // THEN BRANCH: AST_BLOCK
    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;
    
    // Inner if: if (a < 0)
    ASTNode* inner_if = create_ast_node(AST_IF, NULL);
    then_block->left = inner_if;
    
    // Condition: a < 0
    ASTNode* inner_cond = create_ast_node(AST_EXPRESSION, NULL);
    inner_if->left = inner_cond;
    
    ASTNode* op_lt = create_ast_node(AST_OP, "<");
    inner_cond->left = op_lt;
    
    op_lt->left = create_ast_node(AST_IDENTIFIER, "a");
    op_lt->right = create_ast_node(AST_LITERAL_INT, NULL);
    op_lt->right->int_val = 0;
    
    // Inner THEN: negative factorial message
    ASTNode* inner_then = create_ast_node(AST_BLOCK, NULL);
    inner_if->right = inner_then;
    
    // Ifj.write("Faktorial ")
    ASTNode* write_neg1 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    inner_then->left = write_neg1;
    
    ASTNode* arg_neg1 = create_ast_node(AST_FUNC_ARG, NULL);
    arg_neg1->left = NULL;
    arg_neg1->right = create_ast_node(AST_LITERAL_STRING, NULL);
    arg_neg1->right->string_val = my_strdup("Faktorial ");
    write_neg1->left = arg_neg1;
    
    // Ifj.write(a)
    ASTNode* write_neg2 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg1->right = write_neg2;
    
    ASTNode* arg_neg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg_neg2->left = NULL;
    arg_neg2->right = create_ast_node(AST_IDENTIFIER, "a");
    write_neg2->left = arg_neg2;
    
    // Ifj.write(" nelze spocitat\n")
    ASTNode* write_neg3 = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    write_neg2->right = write_neg3;
    
    ASTNode* arg_neg3 = create_ast_node(AST_FUNC_ARG, NULL);
    arg_neg3->left = NULL;
    arg_neg3->right = create_ast_node(AST_LITERAL_STRING, NULL);
    arg_neg3->right->string_val = my_strdup(" nelze spocitat\n");
    write_neg3->left = arg_neg3;
    write_neg3->right = NULL; // end of then block
    
    // Inner ELSE: calculate factorial
    ASTNode* inner_else = create_ast_node(AST_ELSE, NULL);
    then_block->right = inner_else;
    
    ASTNode* inner_else_block = create_ast_node(AST_BLOCK, NULL);
    inner_else->right = inner_else_block;
    
    // var vysl
    ASTNode* var_vysl = create_ast_node(AST_VAR_DECL, NULL);
    inner_else_block->left = var_vysl;
    
    ASTNode* id_vysl = create_ast_node(AST_IDENTIFIER, "vysl");
    id_vysl->data_type = TYPE_UNDEF;
    var_vysl->left = id_vysl;
    
    // vysl = 1
    ASTNode* assign_vysl = create_ast_node(AST_ASSIGN, NULL);
    var_vysl->right = assign_vysl;
    
    ASTNode* equals_vysl = create_ast_node(AST_EQUALS, NULL);
    assign_vysl->left = equals_vysl;
    
    equals_vysl->left = create_ast_node(AST_IDENTIFIER, "vysl");
    equals_vysl->right = create_ast_node(AST_LITERAL_INT, NULL);
    equals_vysl->right->int_val = 1;
    
    // while (a > 0)
    ASTNode* while_stmt = create_ast_node(AST_WHILE, NULL);
    assign_vysl->right = while_stmt;
    
    // Condition: a > 0
    ASTNode* while_cond = create_ast_node(AST_EXPRESSION, NULL);
    while_stmt->left = while_cond;
    
    ASTNode* op_gt = create_ast_node(AST_OP, ">");
    while_cond->left = op_gt;
    
    op_gt->left = create_ast_node(AST_IDENTIFIER, "a");
    op_gt->right = create_ast_node(AST_LITERAL_INT, NULL);
    op_gt->right->int_val = 0;
    
    // While body
    ASTNode* while_body = create_ast_node(AST_BLOCK, NULL);
    while_stmt->right = while_body;
    
    // vysl = vysl * a
    ASTNode* assign_mul = create_ast_node(AST_ASSIGN, NULL);
    while_body->left = assign_mul;
    
    ASTNode* equals_mul = create_ast_node(AST_EQUALS, NULL);
    assign_mul->left = equals_mul;
    
    equals_mul->left = create_ast_node(AST_IDENTIFIER, "vysl");
    
    ASTNode* expr_mul = create_ast_node(AST_EXPRESSION, NULL);
    equals_mul->right = expr_mul;
    
    ASTNode* op_mul = create_ast_node(AST_OP, "*");
    expr_mul->left = op_mul;
    
    op_mul->left = create_ast_node(AST_IDENTIFIER, "vysl");
    op_mul->right = create_ast_node(AST_IDENTIFIER, "a");
    
    // a = a - 1
    ASTNode* assign_dec = create_ast_node(AST_ASSIGN, NULL);
    assign_mul->right = assign_dec;
    
    ASTNode* equals_dec = create_ast_node(AST_EQUALS, NULL);
    assign_dec->left = equals_dec;
    
    equals_dec->left = create_ast_node(AST_IDENTIFIER, "a");
    
    ASTNode* expr_sub = create_ast_node(AST_EXPRESSION, NULL);
    equals_dec->right = expr_sub;
    
    ASTNode* op_sub = create_ast_node(AST_OP, "-");
    expr_sub->left = op_sub;
    
    op_sub->left = create_ast_node(AST_IDENTIFIER, "a");
    op_sub->right = create_ast_node(AST_LITERAL_INT, NULL);
    op_sub->right->int_val = 1;
    
    assign_dec->right = NULL; // end of while body
    
    // vysl = Ifj.floor(vysl)
    ASTNode* assign_floor = create_ast_node(AST_ASSIGN, NULL);
    while_body->right = assign_floor;
    
    ASTNode* equals_floor = create_ast_node(AST_EQUALS, NULL);
    assign_floor->left = equals_floor;
    
    equals_floor->left = create_ast_node(AST_IDENTIFIER, "vysl");
    
    ASTNode* floor_call = create_ast_node(AST_FUNC_CALL, "Ifj.floor");
    equals_floor->right = floor_call;
    
    ASTNode* floor_arg = create_ast_node(AST_FUNC_ARG, NULL);
    floor_arg->left = NULL;
    floor_arg->right = create_ast_node(AST_IDENTIFIER, "vysl");
    floor_call->left = floor_arg;
    
    // vysl = Ifj.str(vysl)
    ASTNode* assign_str = create_ast_node(AST_ASSIGN, NULL);
    assign_floor->right = assign_str;
    
    ASTNode* equals_str = create_ast_node(AST_EQUALS, NULL);
    assign_str->left = equals_str;
    
    equals_str->left = create_ast_node(AST_IDENTIFIER, "vysl");
    
    ASTNode* str_call = create_ast_node(AST_FUNC_CALL, "Ifj.str");
    equals_str->right = str_call;
    
    ASTNode* str_arg = create_ast_node(AST_FUNC_ARG, NULL);
    str_arg->left = NULL;
    str_arg->right = create_ast_node(AST_IDENTIFIER, "vysl");
    str_call->left = str_arg;
    
    // vysl = "Vysledek: " + vysl + "\n"
    ASTNode* assign_concat = create_ast_node(AST_ASSIGN, NULL);
    assign_str->right = assign_concat;
    
    ASTNode* equals_concat = create_ast_node(AST_EQUALS, NULL);
    assign_concat->left = equals_concat;
    
    equals_concat->left = create_ast_node(AST_IDENTIFIER, "vysl");
    
    ASTNode* expr_concat = create_ast_node(AST_EXPRESSION, NULL);
    equals_concat->right = expr_concat;
    
    // Outer +: ("Vysledek: " + vysl) + "\n"
    ASTNode* op_add_outer = create_ast_node(AST_OP, "+");
    expr_concat->left = op_add_outer;
    
    // Inner +: "Vysledek: " + vysl
    ASTNode* expr_concat_inner = create_ast_node(AST_EXPRESSION, NULL);
    op_add_outer->left = expr_concat_inner;
    
    ASTNode* op_add_inner = create_ast_node(AST_OP, "+");
    expr_concat_inner->left = op_add_inner;
    
    op_add_inner->left = create_ast_node(AST_LITERAL_STRING, NULL);
    op_add_inner->left->string_val = my_strdup("Vysledek: ");
    op_add_inner->right = create_ast_node(AST_IDENTIFIER, "vysl");
    
    op_add_outer->right = create_ast_node(AST_LITERAL_STRING, NULL);
    op_add_outer->right->string_val = my_strdup("\n");
    
    // Ifj.write(vysl)
    ASTNode* write_result = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    assign_concat->right = write_result;
    
    ASTNode* write_result_arg = create_ast_node(AST_FUNC_ARG, NULL);
    write_result_arg->left = NULL;
    write_result_arg->right = create_ast_node(AST_IDENTIFIER, "vysl");
    write_result->left = write_result_arg;
    write_result->right = NULL; // end of else block
    
    inner_else_block->right = NULL;
    
    // OUTER ELSE: a == null
    ASTNode* outer_else = create_ast_node(AST_ELSE, NULL);
    then_block->right = outer_else;
    
    ASTNode* outer_else_block = create_ast_node(AST_BLOCK, NULL);
    outer_else->right = outer_else_block;
    
    // Ifj.write("Faktorial pro null nelze spocitat\n")
    ASTNode* write_null = create_ast_node(AST_FUNC_CALL, "Ifj.write");
    outer_else_block->left = write_null;
    
    ASTNode* arg_null = create_ast_node(AST_FUNC_ARG, NULL);
    arg_null->left = NULL;
    arg_null->right = create_ast_node(AST_LITERAL_STRING, NULL);
    arg_null->right->string_val = my_strdup("Faktorial pro null nelze spocitat\n");
    write_null->left = arg_null;
    write_null->right = NULL;
    
    outer_else_block->right = NULL;
    main_body->right = NULL; // end of main body
    
    // ===== RUN SEMANTIC ANALYSIS =====
    int result = semantic_analyze(program);
    printf("Expected: NO_ERROR (%d), Got: %d\n", NO_ERROR, result);
    
    free_ast_tree(program);
    return result;
}


void print_summary() {
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
    printf(COLOR_YELLOW "           TEST SUMMARY\n" COLOR_RESET);
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
    
    



    int percentage = (tests_total > 0) ? (tests_passed * 100) / tests_total : 0;
    
    printf("Tests passed: " COLOR_GREEN "%d/%d\n" COLOR_RESET, tests_passed, tests_total);
    printf("Success rate: ");
    
    if (percentage == 100) {
        printf(COLOR_GREEN "%d%% ✅\n" COLOR_RESET, percentage);
    } else if (percentage >= 70) {
        printf(COLOR_YELLOW "%d%% ⚠️\n" COLOR_RESET, percentage);
    } else {
        printf(COLOR_RED "%d%% ❌\n" COLOR_RESET, percentage);
    }
    
    printf(COLOR_YELLOW "========================================\n" COLOR_RESET);
}

int main() {
    printf(COLOR_BLUE "🧪 Running semantic analysis tests...\n\n" COLOR_RESET);
    
    run_test("ex1-faktorial-iterativne", test_program1_factorial_full);

    // Výpis sumáru
    print_summary();
    
    return (tests_passed == tests_total) ? 0 : 1;
}