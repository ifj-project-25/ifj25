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

// Test 1: Single variable declaration - should pass (NO_ERROR)
int test_single_variable() {
    printf("Testing single variable declaration...\n");

    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // Create simple AST: program -> var_decl -> identifier "x"
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_decl;
    
    ASTNode* identifier = create_ast_node(AST_IDENTIFIER, "x");
    var_decl->left = identifier;
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 2: Multiple different variables - should pass (NO_ERROR)
int test_multiple_different_variables() {
    printf("Testing multiple different variables...\n");

    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // First variable 'a'
    ASTNode* var_a = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_a;
    var_a->left = create_ast_node(AST_IDENTIFIER, "a");
    
    // Second variable 'b'
    ASTNode* var_b = create_ast_node(AST_VAR_DECL, NULL);
    var_a->right = var_b;
    var_b->left = create_ast_node(AST_IDENTIFIER, "b");
    
    // Third variable 'c'
    ASTNode* var_c = create_ast_node(AST_VAR_DECL, NULL);
    var_b->right = var_c;
    var_c->left = create_ast_node(AST_IDENTIFIER, "c");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 3: Multiple same variables - should fail (SEM_ERROR_REDEFINED)
int test_redeclaration_error() {
    printf("Testing variable redeclaration error...\n");
    
     ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // First declaration of 'x'
    ASTNode* var_x1 = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_x1;
    var_x1->left = create_ast_node(AST_IDENTIFIER, "x");
    
    // Second declaration of 'x' (redeclaration)
    ASTNode* var_x2 = create_ast_node(AST_VAR_DECL, NULL);
    var_x1->right = var_x2;
    var_x2->left = create_ast_node(AST_IDENTIFIER, "x");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    
    // Test passes if we get redeclaration error
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 4: Multiple variables in same block - should pass (NO_ERROR)
int test_multiple_vars_same_block() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // Create main function
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;
    
    // Create block for main body
    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;
    
    // First variable 'a' - INSIDE block
    ASTNode* var_a = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_a;
    var_a->left = create_ast_node(AST_IDENTIFIER, "a");
    
    // Second variable 'b' - INSIDE block (same scope)
    ASTNode* var_b = create_ast_node(AST_VAR_DECL, NULL);
    var_a->right = var_b;  // ← next statement INSIDE same block
    var_b->left = create_ast_node(AST_IDENTIFIER, "b");
    
    // Third variable 'c' - INSIDE block (same scope)
    ASTNode* var_c = create_ast_node(AST_VAR_DECL, NULL);
    var_b->right = var_c;  // ← next statement INSIDE same block
    var_c->left = create_ast_node(AST_IDENTIFIER, "c");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 5: Same variable name in different blocks - should pass (NO_ERROR)
int test_same_var_different_blocks() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // Create main function
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;
    
    // Create block for main body
    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;
    
    // Variable 'x' in main block
    ASTNode* var_x1 = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_x1;
    var_x1->left = create_ast_node(AST_IDENTIFIER, "x");
    
    // Inner block - AFTER main block
    ASTNode* inner_block = create_ast_node(AST_BLOCK, NULL);
    main_block->right = inner_block;  // ← AFTER main block
    
    // Variable 'x' in inner block (should be OK - different scope)
    ASTNode* var_x2 = create_ast_node(AST_VAR_DECL, NULL);
    inner_block->left = var_x2;
    var_x2->left = create_ast_node(AST_IDENTIFIER, "x");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 6: Same variable name in same block - should fail (SEM_ERROR_REDEFINED)
int test_same_var_same_block() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // Create main function
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;
    
    // Create block for main body
    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;
    
    // First declaration of 'y' - INSIDE block
    ASTNode* var_y1 = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_y1;
    var_y1->left = create_ast_node(AST_IDENTIFIER, "y");
    
    // Second declaration of 'y' in same block (should cause error)
    ASTNode* var_y2 = create_ast_node(AST_VAR_DECL, NULL);
    var_y1->right = var_y2;  // ← next statement INSIDE same block (same scope)
    var_y2->left = create_ast_node(AST_IDENTIFIER, "y");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    
    // Test passes if we get redeclaration error
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 7: Global variable and local variable with same name - should pass (NO_ERROR)
int test_global_local_same_name() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // Global variable 'z' - in global scope
    ASTNode* global_var = create_ast_node(AST_VAR_DECL, NULL);
    program->left = global_var;
    global_var->left = create_ast_node(AST_IDENTIFIER, "z");
    
    // Main function - AFTER global var
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    global_var->right = main_func;
    
    // Main block - INSIDE main function
    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;
    
    // Local variable 'z' in main (should be OK - different scope)
    ASTNode* local_var = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = local_var;  // ← INSIDE main block (local scope)
    local_var->left = create_ast_node(AST_IDENTIFIER, "z");
    
    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 8: Simple function definition - should pass (NO_ERROR)
int test_func_simple_definition() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    // Empty param list
    func->left = NULL;

    // Function body
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // NO_ERROR
}

// Test 9: Function redefinition with same parameters - should fail (SEM_ERROR_REDEFINED)
int test_func_redefinition_same_params() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // First foo()
    ASTNode* func1 = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func1;
    func1->right = create_ast_node(AST_BLOCK, NULL);

    // Second foo()
    ASTNode* func2 = create_ast_node(AST_FUNC_DEF, "foo");
    func1->right->right = func2;
    func2->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 10: Function with parameters - should fail (SEM_ERROR_REDEFINED)
int test_func_duplicate_param() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    // func foo(a, a)
    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg2->right = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->right = create_ast_node(AST_IDENTIFIER, "a");
    arg1->left = arg2;

    func->left = arg1;
    func->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 11: Function parameter shadowed by local variable - should fail (SEM_ERROR_REDEFINED)
int test_func_param_shadowed_by_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    ASTNode* param = create_ast_node(AST_FUNC_ARG, NULL);
    param->right = create_ast_node(AST_IDENTIFIER, "x");
    func->left = param;

    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    block->left = var_decl;
    var_decl->left = create_ast_node(AST_IDENTIFIER, "x");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 12: Main function redefinition - should fail (SEM_ERROR_REDEFINED)
int test_main_redefinition() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main1 = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main1;
    main1->right = create_ast_node(AST_BLOCK, NULL);

    ASTNode* main2 = create_ast_node(AST_MAIN_DEF, "main");
    main1->right->right = main2;
    main2->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 13: Function redefinition with same parameter count - should fail (SEM_ERROR_REDEFINED)
int test_func_and_builtin_conflict() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "Ifj.write");
    program->left = func;
    
    // Create one parameter to match built-in Ifj.write (1 parameter)
    ASTNode* arg = create_ast_node(AST_FUNC_ARG, NULL);
    func->left = arg;
    arg->right = create_ast_node(AST_IDENTIFIER, "term");
    
    func->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 14: Function parameter shadowed by variable in nested block - should pass (NO_ERROR)
int test_func_param_scope_shadowed_block_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    ASTNode* param = create_ast_node(AST_FUNC_ARG, NULL);
    param->right = create_ast_node(AST_IDENTIFIER, "a");
    func->left = param;

    ASTNode* outer_block = create_ast_node(AST_BLOCK, NULL);
    func->right = outer_block;

    ASTNode* inner_block = create_ast_node(AST_BLOCK, NULL);
    outer_block->left = inner_block;

    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    inner_block->left = var_decl;
    var_decl->left = create_ast_node(AST_IDENTIFIER, "a");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // NO_ERROR
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

    run_test("1. test-redeclaration", test_single_variable);
    run_test("2. test-multiple-different-variables", test_multiple_different_variables);
    run_test("3. test-redeclaration-error", test_redeclaration_error);
    run_test("4. test-multiple-vars-same-block",test_multiple_vars_same_block);
    run_test("5. test-same-var-different-blocks", test_same_var_different_blocks);
    run_test("6. test-same-var-same-block", test_same_var_same_block);
    run_test("7. test-global-local-same-name", test_global_local_same_name);
    run_test("8. test-func-simple-definition", test_func_simple_definition);
    run_test("9. test-func-redefinition-same-params", test_func_redefinition_same_params);
    run_test("10. test-func-duplicate-param", test_func_duplicate_param);
    run_test("11. test-func-param-shadowed-by-var", test_func_param_shadowed_by_var);
    run_test("12. test-main-redefinition", test_main_redefinition);
    run_test("13. test-func-and-builtin-conflict", test_func_and_builtin_conflict);
    run_test("14. test-func-param-scope-shadowed-block-var", test_func_param_scope_shadowed_block_var);

    
    print_summary();
    
    return (tests_passed == tests_total) ? 0 : 1;
}