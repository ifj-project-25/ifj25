#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "expr_ast.h"

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

// Test 15: Simple getter definition - should pass (NO_ERROR)
int test_simple_getter() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* getter = create_ast_node(AST_GETTER_DEF, "value");
    program->left = getter;

    // Getter body
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    getter->right = block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 16: Simple setter definition - should pass (NO_ERROR)
int test_simple_setter() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* setter = create_ast_node(AST_SETTER_DEF, "value");
    program->left = setter;

    // Setter parameter
    ASTNode* param = create_ast_node(AST_IDENTIFIER, "newValue");
    setter->left = param;

    // Setter body
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    setter->right = block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 17: Getter and setter with same name - should pass (NO_ERROR)
int test_getter_setter_same_name() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Getter
    ASTNode* getter = create_ast_node(AST_GETTER_DEF, "data");
    program->left = getter;
    getter->right = create_ast_node(AST_BLOCK, NULL);

    // Setter with same name
    ASTNode* setter = create_ast_node(AST_SETTER_DEF, "data");
    getter->right = setter;
    
    ASTNode* param = create_ast_node(AST_IDENTIFIER, "val");
    setter->left = param;
    setter->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 18: Getter redefinition - should fail (SEM_ERROR_REDEFINED)
int test_getter_redefinition() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // First getter
    ASTNode* getter1 = create_ast_node(AST_GETTER_DEF, "count");
    program->left = getter1;
    getter1->right = create_ast_node(AST_BLOCK, NULL);

    // Second getter with same name
    ASTNode* getter2 = create_ast_node(AST_GETTER_DEF, "count");
    getter1->right = getter2;
    getter2->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 19: Setter redefinition - should fail (SEM_ERROR_REDEFINED)
int test_setter_redefinition() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // First setter
    ASTNode* setter1 = create_ast_node(AST_SETTER_DEF, "name");
    program->left = setter1;
    
    ASTNode* param1 = create_ast_node(AST_IDENTIFIER, "n");
    setter1->left = param1;
    setter1->right = create_ast_node(AST_BLOCK, NULL);

    // Second setter with same name
    ASTNode* setter2 = create_ast_node(AST_SETTER_DEF, "name");
    setter1->right = setter2;
    
    ASTNode* param2 = create_ast_node(AST_IDENTIFIER, "nameVal");
    setter2->left = param2;
    setter2->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

// Test 20: Getter/Setter with regular function same name - should pass (NO_ERROR)
int test_getter_setter_function_same_name() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Regular function
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "calculate");
    program->left = func;
    func->right = create_ast_node(AST_BLOCK, NULL);

    // Getter with same name
    ASTNode* getter = create_ast_node(AST_GETTER_DEF, "calculate");
    func->right = getter;
    getter->right = create_ast_node(AST_BLOCK, NULL);

    // Setter with same name
    ASTNode* setter = create_ast_node(AST_SETTER_DEF, "calculate");
    getter->right = setter;
    
    ASTNode* param = create_ast_node(AST_IDENTIFIER, "calcValue");
    setter->left = param;
    setter->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 21: Complex program with multiple getters/setters/functions
int test_complex_getter_setter_program() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Global variable
    ASTNode* global_var = create_ast_node(AST_VAR_DECL, NULL);
    program->left = global_var;
    global_var->left = create_ast_node(AST_IDENTIFIER, "globalCounter");

    // Getter for counter
    ASTNode* getter = create_ast_node(AST_GETTER_DEF, "counter");
    global_var->right = getter;
    getter->right = create_ast_node(AST_BLOCK, NULL);

    // Setter for counter
    ASTNode* setter = create_ast_node(AST_SETTER_DEF, "counter");
    getter->right = setter;
    
    ASTNode* param = create_ast_node(AST_IDENTIFIER, "newCount");
    setter->left = param;
    setter->right = create_ast_node(AST_BLOCK, NULL);

    // Regular function with same name (overloaded)
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "counter");
    setter->right = func;
    
    // Function with 2 parameters
    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    func->left = arg1;
    arg1->right = create_ast_node(AST_IDENTIFIER, "a");
    
    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = arg2;
    arg2->right = create_ast_node(AST_IDENTIFIER, "b");
    
    func->right = create_ast_node(AST_BLOCK, NULL);

    // Main function
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    func->right = main_func;
    main_func->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 22: Getter in inner scope - should pass (NO_ERROR)
