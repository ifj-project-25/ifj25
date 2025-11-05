/**
 * @file ast.c
 * @brief Abstract Syntax Tree implementation for IFJ25 language
 * @author xmalikm00
 */

#include "ast.h"
#include "symtable.h"
#include <stdlib.h>
#include <string.h>


// helper function to create a new AST node
ASTNode* create_ast_node(ASTNodeType type, const char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->name = name ? my_strdup(name) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->int_val = 0;
    node->float_val = 0.0;
    node->string_val = NULL;
    node->data_type = TYPE_UNDEF;
    node->current_table = NULL;
    return node;
}

// helper function to free AST tree
void free_ast_tree(ASTNode* node) {
    if (!node) return;
    
    free_ast_tree(node->left);
    free_ast_tree(node->right);
    if (node->name) free(node->name);
    if (node->string_val) free(node->string_val);
    if (node->current_table) symtable_free(node->current_table);
    free(node);
}