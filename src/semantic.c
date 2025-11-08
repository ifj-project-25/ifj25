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

int semantic_visit_count = 0;

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

const char* ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_MAIN_DEF: return "MAIN_DEF";
        case AST_FUNC_DEF: return "FUNC_DEF";
        case AST_GETTER_DEF: return "GETTER_DEF";
        case AST_SETTER_DEF: return "SETTER_DEF";
        case AST_VAR_DECL: return "VAR_DECL";
        case AST_ASSIGN: return "ASSIGN";
        case AST_EQUALS: return "EQUALS";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_FUNC_CALL: return "FUNC_CALL";
        case AST_FUNC_ARG: return "FUNC_ARG";
        case AST_IF: return "IF";
        case AST_ELSE: return "ELSE";
        case AST_WHILE: return "WHILE";
        case AST_RETURN: return "RETURN";
        case AST_BLOCK: return "BLOCK";
        case AST_EXPRESSION: return "EXPRESSION";
        /*case AST_OP: return "OP";
        case AST_LITERAL_INT: return "LITERAL_INT";
        case AST_LITERAL_FLOAT: return "LITERAL_FLOAT";
        case AST_LITERAL_STRING: return "LITERAL_STRING";
        case AST_LITERAL_NULL: return "LITERAL_NULL";*/
        default: return "UNKNOWN";
    }
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



// Helper: Infer type of an expression from ExprNode
DataType infer_expr_node_type(ExprNode *expr, Scope *scope) {
    if (!expr) return TYPE_UNDEF;
    
    switch (expr->type) {
        case EXPR_NUM_LITERAL:
            return TYPE_NUM;
        
        case EXPR_STRING_LITERAL:
            return TYPE_STRING;
        
        case EXPR_NULL_LITERAL:
            return TYPE_NULL;
        
        case EXPR_IDENTIFIER: {
            SymTableData *var = lookup_symbol(scope, expr->data.identifier_name);
            if (!var || var->type != NODE_VAR) return TYPE_UNDEF;
            return var->data.var_data->data_type;
        }
        
        case EXPR_BINARY_OP: {
            DataType left = infer_expr_node_type(expr->data.binary.left, scope);
            DataType right = infer_expr_node_type(expr->data.binary.right, scope);
            
            // String concatenation: string + string = string
            if (expr->data.binary.op == OP_ADD && 
                left == TYPE_STRING && right == TYPE_STRING) {
                return TYPE_STRING;
            }
            
            // Arithmetic: num op num = num
            if (left == TYPE_NUM && right == TYPE_NUM) {
                return TYPE_NUM;
            }
            
            // Comparison operators: any compatible types -> num (boolean)
            if ((expr->data.binary.op == OP_EQ || expr->data.binary.op == OP_NEQ ||
                 expr->data.binary.op == OP_LT || expr->data.binary.op == OP_GT ||
                 expr->data.binary.op == OP_LTE || expr->data.binary.op == OP_GTE) &&
                left != TYPE_UNDEF && right != TYPE_UNDEF) {
                return TYPE_NUM; // Comparisons return numeric (0/1)
            }
            
            return TYPE_UNDEF;
        }
        
        default:
            return TYPE_UNDEF;
    }
}

// Helper: Infer type of an expression from ASTNode (new system only)
DataType infer_expression_type(ASTNode *expr, Scope *scope) {
    if (!expr) return TYPE_UNDEF;
    
    // Use only the new ExprNode system
    if (expr->expr) {
        return infer_expr_node_type(expr->expr, scope);
    }
    
    // For AST nodes without expr, we can only handle basic cases
    switch (expr->type) {
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
        
        default:
            return TYPE_UNDEF;
    }
}

// Helper function to count arguments
int count_arguments(ASTNode *arg_list) {
    int count = 0;
    ASTNode *current = arg_list;
    while (current && current->type == AST_FUNC_ARG) {
        count++;
        current = current->left; // Next argument
    }
    return count;
}