int test_getter_inner_scope() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Main function
    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    // Main block
    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // Getter inside main block
    ASTNode* getter = create_ast_node(AST_GETTER_DEF, "localValue");
    main_block->left = getter;
    getter->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result; // Should return NO_ERROR
}

// Test 23: Simple function call (should pass) - UPRAVENÝ
int test_func_call_simple() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define function "foo"
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;
    func_def->left = NULL;
    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Function call: foo()
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;
    func_call->left = NULL;
    func_call->right = NULL;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 24: Call undefined function (should fail) - UPRAVENÝ
int test_func_call_undefined() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "bar");
    program->left = func_call;
    func_call->left = NULL;
    func_call->right = NULL;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_UNDEFINED) ? NO_ERROR : result;
}

// Test 25: Function call with wrong number of arguments (should fail) - UPRAVENÝ
int test_func_call_wrong_arg_count() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo(a, b)
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    func_def->left = arg1;
    arg1->right = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = arg2;
    arg2->right = create_ast_node(AST_IDENTIFIER, "b");

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Function call foo(1) - only one argument instead of two
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;
    func_call->right = NULL;

    ASTNode* call_arg = create_ast_node(AST_FUNC_ARG, NULL);
    func_call->left = call_arg;
    
    // Použijeme nový systém s ExprNode pre argument
    call_arg->expr = create_num_literal_node(1.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_WRONG_PARAMS) ? NO_ERROR : result;
}

// Test 26: Function call with correct argument types (should pass) - UPRAVENÝ
int test_func_call_correct_arg_types() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo(num: Num, s: String)
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    func_def->left = arg1;
    ASTNode* id1 = create_ast_node(AST_IDENTIFIER, "num");
    id1->data_type = TYPE_NUM;
    arg1->right = id1;

    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = arg2;
    ASTNode* id2 = create_ast_node(AST_IDENTIFIER, "s");
    id2->data_type = TYPE_STRING;
    arg2->right = id2;

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Call foo(5, "hello") - nový systém
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;

    ASTNode* arg_call1 = create_ast_node(AST_FUNC_ARG, NULL);
    func_call->left = arg_call1;
    arg_call1->expr = create_num_literal_node(5.0);

    ASTNode* arg_call2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg_call1->left = arg_call2;
    arg_call2->expr = create_string_literal_node("hello");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 27: Function call with wrong argument types (should fail) - UPRAVENÝ
int test_func_call_wrong_arg_types() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo(num: Num, s: String)
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;

    ASTNode* arg1 = create_ast_node(AST_FUNC_ARG, NULL);
    func_def->left = arg1;
    ASTNode* id1 = create_ast_node(AST_IDENTIFIER, "num");
    id1->data_type = TYPE_NUM;
    arg1->right = id1;

    ASTNode* arg2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg1->left = arg2;
    ASTNode* id2 = create_ast_node(AST_IDENTIFIER, "s");
    id2->data_type = TYPE_STRING;
    arg2->right = id2;

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Call foo("oops", 123) - wrong types, nový systém
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;

    ASTNode* arg_call1 = create_ast_node(AST_FUNC_ARG, NULL);
    func_call->left = arg_call1;
    arg_call1->expr = create_string_literal_node("oops"); // Should be num

    ASTNode* arg_call2 = create_ast_node(AST_FUNC_ARG, NULL);
    arg_call1->left = arg_call2;
    arg_call2->expr = create_num_literal_node(123.0); // Should be string

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_TYPE_COMPATIBILITY) ? NO_ERROR : result;
}

