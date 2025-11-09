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

// Premenná pre výber konkrétneho testu (0 = všetky testy)
    int specific_test = 0;  // Zmeň na číslo testu ktorý chceš spustiť, 0 - všetky

// Funkcia pre spustenie jedného testu
void run_test(const char* test_name, int expected_error, int (*test_func)(void)) {
    printf(COLOR_CYAN "=== %s ===\n" COLOR_RESET, test_name);
    
    int result = test_func();
    tests_total++;
    
    if (result == expected_error) {
        tests_passed++;
        printf(COLOR_GREEN "✓ PASSED (Expected: %d, Got: %d)\n" COLOR_RESET, expected_error, result);
    } else {
        printf(COLOR_RED "✗ FAILED (Expected: %d, Got: %d)\n" COLOR_RESET, expected_error, result);
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
    return result;
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
    return result;
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
    return result;
}

// Test 10: Function with parameters - should fail (SEM_ERROR_REDEFINED)
int test_func_duplicate_param() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    // func foo(a, a)
    ASTNode* param2 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param2->right = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* param1 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param1->right = create_ast_node(AST_IDENTIFIER, "a");
    param1->left = param2;

    func->left = param1;
    func->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 11: Function parameter shadowed by local variable - should fail (SEM_ERROR_REDEFINED)
int test_func_param_shadowed_by_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    ASTNode* param = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param->right = create_ast_node(AST_IDENTIFIER, "x");
    func->left = param;

    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, NULL);
    block->left = var_decl;
    var_decl->left = create_ast_node(AST_IDENTIFIER, "x");

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
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
    return result;
}

// Test 13: Function redefinition with same parameter count - should fail (SEM_ERROR_REDEFINED)
int test_func_and_builtin_conflict() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "Ifj.write");
    program->left = func;
    
    // Create one parameter to match built-in Ifj.write (1 parameter)
    ASTNode* param = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func->left = param;
    param->right = create_ast_node(AST_IDENTIFIER, "term");
    
    func->right = create_ast_node(AST_BLOCK, NULL);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 14: Function parameter shadowed by variable in nested block - should pass (NO_ERROR)
int test_func_param_scope_shadowed_block_var() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    ASTNode* param = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
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
    return result;
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
    return result;
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
    ASTNode* param1 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func->left = param1;
    param1->right = create_ast_node(AST_IDENTIFIER, "a");
    
    ASTNode* param2 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param1->left = param2;
    param2->right = create_ast_node(AST_IDENTIFIER, "b");
    
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
    return result;
}

// Test 25: Function call with wrong number of arguments (should fail) - UPRAVENÝ
int test_func_call_wrong_arg_count() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo(a, b)
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;

    ASTNode* param1 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func_def->left = param1;
    param1->right = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* param2 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param1->left = param2;
    param2->right = create_ast_node(AST_IDENTIFIER, "b");

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Function call foo(1) - only one argument instead of two
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;
    func_call->right = NULL;

    ASTNode* arg_call = create_ast_node(AST_FUNC_ARG, NULL);  // Zostáva AST_FUNC_ARG
    func_call->left = arg_call;
    
    // Použijeme nový systém s ExprNode pre argument
    ASTNode* expr = create_ast_node(AST_EXPRESSION, NULL);
    arg_call->right = expr;
    expr->expr = create_num_literal_node(1.0);

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 26: Function call with correct argument types (should pass) - UPRAVENÝ
int test_func_call_correct_arg_types() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Define foo(num: Num, s: String)
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func_def;

    ASTNode* param1 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func_def->left = param1;
    ASTNode* id1 = create_ast_node(AST_IDENTIFIER, "num");
    id1->data_type = TYPE_NUM;
    param1->right = id1;

    ASTNode* param2 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param1->left = param2;
    ASTNode* id2 = create_ast_node(AST_IDENTIFIER, "s");
    id2->data_type = TYPE_STRING;
    param2->right = id2;

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Call foo(5, "hello") - nový systém
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;

    ASTNode* arg_call1 = create_ast_node(AST_FUNC_ARG, NULL);  // Zostáva AST_FUNC_ARG
    func_call->left = arg_call1;
    
    ASTNode* expr1 = create_ast_node(AST_EXPRESSION, NULL);
    arg_call1->right = expr1;
    expr1->expr = create_num_literal_node(5.0);

    ASTNode* arg_call2 = create_ast_node(AST_FUNC_ARG, NULL);  // Zostáva AST_FUNC_ARG
    arg_call1->left = arg_call2;
    
    ASTNode* expr2 = create_ast_node(AST_EXPRESSION, NULL);
    arg_call2->right = expr2;
    expr2->expr = create_string_literal_node("hello");

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

    ASTNode* param1 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func_def->left = param1;
    ASTNode* id1 = create_ast_node(AST_IDENTIFIER, "num");
    id1->data_type = TYPE_NUM;
    param1->right = id1;

    ASTNode* param2 = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param1->left = param2;
    ASTNode* id2 = create_ast_node(AST_IDENTIFIER, "s");
    id2->data_type = TYPE_STRING;
    param2->right = id2;

    func_def->right = create_ast_node(AST_BLOCK, NULL);

    // Call foo("oops", 123) - wrong types, nový systém
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "foo");
    func_def->right->right = func_call;

    // Prvý argument - string namiesto num
    ASTNode* arg_call1 = create_ast_node(AST_FUNC_ARG, NULL);  // Zostáva AST_FUNC_ARG
    func_call->left = arg_call1;
    
    ASTNode* expr1 = create_ast_node(AST_EXPRESSION, NULL);
    arg_call1->right = expr1;
    expr1->expr = create_string_literal_node("oops"); // Should be num

    // Druhý argument - num namiesto string  
    ASTNode* arg_call2 = create_ast_node(AST_FUNC_ARG, NULL);  // Zostáva AST_FUNC_ARG
    arg_call1->left = arg_call2;
    
    ASTNode* expr2 = create_ast_node(AST_EXPRESSION, NULL);
    arg_call2->right = expr2;
    expr2->expr = create_num_literal_node(123.0); // Should be string

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
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

