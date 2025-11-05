/**
 * @file semantic.c
 * @brief Implementation of semantic analysis
 * @author xmalikm00
 */

#include "semantic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

//Debug - print all AST nodes
void print_all_symbols(ASTNode *node) {
    if (!node) {
        return;
    }
    printf("AST Node type: %d", node->type);
    if (node->name) printf(", name: %s", node->name);
    printf("\n");
    
    print_all_symbols(node->left);
    print_all_symbols(node->right);
    
}

// Function to initialize a new scope
Scope* init_scope(){
    Scope* scope = (Scope*)malloc(sizeof(Scope));
    if (!scope) {
        return NULL;
    }
    symtable_init(&scope->symbols);
    scope->parent = NULL;
    return scope;
}
// Function to free a scope and its symbol table recursively
void free_scope(Scope* scope) {
    if (!scope) return;
    symtable_free(&scope->symbols);
    free(scope);
}

//---------- HELPER: Search symbols in table hierarchy ----------
SymTableData* lookup_symbol(Scope *scope, const char *name) {
    while (scope) {
        SymTableData *data = symtable_search(&scope->symbols, name);
        if (data) return data;
        scope = scope->parent;
    }
    return NULL;
}

// Preload built-in functions (in Ifj namespace)
void preload_builtins(Scope *global_scope) {
    // 1. Ifj.read_str() -> String | Null
    SymTableData *read_str = make_function(0, NULL, true, TYPE_STRING);
    symtable_insert(&global_scope->symbols, "Ifj.read_str", read_str);
    
    // 2. Ifj.read_num() -> Num | Null
    SymTableData *read_num = make_function(0, NULL, true, TYPE_NUM);
    symtable_insert(&global_scope->symbols, "Ifj.read_num", read_num);
    
    // 3. Ifj.write(term) -> Null (accepts any type)
    Param *write_param = make_param("term", TYPE_UNDEF); // any type
    SymTableData *write = make_function(1, write_param, true, TYPE_NULL);
    symtable_insert(&global_scope->symbols, "Ifj.write", write);
    
    // 4. Ifj.floor(term: Num) -> Num
    Param *floor_param = make_param("term", TYPE_NUM);
    SymTableData *floor = make_function(1, floor_param, true, TYPE_NUM);
    symtable_insert(&global_scope->symbols, "Ifj.floor", floor);
    
    // 5. Ifj.str(term) -> String (accepts any type)
    Param *str_param = make_param("term", TYPE_UNDEF);
    SymTableData *str = make_function(1, str_param, true, TYPE_STRING);
    symtable_insert(&global_scope->symbols, "Ifj.str", str);
    
    // 6. Ifj.length(s: String) -> Num
    Param *length_param = make_param("s", TYPE_STRING);
    SymTableData *length = make_function(1, length_param, true, TYPE_NUM);
    symtable_insert(&global_scope->symbols, "Ifj.length", length);
    
    // 7. Ifj.substring(s: String, i: Num, j: Num) -> String | Null
    Param *substr_p1 = make_param("s", TYPE_STRING);
    Param *substr_p2 = make_param("i", TYPE_NUM);
    Param *substr_p3 = make_param("j", TYPE_NUM);
    substr_p1->next = substr_p2;
    substr_p2->next = substr_p3;
    SymTableData *substring = make_function(3, substr_p1, true, TYPE_STRING);
    symtable_insert(&global_scope->symbols, "Ifj.substring", substring);
    
    // 8. Ifj.strcmp(s1: String, s2: String) -> Num
    Param *strcmp_p1 = make_param("s1", TYPE_STRING);
    Param *strcmp_p2 = make_param("s2", TYPE_STRING);
    strcmp_p1->next = strcmp_p2;
    SymTableData *strcmp = make_function(2, strcmp_p1, true, TYPE_NUM);
    symtable_insert(&global_scope->symbols, "Ifj.strcmp", strcmp);
    
    // 9. Ifj.ord(s: String, i: Num) -> Num
    Param *ord_p1 = make_param("s", TYPE_STRING);
    Param *ord_p2 = make_param("i", TYPE_NUM);
    ord_p1->next = ord_p2;
    SymTableData *ord = make_function(2, ord_p1, true, TYPE_NUM);
    symtable_insert(&global_scope->symbols, "Ifj.ord", ord);
    
    // 10. Ifj.chr(i: Num) -> String
    Param *chr_param = make_param("i", TYPE_NUM);
    SymTableData *chr = make_function(1, chr_param, true, TYPE_STRING);
    symtable_insert(&global_scope->symbols, "Ifj.chr", chr);
}



