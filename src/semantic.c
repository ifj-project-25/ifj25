/**
 * @file semantic.c
 * @brief Implementation of semantic analysis
 * @author xmalikm00
 */

#include "semantic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int semantic_visit_count = 0;

ASTNode *func_node; // Global root node for AST

// Ensure every identifier expression carries the scope it resolves in.
static void annotate_expr_scopes(ExprNode *expr, Scope *scope) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_IDENTIFIER: {
            if (!expr->current_scope) {
                SymTableData *data = lookup_symbol(scope, expr->data.identifier_name);
                if (data && data->type == NODE_VAR && data->data.var_data) {
                    expr->current_scope = data->data.var_data->scope ? data->data.var_data->scope : scope;
                }
            }
            break;
        }
        case EXPR_BINARY_OP: 
            annotate_expr_scopes(expr->data.binary.left, scope);
            annotate_expr_scopes(expr->data.binary.right, scope);
            break;
        default:
            break;
    }
}

// Helper functions to create getter/setter keys in symtable
static char* make_getter_key(const char *name) {
    size_t len = strlen(name) + 5; // "#get" + '\0'
    char *key = malloc(len);
    if (!key) return NULL;
    snprintf(key, len, "%s$get", name);
    return key;
}

static char* make_setter_key(const char *name) {
    size_t len = strlen(name) + 5; // "$set" + '\0'
    char *key = malloc(len);
    if (!key) return NULL;
    snprintf(key, len, "%s$set", name);
    return key;
}

// Helper: search AVL tree for any key starting with given prefix
static bool symtable_has_key_prefix(SNode *node, const char *prefix) {
    if (!node) return false;
    size_t plen = strlen(prefix);
    if (strncmp(node->key, prefix, plen) == 0) {
        if (node->data && node->data->type == NODE_FUNC) return true;
    }
    // DFS
    if (symtable_has_key_prefix(node->left, prefix)) return true;
    if (symtable_has_key_prefix(node->right, prefix)) return true;
    return false;
}

// Helper: does any overload of function name exist (any arity) in current or parent scopes?
static bool any_arity_function_exists(Scope *scope, const char *func_name) {
    char prefix[128];
    snprintf(prefix, sizeof(prefix), "%s$", func_name);
    for (Scope *s = scope; s; s = s->parent) {
        if (symtable_has_key_prefix(s->symbols.root, prefix)) return true;
    }
    return false;
}

// Simple global flag to track presence of main() with 0 params
static bool main_zero_defined = false;

void free_scope(Scope* scope) {
    if (!scope) return;
    
    // Free symbol table
    symtable_free(&scope->symbols);
    
    // Free the scope itself
    free(scope);
}

/**
 * @brief Frees all semantic analysis resources including AST and symbol tables
 * @param root Root node of the AST tree
 * @param global_scope Global scope to free (can be NULL)
 */
void free_semantic_resources(ASTNode *root, Scope *global_scope) {
    if (global_scope) {
        free_scope(global_scope);
    }
    
    if (root) {
        free_ast_tree(root);  // Použije existujúcu funkciu z ast.c
    }
}