// Check built-in function call
int check_builtin_function_call(ASTNode *node, Scope *scope, const char *func_name) {
    int arg_count = count_arguments(node->left);
    
    if (strcmp(func_name, "Ifj.read_str") == 0 || strcmp(func_name, "Ifj.read_num") == 0) {
        if (arg_count != 0) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 0 arguments, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
    }
    else if (strcmp(func_name, "Ifj.write") == 0 || strcmp(func_name, "Ifj.str") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // For write and str, any type is acceptable - no type checking needed
    }
    else if (strcmp(func_name, "Ifj.floor") == 0 || strcmp(func_name, "Ifj.chr") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check if argument is numeric type
        ASTNode *first_arg = node->left;
        if (first_arg && first_arg->right) {
            DataType arg_type = infer_expression_type(first_arg->right, scope);
            if (arg_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' requires numeric argument\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.length") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check if argument is string type
        ASTNode *first_arg = node->left;
        if (first_arg && first_arg->right) {
            DataType arg_type = infer_expression_type(first_arg->right, scope);
            if (arg_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' requires string argument\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.substring") == 0) {
        if (arg_count != 3) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 3 arguments, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check argument types: string, num, num
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = infer_expression_type(arg->right, scope);
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        // Check 2nd and 3rd arguments are numeric
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = infer_expression_type(arg->left->right, scope);
            if (arg2_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be numeric\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->left && arg->left->left->right) {
            DataType arg3_type = infer_expression_type(arg->left->left->right, scope);
            if (arg3_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' third argument must be numeric\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.strcmp") == 0) {
        if (arg_count != 2) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 2 arguments, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check both arguments are strings
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = infer_expression_type(arg->right, scope);
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = infer_expression_type(arg->left->right, scope);
            if (arg2_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.ord") == 0) {
        if (arg_count != 2) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 2 arguments, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check first argument is string, second is numeric
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = infer_expression_type(arg->right, scope);
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = infer_expression_type(arg->left->right, scope);
            if (arg2_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be numeric\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
    }
    else {
        fprintf(stderr, "[SEMANTIC] Unknown built-in function '%s'\n", func_name);
        return SEM_ERROR_OTHER;
    }
    
    // Set return type for the function call node
    if (strcmp(func_name, "Ifj.read_str") == 0 || strcmp(func_name, "Ifj.str") == 0 || 
        strcmp(func_name, "Ifj.substring") == 0 || strcmp(func_name, "Ifj.chr") == 0) {
        node->data_type = TYPE_STRING;
    }
    else if (strcmp(func_name, "Ifj.read_num") == 0 || strcmp(func_name, "Ifj.floor") == 0 ||
             strcmp(func_name, "Ifj.length") == 0 || strcmp(func_name, "Ifj.strcmp") == 0 ||
             strcmp(func_name, "Ifj.ord") == 0) {
        node->data_type = TYPE_NUM;
    }
    else if (strcmp(func_name, "Ifj.write") == 0) {
        node->data_type = TYPE_NULL;
    }
    
    return NO_ERROR;
}

// Check user function call
int check_user_function_call(ASTNode *node, Scope *scope, SymTableData *func_symbol) {
    if (func_symbol->type != NODE_FUNC) {
        fprintf(stderr, "[SEMANTIC] '%s' is not a function\n", node->name);
        return SEM_ERROR_OTHER;
    }
    
    FunctionData *fdata = func_symbol->data.func_data;
    int arg_count = count_arguments(node->left);
    
    if (fdata->param_count != arg_count) {
        fprintf(stderr, "[SEMANTIC] Function '%s' expects %d arguments, got %d\n", 
                node->name, fdata->param_count, arg_count);
        return SEM_ERROR_WRONG_PARAMS;
    }
    
    // Type checking for each parameter
    ASTNode *arg_node = node->left;
    Param *param = fdata->parameters;

    while (arg_node && param) {
        if (arg_node->right) {
            DataType arg_type = infer_expression_type(arg_node->right, scope);
            if (arg_type != param->data_type && param->data_type != TYPE_UNDEF) {
                fprintf(stderr, "[SEMANTIC] Function '%s' parameter '%s' expects type %d, got %d\n", 
                        node->name, param->name, param->data_type, arg_type);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        arg_node = arg_node->left;
        param = param->next;
    }
    
    // Set return type for the function call node
    node->data_type = fdata->return_type;
    
    return NO_ERROR;
}

//---------- MAIN VISIT: Traverse AST tree ----------
int semantic_visit(ASTNode *node, Scope *current_scope) {
    if (!node) return NO_ERROR;

     semantic_visit_count++;
     
     printf("[SEMANTIC] Visiting node: %s, name: %s\n", 
           ast_node_type_to_string(node->type), 
           node->name ? node->name : "(null)");
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

                // Check for redefinition of main - allow overloading (different parameter counts)
                SymTableData *existing = lookup_symbol(current_scope, func_name);
                if (existing && existing->type == NODE_FUNC) {
                    FunctionData *fdata = existing->data.func_data;
                    if (fdata->param_count == param_count) {
                        fprintf(stderr, "[SEMANTIC] Redefinition of 'main' function with %d parameters.\n", param_count);
                        return SEM_ERROR_REDEFINED;
                    }
                    // Different parameter count - overloading allowed ✓
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


        case AST_GETTER_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                // Get getter name
                const char *getter_name = node->name;
                if (!getter_name) return ERROR_INTERNAL;

                // Check for existing GETTER with same name (only getter conflicts)
                SymTableData *existing = lookup_symbol(current_scope, getter_name);
                if (existing && existing->type == NODE_GETTER) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of getter '%s'.\n", getter_name);
                    return SEM_ERROR_REDEFINED;
                }

                // Create getter symbol - getters have 0 parameters
                SymTableData *getter_symbol = make_getter(TYPE_UNDEF, true);
                if (!getter_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for getter '%s'.\n", getter_name);
                    return ERROR_INTERNAL;
                }

                // Insert into current scope
                if (!symtable_insert(&current_scope->symbols, getter_name, getter_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert getter '%s' into symbol table.\n", getter_name);
                    return ERROR_INTERNAL;
                }

                // Create new scope for getter body
                Scope *getter_scope = init_scope();
                if (!getter_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for getter '%s'.\n", getter_name);
                    return ERROR_INTERNAL;
                }
                getter_scope->parent = current_scope;

                // Analyze getter body with getter scope
                return semantic_visit(node->right, getter_scope);
            } break;

        case AST_SETTER_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                // Get setter name
                const char *setter_name = node->name;
                if (!setter_name) return ERROR_INTERNAL;

                // Check parameters - setters should have exactly 1 parameter
                if (!node->left || node->left->type != AST_IDENTIFIER) {
                    fprintf(stderr, "[SEMANTIC] Setter '%s' must have exactly one parameter.\n", setter_name);
                    return SEM_ERROR_WRONG_PARAMS;
                }

                const char *param_name = node->left->name;
                DataType param_type = node->left->data_type;

                // Check for existing SETTER with same name (only setter conflicts)
                SymTableData *existing = lookup_symbol(current_scope, setter_name);
                if (existing && existing->type == NODE_SETTER) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of setter '%s'.\n", setter_name);
                    return SEM_ERROR_REDEFINED;
                }

                // Create setter symbol - setters have 1 parameter
                SymTableData *setter_symbol = make_setter(param_type, true);
                if (!setter_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for setter '%s'.\n", setter_name);
                    return ERROR_INTERNAL;
                }

                // Insert into current scope
                if (!symtable_insert(&current_scope->symbols, setter_name, setter_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert setter '%s' into symbol table.\n", setter_name);
                    return ERROR_INTERNAL;
                }

                // Create new scope for setter body
                Scope *setter_scope = init_scope();
                if (!setter_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for setter '%s'.\n", setter_name);
                    return ERROR_INTERNAL;
                }
                setter_scope->parent = current_scope;

                // Insert parameter into setter scope
                SymTableData *param_var = make_variable(param_type, true, true);
                if (!param_var || !symtable_insert(&setter_scope->symbols, param_name, param_var)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into setter scope.\n", param_name);
                    return ERROR_INTERNAL;
                }

                // Analyze setter body with setter scope
                return semantic_visit(node->right, setter_scope);
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

        case AST_ASSIGN: {
                // AST_ASSIGN -> left = AST_EQUALS, right = next statement
                if (!node->left || node->left->type != AST_EQUALS) {
                    fprintf(stderr, "[SEMANTIC] Invalid assignment structure\n");
                    return ERROR_INTERNAL;
                }
                
                // Process the assignment (AST_EQUALS)
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;
                
                // Continue with next statement
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_EQUALS: {
                // AST_EQUALS -> left = identifier, expr = expression
                
                if (!node->left || node->left->type != AST_IDENTIFIER) {
                    fprintf(stderr, "[SEMANTIC] Left side of assignment must be identifier\n");
                    return SEM_ERROR_OTHER;
                }
                
                const char* var_name = node->left->name;
                
                // Check if variable exists
                SymTableData* var_data = lookup_symbol(current_scope, var_name);
                if (!var_data) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s' in assignment\n", var_name);
                    return SEM_ERROR_UNDEFINED;
                }
                
                if (var_data->type != NODE_VAR) {
                    fprintf(stderr, "[SEMANTIC] '%s' is not a variable\n", var_name);
                    return SEM_ERROR_OTHER;
                }
                
                if (!node->expr) {
                    fprintf(stderr, "[SEMANTIC] Assignment to '%s' has no value\n", var_name);
                    return SEM_ERROR_OTHER;
                }
                
                DataType right_type = infer_expr_node_type(node->expr, current_scope);
                DataType left_type = var_data->data.var_data->data_type;
                
                // Type compatibility check
                if (right_type != TYPE_UNDEF && left_type != TYPE_UNDEF && 
                    right_type != left_type) {
                    fprintf(stderr, "[SEMANTIC] Type mismatch in assignment to '%s': expected %d, got %d\n",
                            var_name, left_type, right_type);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }
                
                // Mark variable as initialized
                var_data->data.var_data->initialized = true;
                
                // If variable type is undefined, infer it from the assignment
                if (left_type == TYPE_UNDEF && right_type != TYPE_UNDEF) {
                    var_data->data.var_data->data_type = right_type;
                }
                
                return NO_ERROR;
            } break;
        case AST_IDENTIFIER: {
                // AST_IDENTIFIER - check if variable exists and is initialized
                const char* var_name = node->name;
                
                if (!var_name) {
                    fprintf(stderr, "[SEMANTIC] Identifier has no name\n");
                    return ERROR_INTERNAL;
                }
                
                SymTableData* var_data = lookup_symbol(current_scope, var_name);
                
                if (!var_data) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s'\n", var_name);
                    return SEM_ERROR_UNDEFINED;
                }
                
                if (var_data->type != NODE_VAR) {
                    fprintf(stderr, "[SEMANTIC] '%s' is not a variable\n", var_name);
                    return SEM_ERROR_OTHER;
                }
                
                // Check if variable is initialized (if it's not a function parameter)
                if (!var_data->data.var_data->initialized /*&& !var_data->data.var_data->is_param*/) {
                    fprintf(stderr, "[SEMANTIC] Variable '%s' used before initialization\n", var_name);
                    return SEM_ERROR_OTHER;
                }
                
                // Set the data type for this identifier node
                node->data_type = var_data->data.var_data->data_type;
                
                // Continue with any child nodes (though identifiers shouldn't have any in normal use)
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;
                
                return semantic_visit(node->right, current_scope);
            } break;
        case AST_FUNC_CALL: {
                const char *func_name = node->name;
                
                // Check existenstion
                SymTableData *func_symbol = lookup_symbol(current_scope, func_name);
                
                if (!func_symbol) {
                    fprintf(stderr, "[SEMANTIC] Undefined function '%s'\n", func_name);
                    return SEM_ERROR_UNDEFINED;
                }
                
                int err;
                if (strncmp(func_name, "Ifj.", 4) == 0) {
                    err = check_builtin_function_call(node, current_scope, func_name);
                } else {
                    err = check_user_function_call(node, current_scope, func_symbol);
                }
                
                if (err != NO_ERROR) return err;
                
                // Continue with next program
                return semantic_visit(node->right, current_scope);
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

                // add to parameters
                SymTableData *param_data = make_variable(node->right->data_type, true, true);
                if (!param_data) return ERROR_INTERNAL;

                if (!symtable_insert(&current_scope->symbols, param_name, param_data)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into symbol table.\n", param_name);
                    return ERROR_INTERNAL;
                }

                // go trough other arguments
                return semantic_visit(node->left, current_scope);
            } break;
        case AST_IF: {
                // AST_IF -> expr = condition, right = then branch (AST_BLOCK)
                
                if (!node->expr) {
                    fprintf(stderr, "[SEMANTIC] If statement missing condition\n");
                    return SEM_ERROR_OTHER;
                }
                
                DataType cond_type = infer_expr_node_type(node->expr, current_scope);
                
                // Condition should be numeric (truthy)
                if (cond_type != TYPE_NUM && cond_type != TYPE_UNDEF) {
                    fprintf(stderr, "[SEMANTIC] If condition must be numeric expression, got type %d\n", cond_type);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }
                
                // Process then branch
                if (!node->right || node->right->type != AST_BLOCK) {
                    fprintf(stderr, "[SEMANTIC] If statement missing then block\n");
                    return ERROR_INTERNAL;
                }
                
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_ELSE: {
                // AST_ELSE -> left = NULL, right = else branch (AST_BLOCK)
                
                if (!node->right || node->right->type != AST_BLOCK) {
                    fprintf(stderr, "[SEMANTIC] Else statement missing block\n");
                    return ERROR_INTERNAL;
                }
                
                // Process else branch
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_RETURN: {
                // AST_RETURN -> expr = return expression, right = NULL
                
                DataType return_type = TYPE_NULL; // Default for void return
                
                if (node->expr) {
                    return_type = infer_expr_node_type(node->expr, current_scope);
                }
                
                // Store return type for function return type checking
                node->data_type = return_type;
                
                return NO_ERROR;
            } break;
        case AST_WHILE:{

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

        case AST_EXPRESSION: {
            if (node->expr) {
                
                DataType expr_type = infer_expr_node_type(node->expr, current_scope);
                node->data_type = expr_type;
                
                // TODO: Add type compatibility checks for binary operations
            } else {
                
                // This handles the transition period
                DataType expr_type = infer_expression_type(node->left, current_scope);
                node->data_type = expr_type;
            }
            
            return semantic_visit(node->right, current_scope);
        } break;
        /*case AST_OP:   {

            } break;
        case AST_LITERAL_INT:  {

            } break;
        case AST_LITERAL_FLOAT:{

            } break;
        case AST_LITERAL_STRING:   {

            } break;
        case AST_LITERAL_NULL: {

            } break;*/
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