// Helper: Infer type of an expression
DataType infer_expression_type(ASTNode *expr, Scope *scope) {
    if (!expr) return TYPE_UNDEF;
    
    switch (expr->type) {
        case AST_LITERAL_INT:
        case AST_LITERAL_FLOAT:
            return TYPE_NUM;
        
        case AST_LITERAL_STRING:
            return TYPE_STRING;
        
        case AST_LITERAL_NULL:
            return TYPE_NULL;
        
        case AST_IDENTIFIER: {
            SymTableData *var = lookup_symbol(scope, expr->name);
            if (!var || var->type != NODE_VAR) return TYPE_UNDEF;
            return var->data.var_data->data_type;
        }
        
        case AST_FUNC_CALL: {
            SymTableData *func = lookup_symbol(scope, expr->name);
            if (!func || func->type != NODE_FUNC) return TYPE_UNDEF;
            return func->data.func_data->return_type;
        }
        
        case AST_EXPRESSION:
            // Recursively check the operator inside
            return infer_expression_type(expr->left, scope);
        
        case AST_OP: {
            DataType left = infer_expression_type(expr->left, scope);
            DataType right = infer_expression_type(expr->right, scope);
            
            // String concatenation: string + string = string
            if (strcmp(expr->name, "+") == 0 && 
                left == TYPE_STRING && right == TYPE_STRING) {
                return TYPE_STRING;
            }
            
            // Arithmetic: num op num = num
            if (left == TYPE_NUM && right == TYPE_NUM) {
                return TYPE_NUM;
            }
            
            return TYPE_UNDEF;
        }
        
        default:
            return TYPE_UNDEF;
    }
}