//Debug - print all AST nodes
void print_all_symbols(ASTNode *node) {
    if (!node) {
        return;
    }
    printf("AST Node type: %s", ast_node_type_to_string(node->type));
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
        case AST_SETTER_CALL: return "SETTER_CALL";
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
/*void free_scope(Scope* scope) {
    if (!scope) return;
    symtable_free(&scope->symbols);
    free(scope);
}*/

//---------- HELPER: Search symbols in table hierarchy ----------
SymTableData* lookup_symbol(Scope *scope, const char *name) {
    while (scope) {
        SymTableData *data = symtable_search(&scope->symbols, name);
        if (data) return data;
        scope = scope->parent;
    }
    return NULL;
}

void preload_builtins(Scope *global_scope) {

    char keybuf[64];

    // 1. Ifj.read_str() -> String
    SymTableData *read_str = make_function(0, NULL, true, TYPE_STRING);
    snprintf(keybuf, sizeof(keybuf), "Ifj.read_str$0");
    symtable_insert(&global_scope->symbols, keybuf, read_str);

    // 2. Ifj.read_num() -> Num
    SymTableData *read_num = make_function(0, NULL, true, TYPE_NUM);
    snprintf(keybuf, sizeof(keybuf), "Ifj.read_num$0");
    symtable_insert(&global_scope->symbols, keybuf, read_num);

    // 3. Ifj.write(term) -> Null
    Param *write_param = make_param("term", TYPE_UNDEF);
    SymTableData *write = make_function(1, write_param, true, TYPE_NULL);
    snprintf(keybuf, sizeof(keybuf), "Ifj.write$1");
    symtable_insert(&global_scope->symbols, keybuf, write);

    // 4. Ifj.floor(term: Num) -> Num
    Param *floor_param = make_param("term", TYPE_NUM);
    SymTableData *floor = make_function(1, floor_param, true, TYPE_NUM);
    snprintf(keybuf, sizeof(keybuf), "Ifj.floor$1");
    symtable_insert(&global_scope->symbols, keybuf, floor);

    // 5. Ifj.str(term) -> String
    Param *str_param = make_param("term", TYPE_UNDEF);
    SymTableData *str = make_function(1, str_param, true, TYPE_STRING);
    snprintf(keybuf, sizeof(keybuf), "Ifj.str$1");
    symtable_insert(&global_scope->symbols, keybuf, str);

    // 6. Ifj.length(s: String) -> Num
    Param *length_param = make_param("s", TYPE_STRING);
    SymTableData *length = make_function(1, length_param, true, TYPE_NUM);
    snprintf(keybuf, sizeof(keybuf), "Ifj.length$1");
    symtable_insert(&global_scope->symbols, keybuf, length);

    // 7. Ifj.substring(s: String, i: Num, j: Num) -> String
    Param *p1 = make_param("s", TYPE_STRING);
    Param *p2 = make_param("i", TYPE_NUM);
    Param *p3 = make_param("j", TYPE_NUM);
    p1->next = p2; p2->next = p3;
    SymTableData *substring = make_function(3, p1, true, TYPE_STRING);
    snprintf(keybuf, sizeof(keybuf), "Ifj.substring$3");
    symtable_insert(&global_scope->symbols, keybuf, substring);

    // 8. Ifj.strcmp(s1, s2) -> Num
    Param *s1 = make_param("s1", TYPE_STRING);
    Param *s2 = make_param("s2", TYPE_STRING);
    s1->next = s2;
    SymTableData *strcmp = make_function(2, s1, true, TYPE_NUM);
    snprintf(keybuf, sizeof(keybuf), "Ifj.strcmp$2");
    symtable_insert(&global_scope->symbols, keybuf, strcmp);

    // 9. Ifj.ord(s, i) -> Num
    Param *ord1 = make_param("s", TYPE_STRING);
    Param *ord2 = make_param("i", TYPE_NUM);
    ord1->next = ord2;
    SymTableData *ord = make_function(2, ord1, true, TYPE_NUM);
    snprintf(keybuf, sizeof(keybuf), "Ifj.ord$2");
    symtable_insert(&global_scope->symbols, keybuf, ord);

    // 10. Ifj.chr(i) -> String
    Param *chr_param = make_param("i", TYPE_NUM);
    SymTableData *chr = make_function(1, chr_param, true, TYPE_STRING);
    snprintf(keybuf, sizeof(keybuf), "Ifj.chr$1");
    symtable_insert(&global_scope->symbols, keybuf, chr);
}


int check_uninitialized_usage(ExprNode* expr, Scope* scope) {
    if (!expr) return NO_ERROR;
    
    if (expr->type == EXPR_IDENTIFIER) {
        SymTableData* data = lookup_symbol(scope, expr->data.identifier_name);
        if (data && data->data.var_data && !data->data.var_data->initialized) {
            fprintf(stderr, "[SEMANTIC] Use of uninitialized variable '%s'\n", expr->data.identifier_name);
            return SEM_ERROR_OTHER;
        }
    }
    
    // Rekurzívne skontroluj podvýrazy
    if (expr->type == EXPR_BINARY_OP) {
        int err = check_uninitialized_usage(expr->data.binary.left, scope);
        if (err != NO_ERROR) return err;
        return check_uninitialized_usage(expr->data.binary.right, scope);
    }
    
    return NO_ERROR;
}

// Helper: Infer type of an expression from ExprNode
int infer_expr_node_type(ExprNode *expr, Scope *scope, DataType *out_type) {
    if (!out_type) return ERROR_INTERNAL;
    *out_type = TYPE_UNDEF;
    if (!expr) return NO_ERROR; // nothing to infer

    switch (expr->type) {
        case EXPR_NUM_LITERAL:
            *out_type = TYPE_NUM;
            return NO_ERROR;

        case EXPR_STRING_LITERAL:
            *out_type = TYPE_STRING;
            return NO_ERROR;

        case EXPR_NULL_LITERAL:
            *out_type = TYPE_NULL;
            return NO_ERROR;

        case EXPR_TYPE_LITERAL:
            // Type literals (Num, String, Null) used in 'is' operator
            *out_type = TYPE_UNDEF;
            return NO_ERROR;

        case EXPR_IDENTIFIER:
            {
                // First try to find as variable
                SymTableData *identifier = lookup_symbol(scope, expr->data.identifier_name);
                
                // If not found as variable, try as getter with $get suffix
                if (!identifier) {
                    char *getter_key = make_getter_key(expr->data.identifier_name);
                    if (getter_key) {
                        identifier = lookup_symbol(scope, getter_key);
                        free(getter_key);
                    }
                }

                if (!identifier && expr->data.identifier_name[0] == '_' && expr->data.identifier_name[1] == '_') {
                    // search to program root
                    Scope* global_scope = scope;
                    while (global_scope && global_scope->parent) {
                        global_scope = global_scope->parent;
                    }
                    
                    
                    // make global variable
                    SymTableData* global_var = make_variable(TYPE_UNDEF, true, false); // defined=true, initialized=false
                    if (!global_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate global variable '%s'\n", expr->data.identifier_name);
                        return ERROR_INTERNAL;
                    }
                    // Bind the new implicit global to the program root scope
                    global_var->data.var_data->scope = global_scope;
                    
                    // insert into global scope
                    if (!symtable_insert(&global_scope->symbols, expr->data.identifier_name, global_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert global variable '%s'\n", expr->data.identifier_name);
                        free(global_var);
                        return ERROR_INTERNAL;
                    }
                    
                    /*printf("[SEMANTIC] Automatically created global variable '%s'\n", expr->data.identifier_name);*/
                    identifier = global_var;
                }
                
                if (!identifier){
                    fprintf(stderr, "[SEMANTIC] Identifier '%s' not found in expression\n", expr->data.identifier_name);
                    return SEM_ERROR_UNDEFINED;
                }
                if (identifier->type == NODE_VAR) {
                    expr->current_scope = identifier->data.var_data->scope;
                    if (!expr->current_scope) {
                        // Fallback: use the lookup scope if var_data is missing it
                        expr->current_scope = scope;
                    }
                    *out_type = identifier->data.var_data->data_type;
                    return NO_ERROR;
                } else if (identifier->type == NODE_GETTER) {
                    // Mutate identifier expr into a getter-call expr so codegen
                    // emits a getter call instead of a plain variable read.
                    char *name_copy = expr->data.identifier_name;
                    expr->type = EXPR_GETTER_CALL;
                    expr->data.getter_name = name_copy;
                    expr->current_scope = scope;
                    *out_type = identifier->data.getter_data->return_type;
                    return NO_ERROR;
                } else {
                    // Not a variable or getter (could be func/setter) - treat as undef
                    *out_type = TYPE_UNDEF;
                    return NO_ERROR;
                }
            }

        case EXPR_GETTER_CALL: {
            char *getter_key = make_getter_key(expr->data.getter_name);
            if (!getter_key) return ERROR_INTERNAL;
            
            SymTableData *g = lookup_symbol(scope, getter_key);
            free(getter_key);
            
            if (!g) {
                fprintf(stderr, "[SEMANTIC] Getter '%s' not found\n", expr->data.getter_name);
                return SEM_ERROR_UNDEFINED;
            }
            if (g->type != NODE_GETTER) {
                fprintf(stderr, "[SEMANTIC] '%s' is not a getter\n", expr->data.getter_name);
                return SEM_ERROR_OTHER;
            }
            *out_type = g->data.getter_data->return_type;
            return NO_ERROR;
        }

        case EXPR_BINARY_OP: {
            DataType left, right;
            int err = infer_expr_node_type(expr->data.binary.left, scope, &left);
            if (err != NO_ERROR) return err;
            err = infer_expr_node_type(expr->data.binary.right, scope, &right);
            if (err != NO_ERROR) return err;

            // If either operand has undefined type, return UNDEF (not an error)
            if (left == TYPE_UNDEF || right == TYPE_UNDEF) {
                *out_type = TYPE_UNDEF;
                return NO_ERROR;
            }

            BinaryOpType op = expr->data.binary.op;

            switch (op) {
                // Equality operators - always return TYPE_NUM (boolean)
                // NULL je povolený v rovnostných operátoroch
                case OP_EQ:
                case OP_NEQ:
                    *out_type = TYPE_NUM;
                    return NO_ERROR;

                // Relational operators - require NUM and return TYPE_NUM
                case OP_LT:
                case OP_GT:
                case OP_LTE:
                case OP_GTE:
                    // NULL not allowed in relation op
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in relational operator %d\n", op);
                        return SEM_ERROR_TYPE_COMPATIBILITY;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) {
                        *out_type = TYPE_NUM;
                        return NO_ERROR;
                    }
                    break;

                // Type test operator - always returns TYPE_NUM
                case OP_IS:
                    if (expr->data.binary.right->type != EXPR_TYPE_LITERAL) {
                        fprintf(stderr, "[SEMANTIC] Right operand of 'is' must be a type literal\n");
                        return SEM_ERROR_OTHER;
                    }
                    else if(strcmp(expr->data.binary.right->data.identifier_name, "Num") == 0 ){
                        *out_type = TYPE_NUM;
                        return NO_ERROR;
                    }
                    else if(strcmp(expr->data.binary.right->data.identifier_name, "String") == 0 ){
                        *out_type = TYPE_STRING;
                        return NO_ERROR;
                    }
                    else if(strcmp(expr->data.binary.right->data.identifier_name, "Null") == 0 ){
                        *out_type = TYPE_NULL;
                        return NO_ERROR;
                    }

                    else {
                        fprintf(stderr, "[SEMANTIC] Unknown type literal '%s' in 'is' operator\n", expr->data.binary.right->data.identifier_name);
                        return SEM_ERROR_OTHER;
                    }
                    break;

                // Arithmetic operators - NULL not allowed
                case OP_ADD:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator ADD\n");
                        return SEM_ERROR_TYPE_COMPATIBILITY;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) { *out_type = TYPE_NUM; return NO_ERROR; }
                    if (left == TYPE_STRING && right == TYPE_STRING) { *out_type = TYPE_STRING; return NO_ERROR; }
                    break;

                case OP_SUB:
                case OP_DIV:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator %d\n", op);
                        return SEM_ERROR_TYPE_COMPATIBILITY;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) { *out_type = TYPE_NUM; return NO_ERROR; }
                    break;

                case OP_MUL:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator MUL\n");
                        return SEM_ERROR_TYPE_COMPATIBILITY;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) { *out_type = TYPE_NUM; return NO_ERROR; }
                    if (left == TYPE_STRING && right == TYPE_NUM) { *out_type = TYPE_STRING; return NO_ERROR; }
                    break;

                default:
                    break;
            }

            fprintf(stderr, "[SEMANTIC] Expression type compatibility error for operator %d\n", op);
            return SEM_ERROR_TYPE_COMPATIBILITY;
        }
    }

    return NO_ERROR;
}