// Test 28: Chained calls (foo() -> bar()) - UPRAVENÝ
int test_func_call_chained() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo()
    ASTNode* func_foo = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_foo;
    func_foo->left = NULL;
    func_foo->right = create_ast_node(AST_BLOCK, NULL);

    // Define bar()
    ASTNode* func_bar = create_ast_node(AST_FUNC_DEF, "bar");
    func_foo->right->right = func_bar;
    func_bar->left = NULL;
    func_bar->right = create_ast_node(AST_BLOCK, NULL);

    // foo(); bar();
    ASTNode* call_foo = create_ast_node(AST_FUNC_CALL, "foo");
    func_bar->right->right = call_foo;
    call_foo->left = NULL;

    ASTNode* call_bar = create_ast_node(AST_FUNC_CALL, "bar");
    call_foo->right = call_bar;
    call_bar->left = NULL;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 29: Simple assignment - should pass (NO_ERROR)
int test_simple_assignment() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var x
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_decl;
    var_decl->left = create_ast_node(AST_IDENTIFIER, "x");

    // x = 5
    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    var_decl->right = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "x");
    
    // Nový systém: použijeme ExprNode
    equals->expr = create_num_literal_node(5.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 30: Assignment to undefined variable - should fail (SEM_ERROR_UNDEFINED)
int test_assignment_undefined_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // x = 10 (but x is not declared)
    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    program->left = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "x");
    equals->expr = create_num_literal_node(10.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_UNDEFINED) ? NO_ERROR : result;
}

// Test 31: Multiple assignments - should pass (NO_ERROR)
int test_multiple_assignments() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var a, var b
    ASTNode* var_a = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_a;
    var_a->left = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* var_b = create_ast_node(AST_VAR_DECL, NULL);
    var_a->right = var_b;
    var_b->left = create_ast_node(AST_IDENTIFIER, "b");

    // a = 1
    ASTNode* assign1 = create_ast_node(AST_ASSIGN, NULL);
    var_b->right = assign1;

    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign1->left = equals1;
    equals1->left = create_ast_node(AST_IDENTIFIER, "a");
    equals1->expr = create_num_literal_node(1.0);

    // b = a (use previously assigned variable)
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    assign1->right = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "b");
    equals2->expr = create_identifier_node("a");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 32: Use uninitialized variable - should fail (SEM_ERROR_OTHER)
int test_use_uninitialized_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var x (declared but not initialized)
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_decl;
    var_decl->left = create_ast_node(AST_IDENTIFIER, "x");

    // y = x (use uninitialized x)
    ASTNode* var_y = create_ast_node(AST_VAR_DECL, NULL);
    var_decl->right = var_y;
    var_y->left = create_ast_node(AST_IDENTIFIER, "y");

    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    var_y->right = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "y");
    equals->expr = create_identifier_node("x"); // uninitialized!

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_OTHER) ? NO_ERROR : result;
}

// Test 33: Assignment in different scopes - should pass (NO_ERROR)
int test_assignment_different_scopes() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var x in main
    ASTNode* var_x = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_x;
    var_x->left = create_ast_node(AST_IDENTIFIER, "x");

    // x = 10 in main
    ASTNode* assign1 = create_ast_node(AST_ASSIGN, NULL);
    var_x->right = assign1;

    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign1->left = equals1;
    equals1->left = create_ast_node(AST_IDENTIFIER, "x");
    equals1->expr = create_num_literal_node(10.0);

    // Inner block
    ASTNode* inner_block = create_ast_node(AST_BLOCK, NULL);
    assign1->right = inner_block;

    // var x in inner block (different variable)
    ASTNode* var_x_inner = create_ast_node(AST_VAR_DECL, NULL);
    inner_block->left = var_x_inner;
    var_x_inner->left = create_ast_node(AST_IDENTIFIER, "x");

    // x = 20 in inner block
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    var_x_inner->right = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "x");
    equals2->expr = create_num_literal_node(20.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 34: Chained assignments with expressions - should pass (NO_ERROR)
int test_chained_assignments() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var a, b, c
    ASTNode* var_a = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_a;
    var_a->left = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* var_b = create_ast_node(AST_VAR_DECL, NULL);
    var_a->right = var_b;
    var_b->left = create_ast_node(AST_IDENTIFIER, "b");

    ASTNode* var_c = create_ast_node(AST_VAR_DECL, NULL);
    var_b->right = var_c;
    var_c->left = create_ast_node(AST_IDENTIFIER, "c");

    // a = 1
    ASTNode* assign1 = create_ast_node(AST_ASSIGN, NULL);
    var_c->right = assign1;

    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign1->left = equals1;
    equals1->left = create_ast_node(AST_IDENTIFIER, "a");
    equals1->expr = create_num_literal_node(1.0);

    // b = a + 2
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    assign1->right = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "b");
    
    // Vytvoríme expression: a + 2
    ExprNode* add_expr = create_binary_op_node(OP_ADD,
        create_identifier_node("a"),
        create_num_literal_node(2.0));
    equals2->expr = add_expr;

    // c = b * 3
    ASTNode* assign3 = create_ast_node(AST_ASSIGN, NULL);
    assign2->right = assign3;

    ASTNode* equals3 = create_ast_node(AST_EQUALS, NULL);
    assign3->left = equals3;
    equals3->left = create_ast_node(AST_IDENTIFIER, "c");
    
    ExprNode* mul_expr = create_binary_op_node(OP_MUL,
        create_identifier_node("b"),
        create_num_literal_node(3.0));
    equals3->expr = mul_expr;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 35: Assignment to function parameter - should pass (NO_ERROR)
