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

// Pomocná funkcia pre vytvorenie AST uzlu
ASTNode* create_ast_node(ASTNodeType type, const char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->name = name ? my_strdup(name) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->body = NULL;
    node->int_val = 0;
    node->double_val = 0.0;
    node->string_val = NULL;
    node->data_type = TYPE_UNDEF;
    
    return node;
}

// Pomocná funkcia pre rekurzívne uvoľnenie AST stromu
void free_ast_tree(ASTNode* node) {
    if (!node) return;
    
    free_ast_tree(node->left);
    free_ast_tree(node->right);
    free_ast_tree(node->next);
    free_ast_tree(node->body);
    
    if (node->name) free(node->name);
    if (node->string_val) free(node->string_val);
    free(node);
}

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

// === TESTOVÉ FUNKCIE ===

int test_empty_tree() {
    printf("Testing empty AST tree...\n");
    int result = semantic_analyze(NULL);
    printf("Expected: NO_ERROR (%d), Got: %d\n", NO_ERROR, result);
    return result;
}

int test_simple_program() {
    printf("Testing simple program with one variable...\n");
    
    // Vytvor AST: program s jednou premennou
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, "x");
    var_decl->data_type = TYPE_NUM;
    var_decl->right = create_ast_node(AST_LITERAL_INT, NULL);
    var_decl->right->int_val = 5;
    
    program->body = var_decl;
    
    int result = semantic_analyze(program);
    printf("Expected: NO_ERROR (%d), Got: %d\n", NO_ERROR, result);
    
    free_ast_tree(program);
    return result;
}

int test_variable_usage() {
    printf("Testing variable declaration and usage...\n");
    
    // AST: var x = 5; y = x;
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // var x = 5
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, "x");
    var_decl->data_type = TYPE_NUM;
    var_decl->right = create_ast_node(AST_LITERAL_INT, NULL);
    var_decl->right->int_val = 5;
    
    // y = x (should fail - y not declared)
    ASTNode* assign = create_ast_node(AST_ASSIGN, "y");
    assign->right = create_ast_node(AST_IDENTIFIER, "x");
    
    program->body = var_decl;
    var_decl->next = assign;
    
    int result = semantic_analyze(program);
    printf("Expected: ERROR (undefined y), Got: %d\n", result);
    
    free_ast_tree(program);
    
    // Tento test CHCE error (undefined y), takže ak dostaneme error, je to OK
    return (result == SEM_ERROR_UNDEFINED) ? NO_ERROR : result;
}

int test_function_declaration() {
    printf("Testing function declaration...\n");
    
    // AST: static foo() { }
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    ASTNode* func_def = create_ast_node(AST_FUNC_DEF, "foo");
    func_def->body = create_ast_node(AST_BLOCK, NULL);
    
    program->body = func_def;
    
    int result = semantic_analyze(program);
    printf("Expected: NO_ERROR (%d), Got: %d\n", NO_ERROR, result);
    
    free_ast_tree(program);
    return result;
}

int test_undefined_variable() {
    printf("Testing undefined variable (should fail)...\n");
    
    // AST: x = 5 (x not declared)
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    ASTNode* assign = create_ast_node(AST_ASSIGN, "x");
    assign->right = create_ast_node(AST_LITERAL_INT, NULL);
    assign->right->int_val = 5;
    
    program->body = assign;
    
    int result = semantic_analyze(program);
    printf("Expected: SEM_ERROR_UNDEFINED (%d), Got: %d\n", SEM_ERROR_UNDEFINED, result);
    
    free_ast_tree(program);
    
    // Tento test CHCE error, takže ak dostaneme error, je to OK
    return (result == SEM_ERROR_UNDEFINED) ? NO_ERROR : result;
}

int test_redefinition() {
    printf("Testing variable redefinition (should fail)...\n");
    
    // AST: var x = 5; var x = 10;
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    ASTNode* var1 = create_ast_node(AST_VAR_DECL, "x");
    var1->data_type = TYPE_NUM;
    var1->right = create_ast_node(AST_LITERAL_INT, NULL);
    var1->right->int_val = 5;
    
    ASTNode* var2 = create_ast_node(AST_VAR_DECL, "x");
    var2->data_type = TYPE_NUM;
    var2->right = create_ast_node(AST_LITERAL_INT, NULL);
    var2->right->int_val = 10;
    
    program->body = var1;
    var1->next = var2;
    
    int result = semantic_analyze(program);
    printf("Expected: SEM_ERROR_REDEFINED (%d), Got: %d\n", SEM_ERROR_REDEFINED, result);
    
    free_ast_tree(program);
    
    // Tento test CHCE error, takže ak dostaneme error, je to OK
    return (result == SEM_ERROR_REDEFINED) ? NO_ERROR : result;
}

int test_builtin_functions() {
    printf("Testing built-in function call...\n");
    
    // AST: var x = inputs()
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    ASTNode* var_decl = create_ast_node(AST_VAR_DECL, "x");
    var_decl->data_type = TYPE_STRING;
    
    ASTNode* func_call = create_ast_node(AST_FUNC_CALL, "inputs");
    var_decl->right = func_call;
    
    program->body = var_decl;
    
    int result = semantic_analyze(program);
    printf("Expected: NO_ERROR (%d), Got: %d\n", NO_ERROR, result);
    
    free_ast_tree(program);
    return result;
}

int test_binary_operation() {
    printf("Testing binary operation (x + y)...\n");
    
    // AST: var x = 5; var y = 10; var z = x + y;
    ASTNode* program = create_ast_node(AST_PROGRAM, NULL);
    
    // var x = 5
    ASTNode* var_x = create_ast_node(AST_VAR_DECL, "x");
    var_x->data_type = TYPE_NUM;
    var_x->right = create_ast_node(AST_LITERAL_INT, NULL);
    var_x->right->int_val = 5;
    
    // var y = 10
    ASTNode* var_y = create_ast_node(AST_VAR_DECL, "y");
    var_y->data_type = TYPE_NUM;
    var_y->right = create_ast_node(AST_LITERAL_INT, NULL);
    var_y->right->int_val = 10;
    
    // var z = x + y
    ASTNode* var_z = create_ast_node(AST_VAR_DECL, "z");
    var_z->data_type = TYPE_NUM;
    
    ASTNode* binop = create_ast_node(AST_BINOP, "+");
    binop->left = create_ast_node(AST_IDENTIFIER, "x");
    binop->right = create_ast_node(AST_IDENTIFIER, "y");
    var_z->right = binop;
    
    program->body = var_x;
    var_x->next = var_y;
    var_y->next = var_z;
    
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
    
    // Spustenie všetkých testov
    run_test("Empty Tree", test_empty_tree);
    run_test("Simple Program", test_simple_program);
    run_test("Variable Declaration and Usage", test_variable_usage);
    run_test("Function Declaration", test_function_declaration);
    run_test("Undefined Variable (Expected Fail)", test_undefined_variable);
    run_test("Variable Redefinition (Expected Fail)", test_redefinition);
    run_test("Built-in Function Call", test_builtin_functions);
    run_test("Binary Operation", test_binary_operation);
    
    // Výpis sumáru
    print_summary();
    
    return (tests_passed == tests_total) ? 0 : 1;
}