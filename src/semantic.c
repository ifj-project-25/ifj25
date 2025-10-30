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
        case AST_PROGRAM: {
            // program->left = main function
            return semantic_visit(node->left, current_scope);
        }
        
        case AST_MAIN_DEF:
        case AST_FUNC_DEF: {
            // func->left = arguments, func->right = body (AST_BLOCK)
            const char *func_name = node->name ? node->name : "main";
            
            // Check redefinition
            SymTableData *existing = symtable_search(&current_scope->symbols, func_name);
            if (existing) {
                fprintf(stderr, "[SEMANTIC] Redefinition of function '%s'\n", func_name);
                return SEM_ERROR_REDEFINED;
            }
            
            // Count parameters (linked list in node->left via AST_FUNC_ARG->left)
            int param_count = 0;
            Param *params = NULL;
            Param *last_param = NULL;
            
            ASTNode *arg_node = node->left; // AST_FUNC_ARG chain
            while (arg_node && arg_node->type == AST_FUNC_ARG) {
                // arg_node->right = parameter value/identifier
                if (arg_node->right && arg_node->right->type == AST_IDENTIFIER) {
                    Param *new_param = make_param(arg_node->right->name, 
                                                   arg_node->right->data_type);
                    if (!new_param) return ERROR_INTERNAL;
                    
                    // Check duplicate parameter names
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, new_param->name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s'\n", new_param->name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }
                    
                    if (!params) {
                        params = new_param;
                    } else {
                        last_param->next = new_param;
                    }
                    last_param = new_param;
                    param_count++;
                }
                
                arg_node = arg_node->left; // next argument
            }
            
            // Insert function into symbol table
            SymTableData *func_data = make_function(param_count, params, true, TYPE_UNDEF);
            if (!func_data || !symtable_insert(&current_scope->symbols, func_name, func_data)) {
                return ERROR_INTERNAL;
            }
            
            // Create local scope for function body
            Scope *func_scope = init_scope();
            if (!func_scope) return ERROR_INTERNAL;
            func_scope->parent = current_scope;
            
            // Insert parameters into local scope as initialized variables
            for (Param *p = params; p; p = p->next) {
                SymTableData *param_var = make_variable(p->data_type, true, true);
                if (!param_var || !symtable_insert(&func_scope->symbols, p->name, param_var)) {
                    free_scope(func_scope);
                    return ERROR_INTERNAL;
                }
            }
            
            // Analyze function body
            int err = semantic_visit(node->right, func_scope);
            free_scope(func_scope);
            return err;
        }
        
        case AST_GETTER_DEF:
        case AST_SETTER_DEF: {
            // getter/setter->right = body
            // TODO: Implement getter/setter logic
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_VAR_DECL: {
            // var_decl->left = AST_IDENTIFIER, var_decl->right = next statement
            if (!node->left || node->left->type != AST_IDENTIFIER) {
                return ERROR_INTERNAL;
            }
            
            const char *var_name = node->left->name;
            
            // Check redefinition
            SymTableData *existing = symtable_search(&current_scope->symbols, var_name);
            if (existing) {
                fprintf(stderr, "[SEMANTIC] Redefinition of variable '%s'\n", var_name);
                return SEM_ERROR_REDEFINED;
            }
            
            // Insert variable (not initialized yet)
            DataType var_type = node->left->data_type;
            SymTableData *var_data = make_variable(var_type, true, false);
            if (!var_data || !symtable_insert(&current_scope->symbols, var_name, var_data)) {
                return ERROR_INTERNAL;
            }
            
            // Continue to next statement
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_ASSIGN: {
            // assign->left = AST_EQUALS, assign->right = next statement
            int err = semantic_visit(node->left, current_scope);
            if (err != NO_ERROR) return err;
            
            // Continue to next statement
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_EQUALS: {
            // equals->left = AST_IDENTIFIER, equals->right = expression
            if (!node->left || node->left->type != AST_IDENTIFIER) {
                return ERROR_INTERNAL;
            }
            
            const char *var_name = node->left->name;
            
            // Check if variable exists
            SymTableData *var = lookup_symbol(current_scope, var_name);
            if (!var) {
                fprintf(stderr, "[SEMANTIC] Undefined variable '%s'\n", var_name);
                return SEM_ERROR_UNDEFINED;
            }
            
            if (var->type != NODE_VAR) {
                fprintf(stderr, "[SEMANTIC] '%s' is not a variable\n", var_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
            
            // Analyze right-hand side expression
            int err = semantic_visit(node->right, current_scope);
            if (err != NO_ERROR) return err;
            
            // Infer type of expression
            DataType expr_type = infer_expression_type(node->right, current_scope);
            if (expr_type == TYPE_UNDEF) {
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
            
            // Check type compatibility
            VariableData *var_data = var->data.var_data;
            if (var_data->data_type != TYPE_UNDEF && expr_type != TYPE_UNDEF &&
                var_data->data_type != expr_type) {
                fprintf(stderr, "[SEMANTIC] Type mismatch in assignment to '%s'\n", var_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
            
            // Mark variable as initialized
            var_data->initialized = true;
            if (var_data->data_type == TYPE_UNDEF) {
                var_data->data_type = expr_type;
            }
            
            return NO_ERROR;
        }
        
        case AST_IDENTIFIER: {
            // Check if variable exists and is initialized
            SymTableData *var = lookup_symbol(current_scope, node->name);
            if (!var) {
                fprintf(stderr, "[SEMANTIC] Undefined identifier '%s'\n", node->name);
                return SEM_ERROR_UNDEFINED;
            }
            
            if (var->type == NODE_VAR && !var->data.var_data->initialized) {
                fprintf(stderr, "[SEMANTIC] Variable '%s' used before initialization\n", node->name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
            
            return NO_ERROR;
        }
        
        case AST_FUNC_CALL: {
            // func_call->left = AST_FUNC_ARG (arguments), func_call->right = next statement
            
            // Lookup function
            SymTableData *func = lookup_symbol(current_scope, node->name);
            if (!func || func->type != NODE_FUNC) {
                fprintf(stderr, "[SEMANTIC] Undefined function '%s'\n", node->name);
                return SEM_ERROR_UNDEFINED;
            }
            
            FunctionData *fn = func->data.func_data;
            
            // Count arguments (linked list via arg->left)
            int arg_count = 0;
            ASTNode *arg = node->left;
            while (arg && arg->type == AST_FUNC_ARG) {
                // Analyze argument value (arg->right)
                int err = semantic_visit(arg->right, current_scope);
                if (err != NO_ERROR) return err;
                
                arg_count++;
                arg = arg->left; // next argument
            }
            
            // Check argument count
            if (fn->param_count >= 0 && arg_count != fn->param_count) {
                fprintf(stderr, "[SEMANTIC] Function '%s' expects %d args, got %d\n",
                        node->name, fn->param_count, arg_count);
                return SEM_ERROR_WRONG_PARAMS;
            }
            
            // TODO: Check argument types
            
            // Continue to next statement
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_FUNC_ARG: {
            // arg->left = next argument, arg->right = value
            // Analyze the value
            int err = semantic_visit(node->right, current_scope);
            if (err != NO_ERROR) return err;
            
            // Continue to next argument
            return semantic_visit(node->left, current_scope);
        }
        
        case AST_IF: {
            // if->left = condition (AST_EXPRESSION), if->right = AST_BLOCK (then)
            
            // Analyze condition
            int err = semantic_visit(node->left, current_scope);
            if (err != NO_ERROR) return err;
            
            // Analyze then branch
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_ELSE: {
            // else->right = AST_BLOCK
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_WHILE: {
            // while->left = condition, while->right = AST_BLOCK
            
            // Analyze condition
            int err = semantic_visit(node->left, current_scope);
            if (err != NO_ERROR) return err;
            
            // Analyze body
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_RETURN: {
            // return->left = expression
            // TODO: Check if inside function
            return semantic_visit(node->left, current_scope);
        }
        
        case AST_BLOCK: {
            // block->left = statements (chain via ->right), block->right = next (AST_ELSE or NULL)
            
            // Analyze statements inside block
            int err = semantic_visit(node->left, current_scope);
            if (err != NO_ERROR) return err;
            
            // Continue to next (e.g., else branch)
            return semantic_visit(node->right, current_scope);
        }
        
        case AST_EXPRESSION: {
            // expression->left = AST_OP or value
            return semantic_visit(node->left, current_scope);
        }
        
        case AST_OP: {
            // op->left = left operand, op->right = right operand
            int err = semantic_visit(node->left, current_scope);
            if (err != NO_ERROR) return err;
            
            return semantic_visit(node->right, current_scope);
        }
        
        // Literals don't need checking
        case AST_LITERAL_INT:
        case AST_LITERAL_FLOAT:
        case AST_LITERAL_STRING:
        case AST_LITERAL_NULL:
            return NO_ERROR;
        
        default:
            fprintf(stderr, "[SEMANTIC] Unhandled AST node type: %d\n", node->type);
            return NO_ERROR;
    }
}


/**
 * @brief Performs a semantic analysis .
 * @param root Root node of the symbol table or parsed program structure.
 * @return SemanticResult error code or NO_ERROR if everything is valid.
 */
int semantic_analyze(ASTNode *root) {
    if (!root) {
        fprintf(stderr, "[SEMANTIC] Empty AST tree.\n");
        return SEM_ERROR_OTHER;
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