//---------- MAIN VISIT: Traverse AST tree ----------
int semantic_visit(ASTNode *node, Scope *current_scope) {
    if (!node) return NO_ERROR;

    switch (node->type) {
        case AST_PROGRAM:   {
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;
                
                return semantic_visit(node->right, current_scope);
            } break;
        case AST_MAIN_DEF: {
                const char *func_name = "main";

                // Count parameters
                int param_count = 0;
                Param *params = NULL;
                Param *last_param = NULL;

                ASTNode *arg_node = node->left;
                while (arg_node && arg_node->type == AST_FUNC_ARG) {
                    if (!arg_node->right || arg_node->right->type != AST_IDENTIFIER) {
                        fprintf(stderr, "[SEMANTIC] Invalid argument node in main().\n");
                        return ERROR_INTERNAL;
                    }

                    const char *param_name = arg_node->right->name;
                    DataType param_type = arg_node->right->data_type;

                    Param *new_param = make_param(param_name, param_type);
                    if (!new_param) return ERROR_INTERNAL;

                    // Check for duplicates
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in main().\n", param_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    if (!params)
                        params = new_param;
                    else
                        last_param->next = new_param;
                    last_param = new_param;
                    param_count++;

                    arg_node = arg_node->left;
                }

                // Check for redefinition of main - use lookup_symbol to check all parent scopes
                SymTableData *existing = lookup_symbol(current_scope, func_name);
                if (existing && existing->type == NODE_FUNC) {
                    // For main, we don't allow overloading - any main function is error
                    fprintf(stderr, "[SEMANTIC] Redefinition of 'main' function.\n");
                    return SEM_ERROR_REDEFINED;
                }

                // Create and insert main symbol 
                SymTableData *main_symbol = make_function(param_count, params, true, TYPE_UNDEF);
                if (!main_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for 'main'.\n");
                    return ERROR_INTERNAL;
                }

                if (!symtable_insert(&current_scope->symbols, func_name, main_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert 'main' into global symbol table.\n");
                    return ERROR_INTERNAL;
                }

                // Create new scope for main function body
                Scope *main_scope = init_scope();
                if (!main_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for 'main'.\n");
                    return ERROR_INTERNAL;
                }
                main_scope->parent = current_scope;

                // Insert parameters into main scope
                for (Param *p = params; p; p = p->next) {
                    SymTableData *param_var = make_variable(p->data_type, true, true);
                    if (!param_var || !symtable_insert(&main_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into main scope.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                }

                // Analyze main function body with main scope
                return semantic_visit(node->right, main_scope);
            } break;

        case AST_FUNC_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                // Get function name
                const char *func_name = node->name;
                if (!func_name) return ERROR_INTERNAL;

                // Count parameters
                int param_count = 0;
                Param *params = NULL;
                Param *last_param = NULL;

                ASTNode *arg_node = node->left;
                while (arg_node && arg_node->type == AST_FUNC_ARG) {
                    if (!arg_node->right || arg_node->right->type != AST_IDENTIFIER) {
                        fprintf(stderr, "[SEMANTIC] Invalid argument node in function '%s'.\n", func_name);
                        return ERROR_INTERNAL;
                    }

                    const char *param_name = arg_node->right->name;
                    DataType param_type = arg_node->right->data_type;

                    // Create parameter structure
                    Param *new_param = make_param(param_name, param_type);
                    if (!new_param) return ERROR_INTERNAL;

                    // Check for duplicate parameter names
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in function '%s'.\n",
                                    param_name, func_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    if (!params)
                        params = new_param;
                    else
                        last_param->next = new_param;
                    last_param = new_param;

                    param_count++;
                    arg_node = arg_node->left;
                }

                // Check for redefinition 
                SymTableData *existing = lookup_symbol(current_scope, func_name);
                if (existing && existing->type == NODE_FUNC) {
                    FunctionData *fdata = existing->data.func_data;
                    if (fdata->param_count == param_count) {
                        fprintf(stderr, "[SEMANTIC] Redefinition of function '%s' with %d parameters.\n",
                                func_name, param_count);
                        return SEM_ERROR_REDEFINED;
                    }
                }
                // Create symbol for the function
                SymTableData *func_symbol = make_function(param_count, params, true, TYPE_UNDEF);
                if (!func_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for function '%s'.\n", func_name);
                    return ERROR_INTERNAL;
                }

                // Insert into the current (global) scope
                if (!symtable_insert(&current_scope->symbols, func_name, func_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert function '%s' into symbol table.\n", func_name);
                    return ERROR_INTERNAL;
                }

                // Create new scope for function body
                Scope *func_scope = init_scope();
                if (!func_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for function '%s'.\n", func_name);
                    return ERROR_INTERNAL;
                }
                func_scope->parent = current_scope;

                // Insert parameters into function scope
                for (Param *p = params; p; p = p->next) {
                    SymTableData *param_var = make_variable(p->data_type, true, true);
                    if (!param_var || !symtable_insert(&func_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into function scope.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                }

                // Analyze function body with function scope
                // The AST_BLOCK will handle its own scope creation and cleanup
                return semantic_visit(node->right, func_scope);
            } break;


        case AST_GETTER_DEF:   {

            } break;
        case AST_SETTER_DEF:   {

            } break;
        case AST_VAR_DECL: {
                if (!node->left || node->left->type != AST_IDENTIFIER ) return ERROR_INTERNAL;

                const char *name = node->left->name;
                SymTableData *existing = symtable_search(&current_scope->symbols, name);

                // Check for redefinition
                if (existing) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of symbol: %s\n", name);
                    return SEM_ERROR_REDEFINED;
                }
                DataType var_type = node->left->data_type;

                // Create variable symbol
                SymTableData *var_data = make_variable(var_type, true, false);
                if (!var_data) {
                    fprintf(stderr, "[SEMANTIC] Memory allocation failed for variable: %s\n", name);
                    return ERROR_INTERNAL;
                }

                // Insert into current scope's symbol table
                if(!symtable_insert(&current_scope->symbols, name, var_data)){
                    fprintf(stderr, "[SEMANTIC] Failed to insert variable into symbol table: %s\n", name);
                    free(var_data);
                    return ERROR_INTERNAL;
                }
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_ASSIGN:   {

            } break;
        case AST_EQUALS:   {

            } break;
        case AST_IDENTIFIER:   {
                
            } break;
        case AST_FUNC_CALL:{

            } break;
        case AST_FUNC_ARG: {
            // Checks argument structure
            if (!node->right || node->right->type != AST_IDENTIFIER) {
                fprintf(stderr, "[SEMANTIC] Invalid argument structure.\n");
                return ERROR_INTERNAL;
            }

            // Argument name check
            const char *param_name = node->right->name;
            SymTableData *existing = symtable_search(&current_scope->symbols, param_name);
            if (existing) {
                fprintf(stderr, "[SEMANTIC] Parameter '%s' already declared in this scope.\n", param_name);
                return SEM_ERROR_REDEFINED;
            }

            // Pridaj parameter do aktuálneho scope
            SymTableData *param_data = make_variable(node->right->data_type, true, true);
            if (!param_data) return ERROR_INTERNAL;

            if (!symtable_insert(&current_scope->symbols, param_name, param_data)) {
                fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into symbol table.\n", param_name);
                return ERROR_INTERNAL;
            }

            // Prejdi reťaz ďalších argumentov
            return semantic_visit(node->left, current_scope);
            } break;
        case AST_IF:   {

            } break;
        case AST_ELSE: {

            } break;
        case AST_WHILE:{

            } break;
        case AST_RETURN:   {

            } break;
        case AST_BLOCK:{
            // New scope init 
            Scope* block_scope = current_scope;

            //bool created_new_scope = false;

            if (!node->current_table) {
                block_scope = init_scope();
                if (!block_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to initialize block scope.\n");
                    return ERROR_INTERNAL;
                }
                block_scope->parent = current_scope;
                node->current_table = &block_scope->symbols;
                //created_new_scope = true;
            }

            // Search inside of scope
            int err = semantic_visit(node->left, block_scope);
            // Error handle
            if (err != NO_ERROR) return err;
            
            return semantic_visit(node->right, current_scope);
        
            } break;

            // TODO: implement expressions later
        case AST_EXPRESSION:   {

            } break;
        case AST_OP:   {

            } break;
        case AST_LITERAL_INT:  {

            } break;
        case AST_LITERAL_FLOAT:{

            } break;
        case AST_LITERAL_STRING:   {

            } break;
        case AST_LITERAL_NULL: {

            } break;
            // TODO: implement handling for each case
            break;

        default:
            fprintf(stderr, "[SEMANTIC] Unhandled AST node type: %d\n", node->type);
            return ERROR_INTERNAL;
            break;
    }

    return NO_ERROR;
}



/**
 * @brief Performs a semantic analysis .
 * @param root Root node of the symbol table or parsed program structure.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_analyze(ASTNode *root) {
    if (!root) {
        fprintf(stderr, "[SEMANTIC] Empty AST tree.\n");
        return ERROR_INTERNAL;
    }
    
    // Debug: print all nodes
    print_all_symbols(root);
    
    // Initialize global scope
    Scope* global_scope = init_scope();
    if (!global_scope) {
        fprintf(stderr, "[SEMANTIC] Failed to initialize global scope.\n");
        return ERROR_INTERNAL;
    }
    
    // Preload built-in functions
    preload_builtins(global_scope);
    
    // Analyze AST
    int result = semantic_visit(root, global_scope);
    
    // Cleanup
    free_scope(global_scope);
    
    return result;
}
