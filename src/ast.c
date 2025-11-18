/**
 * @file ast.c
 * @brief Abstract Syntax Tree implementation for IFJ25 language
 * @author xmalikm00
 */

#include "ast.h"
#include "symtable.h"
#include "expr_ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declaration for my_strdup from symtable.c
char *my_strdup(const char *s);


// helper function to create a new AST node
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TREE_DEPTH 500  // Maximum tree depth to prevent buffer overflow

ASTNode* create_ast_node(ASTNodeType type, const char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    node->name = name ? my_strdup(name) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->expr = NULL;
    node->data_type = TYPE_UNDEF;
    node->current_table = NULL;
    return node;
}

// helper function to free AST tree
void free_ast_tree(ASTNode* node) {
    if (!node) return;
    
    free_ast_tree(node->left);
    free_ast_tree(node->right);
    if(node->expr) free_expr_node(node->expr);
    if (node->name) free(node->name);
    if (node->current_table) symtable_free(node->current_table);
    free(node);
}