int test_assignment_to_parameter() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function foo(x)
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    ASTNode* param = create_ast_node(AST_FUNC_ARG, NULL);
    func->left = param;
    param->right = create_ast_node(AST_IDENTIFIER, "x");

    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // x = 5 (assignment to parameter)
    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    block->left = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "x");
    equals->expr = create_num_literal_node(5.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 36: Complex nested assignments - should pass (NO_ERROR)
int test_complex_nested_assignments() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var result
    ASTNode* var_result = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_result;
    var_result->left = create_ast_node(AST_IDENTIFIER, "result");

    // result = 0
    ASTNode* assign1 = create_ast_node(AST_ASSIGN, NULL);
    var_result->right = assign1;

    ASTNode* equals1 = create_ast_node(AST_EQUALS, NULL);
    assign1->left = equals1;
    equals1->left = create_ast_node(AST_IDENTIFIER, "result");
    equals1->expr = create_num_literal_node(0.0);

    // if block with assignment inside
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign1->right = if_stmt;
    
    // condition: true (using literal 1)
    if_stmt->expr = create_num_literal_node(1.0);

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // result = 42 inside if
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    then_block->left = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "result");
    equals2->expr = create_num_literal_node(42.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 37: String assignment - should pass (NO_ERROR)
int test_string_assignment() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var text
    ASTNode* var_text = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_text;
    var_text->left = create_ast_node(AST_IDENTIFIER, "text");

    // text = "hello"
    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    var_text->right = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "text");
    equals->expr = create_string_literal_node("hello");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 38: Null assignment - should pass (NO_ERROR)
int test_null_assignment() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // var data
    ASTNode* var_data = create_ast_node(AST_VAR_DECL, NULL);
    program->left = var_data;
    var_data->left = create_ast_node(AST_IDENTIFIER, "data");

    // data = null
    ASTNode* assign = create_ast_node(AST_ASSIGN, NULL);
    var_data->right = assign;

    ASTNode* equals = create_ast_node(AST_EQUALS, NULL);
    assign->left = equals;
    equals->left = create_ast_node(AST_IDENTIFIER, "data");
    equals->expr = create_null_literal_node();

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 39: Simple if statement - should pass (NO_ERROR)
int test_simple_if() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // if (1) { }
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    main_block->left = if_stmt;
    if_stmt->expr = create_num_literal_node(1.0); // true condition

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 40: If with else - should pass (NO_ERROR)
int test_if_else() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // if (x > 0) { } else { }
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    main_block->left = if_stmt;
    
    // Condition: x > 0
    if_stmt->expr = create_binary_op_node(OP_GT,
        create_identifier_node("x"),
        create_num_literal_node(0.0));

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    ASTNode* else_stmt = create_ast_node(AST_ELSE, NULL);
    then_block->right = else_stmt;

    ASTNode* else_block = create_ast_node(AST_BLOCK, NULL);
    else_stmt->right = else_block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 41: If with non-numeric condition - should fail (SEM_ERROR_TYPE_COMPATIBILITY)
int test_if_non_numeric_condition() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // if ("string") { } - invalid condition
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    main_block->left = if_stmt;
    if_stmt->expr = create_string_literal_node("invalid condition");

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return (result == SEM_ERROR_TYPE_COMPATIBILITY) ? NO_ERROR : result;
}