// Test 29: Simple assignment - should pass (NO_ERROR) - OPRAVENÝ
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
    
    ASTNode* expression = create_ast_node(AST_EXPRESSION, NULL);
    equals->right = expression;
    expression->expr = create_num_literal_node(5.0);

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
    return result;
}

// Test 31: Multiple assignments - should pass (NO_ERROR) - OPRAVENÝ
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
    
    ASTNode* expr1 = create_ast_node(AST_EXPRESSION, NULL);
    equals1->right = expr1;
    expr1->expr = create_num_literal_node(1.0);

    // b = a (use previously assigned variable)
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    assign1->right = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "b");
    
    ASTNode* expr2 = create_ast_node(AST_EXPRESSION, NULL);
    equals2->right = expr2;
    expr2->expr = create_identifier_node("a");

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
    return result;
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
    ASTNode* expression1 = create_ast_node(AST_EXPRESSION, NULL);

    equals1->right = expression1;
    expression1->expr = create_num_literal_node(10.0);

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
    ASTNode* expression2 = create_ast_node(AST_EXPRESSION, NULL);

    equals2->right = expression2;
    expression2->expr = create_num_literal_node(20.0);

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
    ASTNode* expression1 = create_ast_node(AST_EXPRESSION, NULL);

    equals1->right = expression1;
    expression1->expr = create_num_literal_node(1.0);

    // b = a + 2
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    assign1->right = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "b");
    ASTNode* expression2 = create_ast_node(AST_EXPRESSION, NULL);

    equals2->right = expression2;
    
    // Vytvoríme expression: a + 2
    ExprNode* add_expr = create_binary_op_node(OP_ADD,
        create_identifier_node("a"),
        create_num_literal_node(2.0));
    expression2->expr = add_expr;

    // c = b * 3
    ASTNode* assign3 = create_ast_node(AST_ASSIGN, NULL);
    assign2->right = assign3;

    ASTNode* equals3 = create_ast_node(AST_EQUALS, NULL);
    assign3->left = equals3;
    equals3->left = create_ast_node(AST_IDENTIFIER, "c");

    ASTNode* expression3 = create_ast_node(AST_EXPRESSION, NULL);

    equals3->right = expression3;
    
    ExprNode* mul_expr = create_binary_op_node(OP_MUL,
        create_identifier_node("b"),
        create_num_literal_node(3.0));
    expression3->expr = mul_expr;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
}

