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

// Helper to get node type name as string
static const char* get_node_type_name(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_FUNC_CALL: return "FUNC_CALL";
        case AST_FUNC_ARG: return "FUNC_ARG";
        case AST_BLOCK: return "BLOCK";
        case AST_VAR_DECL: return "VAR_DECL";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_MAIN_DEF: return "MAIN_DEF";
        case AST_FUNC_DEF: return "FUNC_DEF";
        case AST_GETTER_DEF: return "GETTER_DEF";
        case AST_SETTER_DEF: return "SETTER_DEF";
        case AST_IF: return "IF";
        case AST_ELSE: return "ELSE";
        case AST_WHILE: return "WHILE";
        case AST_RETURN: return "RETURN";
        case AST_ASSIGN: return "ASSIGN";
        case AST_EQUALS: return "EQUALS";
        case AST_EXPRESSION: return "EXPRESSION";
        default: return "UNKNOWN";
    }
}

// Helper to print indentation with tree characters
static void print_tree_indent(int depth, int is_last[], int is_right) {
    for (int i = 0; i < depth - 1; i++) {
        printf(is_last[i] ? "    " : "│   ");
    }
    if (depth > 0) {
        const char* branch = is_last[depth - 1] ? "└── " : "├── ";
        const char* side = is_right ? "R - " : "L - ";
        printf("%s%s", branch, side);
    }
}

// Forward declaration for expression inline printing
static void print_expr_inline(ExprNode* expr, int depth, int is_last[]);

// Recursive helper to print AST tree
static void print_ast_node(ASTNode* node, int depth, int is_last[], int is_right_child) {
    if (!node) {
        print_tree_indent(depth, is_last, is_right_child);
        printf("(null)\n");
        return;
    }
    
    print_tree_indent(depth, is_last, is_right_child);
    printf("%s", get_node_type_name(node->type));
    
    // Print node name if it exists
    if (node->name) {
        printf(" [%s]", node->name);
    }
    
    // Print data type if defined
    if (node->data_type != TYPE_UNDEF) {
        printf(" (type: ");
        switch (node->data_type) {
            case TYPE_NULL: printf("NULL"); break;
            case TYPE_NUM: printf("NUM"); break;
            case TYPE_STRING: printf("STRING"); break;
            //case TYPE_BOOL: printf("BOOL"); break;
            default: printf("UNDEF"); break;
        }
        printf(")");
    }
    
    printf("\n");
    
    // Recursively print children (print RIGHT first, then LEFT)
    // But also include EXPR as a pseudo-child if present
    int has_expr = (node->expr != NULL);
    int has_left = (node->left != NULL);
    int has_right = (node->right != NULL);
    int total_children = has_expr + has_left + has_right;
    
    if (total_children > 0) {
        // Update is_last array for next level
        is_last[depth] = 0;
        int children_printed = 0;

        // Print right child first
        if (node->right) {
            children_printed++;
            is_last[depth] = (children_printed == total_children) ? 1 : 0;
            print_ast_node(node->right, depth + 1, is_last, 1);
        }

        // Print expr as a pseudo-child (label depends on node type)
        if (node->expr) {
            children_printed++;
            is_last[depth] = (children_printed == total_children) ? 1 : 0;
            print_tree_indent(depth + 1, is_last, 0);
            
            // Choose appropriate label based on parent node type
            switch (node->type) {
                case AST_IF:
                case AST_WHILE:
                    printf("CONDITION:\n");
                    break;
                case AST_ASSIGN:
                    printf("EXPR:\n");
                    break;
                default:
                    printf("EXPR:\n");
                    break;
            }
            
            // Print expression tree inline with proper tree connectors
            print_expr_inline(node->expr, depth + 2, is_last);
        }

        // Then print left child
        if (node->left) {
            children_printed++;
            is_last[depth] = 1; // Left is always last in this ordering
            print_ast_node(node->left, depth + 1, is_last, 0);
        }
    }
}

// Helper to print expression tree with AST-style connectors
static void print_expr_inline(ExprNode* expr, int depth, int is_last[]) {
    if (!expr) return;
    
    for (int i = 0; i < depth - 1; i++) {
        printf(is_last[i] ? "    " : "│   ");
    }
    
    switch (expr->type) {
        case EXPR_NUM_LITERAL:
            printf("NUM: %.2f\n", expr->data.num_literal);
            break;
        case EXPR_STRING_LITERAL:
            printf("STRING: \"%s\"\n", expr->data.string_literal);
            break;
        case EXPR_NULL_LITERAL:
            printf("NULL\n");
            break;
        case EXPR_TYPE_LITERAL:
            printf("TYPE: %s\n", expr->data.identifier_name);
            break;
        case EXPR_IDENTIFIER:
            printf("ID: %s\n", expr->data.identifier_name);
            break;
        case EXPR_GETTER_CALL:
            printf("GETTER: %s\n", expr->data.getter_name);
            break;
        case EXPR_BINARY_OP: {
            const char* ops[] = {"+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=", "IS"};
            printf("OP: %s\n", ops[expr->data.binary.op]);
            if (expr->data.binary.left) {
                for (int i = 0; i < depth - 1; i++) {
                    printf(is_last[i] ? "    " : "│   ");
                }
                printf("├── L: ");
                // Print left operand inline
                switch (expr->data.binary.left->type) {
                    case EXPR_NUM_LITERAL:
                        printf("%.2f\n", expr->data.binary.left->data.num_literal);
                        break;
                    case EXPR_IDENTIFIER:
                        printf("%s\n", expr->data.binary.left->data.identifier_name);
                        break;
                    default:
                        printf("\n");
                        print_expr_inline(expr->data.binary.left, depth + 1, is_last);
                        break;
                }
            }
            if (expr->data.binary.right) {
                for (int i = 0; i < depth - 1; i++) {
                    printf(is_last[i] ? "    " : "│   ");
                }
                printf("└── R: ");
                // Print right operand inline
                switch (expr->data.binary.right->type) {
                    case EXPR_NUM_LITERAL:
                        printf("%.2f\n", expr->data.binary.right->data.num_literal);
                        break;
                    case EXPR_IDENTIFIER:
                        printf("%s\n", expr->data.binary.right->data.identifier_name);
                        break;
                    default:
                        printf("\n");
                        print_expr_inline(expr->data.binary.right, depth + 1, is_last);
                        break;
                }
            }
            break;
        }
    }
}

// Public function to print AST tree with visual connections
void print_ast_tree(ASTNode* node) {
    printf("\n========== AST Tree Structure ==========\n");
    int is_last[100] = {0}; // Track which levels are last children
    print_ast_node(node, 0, is_last, 0);
    printf("========================================\n\n");
}