// Test 42: Return with value - should pass (NO_ERROR)
int test_return_with_value() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function foo()
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;
    func->left = NULL;
    
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // return 42;
    ASTNode* return_stmt = create_ast_node(AST_RETURN, NULL);
    block->left = return_stmt;
    return_stmt->expr = create_num_literal_node(42.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 43: Void return (no value) - should pass (NO_ERROR)
int test_void_return() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function foo()
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;
    func->left = NULL;
    
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // return; (no value)
    ASTNode* return_stmt = create_ast_node(AST_RETURN, NULL);
    block->left = return_stmt;
    // No expr set - void return

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 44: Return with expression - should pass (NO_ERROR)
int test_return_with_expression() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function calculate()
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "calculate");
    program->left = func;
    func->left = NULL;
    
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // return (10 + 5) * 2;
    ASTNode* return_stmt = create_ast_node(AST_RETURN, NULL);
    block->left = return_stmt;
    
    ExprNode* complex_expr = create_binary_op_node(OP_MUL,
        create_binary_op_node(OP_ADD,
            create_num_literal_node(10.0),
            create_num_literal_node(5.0)),
        create_num_literal_node(2.0));
    return_stmt->expr = complex_expr;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 45: Complex if-else with returns - should pass (NO_ERROR)
int test_complex_if_else_returns() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function max(a, b)
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "max");
    program->left = func;

    // Parameters: a, b
    ASTNode* param_a = create_ast_node(AST_FUNC_ARG, NULL);
    func->left = param_a;
    param_a->right = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* param_b = create_ast_node(AST_FUNC_ARG, NULL);
    param_a->left = param_b;
    param_b->right = create_ast_node(AST_IDENTIFIER, "b");

    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // if (a > b) { return a; } else { return b; }
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    block->left = if_stmt;
    
    // Condition: a > b
    if_stmt->expr = create_binary_op_node(OP_GT,
        create_identifier_node("a"),
        create_identifier_node("b"));

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // return a;
    ASTNode* return1 = create_ast_node(AST_RETURN, NULL);
    then_block->left = return1;
    return1->expr = create_identifier_node("a");

    ASTNode* else_stmt = create_ast_node(AST_ELSE, NULL);
    then_block->right = else_stmt;

    ASTNode* else_block = create_ast_node(AST_BLOCK, NULL);
    else_stmt->right = else_block;

    // return b;
    ASTNode* return2 = create_ast_node(AST_RETURN, NULL);
    else_block->left = return2;
    return2->expr = create_identifier_node("b");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 46: Nested if statements - should pass (NO_ERROR)
int test_nested_ifs() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // if (x) { if (y) { } }
    ASTNode* if_outer = create_ast_node(AST_IF, NULL);
    main_block->left = if_outer;
    if_outer->expr = create_identifier_node("x");

    ASTNode* then_outer = create_ast_node(AST_BLOCK, NULL);
    if_outer->right = then_outer;

    ASTNode* if_inner = create_ast_node(AST_IF, NULL);
    then_outer->left = if_inner;
    if_inner->expr = create_identifier_node("y");

    ASTNode* then_inner = create_ast_node(AST_BLOCK, NULL);
    if_inner->right = then_inner;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 47: Multiple if-else statements (not else-if chain) - should pass (NO_ERROR)