// Scan a subtree for the first return statement with an inferable type.
// Returns NO_ERROR and sets *out_type if found. If none found, *out_type is TYPE_UNDEF.
static int scan_return_type(ASTNode *n, Scope *scope, DataType *out_type) {
    if (!out_type) return ERROR_INTERNAL;
    *out_type = TYPE_UNDEF;
    if (!n) return NO_ERROR;

    if (n->type == AST_RETURN) {
        if (n->expr) {
            DataType t;
            int ierr = infer_expr_node_type(n->expr, scope, &t);
            if (ierr != NO_ERROR) return ierr;
            if (t != TYPE_UNDEF) { *out_type = t; return NO_ERROR; }
        } else if (n->left) {
            if (n->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(n->left, scope);
                if (err != NO_ERROR) return err;
                if (n->left->data_type != TYPE_UNDEF) {
                    *out_type = n->left->data_type;
                    return NO_ERROR;
                }
            } else if (n->left->type == AST_EXPRESSION) {
                DataType t = TYPE_UNDEF;
                if (n->left->expr) {
                    int ierr = infer_expr_node_type(n->left->expr, scope, &t);
                    if (ierr != NO_ERROR) return ierr;
                } else if (n->left->left && n->left->left->type == AST_FUNC_CALL) {
                    int err = semantic_visit(n->left->left, scope);
                    if (err != NO_ERROR) return err;
                    t = n->left->left->data_type;
                }
                if (t != TYPE_UNDEF) { *out_type = t; return NO_ERROR; }
            }
        }
    }

    DataType left_type;
    int lerr = scan_return_type(n->left, scope, &left_type);
    if (lerr != NO_ERROR) return lerr;
    if (left_type != TYPE_UNDEF) { *out_type = left_type; return NO_ERROR; }

    return scan_return_type(n->right, scope, out_type);
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
    
    /*if (strcmp(func_name, "Ifj.read_str") == 0 || strcmp(func_name, "Ifj.read_num") == 0) {
        if (arg_count != 0) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 0 arguments, got %d\n", 
                    func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
    }
    else*/ if (strcmp(func_name, "Ifj.write") == 0 || strcmp(func_name, "Ifj.str") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // For write and str, any type is acceptable - no type checking needed
    }
    else if (strcmp(func_name, "Ifj.floor") == 0 || strcmp(func_name, "Ifj.chr") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check if argument is numeric type
        ASTNode *first_arg = node->left;
        if (first_arg && first_arg->right) {
            DataType arg_type = TYPE_UNDEF;
            if (first_arg->right->expr) {
                int ierr = infer_expr_node_type(first_arg->right->expr, scope, &arg_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (first_arg->right->left && first_arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(first_arg->right->left, scope);
                if (err != NO_ERROR) return err;
                arg_type = first_arg->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }

            if (arg_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' requires numeric argument, got type %d\n", func_name, arg_type);
                return SEM_ERROR_WRONG_PARAMS;
            }

            // Additional check for Ifj.chr: if the argument is a literal, it must be integer
            if (strcmp(func_name, "Ifj.chr") == 0) {
                if (first_arg->right->expr && first_arg->right->expr->type == EXPR_NUM_LITERAL) {
                    double v = first_arg->right->expr->data.num_literal;
                    if ((double)(long long)v != v) {
                        fprintf(stderr, "[SEMANTIC] Built-in function '%s' requires integer literal argument\n", func_name);
                        return SEM_ERROR_WRONG_PARAMS;
                    }
                }
            }
        }
    }
    else if (strcmp(func_name, "Ifj.length") == 0) {
        if (arg_count != 1) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 1 argument, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check if argument is string type
        ASTNode *first_arg = node->left;
        if (first_arg && first_arg->right) {
            DataType arg_type = TYPE_UNDEF;
            if (first_arg->right->expr) {
                int ierr = infer_expr_node_type(first_arg->right->expr, scope, &arg_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (first_arg->right->left && first_arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(first_arg->right->left, scope);
                if (err != NO_ERROR) return err;
                arg_type = first_arg->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' requires string argument, got type %d\n", func_name, arg_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.substring") == 0) {
        if (arg_count != 3) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 3 arguments, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check argument types: string, num, num
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = TYPE_UNDEF;
            if (arg->right->expr) {
                int ierr = infer_expr_node_type(arg->right->expr, scope, &arg1_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                arg1_type = arg->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string, got type %d\n", func_name, arg1_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
        // Check 2nd and 3rd arguments are numeric
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = TYPE_UNDEF;
            if (arg->left->right->expr) {
                int ierr = infer_expr_node_type(arg->left->right->expr, scope, &arg2_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                arg2_type = arg->left->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }

            if (arg2_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be numeric, got type %d\n", func_name, arg2_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
        if (arg && arg->left && arg->left->left && arg->left->left->right) {
            DataType arg3_type = TYPE_UNDEF;
            if (arg->left->left->right->expr) {
                int ierr = infer_expr_node_type(arg->left->left->right->expr, scope, &arg3_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->left->left->right->left && arg->left->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->left->right->left, scope);
                if (err != NO_ERROR) return err;
                arg3_type = arg->left->left->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid third argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }

            if (arg3_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' third argument must be numeric, got type %d\n", func_name, arg3_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.strcmp") == 0) {
        if (arg_count != 2) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 2 arguments, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check both arguments are strings
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = TYPE_UNDEF;
            if (arg->right->expr) {
                int ierr = infer_expr_node_type(arg->right->expr, scope, &arg1_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                arg1_type = arg->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string, got type %d\n", func_name, arg1_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = TYPE_UNDEF;
            if (arg->left->right->expr) {
                int ierr = infer_expr_node_type(arg->left->right->expr, scope, &arg2_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                arg2_type = arg->left->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg2_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be string, got type %d\n", func_name, arg2_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
    }
    else if (strcmp(func_name, "Ifj.ord") == 0) {
        if (arg_count != 2) {
            fprintf(stderr, "[SEMANTIC] Built-in function '%s' takes 2 arguments, got %d\n", func_name, arg_count);
            return SEM_ERROR_WRONG_PARAMS;
        }
        // Check first argument is string, second is numeric
        ASTNode *arg = node->left;
        if (arg && arg->right) {
            DataType arg1_type = TYPE_UNDEF;
            if (arg->right->expr) {
                int ierr = infer_expr_node_type(arg->right->expr, scope, &arg1_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                arg1_type = arg->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg1_type != TYPE_STRING) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string, got type %d\n", func_name, arg1_type);
                return SEM_ERROR_WRONG_PARAMS;
            }
        }
        if (arg && arg->left && arg->left->right) {
            DataType arg2_type = TYPE_UNDEF;
            if (arg->left->right->expr) {
                int ierr = infer_expr_node_type(arg->left->right->expr, scope, &arg2_type);
                if (ierr != NO_ERROR) return ierr;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                arg2_type = arg->left->right->left->data_type;
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg2_type != TYPE_NUM) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be numeric, got type %d\n", func_name, arg2_type);
                return SEM_ERROR_WRONG_PARAMS;
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

void add_node_to_func_node(ASTNode *node, ASTNode *func_node){
    ASTNode *tmp = func_node;
    while(tmp->var_next != NULL){
        tmp = tmp->var_next;
    }
    tmp->var_next = node;
}

// Check user function call
int check_user_function_call(ASTNode *node, Scope *scope, SymTableData *func_symbol) {
    if (func_symbol->type != NODE_FUNC) {
        fprintf(stderr, "[SEMANTIC] '%s' is not a function\n", node->name);
        return SEM_ERROR_OTHER;
    }
    
    FunctionData *fdata = func_symbol->data.func_data;
    //int arg_count = count_arguments(node->left);
    
    /*if (fdata->param_count != arg_count) {

        
        fprintf(stderr, "[SEMANTIC] Function '%s' expects %d arguments, got %d\n", 
                node->name, fdata->param_count, arg_count);
        return SEM_ERROR_WRONG_PARAMS;
    }*/
    
    // Type checking for each parameter
    ASTNode *arg_node = node->left;
    Param *param = fdata->parameters;

    while (arg_node && param) {
    if (arg_node->right) {
        DataType arg_type = TYPE_UNDEF;
        
        if (arg_node->right->expr) {
            // NEW SYSTEM: Use expr for expressions
            int ierr = infer_expr_node_type(arg_node->right->expr, scope, &arg_type);
            if (ierr != NO_ERROR) return ierr;
        } else if (arg_node->right->left && arg_node->right->left->type == AST_FUNC_CALL) {
            // FUNCTION CALL: Process it first to get its return type
            int err = semantic_visit(arg_node->right->left, scope);
            if (err != NO_ERROR) return err;
            arg_type = arg_node->right->left->data_type;
        } else {
            fprintf(stderr, "[SEMANTIC] Invalid argument expression\n");
            return SEM_ERROR_OTHER;
        }
        
        if (arg_type != param->data_type && param->data_type != TYPE_UNDEF) {
            fprintf(stderr, "[SEMANTIC] Function '%s' parameter '%s' expects type %d, got %d\n", node->name, param->name, param->data_type, arg_type);
            return SEM_ERROR_WRONG_PARAMS;
        }
    }
    arg_node = arg_node->left;
    param = param->next;
}
    
    // Set return type for the function call node
    node->data_type = fdata->return_type;
    
    return NO_ERROR;
}

int semantic_definition(ASTNode *node, Scope *current_scope){
    ASTNode *actual = node->left;   //program->left -> first function definition
    while(actual){
        if(actual->type == AST_FUNC_DEF){
            {
                if (!actual->right) return ERROR_INTERNAL;

                // Get function name
                const char *func_name = actual->name;
                if (!func_name) return ERROR_INTERNAL;

                // Count parameters and check for duplicates
                int param_count = 0;
                Param *params = NULL;
                Param *last_param = NULL;

                ASTNode *param_actual = actual->left;
                while (param_actual && param_actual->type == AST_FUNC_ARG) {  
                    if (!param_actual->right || param_actual->right->type != AST_IDENTIFIER) {
                        fprintf(stderr, "[SEMANTIC] Invalid parameter actual in function '%s'.\n", func_name);
                        return ERROR_INTERNAL;
                    }

                    const char *param_name = param_actual->right->name;
                    DataType param_type = param_actual->right->data_type;

                    // Check for duplicate parameter names
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in function '%s'.\n", param_name, func_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    // Create parameter structure
                    Param *new_param = make_param(param_name, param_type);
                    if (!new_param) return ERROR_INTERNAL;

                    if (!params)
                        params = new_param;
                    else
                        last_param->next = new_param;
                    last_param = new_param;

                    param_count++;
                    param_actual = param_actual->left;
                }

                // Build overload key: "name$argc"
                char overload_key[128];
                snprintf(overload_key, sizeof(overload_key), "%s$%d", func_name, param_count);

                // Error on redefinition of the same signature
                SymTableData *existing = lookup_symbol(current_scope, overload_key);
                if (existing && existing->type == NODE_FUNC) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of function '%s' with %d parameters.\n", func_name, param_count);
                    return SEM_ERROR_REDEFINED;
                }

                // Insert new function symbol for this signature
                SymTableData *func_symbol = make_function(param_count, params, true, TYPE_UNDEF);
                if (!func_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for function '%s'.\n", func_name);
                    return ERROR_INTERNAL;
                }
                if (!symtable_insert(&current_scope->symbols, overload_key, func_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert function '%s' overload '%s' into symbol table.\n", func_name, overload_key);
                    return ERROR_INTERNAL;
                }

                // TEMPORARY, BC AST_MAIN_DEF NOT WORKING RIGHT NOW
                if(strcmp(func_name, "main") == 0 && param_count == 0) {
                    main_zero_defined = true;
                    actual->type = AST_MAIN_DEF;
                }
                
                // Update function name to include hashtag
                free(actual->name);
                actual->name = my_strdup(overload_key);
                if (!actual->name) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate memory for function name with hashtag.\n");
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
                    SymTableData *param_var = make_variable(p->data_type, true, true);  // defined=true, initialized=true
                    if (!param_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                    // Bind parameter's variable data to this function scope
                    param_var->data.var_data->scope = func_scope;
                    if (!symtable_insert(&func_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into function scope.\n", p->name);
                        free(param_var);
                        return ERROR_INTERNAL;
                    }
                }

                // Mark AST parameter identifiers with their scope so the generator
                // can emit the correct frame suffix.
                param_actual = actual->left;
                while (param_actual && param_actual->type == AST_FUNC_ARG) {
                    if (param_actual->right && param_actual->right->type == AST_IDENTIFIER) {
                        param_actual->right->current_scope = func_scope;
                    }
                    param_actual = param_actual->left;
                }

                actual->right->current_table = &func_scope->symbols;
            } 
            
            
        }
        else if (actual->type == AST_SETTER_DEF){
            
            if (!actual->right) return ERROR_INTERNAL;

                // Get setter name
                const char *setter_name = actual->name;
                if (!setter_name) return ERROR_INTERNAL;

                // Check parameters - setters should have exactly 1 parameter
                if (!actual->left || actual->left->type != AST_IDENTIFIER) {
                    fprintf(stderr, "[SEMANTIC] Setter '%s' must have exactly one parameter.\n", setter_name);
                    return SEM_ERROR_WRONG_PARAMS;
                }

                const char *param_name = actual->left->name;
                DataType param_type = actual->left->data_type;

                // Create setter key with $set suffix
                char *setter_key = make_setter_key(setter_name);
                if (!setter_key) return ERROR_INTERNAL;

                // Check for existing SETTER with same name (only setter conflicts)
                SymTableData *existing = symtable_search(&current_scope->symbols, setter_key);
                if (existing && existing->type == NODE_SETTER) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of setter '%s'.\n", setter_name);
                    free(setter_key);
                    return SEM_ERROR_REDEFINED;
                }

                // Create setter symbol - setters have 1 parameter
                SymTableData *setter_symbol = make_setter(param_type, true);
                if (!setter_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for setter '%s'.\n", setter_name);
                    free(setter_key);
                    return ERROR_INTERNAL;
                }

                // Insert into current scope
                if (!symtable_insert(&current_scope->symbols, setter_key, setter_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert setter '%s' into symbol table.\n", setter_name);
                    free(setter_key);
                    return ERROR_INTERNAL;
                }
                free(setter_key);

                // Create new scope for setter body
                Scope *setter_scope = init_scope();
                if (!setter_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for setter '%s'.\n", setter_name);
                    return ERROR_INTERNAL;
                }
                setter_scope->parent = current_scope;

                // Insert parameter into setter scope
                SymTableData *param_var = make_variable(param_type, true, true);
                if (!param_var) {
                    fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", param_name);
                    return ERROR_INTERNAL;
                }
                // Bind parameter variable to this setter scope
                param_var->data.var_data->scope = setter_scope;
                if (!symtable_insert(&setter_scope->symbols, param_name, param_var)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into setter scope.\n", param_name);
                    free(param_var);
                    return ERROR_INTERNAL;
                }

                // Annotate setter parameter identifier with its scope for codegen
                if (actual->left && actual->left->type == AST_IDENTIFIER) {
                    actual->left->current_scope = setter_scope;
                }

            actual->right->current_table = &setter_scope->symbols;
        }
        else if (actual->type == AST_GETTER_DEF){
            if (!actual->right) return ERROR_INTERNAL;

                // Get getter name
                const char *getter_name = actual->name;
                if (!getter_name) return ERROR_INTERNAL;

                // Create getter key with $get suffix
                char *getter_key = make_getter_key(getter_name);
                if (!getter_key) return ERROR_INTERNAL;

                // Check for existing GETTER with same name (only getter conflicts)
                SymTableData *existing = symtable_search(&current_scope->symbols, getter_key);
                if (existing && existing->type == NODE_GETTER) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of getter '%s'.\n", getter_name);
                    free(getter_key);
                    return SEM_ERROR_REDEFINED;
                }

                // Create getter symbol - getters have 0 parameters
                SymTableData *getter_symbol = make_getter(TYPE_UNDEF, true);
                if (!getter_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for getter '%s'.\n", getter_name);
                    free(getter_key);
                    return ERROR_INTERNAL;
                }

                // Insert into current scope
                if (!symtable_insert(&current_scope->symbols, getter_key, getter_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert getter '%s' into symbol table.\n", getter_name);
                    free(getter_key);
                    return ERROR_INTERNAL;
                }

                // Create new scope for getter body
                Scope *getter_scope = init_scope();
                if (!getter_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for getter '%s'.\n", getter_name);
                    return ERROR_INTERNAL;
                }
                getter_scope->parent = current_scope;

                // Before analyzing the getter body, try to infer the getter's
                // return type by scanning the body for return statements that
                // contain literal or directly inferable expressions. Setting
                // the getter symbol's return_type early ensures that later
                // statements in the same block (which may reference the
                // getter) will see its return type.
                ASTNode *scan = actual->right; // should be a BLOCK
                DataType found_type = TYPE_UNDEF;
                int serr = scan_return_type(scan, getter_scope, &found_type);
                if (serr != NO_ERROR) return serr;

                if (found_type != TYPE_UNDEF) {
                    char *lookup_key = make_getter_key(getter_name);
                    if (lookup_key) {
                        SymTableData *s = symtable_search(&current_scope->symbols, lookup_key);
                        if (s && s->type == NODE_GETTER) {
                            s->data.getter_data->return_type = found_type;
                        }
                        free(lookup_key);
                    }
                }
                actual->right->current_table = &getter_scope->symbols;
        }
        actual = actual->right;
    }
    return NO_ERROR;
}
//---------- MAIN VISIT: Traverse AST tree ----------
int semantic_visit(ASTNode *node, Scope *current_scope) {
    if (!node) return NO_ERROR;

     semantic_visit_count++;
     
     /*printf("[SEMANTIC] Visiting node: %s, name: %s\n", 
           ast_node_type_to_string(node->type), 
           node->name ? node->name : "(null)");*/
    switch (node->type) {
        case AST_PROGRAM:   {
            node->current_scope = current_scope;
                return semantic_visit(node->left, current_scope);
            } break;
        case AST_MAIN_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                const char *func_name = "main";
                func_node = node;  // Set global func_node for main

                // Count parameters and check for duplicates
                int param_count = 0;
                Param *params = NULL;
                Param *last_param = NULL;

                ASTNode *param_node = node->left;
                while (param_node && param_node->type == AST_FUNC_ARG) { 
                    if (!param_node->right || param_node->right->type != AST_IDENTIFIER) {
                        fprintf(stderr, "[SEMANTIC] Invalid parameter node in main().\n");
                        return ERROR_INTERNAL;
                    }

                    const char *param_name = param_node->right->name;
                    DataType param_type = param_node->right->data_type;

                    // Check for duplicate parameter names
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in main().\n", param_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    // Create parameter structure
                    Param *new_param = make_param(param_name, param_type);
                    if (!new_param) return ERROR_INTERNAL;

                    if (!params)
                        params = new_param;
                    else
                        last_param->next = new_param;
                    last_param = new_param;

                    param_count++;
                    param_node = param_node->left;
                }
                
                // Check for redefinition of main
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


                // Mark that main() with this arity exists (we require 0 params)
                if (param_count == 0) {
                    main_zero_defined = true;
                }


                // Create new scope for main function body
                Scope *main_scope = init_scope();
                if (!main_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for 'main'.\n");
                    return ERROR_INTERNAL;
                }
                main_scope->parent = current_scope;
                node->right->current_table = &main_scope->symbols;

                // Insert parameters into main scope
                for (Param *p = params; p; p = p->next) {
                    SymTableData *param_var = make_variable(p->data_type, true, true);
                    if (!param_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                    // Bind parameter variable to this main scope
                    param_var->data.var_data->scope = main_scope;
                    if (!symtable_insert(&main_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into main scope.\n", p->name);
                        free(param_var);
                        return ERROR_INTERNAL;
                    }
                }

                // Mark AST parameter identifiers with their scope for codegen
                param_node = node->left;
                while (param_node && param_node->type == AST_FUNC_ARG) {
                    if (param_node->right && param_node->right->type == AST_IDENTIFIER) {
                        param_node->right->current_scope = main_scope;
                    }
                    param_node = param_node->left;
                }

                // Analyze main function body with main scope
                int result = semantic_visit(node->right, main_scope);
                
                
                // free_scope(main_scope);  
                
                return result;
            } break;

        case AST_FUNC_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                // Get function name
                const char *func_name = node->name;
                func_node = node;
                if (!func_name) return ERROR_INTERNAL;

                // Count parameters and check for duplicates
                int param_count = 0;
                Param *params = NULL;
                Param *last_param = NULL;

                ASTNode *param_node = node->left;
                while (param_node && param_node->type == AST_FUNC_ARG) {  
                    if (!param_node->right || param_node->right->type != AST_IDENTIFIER) {
                        fprintf(stderr, "[SEMANTIC] Invalid parameter node in function '%s'.\n", func_name);
                        return ERROR_INTERNAL;
                    }

                    const char *param_name = param_node->right->name;
                    DataType param_type = param_node->right->data_type;

                    // Check for duplicate parameter names
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in function '%s'.\n", param_name, func_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    // Create parameter structure
                    Param *new_param = make_param(param_name, param_type);
                    if (!new_param) return ERROR_INTERNAL;

                    if (!params)
                        params = new_param;
                    else
                        last_param->next = new_param;
                    last_param = new_param;

                    param_count++;
                    param_node = param_node->left;
                }

                // Build overload key: "name$argc"
                char overload_key[128];
                snprintf(overload_key, sizeof(overload_key), "%s$%d", func_name, param_count);

                // Ensure the function symbol exists (insert if missing)
                SymTableData *existing = lookup_symbol(current_scope, overload_key);
                if (!existing) {
                    SymTableData *func_symbol = make_function(param_count, params, true, TYPE_UNDEF);
                    if (!func_symbol) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for function '%s'.\n", func_name);
                        return ERROR_INTERNAL;
                    }
                    if (!symtable_insert(&current_scope->symbols, overload_key, func_symbol)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert function '%s' overload '%s' into symbol table.\n", func_name, overload_key);
                        return ERROR_INTERNAL;
                    }
                }
                
                // TEMPORARY, BC AST_MAIN_DEF NOT WORKING RIGHT NOW
                if(strcmp(func_name, "main") == 0 && param_count == 0) {
                    main_zero_defined = true;
                    node->type = AST_MAIN_DEF;
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
                    SymTableData *param_var = make_variable(p->data_type, true, true);  // defined=true, initialized=true
                    if (!param_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                    // Bind parameter variable to this function scope
                    param_var->data.var_data->scope = func_scope;
                    if (!symtable_insert(&func_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into function scope.\n", p->name);
                        free(param_var);
                        return ERROR_INTERNAL;
                    }
                }

                // Annotate AST parameter identifiers with their scope for codegen
                param_node = node->left;
                while (param_node && param_node->type == AST_FUNC_ARG) {
                    if (param_node->right && param_node->right->type == AST_IDENTIFIER) {
                        param_node->right->current_scope = func_scope;
                    }
                    param_node = param_node->left;
                }

                node->right->current_table = &func_scope->symbols;
                // Analyze function body with function scope
                // The AST_BLOCK will handle its own scope creation and cleanup
                int result = semantic_visit(node->right, func_scope);

                return result;
                // Cleanup function scope
                //free_scope(func_scope);
                
                return NO_ERROR;
            } break;


        case AST_GETTER_DEF: {
                if (!node->right) return ERROR_INTERNAL;
                func_node = node;
                // Get getter name
                const char *getter_name = node->name;
                if (!getter_name) return ERROR_INTERNAL;

                // Create getter key with $get suffix
                char *getter_key = make_getter_key(getter_name);
                if (!getter_key) return ERROR_INTERNAL;

                // If predeclared, reuse; otherwise create and insert
                SymTableData *existing = symtable_search(&current_scope->symbols, getter_key);
                SymTableData *getter_symbol = NULL;
                if (existing) {
                    if (existing->type != NODE_GETTER) {
                        fprintf(stderr, "[SEMANTIC] Symbol '%s' exists and is not a getter.\n", getter_name);
                        free(getter_key);
                        return SEM_ERROR_REDEFINED;
                    }
                    getter_symbol = existing;
                    free(getter_key);
                } else {
                    getter_symbol = make_getter(TYPE_UNDEF, true);
                    if (!getter_symbol) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for getter '%s'.\n", getter_name);
                        free(getter_key);
                        return ERROR_INTERNAL;
                    }
                    if (!symtable_insert(&current_scope->symbols, getter_key, getter_symbol)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert getter '%s' into symbol table.\n", getter_name);
                        free(getter_key);
                        return ERROR_INTERNAL;
                    }
                    free(getter_key);
                }

                // Create new scope for getter body
                Scope *getter_scope = init_scope();
                if (!getter_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for getter '%s'.\n", getter_name);
                    return ERROR_INTERNAL;
                }
                getter_scope->parent = current_scope;

                // Before analyzing the getter body, try to infer the getter's
                // return type by scanning the body for return statements that
                // contain literal or directly inferable expressions. Setting
                // the getter symbol's return_type early ensures that later
                // statements in the same block (which may reference the
                // getter) will see its return type.
                ASTNode *scan = node->right; // should be a BLOCK
                DataType found_type = TYPE_UNDEF;
                int serr = scan_return_type(scan, getter_scope, &found_type);
                if (serr != NO_ERROR) return serr;

                if (found_type != TYPE_UNDEF && getter_symbol && getter_symbol->type == NODE_GETTER) {
                    getter_symbol->data.getter_data->return_type = found_type;
                }

                // Make the getter body block use this scope directly
                node->right->current_table = &getter_scope->symbols;

                // Now analyze getter body with getter scope
                int result = semantic_visit(node->right, getter_scope);

                return result;
            } break;

        case AST_SETTER_DEF: {
                if (!node->right) return ERROR_INTERNAL;
                func_node = node;
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

                // Create setter key with $set suffix
                char *setter_key = make_setter_key(setter_name);
                if (!setter_key) return ERROR_INTERNAL;

                // If predeclared, reuse; otherwise create and insert
                SymTableData *existing = symtable_search(&current_scope->symbols, setter_key);
                SymTableData *setter_symbol = NULL;
                if (existing) {
                    if (existing->type != NODE_SETTER) {
                        fprintf(stderr, "[SEMANTIC] Symbol '%s' exists and is not a setter.\n", setter_name);
                        free(setter_key);
                        return SEM_ERROR_REDEFINED;
                    }
                    setter_symbol = existing;
                    if (setter_symbol->data.setter_data->param_type == TYPE_UNDEF && param_type != TYPE_UNDEF) {
                        setter_symbol->data.setter_data->param_type = param_type;
                    }
                    free(setter_key);
                } else {
                    setter_symbol = make_setter(param_type, true);
                    if (!setter_symbol) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for setter '%s'.\n", setter_name);
                        free(setter_key);
                        return ERROR_INTERNAL;
                    }
                    if (!symtable_insert(&current_scope->symbols, setter_key, setter_symbol)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert setter '%s' into symbol table.\n", setter_name);
                        free(setter_key);
                        return ERROR_INTERNAL;
                    }
                    free(setter_key);
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
                if (!param_var) {
                    fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", param_name);
                    return ERROR_INTERNAL;
                }
                // Bind parameter variable to this setter scope
                param_var->data.var_data->scope = setter_scope;
                if (!symtable_insert(&setter_scope->symbols, param_name, param_var)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into setter scope.\n", param_name);
                    free(param_var);
                    return ERROR_INTERNAL;
                }

                // Annotate AST parameter identifier with its scope for codegen
                node->left->current_scope = setter_scope;

                // Make the setter body block use this scope directly
                node->right->current_table = &setter_scope->symbols;

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

                var_data->data.var_data->scope = current_scope;
                
                add_node_to_func_node(node, func_node); //adds vardecl to program's var decl list
                // Set current_scope for the identifier node
                if (node->left) {
                    node->left->current_scope = current_scope;
                }
                
                
                return semantic_visit(node->right, current_scope);
                
            } break;

        case AST_ASSIGN: {
                ASTNode* equals = node->left;
                if (!equals || equals->type != AST_EQUALS) return ERROR_INTERNAL;
                
                // must exists
                /*if (!equals->left || equals->left->type != AST_IDENTIFIER) return ERROR_INTERNAL;
                const char* left_var = equals->left->name;
                SymTableData* left_data = lookup_symbol(current_scope, left_var);
                if (!left_data) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s' in assignment\n", left_var);  
                    return SEM_ERROR_UNDEFINED;
                }
                
                // Right side - check of uninitialiezed variables
                if (equals->right && equals->right->type == AST_EXPRESSION) {
                    int err = check_uninitialized_usage(equals->right->expr, current_scope);
                    if (err != NO_ERROR) return err;
                }*/
                
                // If left side refers to a setter symbol, transform the whole AST_ASSIGN
                // into an AST_SETTER_CALL: move RHS expression into node->left and
                // store the setter name in node->name. Otherwise, process normally.
                if (equals->left && equals->left->type == AST_IDENTIFIER) {
                    const char *left_name = equals->left->name;
                    
                    // Try to find as setter with $set suffix
                    char *setter_key = make_setter_key(left_name);
                    SymTableData* sym = NULL;
                    if (setter_key) {
                        sym = lookup_symbol(current_scope, setter_key);
                        free(setter_key);
                    }
                    
                    if (sym && sym->type == NODE_SETTER) {
                        // Prepare to transform: detach expression from equals
                        ASTNode* rhs_expr = equals->right;
                        ASTNode* id_node = equals->left;

                        if (!rhs_expr || rhs_expr->type != AST_EXPRESSION) {
                            fprintf(stderr, "[SEMANTIC] Setter assignment to '%s' has no expression on the right side\n", left_name);
                            return SEM_ERROR_OTHER;
                        }

                        // Transfer identifier name to this node (free existing name first)
                        if (node->name) free(node->name);
                        node->name = id_node->name; // take ownership
                        id_node->name = NULL;

                        // Rewire node: become AST_SETTER_CALL with left = expression
                        node->type = AST_SETTER_CALL;
                        node->current_scope = current_scope;

                        // detach rhs_expr from equals to avoid double-free
                        equals->right = NULL;
                        equals->left = NULL;

                        node->left = rhs_expr;
                        // node->right already points to next statement (keep it)

                        // free the old equals and identifier structures (name moved)
                        free_ast_tree(equals);

                        // Now perform type inference/check for the setter parameter
                        DataType right_type = TYPE_UNDEF;
                        if (node->left->expr) {
                            int ierr = infer_expr_node_type(node->left->expr, current_scope, &right_type);
                            if (ierr != NO_ERROR) return ierr;
                        } else if (node->left->left && node->left->left->type == AST_FUNC_CALL) {
                            int err = semantic_visit(node->left->left, current_scope);
                            if (err != NO_ERROR) return err;
                            right_type = node->left->left->data_type;
                        } else {
                            fprintf(stderr, "[SEMANTIC] Invalid expression structure in setter assignment to '%s'\n", node->name);
                            return SEM_ERROR_OTHER;
                        }

                        DataType setter_param = sym->data.setter_data->param_type;
                        if (right_type != TYPE_UNDEF && setter_param != TYPE_UNDEF && right_type != setter_param) {
                            fprintf(stderr, "[SEMANTIC] Type mismatch in setter call to '%s': expected %d, got %d\n", node->name, setter_param, right_type);
                            return SEM_ERROR_TYPE_COMPATIBILITY;
                        }

                        if (setter_param == TYPE_UNDEF && right_type != TYPE_UNDEF) {
                            sym->data.setter_data->param_type = right_type;
                        }

                        // After transforming to AST_SETTER_CALL, continue visiting next statement
                        return semantic_visit(node->right, current_scope);
                    }
                }

                // Process the assignment (AST_EQUALS) normally
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;

                // Continue with next statement
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_SETTER_CALL: {
                // node->name = setter name, node->left = expression to pass, node->right = next statement
                if (!node->name) {
                    fprintf(stderr, "[SEMANTIC] Setter call without name\n");
                    return ERROR_INTERNAL;
                }

                SymTableData* sym = lookup_symbol(current_scope, node->name);
                if (!sym) {
                    fprintf(stderr, "[SEMANTIC] Undefined setter '%s'\n", node->name);
                    return SEM_ERROR_UNDEFINED;
                }
                if (sym->type != NODE_SETTER) {
                    fprintf(stderr, "[SEMANTIC] '%s' is not a setter\n", node->name);
                    return SEM_ERROR_OTHER;
                }

                if (!node->left || node->left->type != AST_EXPRESSION) {
                    fprintf(stderr, "[SEMANTIC] Setter call to '%s' missing expression argument\n", node->name);
                    return SEM_ERROR_OTHER;
                }

                DataType right_type = TYPE_UNDEF;
                if (node->left->expr) {
                    int ierr = infer_expr_node_type(node->left->expr, current_scope, &right_type);
                    if (ierr != NO_ERROR) return ierr;
                    annotate_expr_scopes(node->left->expr, current_scope);
                } else if (node->left->left && node->left->left->type == AST_FUNC_CALL) {
                    int err = semantic_visit(node->left->left, current_scope);
                    if (err != NO_ERROR) return err;
                    right_type = node->left->left->data_type;
                } else {
                    fprintf(stderr, "[SEMANTIC] Invalid expression structure in setter call to '%s'\n", node->name);
                    return SEM_ERROR_OTHER;
                }

                DataType setter_param = sym->data.setter_data->param_type;
                if (right_type != TYPE_UNDEF && setter_param != TYPE_UNDEF && right_type != setter_param) {
                    fprintf(stderr, "[SEMANTIC] Type mismatch in setter call to '%s': expected %d, got %d\n", node->name, setter_param, right_type);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }

                if (setter_param == TYPE_UNDEF && right_type != TYPE_UNDEF) {
                    sym->data.setter_data->param_type = right_type;
                }

                // Continue with next statement
                return semantic_visit(node->right, current_scope);
            } break;

        case AST_EQUALS: {
                // AST_EQUALS -> left = identifier, right = AST_EXPRESSION

                if (!node->left || node->left->type != AST_IDENTIFIER) {
                    fprintf(stderr, "[SEMANTIC] Left side of assignment must be identifier\n");
                    return SEM_ERROR_OTHER;
                }

                const char* var_name = node->left->name;

                // Check if variable exists
                SymTableData* var_data = lookup_symbol(current_scope, var_name);

                // If not found and starts with "__", create global variable
                if (!var_data && var_name[0] == '_' && var_name[1] == '_') {
                    // search to program root
                    Scope* global_scope = current_scope;
                    while (global_scope && global_scope->parent) {
                        global_scope = global_scope->parent;
                    }
                    
                    // make global variable
                    SymTableData* global_var = make_variable(TYPE_UNDEF, true, false); // defined=true, initialized=false
                    if (!global_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate global variable '%s'\n", var_name);
                        return ERROR_INTERNAL;
                    }
                    
                    // Set scope for the global variable
                    global_var->data.var_data->scope = global_scope;
                    
                    // insert into global scope
                    if (!symtable_insert(&global_scope->symbols, var_name, global_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert global variable '%s'\n", var_name);
                        free(global_var);
                        return ERROR_INTERNAL;
                    }
                    
                    /*printf("[SEMANTIC] Automatically created global variable '%s'\n", var_name);*/
                    var_data = global_var;
                }


                if (!var_data) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s' in assignment\n", var_name);
                    return SEM_ERROR_UNDEFINED;
                }

                if (var_data->type != NODE_VAR) {
                    fprintf(stderr, "[SEMANTIC] '%s' is not a variable\n", var_name);
                    return SEM_ERROR_OTHER;
                }

                // The right side must be an expression node
                ASTNode* expr_node = node->right;
                if (!expr_node || expr_node->type != AST_EXPRESSION) {
                    fprintf(stderr, "[SEMANTIC] Assignment to '%s' has no expression on the right side\n", var_name);
                    return SEM_ERROR_OTHER;
                }

                DataType right_type = TYPE_UNDEF;

                // Handle AST_EXPRESSION internals
                if (expr_node->expr) {
                    // Expression contains an internal expression tree (from expr_ast)
                    int ierr = infer_expr_node_type(expr_node->expr, current_scope, &right_type);
                    if (ierr != NO_ERROR) return ierr;
                    annotate_expr_scopes(expr_node->expr, current_scope);
                } 
                else if (expr_node->left && expr_node->left->type == AST_FUNC_CALL) {
                    // Expression is just a function call (e.g., a = Ifj.read_num())
                    int err = semantic_visit(expr_node->left, current_scope);
                    if (err != NO_ERROR) return err;
                    right_type = expr_node->left->data_type;
                } 
                else {
                    fprintf(stderr, "[SEMANTIC] Invalid expression structure in assignment to '%s'\n", var_name);
                    return SEM_ERROR_OTHER;
                }

                //DataType left_type = var_data->data.var_data->data_type;

                // Dynamic typing: always update variable type to the assigned expression type when known
                if (right_type != TYPE_UNDEF) {
                    var_data->data.var_data->data_type = right_type;
                }

                // Mark variable as initialized
                var_data->data.var_data->initialized = true;

                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;

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
                
                if (!var_data && var_name[0] == '_' && var_name[1] == '_') {
                    // search to program root
                    Scope* global_scope = current_scope;
                    while (global_scope && global_scope->parent) {
                        global_scope = global_scope->parent;
                    }
                    
                    
                    // make global variable
                    SymTableData* global_var = make_variable(TYPE_UNDEF, true, false); // defined=true, initialized=false
                    if (!global_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to allocate global variable '%s'\n", var_name);
                        return ERROR_INTERNAL;
                    }
                    
                    // Set scope for the newly created global variable
                    global_var->data.var_data->scope = global_scope;
                    
                    // insert into global scope
                    if (!symtable_insert(&global_scope->symbols, var_name, global_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert global variable '%s'\n", var_name);
                        free(global_var);
                        return ERROR_INTERNAL;
                    }
                    
                    /*printf("[SEMANTIC] Automatically created global variable '%s'\n", var_name);*/
                    var_data = global_var;
                }
    

                if (!var_data) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s'\n", var_name);
                    return SEM_ERROR_UNDEFINED;
                }
                
                if (var_data->type != NODE_VAR) {
                    fprintf(stderr, "[SEMANTIC] '%s' is not a variable\n", var_name);
                    return SEM_ERROR_OTHER;
                }
                
                // Set current_scope for the identifier node
                node->current_scope = var_data->data.var_data->scope;
                
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
                int argc = count_arguments(node->left);

                // Zložíme lookup key
                char keybuf[128];
                snprintf(keybuf, sizeof(keybuf), "%s$%d", func_name, argc);

                // Hľadáme presné preťaženie
                SymTableData *func_symbol = lookup_symbol(current_scope, keybuf);
                    if (!func_symbol) {
                        // Ak existuje funkcia s iným počtom parametrov, vráť chybu o nesprávnom počte parametrov
                        if (any_arity_function_exists(current_scope, func_name)) {
                            fprintf(stderr, "[SEMANTIC] Function '%s' called with wrong parameter count: got %d\n", func_name, argc);
                            return SEM_ERROR_WRONG_PARAMS;
                        }
                        fprintf(stderr, "[SEMANTIC] Undefined function '%s' with %d arguments\n", func_name, argc);
                        return SEM_ERROR_UNDEFINED;
                    }
                int err =  check_user_function_call(node, current_scope, func_symbol);
                
                if(err != NO_ERROR) return err;
                err = semantic_visit(node->right, current_scope);
                return err;
                
            } break;
        /*case AST_FUNC_PARAM: {
                // Parameters are already added in AST_FUNC_DEF, just check structure
                if (!node->right || node->right->type != AST_IDENTIFIER) {
                    fprintf(stderr, "[SEMANTIC] Invalid parameter structure.\n");
                    return ERROR_INTERNAL;
                }

                // Optional: Check for duplicate parameters
                const char* param_name = node->right->name;
                SymTableData* existing = symtable_search(&current_scope->symbols, param_name);
                if (existing) {
                    fprintf(stderr, "[SEMANTIC] Parameter '%s' already declared\n", param_name);
                    return SEM_ERROR_REDEFINED;
                }

                return semantic_visit(node->left, current_scope);
            } break;*/
        case AST_FUNC_ARG: {
                // This node can represent two contexts:
                // 1. Parameters of function (in definitions) - right = AST_IDENTIFIER
                // 2. Arguments of calls (in calls) - right = AST_EXPRESSION
                
                if (!node->right) {
                    fprintf(stderr, "[SEMANTIC] Invalid FUNC_ARG structure.\n");
                    return ERROR_INTERNAL;
                }

                
                if (node->right->type == AST_IDENTIFIER) {
                    const char* param_name = node->right->name;
                    SymTableData* existing = symtable_search(&current_scope->symbols, param_name);
                    if (existing) {
                        fprintf(stderr, "[SEMANTIC] Parameter '%s' already declared\n", param_name);
                        return SEM_ERROR_REDEFINED;
                    }
                    // Ensure the AST identifier for the parameter is aware of its scope
                    node->right->current_scope = current_scope;
                } 
                else if (node->right->type == AST_EXPRESSION) {
                    int err = semantic_visit(node->right, current_scope);
                    if (err != NO_ERROR) return err;
                }
                else {
                    fprintf(stderr, "[SEMANTIC] Invalid FUNC_ARG right child type: %d\n", node->right->type);
                    return ERROR_INTERNAL;
                }

                return semantic_visit(node->left, current_scope);
            } break;
        case AST_IF: {
                // AST_IF -> left = condition (AST_EXPRESSION), right = then branch (AST_BLOCK)

                if (!node->left || node->left->type != AST_EXPRESSION) {
                    fprintf(stderr, "[SEMANTIC] If statement missing or invalid condition expression\n");
                    return SEM_ERROR_OTHER;
                }

                // Analyze condition expression
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;

                DataType cond_type = node->left->data_type;

                if (cond_type != TYPE_NUM && cond_type != TYPE_UNDEF) {
                    fprintf(stderr, "[SEMANTIC] If condition must be numeric expression, got type %d\n", cond_type);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }

                // Visit then branch
                if (!node->right || node->right->type != AST_BLOCK) {
                    fprintf(stderr, "[SEMANTIC] If statement missing then block\n");
                    return ERROR_INTERNAL;
                }

                // Visit then branch
                /*if (!node->right->right || node->right->right->type != AST_ELSE) {
                    fprintf(stderr, "[SEMANTIC] If statement missing then block\n");
                    return ERROR_INTERNAL;
                }*/

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
                    // NEW SYSTEM: Use expr for expressions
                    int ierr = infer_expr_node_type(node->expr, current_scope, &return_type);
                    if (ierr != NO_ERROR) return ierr;
                    annotate_expr_scopes(node->expr, current_scope);
                } else if (node->left && node->left->type == AST_FUNC_CALL) {
                    // FUNCTION CALL: Process it first to get its return type
                    int err = semantic_visit(node->left, current_scope);
                    if (err != NO_ERROR) return err;
                    return_type = node->left->data_type;
                }
               
                // If neither expr nor function call, it's a void return (TYPE_NULL)
                int err = semantic_visit(node->left, current_scope);
                    if (err != NO_ERROR) return err;
                // Store return type for function return type checking
                node->data_type = return_type;
                
                return NO_ERROR;
            } break;
        case AST_WHILE: {
                // AST_WHILE -> left = condition (AST_EXPRESSION), right = loop body (AST_BLOCK)

                if (!node->left || node->left->type != AST_EXPRESSION) {
                    fprintf(stderr, "[SEMANTIC] While statement missing or invalid condition expression\n");
                    return SEM_ERROR_OTHER;
                }

                // Visit the expression node first — it will infer its data type internally
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;

                DataType cond_type = node->left->data_type;

                // Condition should be numeric (truthy)
                if (cond_type != TYPE_NUM && cond_type != TYPE_UNDEF) {
                    fprintf(stderr, "[SEMANTIC] While condition must be numeric expression, got type %d\n", cond_type);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }

                // Loop body must exist and be a block
                if (!node->right || node->right->type != AST_BLOCK) {
                    fprintf(stderr, "[SEMANTIC] While statement missing loop body block\n");
                    return ERROR_INTERNAL;
                }

                // Visit loop body — AST_BLOCK already handles new scope creation
                err = semantic_visit(node->right, current_scope);
                if (err != NO_ERROR) return err;

                return NO_ERROR;
            } break;

        case AST_BLOCK: {
                Scope* block_scope = current_scope;  

                if (!node->current_table) {
                    block_scope = init_scope();
                    if (!block_scope) {
                        fprintf(stderr, "[SEMANTIC] Failed to initialize block scope.\n");
                        return ERROR_INTERNAL;
                    }
                    
                    block_scope->parent = current_scope;
                    node->current_table = &block_scope->symbols;
                   
                }

                int err = semantic_visit(node->left, block_scope);
                if (err != NO_ERROR) {
                    return err;
                }
                
                err = semantic_visit(node->right, current_scope);
               
                return err;
            } break;

        case AST_EXPRESSION: {
            
            DataType expr_type = TYPE_UNDEF;
            
                if (node->expr) {
                // NEW SYSTEM: Use expr for all expressions (literals, identifiers, binary operations)
                // If the expr is a plain identifier that refers to a getter, convert
                // it into a getter-call expr so downstream code (type inference/
                // generator) can treat it uniformly.
                if (node->expr->type == EXPR_IDENTIFIER) {
                    SymTableData *s = lookup_symbol(current_scope, node->expr->data.identifier_name);
                    if (s && s->type == NODE_GETTER) {
                        // replace identifier expr with getter-call expr
                        char *name_copy = node->expr->data.identifier_name;
                        // create getter call node
                        ExprNode *g = create_getter_call_node(name_copy);
                        if (!g) {
                            fprintf(stderr, "[SEMANTIC] Failed to allocate getter expr for '%s'\n", name_copy);
                            return ERROR_INTERNAL;
                        }
                        // free old identifier structure but avoid double-free of string
                        free(node->expr);
                        node->expr = g;
                    }
                }

                DataType expr_type;
                int ierr = infer_expr_node_type(node->expr, current_scope, &expr_type);
                if (ierr != NO_ERROR) return ierr;
                node->data_type = expr_type;
                annotate_expr_scopes(node->expr, current_scope);

                // Check if expression type inference failed
            /* if (expr_type == TYPE_UNDEF) {
                fprintf(stderr, "[SEMANTIC] Expression type inference failed\n");
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }*/
                
                
            } else if (node->left && node->left->type == AST_FUNC_CALL) {
                // SPECIAL CASE: Function call within expression
                // Process the function call first
                int err = semantic_visit(node->left, current_scope);
                if (err != NO_ERROR) return err;
                
                // Get return type from function call
                expr_type = node->left->data_type;
                node->data_type = expr_type;
                
            } else {
                // Error: Invalid expression structure
                fprintf(stderr, "[SEMANTIC] Invalid expression structure - missing expr or function call\n");
                return SEM_ERROR_OTHER;
            }
            
            // Continue with next statement
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
    //print_all_symbols(root);

    // Reset simple global state for main() detection
    main_zero_defined = false;
    
    // Initialize global scope
    Scope* global_scope = init_scope();
    if (!global_scope) {
        fprintf(stderr, "[SEMANTIC] Failed to initialize global scope.\n");
        return ERROR_INTERNAL;
    }
    
    // Preload built-in functions
    preload_builtins(global_scope);
    
    // First pass: process definitions (functions, getters, setters, global vars)
    int err = semantic_definition(root, global_scope);
    if(err != NO_ERROR) return err;

    // Analyze AST
    int result = semantic_visit(root, global_scope);

    if (result != NO_ERROR) return result;

    // Simple final check based on the flag set in AST_MAIN_DEF
    if (!main_zero_defined) {
        fprintf(stderr, "[SEMANTIC] Program must define 'main' as a function with 0 parameters\n");
        return SEM_ERROR_UNDEFINED;
    }

    // Propagate the global scope to the AST root so codegen can emit globals
    root->current_scope = global_scope;

    return NO_ERROR;
}
