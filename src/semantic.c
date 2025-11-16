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
        case AST_GETTER_CALL: return "GETTER_CALL";
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
DataType infer_expr_node_type(ExprNode *expr, Scope *scope) {
    if (!expr) return TYPE_UNDEF;
    
    switch (expr->type) {
        case EXPR_NUM_LITERAL:
            return TYPE_NUM;
            
        case EXPR_STRING_LITERAL:
            return TYPE_STRING;
            
        case EXPR_NULL_LITERAL:
            return TYPE_NULL;
            
        case EXPR_TYPE_LITERAL:
            // Type literals (Num, String, Null) used in 'is' operator
            return TYPE_UNDEF;
            
        case EXPR_IDENTIFIER:
            {
                SymTableData *identifier = lookup_symbol(scope, expr->data.identifier_name);
                if (!identifier){
                    fprintf(stderr, "[SEMANTIC] Identifier '%s' not found in expression\n", expr->data.identifier_name);
                    return TYPE_UNDEF;
                }
                if (identifier->type == NODE_VAR) {
                    return identifier->data.var_data->data_type;
                } else if (identifier->type == NODE_GETTER) {
                    return identifier->data.getter_data->return_type;
                } else {
                    // Not a variable or getter (could be func/setter) - treat as undef
                    return TYPE_UNDEF;
                }
            }

        case EXPR_GETTER_CALL: {
            SymTableData *g = lookup_symbol(scope, expr->data.getter_name);
            if (!g) {
                fprintf(stderr, "[SEMANTIC] Getter '%s' not found\n", expr->data.getter_name);
                return TYPE_UNDEF;
            }
            if (g->type != NODE_GETTER) {
                fprintf(stderr, "[SEMANTIC] '%s' is not a getter\n", expr->data.getter_name);
                return TYPE_UNDEF;
            }
            return g->data.getter_data->return_type;
        }
            
            
            
        case EXPR_BINARY_OP:
            DataType left = infer_expr_node_type(expr->data.binary.left, scope);
            DataType right = infer_expr_node_type(expr->data.binary.right, scope);
            
            // If either operand has undefined type, return UNDEF
            if (left == TYPE_UNDEF || right == TYPE_UNDEF) {
                return TYPE_UNDEF;
            }
            
            BinaryOpType op = expr->data.binary.op;
            
            switch (op) {
                // Equality operators - always return TYPE_NUM (boolean)
                // NULL je povolený v rovnostných operátoroch
                case OP_EQ:
                case OP_NEQ:
                    return TYPE_NUM;
                
                // Relational operators - require NUM and return TYPE_NUM
                case OP_LT:
                case OP_GT:
                case OP_LTE:
                case OP_GTE:
                    // NULL not allowed in relation op
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in relational operator %d\n", op);
                        return TYPE_UNDEF;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) {
                        return TYPE_NUM;
                    }
                    break;
                
                // Type test operator - always returns TYPE_NUM
                case OP_IS:
                    
                    return TYPE_NUM;
                
                // Arithmetic operators - NULL not allowed
                case OP_ADD:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator ADD\n");
                        return TYPE_UNDEF;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) return TYPE_NUM;
                    if (left == TYPE_STRING && right == TYPE_STRING) return TYPE_STRING;
                    break;
                    
                case OP_SUB:
                case OP_DIV:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator %d\n", op);
                        return TYPE_UNDEF;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) return TYPE_NUM;
                    break;
                    
                case OP_MUL:
                    if (left == TYPE_NULL || right == TYPE_NULL) {
                        fprintf(stderr, "[SEMANTIC] TYPE_NULL not allowed in arithmetic operator MUL\n");
                        return TYPE_UNDEF;
                    }
                    if (left == TYPE_NUM && right == TYPE_NUM) return TYPE_NUM;
                    if (left == TYPE_STRING && right == TYPE_NUM) return TYPE_STRING;
                    break;
                
                default:
                    break;
            }
            
        fprintf(stderr, "[SEMANTIC] Expression type compatibility error for operator %d\n", op);
        return TYPE_UNDEF;
}
    return TYPE_UNDEF;
}

