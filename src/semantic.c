/**
 * @file semantic.c
 * @brief Implementation of semantic analysis
 * @author xmalikm00
 */

#include "semantic.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


void print_all_symbols(ASTNode *node) {
    if (!node) {
        return;
    }
    printf("AST Node type: %d", node->type);
    if (node->name) printf(", name: %s", node->name);
    printf("\n");
    
    print_all_symbols(node->left);
    print_all_symbols(node->right);
    print_all_symbols(node->body);
    print_all_symbols(node->next);
    
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
void free_scope(Scope* scope){
    free(scope);
}


// Preload built-in functions into the global scope
void preload_builtins(Scope *global_scope) {
    // inputs() -> string?
    SymTableData *inputs = make_function(0, NULL, true);
    symtable_insert(&global_scope->symbols, "inputs", inputs);
    
    // print(...) - variadic
    SymTableData *print = make_function(-1, NULL, true);
    symtable_insert(&global_scope->symbols, "print", print);
    
    // len(s: string) -> int
    Param *len_p = make_param("s", TYPE_STRING);
    SymTableData *len = make_function(1, len_p, true);
    symtable_insert(&global_scope->symbols, "len", len);
    
    // ... TODO for inputi, inputf, substr, ord, chr...
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
//---------- MAIN VISIT: Prechádza AST strom ----------
int semantic_visit(ASTNode *node, Scope *current_scope) {
    if (!node) return NO_ERROR;
    
    switch (node->type) {
        case AST_PROGRAM:
            // Prejdi všetky statements (linked list cez ->next alebo ->body)
            // Závisí od tvojho parsera - kde uložil statements?
            // Predpoklad: AST_PROGRAM má statements v node->body, ďalšie v ->next
            {
                ASTNode *stmt = node->body; // alebo node->left
                while (stmt) {
                    int err = semantic_visit(stmt, current_scope);
                    if (err) return err;
                    stmt = stmt->next;
                }
            }
            break;
            
        case AST_FUNC_DEF:
            // 1. Vlož funkciu do current scope (node->name)
            // 2. Vytvor nový scope pre telo funkcie
            // 3. Rekurzívne analyzuj node->body
            {
                // TODO: Spočítaj parametre, vlož do symboltable
                // Scope *func_scope = init_scope();
                // func_scope->parent = current_scope;
                // semantic_visit(node->body, func_scope);
                // free_scope(func_scope);
            }
            break;
            
        case AST_VAR_DECL:
            // 1. Skontroluj redefiníciu: lookup_symbol(current_scope, node->name)
            // 2. Analyzuj init_expr (node->right alebo node->left?)
            // 3. Vlož premennú: symtable_insert(&current_scope->symbols, node->name, ...)
            {
                SymTableData *existing = symtable_search(&current_scope->symbols, node->name);
                if (existing) {
                    fprintf(stderr, "[SEMANTIC] Redefinition of '%s'\n", node->name);
                    return SEM_ERROR_REDEFINED;
                }
                // TODO: Analyzuj node->right (init expression)
                // TODO: Vlož do symtable
            }
            break;
            
        case AST_ASSIGN:
            // 1. Skontroluj existenciu: lookup_symbol(current_scope, node->name)
            // 2. Analyzuj výraz: semantic_visit(node->right, current_scope)
            // 3. Porovnaj typy
            {
                SymTableData *var = lookup_symbol(current_scope, node->name);
                if (!var) {
                    fprintf(stderr, "[SEMANTIC] Undefined variable '%s'\n", node->name);
                    return SEM_ERROR_UNDEFINED;
                }
                // TODO: Kontrola typov
            }
            break;
            
        case AST_IDENTIFIER:
            // Skontroluj existenciu a inicializáciu
            {
                SymTableData *var = lookup_symbol(current_scope, node->name);
                if (!var) {
                    fprintf(stderr, "[SEMANTIC] Undefined '%s'\n", node->name);
                    return SEM_ERROR_UNDEFINED;
                }
                if (var->type == NODE_VAR && !var->data.var_data->initialized) {
                    fprintf(stderr, "[SEMANTIC] Variable '%s' not initialized\n", node->name);
                    return SEM_ERROR_TYPE_COMPATIBILITY;
                }
            }
            break;
            
        case AST_FUNC_CALL:
            // 1. Skontroluj funkciu: lookup_symbol(current_scope, node->name)
            // 2. Analyzuj argumenty (linked list v node->left alebo node->body?)
            // 3. Skontroluj počet argumentov
            {
                SymTableData *func = lookup_symbol(current_scope, node->name);
                if (!func || func->type != NODE_FUNC) {
                    fprintf(stderr, "[SEMANTIC] Undefined function '%s'\n", node->name);
                    return SEM_ERROR_UNDEFINED;
                }
                
                // TODO: Spočítaj argumenty v linked liste
                // ASTNode *arg = node->left;
                // int arg_count = 0;
                // while (arg) {
                //     semantic_visit(arg, current_scope);
                //     arg_count++;
                //     arg = arg->next;
                // }
                
                // TODO: Porovnaj arg_count s func->data.func_data->param_count
            }
            break;
            
        case AST_IF:
            // 1. Analyzuj condition (node->left)
            // 2. Analyzuj then_branch (node->body alebo node->right?)
            // 3. Analyzuj else_branch (node->next alebo kde?)
            {
                if (node->left) {
                    int err = semantic_visit(node->left, current_scope);
                    if (err) return err;
                }
                if (node->body) { // then branch
                    int err = semantic_visit(node->body, current_scope);
                    if (err) return err;
                }
                if (node->right) { // else branch?
                    int err = semantic_visit(node->right, current_scope);
                    if (err) return err;
                }
            }
            break;
            
        case AST_WHILE:
            // 1. Analyzuj condition (node->left)
            // 2. Analyzuj body (node->body)
            {
                if (node->left) {
                    int err = semantic_visit(node->left, current_scope);
                    if (err) return err;
                }
                if (node->body) {
                    int err = semantic_visit(node->body, current_scope);
                    if (err) return err;
                }
            }
            break;
            
        case AST_RETURN:
            // 1. TODO: Skontroluj, či sme vo funkcii (potrebuješ flag v Scope)
            // 2. Analyzuj return expression (node->left alebo node->right?)
            {
                if (node->left) {
                    int err = semantic_visit(node->left, current_scope);
                    if (err) return err;
                }
            }
            break;
            
        case AST_BLOCK:
            // Prejdi všetky statements (linked list cez ->next alebo ->body?)
            {
                ASTNode *stmt = node->body; // alebo node->left
                while (stmt) {
                    int err = semantic_visit(stmt, current_scope);
                    if (err) return err;
                    stmt = stmt->next;
                }
            }
            break;
            
        case AST_BINOP:
            // 1. Analyzuj ľavý operand (node->left)
            // 2. Analyzuj pravý operand (node->right)
            // 3. TODO: Skontroluj typovú kompatibilitu
            {
                if (node->left) {
                    int err = semantic_visit(node->left, current_scope);
                    if (err) return err;
                }
                if (node->right) {
                    int err = semantic_visit(node->right, current_scope);
                    if (err) return err;
                }
            }
            break;
            
        case AST_UNOP:
            // Analyzuj operand (node->left alebo node->right?)
            {
                if (node->left) {
                    int err = semantic_visit(node->left, current_scope);
                    if (err) return err;
                }
            }
            break;
            
        // Literály nepotrebujú kontrolu
        case AST_LITERAL_INT:
        case AST_LITERAL_DOUBLE:
        case AST_LITERAL_STRING:
        case AST_LITERAL_NULL:
            break;
            
        default:
            // Ostatné typy ignoruj alebo pridaj warning
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
    // check for empty tree
    if (!root) {
        fprintf(stderr, "[SEMANTIC] Empty symbol tree.\n");
        return NO_ERROR;
    }
    // debug: print all symbols in the tree
    print_all_symbols(root);

    Scope* global_scope = init_scope();
    if (!global_scope) {
        fprintf(stderr, "[SEMANTIC] Failed to initialize global scope.\n");
        return ERROR_INTERNAL;
    }

    preload_builtins(global_scope);

    int result = semantic_visit(root, global_scope);
    
    free_scope(global_scope);

    return result;
}