int test_multiple_if_else() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* main_func = create_ast_node(AST_MAIN_DEF, "main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // First if-else
    ASTNode* if1 = create_ast_node(AST_IF, NULL);
    main_block->left = if1;
    if1->expr = create_binary_op_node(OP_LT,
        create_identifier_node("x"),
        create_num_literal_node(0.0));

    ASTNode* then1 = create_ast_node(AST_BLOCK, NULL);
    if1->right = then1;

    ASTNode* return_neg1 = create_ast_node(AST_RETURN, NULL);
    then1->left = return_neg1;
    return_neg1->expr = create_num_literal_node(-1.0);

    ASTNode* else1 = create_ast_node(AST_ELSE, NULL);
    then1->right = else1;

    ASTNode* else_block1 = create_ast_node(AST_BLOCK, NULL);
    else1->right = else_block1;

    // Second independent if-else (not nested in else)
    ASTNode* if2 = create_ast_node(AST_IF, NULL);
    else_block1->left = if2;  // This if comes AFTER the first if-else
    if2->expr = create_binary_op_node(OP_GT,
        create_identifier_node("x"),
        create_num_literal_node(0.0));

    ASTNode* then2 = create_ast_node(AST_BLOCK, NULL);
    if2->right = then2;

    ASTNode* return_pos1 = create_ast_node(AST_RETURN, NULL);
    then2->left = return_pos1;
    return_pos1->expr = create_num_literal_node(1.0);

    ASTNode* else2 = create_ast_node(AST_ELSE, NULL);
    then2->right = else2;

    ASTNode* else_block2 = create_ast_node(AST_BLOCK, NULL);
    else2->right = else_block2;

    ASTNode* return_zero = create_ast_node(AST_RETURN, NULL);
    else_block2->left = return_zero;
    return_zero->expr = create_num_literal_node(0.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 48: Return with string value - should pass (NO_ERROR)
int test_return_string() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function greet()
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "greet");
    program->left = func;
    func->left = NULL;
    
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // return "Hello";
    ASTNode* return_stmt = create_ast_node(AST_RETURN, NULL);
    block->left = return_stmt;
    return_stmt->expr = create_string_literal_node("Hello");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 49: Return with null - should pass (NO_ERROR)
int test_return_null() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function getData()
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "getData");
    program->left = func;
    func->left = NULL;
    
    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // return null;
    ASTNode* return_stmt = create_ast_node(AST_RETURN, NULL);
    block->left = return_stmt;
    return_stmt->expr = create_null_literal_node();

    int result = semantic_analyze(program);
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
    run_test("15. Simple getter", test_simple_getter);
    run_test("16. Simple setter", test_simple_setter);
    run_test("17. Getter and setter same name", test_getter_setter_same_name);
    run_test("18. Getter redefinition", test_getter_redefinition);
    run_test("19. Setter redefinition", test_setter_redefinition);
    run_test("20. Getter/Setter/Function same name", test_getter_setter_function_same_name);
    run_test("21. Complex getter/setter program", test_complex_getter_setter_program);
    run_test("22. Getter in inner scope", test_getter_inner_scope);
    run_test("23. test-func-call-simple", test_func_call_simple);
    run_test("24. test-func-call-undefined", test_func_call_undefined);
    run_test("25. test-func-call-wrong-arg-count", test_func_call_wrong_arg_count);
    run_test("26. test-func-call-correct-arg-types", test_func_call_correct_arg_types);
    run_test("27. test-func-call-wrong-arg-types", test_func_call_wrong_arg_types);
    run_test("28. test-func-call-chained", test_func_call_chained);
    run_test("29. Simple assignment", test_simple_assignment);
    run_test("30. Assignment to undefined var", test_assignment_undefined_var);
    run_test("31. Multiple assignments", test_multiple_assignments);
    run_test("32. Use uninitialized var", test_use_uninitialized_var);
    run_test("33. Assignment different scopes", test_assignment_different_scopes);
    run_test("34. Chained assignments", test_chained_assignments);
    run_test("35. Assignment to parameter", test_assignment_to_parameter);
    run_test("36. Complex nested assignments", test_complex_nested_assignments);
    run_test("37. String assignment", test_string_assignment);
    run_test("38. Null assignment", test_null_assignment);
    run_test("39. Simple if", test_simple_if);
    run_test("40. If with else", test_if_else);
    run_test("41. If non-numeric condition", test_if_non_numeric_condition);
    run_test("42. Return with value", test_return_with_value);
    run_test("43. Void return", test_void_return);
    run_test("44. Return with expression", test_return_with_expression);
    run_test("45. Complex if-else returns", test_complex_if_else_returns);
    run_test("46. Nested ifs", test_nested_ifs);
    run_test("47. If-else-if chain", test_multiple_if_else);
    run_test("48. Return string", test_return_string);
    run_test("49. Return null", test_return_null);

    print_summary();
    
    return (tests_passed == tests_total) ? 0 : 1;
}