// Scan a subtree for the first return statement with an inferable type.
// Returns TYPE_UNDEF if none found.
static DataType scan_return_type(ASTNode *n, Scope *scope) {
    if (!n) return TYPE_UNDEF;

    if (n->type == AST_RETURN) {
        if (n->expr) {
            DataType t = infer_expr_node_type(n->expr, scope);
            if (t != TYPE_UNDEF) return t;
        } else if (n->left && n->left->type == AST_FUNC_CALL) {
            if (semantic_visit(n->left, scope) == NO_ERROR && n->left->data_type != TYPE_UNDEF) {
                return n->left->data_type;
            }
        }
    }

    DataType left = scan_return_type(n->left, scope);
    if (left != TYPE_UNDEF) return left;
    return scan_return_type(n->right, scope);
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
            ExprNodeType arg_type = EXPR_NULL_LITERAL;
            if (first_arg->right->expr) {
                arg_type = first_arg->right->expr->type;
            } else if (first_arg->right->left && first_arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(first_arg->right->left, scope);
                if (err != NO_ERROR) return err;
                // For function calls, we need to check the return type from data_type
                DataType return_type = first_arg->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg_type != EXPR_NUM_LITERAL) {
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
            ExprNodeType arg_type = EXPR_NULL_LITERAL;
            if (first_arg->right->expr) {
                arg_type = first_arg->right->expr->type;
            } else if (first_arg->right->left && first_arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(first_arg->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = first_arg->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg_type != EXPR_STRING_LITERAL) {
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
            ExprNodeType arg1_type = EXPR_NULL_LITERAL;
            if (arg->right->expr) {
                arg1_type = arg->right->expr->type;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg1_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg1_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg1_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg1_type != EXPR_STRING_LITERAL) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        // Check 2nd and 3rd arguments are numeric
        if (arg && arg->left && arg->left->right) {
            ExprNodeType arg2_type = EXPR_NULL_LITERAL;
            if (arg->left->right->expr) {
                arg2_type = arg->left->right->expr->type;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->left->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg2_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg2_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg2_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg2_type != EXPR_NUM_LITERAL) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' second argument must be numeric\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->left && arg->left->left->right) {
            ExprNodeType arg3_type = EXPR_NULL_LITERAL;
            if (arg->left->left->right->expr) {
                arg3_type = arg->left->left->right->expr->type;
            } else if (arg->left->left->right->left && arg->left->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->left->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->left->left->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg3_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg3_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg3_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid third argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg3_type != EXPR_NUM_LITERAL) {
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
            ExprNodeType arg1_type = EXPR_NULL_LITERAL;
            if (arg->right->expr) {
                arg1_type = arg->right->expr->type;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg1_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg1_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg1_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            
            if (arg1_type != EXPR_STRING_LITERAL) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->right) {
            ExprNodeType arg2_type = EXPR_NULL_LITERAL;
            if (arg->left->right->expr) {
                arg2_type = arg->left->right->expr->type;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->left->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg2_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg2_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg2_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg2_type != EXPR_STRING_LITERAL) {
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
            ExprNodeType arg1_type = EXPR_NULL_LITERAL;
            if (arg->right->expr) {
                arg1_type = arg->right->expr->type;
            } else if (arg->right->left && arg->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg1_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg1_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg1_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid first argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg1_type != EXPR_STRING_LITERAL) {
                fprintf(stderr, "[SEMANTIC] Built-in function '%s' first argument must be string\n", func_name);
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
        }
        if (arg && arg->left && arg->left->right) {
            ExprNodeType arg2_type = EXPR_NULL_LITERAL;
            if (arg->left->right->expr) {
                arg2_type = arg->left->right->expr->type;
            } else if (arg->left->right->left && arg->left->right->left->type == AST_FUNC_CALL) {
                int err = semantic_visit(arg->left->right->left, scope);
                if (err != NO_ERROR) return err;
                DataType return_type = arg->left->right->left->data_type;
                if (return_type == TYPE_NUM) {
                    arg2_type = EXPR_NUM_LITERAL;
                } else if (return_type == TYPE_STRING) {
                    arg2_type = EXPR_STRING_LITERAL;
                } else if (return_type == TYPE_NULL) {
                    arg2_type = EXPR_NULL_LITERAL;
                }
            } else {
                fprintf(stderr, "[SEMANTIC] Invalid second argument expression for '%s'\n", func_name);
                return SEM_ERROR_OTHER;
            }
            
            if (arg2_type != EXPR_NUM_LITERAL) {
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
        DataType arg_type = TYPE_UNDEF;
        
        if (arg_node->right->expr) {
            // NEW SYSTEM: Use expr for expressions
            arg_type = infer_expr_node_type(arg_node->right->expr, scope);
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
                return semantic_visit(node->left, current_scope);
            } break;
        case AST_MAIN_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                const char *func_name = "main";

                // Count parameters and check for duplicates - ROVNAKO AKO V AST_FUNC_DEF
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

                    // Check for duplicate parameter names - ROVNAKO AKO V AST_FUNC_DEF
                    for (Param *p = params; p; p = p->next) {
                        if (strcmp(p->name, param_name) == 0) {
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in main().\n", param_name);
                            return SEM_ERROR_REDEFINED;
                        }
                    }

                    // Create parameter structure - ROVNAKO AKO V AST_FUNC_DEF
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

                // Check for redefinition of main - ROVNAKO AKO V AST_FUNC_DEF
                SymTableData *existing = lookup_symbol(current_scope, func_name);
                if (existing && existing->type == NODE_FUNC) {
                    FunctionData *fdata = existing->data.func_data;
                    if (fdata->param_count == param_count) {
                        fprintf(stderr, "[SEMANTIC] Redefinition of 'main' function with %d parameters.\n", param_count);
                        return SEM_ERROR_REDEFINED;
                    }
                    // Different parameter count - overloading allowed ✓
                }

                // Create and insert main symbol - ROVNAKO AKO V AST_FUNC_DEF
                SymTableData *main_symbol = make_function(param_count, params, true, TYPE_UNDEF);
                if (!main_symbol) {
                    fprintf(stderr, "[SEMANTIC] Failed to allocate symbol for 'main'.\n");
                    return ERROR_INTERNAL;
                }

                if (!symtable_insert(&current_scope->symbols, func_name, main_symbol)) {
                    fprintf(stderr, "[SEMANTIC] Failed to insert 'main' into global symbol table.\n");
                    return ERROR_INTERNAL;
                }

                // Create new scope for main function body - ROVNAKO AKO V AST_FUNC_DEF
                Scope *main_scope = init_scope();
                if (!main_scope) {
                    fprintf(stderr, "[SEMANTIC] Failed to create scope for 'main'.\n");
                    return ERROR_INTERNAL;
                }
                main_scope->parent = current_scope;
                node->right->current_table = &main_scope->symbols;

                // Insert parameters into main scope - ROVNAKO AKO V AST_FUNC_DEF
                for (Param *p = params; p; p = p->next) {
                    SymTableData *param_var = make_variable(p->data_type, true, true);
                    if (!param_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                    if (!symtable_insert(&main_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into main scope.\n", p->name);
                        free(param_var);
                        return ERROR_INTERNAL;
                    }
                }

                // Analyze main function body with main scope - ROVNAKO AKO V AST_FUNC_DEF
                int result = semantic_visit(node->right, main_scope);
                
                
                // free_scope(main_scope);  // TOTO ODSTRÁŇTE!
                
                return result;
            } break;

        case AST_FUNC_DEF: {
                if (!node->right) return ERROR_INTERNAL;

                // Get function name
                const char *func_name = node->name;
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
                            fprintf(stderr, "[SEMANTIC] Duplicate parameter '%s' in function '%s'.\n",
                                    param_name, func_name);
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

                // Check for function redefinition 
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
                    SymTableData *param_var = make_variable(p->data_type, true, true);  // defined=true, initialized=true
                    if (!param_var) {
                        fprintf(stderr, "[SEMANTIC] Failed to create parameter '%s'.\n", p->name);
                        return ERROR_INTERNAL;
                    }
                    if (!symtable_insert(&func_scope->symbols, p->name, param_var)) {
                        fprintf(stderr, "[SEMANTIC] Failed to insert parameter '%s' into function scope.\n", p->name);
                        free(param_var);
                        return ERROR_INTERNAL;
                    }
                }

                node->right->current_table = &func_scope->symbols;
                // Analyze function body with function scope
                // The AST_BLOCK will handle its own scope creation and cleanup
                int result = semantic_visit(node->right, func_scope);
                
                // Cleanup function scope
                //free_scope(func_scope);
                
                return result;
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

                // Before analyzing the getter body, try to infer the getter's
                // return type by scanning the body for return statements that
                // contain literal or directly inferable expressions. Setting
                // the getter symbol's return_type early ensures that later
                // statements in the same block (which may reference the
                // getter) will see its return type.
                ASTNode *scan = node->right; // should be a BLOCK
                DataType found_type = scan_return_type(scan, getter_scope);

                if (found_type != TYPE_UNDEF) {
                    SymTableData *s = symtable_search(&current_scope->symbols, getter_name);
                    if (s && s->type == NODE_GETTER) {
                        s->data.getter_data->return_type = found_type;
                    }
                }

                // Now analyze getter body with getter scope
                int result = semantic_visit(node->right, getter_scope);

                return result;
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
                    SymTableData* sym = lookup_symbol(current_scope, left_name);
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
                            right_type = infer_expr_node_type(node->left->expr, current_scope);
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
                            fprintf(stderr, "[SEMANTIC] Type mismatch in setter call to '%s': expected %d, got %d\n",
                                    node->name, setter_param, right_type);
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
                    right_type = infer_expr_node_type(node->left->expr, current_scope);
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
                    fprintf(stderr, "[SEMANTIC] Type mismatch in setter call to '%s': expected %d, got %d\n",
                            node->name, setter_param, right_type);
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
                    right_type = infer_expr_node_type(expr_node->expr, current_scope);
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

                DataType left_type = var_data->data.var_data->data_type;

                // Type compatibility check
                if (right_type != TYPE_UNDEF && left_type != TYPE_UNDEF &&
                    right_type != left_type) {
                    fprintf(stderr,
                            "[SEMANTIC] Type mismatch in assignment to '%s': expected %d, got %d\n",
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
                node->current_scope = current_scope;
                
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

                // First, visit argument list so that expressions inside args
                // (including identifier->getter conversion and type inference)
                // are processed before we validate the call signature.
                if (node->left) {
                    int aerr = semantic_visit(node->left, current_scope);
                    if (aerr != NO_ERROR) return aerr;
                }

                // Check existence
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
                    return_type = infer_expr_node_type(node->expr, current_scope);
                } else if (node->left && node->left->type == AST_FUNC_CALL) {
                    // FUNCTION CALL: Process it first to get its return type
                    int err = semantic_visit(node->left, current_scope);
                    if (err != NO_ERROR) return err;
                    return_type = node->left->data_type;
                }
                // If neither expr nor function call, it's a void return (TYPE_NULL)
                
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

                // Continue traversing statements in this block using the
                // block scope (not the parent scope). This ensures that
                // subsequent statements in the same block see symbols
                // (e.g., getters) defined earlier in the block.
                return semantic_visit(node->right, current_scope);
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

                expr_type = infer_expr_node_type(node->expr, current_scope);
                node->data_type = expr_type;

                // Check if expression type inference failed
            if (expr_type == TYPE_UNDEF) {
                fprintf(stderr, "[SEMANTIC] Expression type inference failed\n");
                return SEM_ERROR_TYPE_COMPATIBILITY;
            }
                
                
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
    //free_scope(global_scope);
    
    return result;
}