// Test 35: Assignment to parameter - should pass (NO_ERROR)
int test_assignment_to_parameter() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // function foo(x)
    ASTNode* func = create_ast_node(AST_FUNC_DEF, "foo");
    program->left = func;

    // Zmenené: AST_FUNC_ARG -> AST_FUNC_PARAM
    ASTNode* param = create_ast_node(AST_FUNC_PARAM, NULL);  // ← ZMENENÉ
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
    ASTNode* expression = create_ast_node(AST_EXPRESSION, NULL);

    equals->right = expression;
    expression->expr = create_num_literal_node(5.0);

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
    ASTNode* expression1 = create_ast_node(AST_EXPRESSION, NULL);

    equals1->right = expression1;
    expression1->expr = create_num_literal_node(0.0);

    // if block with assignment inside
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign1->right = if_stmt;
    
    // condition: true (using literal 1) - OPRAVENÉ
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_num_literal_node(1.0);

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // result = 42 inside if
    ASTNode* assign2 = create_ast_node(AST_ASSIGN, NULL);
    then_block->left = assign2;

    ASTNode* equals2 = create_ast_node(AST_EQUALS, NULL);
    assign2->left = equals2;
    equals2->left = create_ast_node(AST_IDENTIFIER, "result");
    ASTNode* expression2 = create_ast_node(AST_EXPRESSION, NULL);

    equals2->right = expression2;
    expression2->expr = create_num_literal_node(42.0);

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
    
    ASTNode* expression = create_ast_node(AST_EXPRESSION, NULL);

    equals->right = expression;
    expression->expr = create_string_literal_node("hello");

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
    ASTNode* expression = create_ast_node(AST_EXPRESSION, NULL);

    equals->right = expression;
    expression->expr = create_null_literal_node();

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

    // if (1) { } - OPRAVENÉ
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    main_block->left = if_stmt;
    
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_num_literal_node(1.0); // true condition

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

    // Declare and initialize variable x
    ASTNode* var_x = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_x;
    var_x->left = create_ast_node(AST_IDENTIFIER, "x");

    ASTNode* assign_x = create_ast_node(AST_ASSIGN, NULL);
    var_x->right = assign_x;

    ASTNode* equals_x = create_ast_node(AST_EQUALS, NULL);
    assign_x->left = equals_x;
    equals_x->left = create_ast_node(AST_IDENTIFIER, "x");
    
    ASTNode* expr_x = create_ast_node(AST_EXPRESSION, NULL);
    equals_x->right = expr_x;
    expr_x->expr = create_num_literal_node(5.0);  // inicializácia

    // if (x > 0) { } else { } - OPRAVENÉ
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_x->right = if_stmt;
    
    // Condition: x > 0 - OPRAVENÉ
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_binary_op_node(OP_GT,
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

    // if ("string") { } - invalid condition - OPRAVENÉ
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    main_block->left = if_stmt;
    
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_string_literal_node("invalid condition");

    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    int result = semantic_analyze(program);
    free_ast_tree(program);
    return result;
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
    ASTNode* param_a = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    func->left = param_a;
    ASTNode* id_a = create_ast_node(AST_IDENTIFIER, "a");
    id_a->data_type = TYPE_NUM;
    param_a->right = id_a;

    ASTNode* param_b = create_ast_node(AST_FUNC_PARAM, NULL);  // Zmenené
    param_a->left = param_b;
    ASTNode* id_b = create_ast_node(AST_IDENTIFIER, "b");
    id_b->data_type = TYPE_NUM;
    param_b->right = id_b;

    ASTNode* block = create_ast_node(AST_BLOCK, NULL);
    func->right = block;

    // if (a > b) { return a; } else { return b; } - OPRAVENÉ
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    block->left = if_stmt;
    
    // Condition: a > b - OPRAVENÉ
    ASTNode* if_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_stmt->left = if_expr;
    if_expr->expr = create_binary_op_node(OP_GT,
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

    // Declare and initialize variable x
    ASTNode* var_x = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_x;
    var_x->left = create_ast_node(AST_IDENTIFIER, "x");

    ASTNode* assign_x = create_ast_node(AST_ASSIGN, NULL);
    var_x->right = assign_x;

    ASTNode* equals_x = create_ast_node(AST_EQUALS, NULL);
    assign_x->left = equals_x;
    equals_x->left = create_ast_node(AST_IDENTIFIER, "x");
    
    ASTNode* expr_x = create_ast_node(AST_EXPRESSION, NULL);
    equals_x->right = expr_x;
    expr_x->expr = create_num_literal_node(1.0);  // inicializácia

    // Declare and initialize variable y
    ASTNode* var_y = create_ast_node(AST_VAR_DECL, NULL);
    assign_x->right = var_y;
    var_y->left = create_ast_node(AST_IDENTIFIER, "y");

    ASTNode* assign_y = create_ast_node(AST_ASSIGN, NULL);
    var_y->right = assign_y;

    ASTNode* equals_y = create_ast_node(AST_EQUALS, NULL);
    assign_y->left = equals_y;
    equals_y->left = create_ast_node(AST_IDENTIFIER, "y");
    
    ASTNode* expr_y = create_ast_node(AST_EXPRESSION, NULL);
    equals_y->right = expr_y;
    expr_y->expr = create_num_literal_node(0.0);  // inicializácia

    // if (x) { if (y) { } } - OPRAVENÉ
    ASTNode* if_outer = create_ast_node(AST_IF, NULL);
    assign_y->right = if_outer;
    
    ASTNode* if_outer_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_outer->left = if_outer_expr;
    if_outer_expr->expr = create_identifier_node("x");

    ASTNode* then_outer = create_ast_node(AST_BLOCK, NULL);
    if_outer->right = then_outer;

    ASTNode* if_inner = create_ast_node(AST_IF, NULL);
    then_outer->left = if_inner;
    
    ASTNode* if_inner_expr = create_ast_node(AST_EXPRESSION, NULL);
    if_inner->left = if_inner_expr;
    if_inner_expr->expr = create_identifier_node("y");

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

    // Declare variable x
    ASTNode* var_x = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = var_x;
    var_x->left = create_ast_node(AST_IDENTIFIER, "x");

    // Initialize x with a value (e.g., 0)
    ASTNode* assign_x = create_ast_node(AST_ASSIGN, NULL);
    var_x->right = assign_x;

    ASTNode* equals_x = create_ast_node(AST_EQUALS, NULL);
    assign_x->left = equals_x;
    equals_x->left = create_ast_node(AST_IDENTIFIER, "x");
    
    ASTNode* expr_x = create_ast_node(AST_EXPRESSION, NULL);
    equals_x->right = expr_x;
    expr_x->expr = create_num_literal_node(0.0);

    // First if-else - OPRAVENÉ
    ASTNode* if1 = create_ast_node(AST_IF, NULL);
    assign_x->right = if1;
    
    ASTNode* if1_expr = create_ast_node(AST_EXPRESSION, NULL);
    if1->left = if1_expr;
    if1_expr->expr = create_binary_op_node(OP_LT,
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

    // Second independent if-else (not nested in else) - OPRAVENÉ
    ASTNode* if2 = create_ast_node(AST_IF, NULL);
    else_block1->left = if2;  // This if comes AFTER the first if-else
    
    ASTNode* if2_expr = create_ast_node(AST_EXPRESSION, NULL);
    if2->left = if2_expr;
    if2_expr->expr = create_binary_op_node(OP_GT,
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

// Test 50: Complex program with proper variable declarations (NO_ERROR)
int test_complex_program_fixed() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // === Function Program.main ===
    ASTNode* main_func = create_ast_node(AST_FUNC_DEF, "Program.main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // var x
    ASTNode* decl_x = create_ast_node(AST_VAR_DECL, NULL);
    main_block->left = decl_x;
    decl_x->left = create_ast_node(AST_IDENTIFIER, "x");
    decl_x->left->data_type = TYPE_UNDEF;

    // x = 9 * 9 / (4 + 6)
    ASTNode* assign_x = create_ast_node(AST_ASSIGN, NULL);
    decl_x->right = assign_x;

    ASTNode* equals_x = create_ast_node(AST_EQUALS, NULL);
    assign_x->left = equals_x;
    equals_x->left = create_ast_node(AST_IDENTIFIER, "x");

    // Expression for RHS: (9 * 9) / (4 + 6)
    ASTNode* expr_arith = create_ast_node(AST_EXPRESSION, NULL);
    expr_arith->expr = create_binary_op_node(
        OP_DIV,
        create_binary_op_node(OP_MUL,
            create_num_literal_node(9),
            create_num_literal_node(9)),
        create_binary_op_node(OP_ADD,
            create_num_literal_node(4),
            create_num_literal_node(6))
    );
    equals_x->right = expr_arith;

    // === Function Program.second ===
    ASTNode* second_func = create_ast_node(AST_FUNC_DEF, "Program.second");
    assign_x->right = second_func; // chain functions

    ASTNode* second_block = create_ast_node(AST_BLOCK, NULL);
    second_func->right = second_block;

    // var secondeX
    ASTNode* decl_secondeX = create_ast_node(AST_VAR_DECL, NULL);
    second_block->left = decl_secondeX;
    decl_secondeX->left = create_ast_node(AST_IDENTIFIER, "secondeX");
    decl_secondeX->left->data_type = TYPE_UNDEF;

    // var d
    ASTNode* decl_d = create_ast_node(AST_VAR_DECL, NULL);
    decl_secondeX->right = decl_d;
    decl_d->left = create_ast_node(AST_IDENTIFIER, "d");
    decl_d->left->data_type = TYPE_UNDEF;

    // d = 0
    ASTNode* assign_d = create_ast_node(AST_ASSIGN, NULL);
    decl_d->right = assign_d;

    ASTNode* equals_d = create_ast_node(AST_EQUALS, NULL);
    assign_d->left = equals_d;
    equals_d->left = create_ast_node(AST_IDENTIFIER, "d");

    ASTNode* expr_zero = create_ast_node(AST_EXPRESSION, NULL);
    expr_zero->expr = create_num_literal_node(0);
    equals_d->right = expr_zero;

    // secondeX = d
    ASTNode* assign_secondeX = create_ast_node(AST_ASSIGN, NULL);
    assign_d->right = assign_secondeX;

    ASTNode* equals_secondeX = create_ast_node(AST_EQUALS, NULL);
    assign_secondeX->left = equals_secondeX;
    equals_secondeX->left = create_ast_node(AST_IDENTIFIER, "secondeX");

    ASTNode* expr_d = create_ast_node(AST_EXPRESSION, NULL);
    expr_d->expr = create_identifier_node("d");
    equals_secondeX->right = expr_d;

    // === Function Program.thirt ===
    ASTNode* thirt_func = create_ast_node(AST_FUNC_DEF, "Program.thirt");
    assign_secondeX->right = thirt_func;

    ASTNode* thirt_block = create_ast_node(AST_BLOCK, NULL);
    thirt_func->right = thirt_block;

    // var asd
    ASTNode* decl_asd = create_ast_node(AST_VAR_DECL, NULL);
    thirt_block->left = decl_asd;
    decl_asd->left = create_ast_node(AST_IDENTIFIER, "asd");
    decl_asd->left->data_type = TYPE_UNDEF;

    // asd = 2.0
    ASTNode* assign_asd1 = create_ast_node(AST_ASSIGN, NULL);
    decl_asd->right = assign_asd1;

    ASTNode* equals_asd1 = create_ast_node(AST_EQUALS, NULL);
    assign_asd1->left = equals_asd1;
    equals_asd1->left = create_ast_node(AST_IDENTIFIER, "asd");

    ASTNode* expr_2 = create_ast_node(AST_EXPRESSION, NULL);
    expr_2->expr = create_num_literal_node(2.0);
    equals_asd1->right = expr_2;

    // var a
    ASTNode* decl_a = create_ast_node(AST_VAR_DECL, NULL);
    assign_asd1->right = decl_a;
    decl_a->left = create_ast_node(AST_IDENTIFIER, "a");
    decl_a->left->data_type = TYPE_UNDEF;

    // a = 3
    ASTNode* assign_a = create_ast_node(AST_ASSIGN, NULL);
    decl_a->right = assign_a;

    ASTNode* equals_a = create_ast_node(AST_EQUALS, NULL);
    assign_a->left = equals_a;
    equals_a->left = create_ast_node(AST_IDENTIFIER, "a");

    ASTNode* expr_3 = create_ast_node(AST_EXPRESSION, NULL);
    expr_3->expr = create_num_literal_node(3);
    equals_a->right = expr_3;

    // asd = asd * a
    ASTNode* assign_asd2 = create_ast_node(AST_ASSIGN, NULL);
    assign_a->right = assign_asd2;

    ASTNode* equals_asd2 = create_ast_node(AST_EQUALS, NULL);
    assign_asd2->left = equals_asd2;
    equals_asd2->left = create_ast_node(AST_IDENTIFIER, "asd");

    ASTNode* expr_asd_a = create_ast_node(AST_EXPRESSION, NULL);
    expr_asd_a->expr = create_binary_op_node(
        OP_MUL,
        create_identifier_node("asd"),
        create_identifier_node("a")
    );
    equals_asd2->right = expr_asd_a;

    // Run semantic analysis
    int result = semantic_analyze(program);

    printf("Expected: %d, got: %d\n", NO_ERROR, result);

    free_ast_tree(program);
    return result;
}

// Test 51: Simple global variable usage (NO_ERROR)
int test_global_var_simple() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Function Program.main
    ASTNode* main_func = create_ast_node(AST_FUNC_DEF, "Program.main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // __counter = 1
    ASTNode* assign_counter = create_ast_node(AST_ASSIGN, NULL);
    main_block->left = assign_counter;

    ASTNode* equals_counter = create_ast_node(AST_EQUALS, NULL);
    assign_counter->left = equals_counter;
    equals_counter->left = create_ast_node(AST_IDENTIFIER, "__counter");

    ASTNode* expr_1 = create_ast_node(AST_EXPRESSION, NULL);
    expr_1->expr = create_num_literal_node(1);
    equals_counter->right = expr_1;

    // var result = __counter + 5
    ASTNode* decl_result = create_ast_node(AST_VAR_DECL, NULL);
    assign_counter->right = decl_result;
    decl_result->left = create_ast_node(AST_IDENTIFIER, "result");
    decl_result->left->data_type = TYPE_UNDEF;

    ASTNode* assign_result = create_ast_node(AST_ASSIGN, NULL);
    decl_result->right = assign_result;

    ASTNode* equals_result = create_ast_node(AST_EQUALS, NULL);
    assign_result->left = equals_result;
    equals_result->left = create_ast_node(AST_IDENTIFIER, "result");

    ASTNode* expr_counter_plus_5 = create_ast_node(AST_EXPRESSION, NULL);
    expr_counter_plus_5->expr = create_binary_op_node(
        OP_ADD,
        create_identifier_node("__counter"),
        create_num_literal_node(5)
    );
    equals_result->right = expr_counter_plus_5;

    int result = semantic_analyze(program);
    printf("Test 51 - Expected: %d, got: %d\n", NO_ERROR, result);
    free_ast_tree(program);
    return result;
}

// Test 52: Global variable across multiple functions (NO_ERROR)
int test_global_var_multiple_funcs() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Function Program.init
    ASTNode* init_func = create_ast_node(AST_FUNC_DEF, "Program.init");
    program->left = init_func;

    ASTNode* init_block = create_ast_node(AST_BLOCK, NULL);
    init_func->right = init_block;

    // __value = 100
    ASTNode* assign_value = create_ast_node(AST_ASSIGN, NULL);
    init_block->left = assign_value;

    ASTNode* equals_value = create_ast_node(AST_EQUALS, NULL);
    assign_value->left = equals_value;
    equals_value->left = create_ast_node(AST_IDENTIFIER, "__value");

    ASTNode* expr_100 = create_ast_node(AST_EXPRESSION, NULL);
    expr_100->expr = create_num_literal_node(100);
    equals_value->right = expr_100;

    // Function Program.compute
    ASTNode* compute_func = create_ast_node(AST_FUNC_DEF, "Program.compute");
    assign_value->right = compute_func;

    ASTNode* compute_block = create_ast_node(AST_BLOCK, NULL);
    compute_func->right = compute_block;

    // var x = __value * 2
    ASTNode* decl_x = create_ast_node(AST_VAR_DECL, NULL);
    compute_block->left = decl_x;
    decl_x->left = create_ast_node(AST_IDENTIFIER, "x");
    decl_x->left->data_type = TYPE_UNDEF;

    ASTNode* assign_x = create_ast_node(AST_ASSIGN, NULL);
    decl_x->right = assign_x;

    ASTNode* equals_x = create_ast_node(AST_EQUALS, NULL);
    assign_x->left = equals_x;
    equals_x->left = create_ast_node(AST_IDENTIFIER, "x");

    ASTNode* expr_value_times_2 = create_ast_node(AST_EXPRESSION, NULL);
    expr_value_times_2->expr = create_binary_op_node(
        OP_MUL,
        create_identifier_node("__value"),
        create_num_literal_node(2)
    );
    equals_x->right = expr_value_times_2;

    // __value = x
    ASTNode* assign_value2 = create_ast_node(AST_ASSIGN, NULL);
    assign_x->right = assign_value2;

    ASTNode* equals_value2 = create_ast_node(AST_EQUALS, NULL);
    assign_value2->left = equals_value2;
    equals_value2->left = create_ast_node(AST_IDENTIFIER, "__value");

    ASTNode* expr_x = create_ast_node(AST_EXPRESSION, NULL);
    expr_x->expr = create_identifier_node("x");
    equals_value2->right = expr_x;

    // Function Program.printResult
    ASTNode* print_func = create_ast_node(AST_FUNC_DEF, "Program.printResult");
    assign_value2->right = print_func;

    ASTNode* print_block = create_ast_node(AST_BLOCK, NULL);
    print_func->right = print_block;

    // var output = __value
    ASTNode* decl_output = create_ast_node(AST_VAR_DECL, NULL);
    print_block->left = decl_output;
    decl_output->left = create_ast_node(AST_IDENTIFIER, "output");
    decl_output->left->data_type = TYPE_UNDEF;

    ASTNode* assign_output = create_ast_node(AST_ASSIGN, NULL);
    decl_output->right = assign_output;

    ASTNode* equals_output = create_ast_node(AST_EQUALS, NULL);
    assign_output->left = equals_output;
    equals_output->left = create_ast_node(AST_IDENTIFIER, "output");

    ASTNode* expr_value = create_ast_node(AST_EXPRESSION, NULL);
    expr_value->expr = create_identifier_node("__value");
    equals_output->right = expr_value;

    int result = semantic_analyze(program);
    printf("Test 52 - Expected: %d, got: %d\n", NO_ERROR, result);
    free_ast_tree(program);
    return result;
}

// Test 53: Multiple global variables with different types (NO_ERROR)
int test_global_var_multiple_types() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Function Program.main
    ASTNode* main_func = create_ast_node(AST_FUNC_DEF, "Program.main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // __number = 42
    ASTNode* assign_number = create_ast_node(AST_ASSIGN, NULL);
    main_block->left = assign_number;

    ASTNode* equals_number = create_ast_node(AST_EQUALS, NULL);
    assign_number->left = equals_number;
    equals_number->left = create_ast_node(AST_IDENTIFIER, "__number");

    ASTNode* expr_42 = create_ast_node(AST_EXPRESSION, NULL);
    expr_42->expr = create_num_literal_node(42);
    equals_number->right = expr_42;

    // __text = "hello"
    ASTNode* assign_text = create_ast_node(AST_ASSIGN, NULL);
    assign_number->right = assign_text;

    ASTNode* equals_text = create_ast_node(AST_EQUALS, NULL);
    assign_text->left = equals_text;
    equals_text->left = create_ast_node(AST_IDENTIFIER, "__text");

    ASTNode* expr_hello = create_ast_node(AST_EXPRESSION, NULL);
    expr_hello->expr = create_string_literal_node("hello");
    equals_text->right = expr_hello;

    // __flag = 1
    ASTNode* assign_flag = create_ast_node(AST_ASSIGN, NULL);
    assign_text->right = assign_flag;

    ASTNode* equals_flag = create_ast_node(AST_EQUALS, NULL);
    assign_flag->left = equals_flag;
    equals_flag->left = create_ast_node(AST_IDENTIFIER, "__flag");

    ASTNode* expr_1 = create_ast_node(AST_EXPRESSION, NULL);
    expr_1->expr = create_num_literal_node(1);
    equals_flag->right = expr_1;

    // var num = __number
    ASTNode* decl_num = create_ast_node(AST_VAR_DECL, NULL);
    assign_flag->right = decl_num;
    decl_num->left = create_ast_node(AST_IDENTIFIER, "num");
    decl_num->left->data_type = TYPE_UNDEF;

    ASTNode* assign_num = create_ast_node(AST_ASSIGN, NULL);
    decl_num->right = assign_num;

    ASTNode* equals_num = create_ast_node(AST_EQUALS, NULL);
    assign_num->left = equals_num;
    equals_num->left = create_ast_node(AST_IDENTIFIER, "num");

    ASTNode* expr_number = create_ast_node(AST_EXPRESSION, NULL);
    expr_number->expr = create_identifier_node("__number");
    equals_num->right = expr_number;

    // var str = __text
    ASTNode* decl_str = create_ast_node(AST_VAR_DECL, NULL);
    assign_num->right = decl_str;
    decl_str->left = create_ast_node(AST_IDENTIFIER, "str");
    decl_str->left->data_type = TYPE_UNDEF;

    ASTNode* assign_str = create_ast_node(AST_ASSIGN, NULL);
    decl_str->right = assign_str;

    ASTNode* equals_str = create_ast_node(AST_EQUALS, NULL);
    assign_str->left = equals_str;
    equals_str->left = create_ast_node(AST_IDENTIFIER, "str");

    ASTNode* expr_text = create_ast_node(AST_EXPRESSION, NULL);
    expr_text->expr = create_identifier_node("__text");
    equals_str->right = expr_text;

    // var bool = __flag
    ASTNode* decl_bool = create_ast_node(AST_VAR_DECL, NULL);
    assign_str->right = decl_bool;
    decl_bool->left = create_ast_node(AST_IDENTIFIER, "bool");
    decl_bool->left->data_type = TYPE_UNDEF;

    ASTNode* assign_bool = create_ast_node(AST_ASSIGN, NULL);
    decl_bool->right = assign_bool;

    ASTNode* equals_bool = create_ast_node(AST_EQUALS, NULL);
    assign_bool->left = equals_bool;
    equals_bool->left = create_ast_node(AST_IDENTIFIER, "bool");

    ASTNode* expr_flag = create_ast_node(AST_EXPRESSION, NULL);
    expr_flag->expr = create_identifier_node("__flag");
    equals_bool->right = expr_flag;

    int result = semantic_analyze(program);
    printf("Test 53 - Expected: %d, got: %d\n", NO_ERROR, result);
    free_ast_tree(program);
    return result;
}
// Test 54: Global variable in blocks and conditions (NO_ERROR)
int test_global_var_blocks_conditions() {
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);

    // Function Program.main
    ASTNode* main_func = create_ast_node(AST_FUNC_DEF, "Program.main");
    program->left = main_func;

    ASTNode* main_block = create_ast_node(AST_BLOCK, NULL);
    main_func->right = main_block;

    // __total = 0
    ASTNode* assign_total = create_ast_node(AST_ASSIGN, NULL);
    main_block->left = assign_total;

    ASTNode* equals_total = create_ast_node(AST_EQUALS, NULL);
    assign_total->left = equals_total;
    equals_total->left = create_ast_node(AST_IDENTIFIER, "__total");

    ASTNode* expr_0 = create_ast_node(AST_EXPRESSION, NULL);
    expr_0->expr = create_num_literal_node(0);
    equals_total->right = expr_0;

    // if (1) { __total = __total + 10 }
    ASTNode* if_stmt = create_ast_node(AST_IF, NULL);
    assign_total->right = if_stmt;

    // Condition: 1
    ASTNode* cond_expr = create_ast_node(AST_EXPRESSION, NULL);
    cond_expr->expr = create_num_literal_node(1);
    if_stmt->left = cond_expr;

    // Then block
    ASTNode* then_block = create_ast_node(AST_BLOCK, NULL);
    if_stmt->right = then_block;

    // __total = __total + 10
    ASTNode* assign_total2 = create_ast_node(AST_ASSIGN, NULL);
    then_block->left = assign_total2;

    ASTNode* equals_total2 = create_ast_node(AST_EQUALS, NULL);
    assign_total2->left = equals_total2;
    equals_total2->left = create_ast_node(AST_IDENTIFIER, "__total");

    ASTNode* expr_total_plus_10 = create_ast_node(AST_EXPRESSION, NULL);
    expr_total_plus_10->expr = create_binary_op_node(
        OP_ADD,
        create_identifier_node("__total"),
        create_num_literal_node(10)
    );
    equals_total2->right = expr_total_plus_10;

    // Block { __total = __total * 2 }
    ASTNode* inner_block = create_ast_node(AST_BLOCK, NULL);
    then_block->right = inner_block;

    // __total = __total * 2
    ASTNode* assign_total3 = create_ast_node(AST_ASSIGN, NULL);
    inner_block->left = assign_total3;

    ASTNode* equals_total3 = create_ast_node(AST_EQUALS, NULL);
    assign_total3->left = equals_total3;
    equals_total3->left = create_ast_node(AST_IDENTIFIER, "__total");

    ASTNode* expr_total_times_2 = create_ast_node(AST_EXPRESSION, NULL);
    expr_total_times_2->expr = create_binary_op_node(
        OP_MUL,
        create_identifier_node("__total"),
        create_num_literal_node(2)
    );
    equals_total3->right = expr_total_times_2;

    // var final = __total
    ASTNode* decl_final = create_ast_node(AST_VAR_DECL, NULL);
    inner_block->right = decl_final;
    decl_final->left = create_ast_node(AST_IDENTIFIER, "final");
    decl_final->left->data_type = TYPE_UNDEF;

    ASTNode* assign_final = create_ast_node(AST_ASSIGN, NULL);
    decl_final->right = assign_final;

    ASTNode* equals_final = create_ast_node(AST_EQUALS, NULL);
    assign_final->left = equals_final;
    equals_final->left = create_ast_node(AST_IDENTIFIER, "final");

    ASTNode* expr_total_final = create_ast_node(AST_EXPRESSION, NULL);
    expr_total_final->expr = create_identifier_node("__total");
    equals_final->right = expr_total_final;

    int result = semantic_analyze(program);
    printf("Test 54 - Expected: %d, got: %d\n", NO_ERROR, result);
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

    
    
    struct TestCase {
            int id;
            const char* name;
            const char* error_description;  // Nové pole pre popis chyby
            int expected_error;             // Nové pole pre očakávaný error kód
            int (*func)(void);
        } tests[] = {
            {1, "Single variable declaration", "NO_ERROR", NO_ERROR, test_single_variable},
            {2, "Multiple different variables", "NO_ERROR", NO_ERROR, test_multiple_different_variables},
            {3, "Variable redeclaration error", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_redeclaration_error},
            {4, "Multiple vars same block", "NO_ERROR", NO_ERROR, test_multiple_vars_same_block},
            {5, "Same var different blocks", "NO_ERROR", NO_ERROR, test_same_var_different_blocks},
            {6, "Same var same block", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_same_var_same_block},
            {7, "Global local same name", "NO_ERROR", NO_ERROR, test_global_local_same_name},
            {8, "Function simple definition", "NO_ERROR", NO_ERROR, test_func_simple_definition},
            {9, "Function redefinition same params", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_func_redefinition_same_params},
            {10, "Function duplicate param", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_func_duplicate_param},
            {11, "Function param shadowed by var", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_func_param_shadowed_by_var},
            {12, "Main redefinition", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_main_redefinition},
            {13, "Function and builtin conflict", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_func_and_builtin_conflict},
            {14, "Function param scope shadowed block var", "NO_ERROR", NO_ERROR, test_func_param_scope_shadowed_block_var},
            {15, "Simple getter", "NO_ERROR", NO_ERROR, test_simple_getter},
            {16, "Simple setter", "NO_ERROR", NO_ERROR, test_simple_setter},
            {17, "Getter and setter same name", "NO_ERROR", NO_ERROR, test_getter_setter_same_name},
            {18, "Getter redefinition", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_getter_redefinition},
            {19, "Setter redefinition", "SEM_ERROR_REDEFINED", SEM_ERROR_REDEFINED, test_setter_redefinition},
            {20, "Getter/Setter/Function same name", "NO_ERROR", NO_ERROR, test_getter_setter_function_same_name},
            {21, "Complex getter/setter program", "NO_ERROR", NO_ERROR, test_complex_getter_setter_program},
            {22, "Getter in inner scope", "NO_ERROR", NO_ERROR, test_getter_inner_scope},
            {23, "Function call simple", "NO_ERROR", NO_ERROR, test_func_call_simple},
            {24, "Function call undefined", "SEM_ERROR_UNDEFINED", SEM_ERROR_UNDEFINED, test_func_call_undefined},
            {25, "Function call wrong arg count", "SEM_ERROR_WRONG_PARAMS", SEM_ERROR_WRONG_PARAMS, test_func_call_wrong_arg_count},
            {26, "Function call correct arg types", "NO_ERROR", NO_ERROR, test_func_call_correct_arg_types},
            {27, "Function call wrong arg types", "SEM_ERROR_TYPE_COMPATIBILITY", SEM_ERROR_TYPE_COMPATIBILITY, test_func_call_wrong_arg_types},
            {28, "Function call chained", "NO_ERROR", NO_ERROR, test_func_call_chained},
            {29, "Simple assignment", "NO_ERROR", NO_ERROR, test_simple_assignment},
            {30, "Assignment to undefined var", "SEM_ERROR_UNDEFINED", SEM_ERROR_UNDEFINED, test_assignment_undefined_var},
            {31, "Multiple assignments", "NO_ERROR", NO_ERROR, test_multiple_assignments},
            {32, "Use uninitialized var", "SEM_ERROR_OTHER", SEM_ERROR_OTHER, test_use_uninitialized_var},
            {33, "Assignment different scopes", "NO_ERROR", NO_ERROR, test_assignment_different_scopes},
            {34, "Chained assignments", "NO_ERROR", NO_ERROR, test_chained_assignments},
            {35, "Assignment to parameter", "NO_ERROR", NO_ERROR, test_assignment_to_parameter},
            {36, "Complex nested assignments", "NO_ERROR", NO_ERROR, test_complex_nested_assignments},
            {37, "String assignment", "NO_ERROR", NO_ERROR, test_string_assignment},
            {38, "Null assignment", "NO_ERROR", NO_ERROR, test_null_assignment},
            {39, "Simple if", "NO_ERROR", NO_ERROR, test_simple_if},
            {40, "If with else", "NO_ERROR", NO_ERROR, test_if_else},
            {41, "If non-numeric condition", "SEM_ERROR_TYPE_COMPATIBILITY", SEM_ERROR_TYPE_COMPATIBILITY, test_if_non_numeric_condition},
            {42, "Return with value", "NO_ERROR", NO_ERROR, test_return_with_value},
            {43, "Void return", "NO_ERROR", NO_ERROR, test_void_return},
            {44, "Return with expression", "NO_ERROR", NO_ERROR, test_return_with_expression},
            {45, "Complex if-else returns", "NO_ERROR", NO_ERROR, test_complex_if_else_returns},
            {46, "Nested ifs", "NO_ERROR", NO_ERROR, test_nested_ifs},
            {47, "If-else-if chain", "NO_ERROR", NO_ERROR, test_multiple_if_else},
            {48, "Return string", "NO_ERROR", NO_ERROR, test_return_string},
            {49, "Return null", "NO_ERROR", NO_ERROR, test_return_null},
            {50, "MATKOV program","NO_ERROR",NO_ERROR,test_complex_program_fixed},
            {51, "Simple global variable", "NO_ERROR", NO_ERROR, test_global_var_simple},
            {52, "Global variable across functions", "NO_ERROR", NO_ERROR, test_global_var_multiple_funcs},
            {53, "Multiple global variables", "NO_ERROR", NO_ERROR, test_global_var_multiple_types},
            {54, "Global variable in blocks", "NO_ERROR", NO_ERROR, test_global_var_blocks_conditions},
            {0, NULL, NULL, 0, NULL} // Ukončovací prvok
        };

    if (specific_test != 0) {
    // Spustenie konkrétneho testu
    int found = 0;
    for (int i = 0; tests[i].id != 0; i++) {
        if (tests[i].id == specific_test) {
            char test_name[150];
            snprintf(test_name, sizeof(test_name), "Test %d: %s - %s", 
                    specific_test, tests[i].name, tests[i].error_description);
            printf(COLOR_CYAN "=== %s ===\n" COLOR_RESET, test_name);
            
            int result = tests[i].func();
            tests_total++;
            
            if (result == tests[i].expected_error) {
                tests_passed++;
                printf(COLOR_GREEN "✓ PASSED (Expected: %d, Got: %d)\n" COLOR_RESET, tests[i].expected_error, result);
            } else {
                printf(COLOR_RED "✗ FAILED (Expected: %d, Got: %d)\n" COLOR_RESET, tests[i].expected_error, result);
            }
            found = 1;
            break;
        }
    }
    if (!found) {
        printf(COLOR_RED "Test %d not found!\n" COLOR_RESET, specific_test);
        return 1;
    }
} else {
    // Spustenie všetkých testov
    for (int i = 0; tests[i].id != 0; i++) {
        char test_name[150];
        snprintf(test_name, sizeof(test_name), "Test %d: %s - %s", 
                tests[i].id, tests[i].name, tests[i].error_description);
        printf(COLOR_CYAN "=== %s ===\n" COLOR_RESET, test_name);
        
        int result = tests[i].func();
        tests_total++;
        
        if (result == tests[i].expected_error) {
            tests_passed++;
            printf(COLOR_GREEN "✓ PASSED (Expected: %d, Got: %d)\n" COLOR_RESET, tests[i].expected_error, result);
        } else {
            printf(COLOR_RED "✗ FAILED (Expected: %d, Got: %d)\n" COLOR_RESET, tests[i].expected_error, result);
        }
        printf("\n");
    }
}

    print_summary();
    
    return (tests_passed == tests_total) ? 0 : 1;
}