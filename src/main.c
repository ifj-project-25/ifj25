/**
 * @file main.c
 * @author xcernoj00,xmikusm00
 * @brief Main function of interpret
 */

#include "error.h"
#include "scanner.h"
#include "semantic.h"
#include <stdio.h>
#include "parser.h"
#include "symtable.h"
#include "expr_parser.h"
#include "expr_precedence_parser.h"
#include "expr_ast.h"
#include "ast.h"



static void symtable_dump_node(SNode *node, int depth) {
    if (!node) return;
    symtable_dump_node(node->right, depth + 1);
    for (int i = 0; i < depth; ++i) fputs("  ", stdout);
    printf("%s\n", node->key);
    symtable_dump_node(node->left, depth + 1);
}

void symtable_dump(const SymTable *table) {
    symtable_dump_node(table->root, 0);
}

static void expr_ast_dump_node(const ExprNode *node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; ++i) fputs("  ", stdout);

    switch (node->type) {
    case EXPR_NUM_LITERAL:
        printf("num(%f)\n", node->data.num_literal);
        break;
    case EXPR_STRING_LITERAL:
        printf("str(\"%s\")\n", node->data.string_literal);
        break;
    case EXPR_IDENTIFIER:
        printf("id(%s)\n", node->data.identifier_name);
        break;
    case EXPR_BINARY_OP:
        printf("op(%d)\n", node->data.binary.op);  // map enum to text if you like
        expr_ast_dump_node(node->data.binary.left,  depth + 1);
        expr_ast_dump_node(node->data.binary.right, depth + 1);
        return;
    default:
        puts("??");
        break;
    }
}

void expr_ast_dump(const ExprNode *root) {
    expr_ast_dump_node(root, 0);
}

int main() {

    FILE *source_file;
    source_file = stdin;
 
    set_source_file(source_file);
    ASTNode* PROGRAM = create_ast_node(AST_PROGRAM, NULL);
    
    int error_code = parser(PROGRAM);

    if (PROGRAM) {
        print_ast_tree(PROGRAM);
    } 
    if (error_code != NO_ERROR) {
        fprintf(stderr, "Error during parsing: %d\n", error_code);
        return error_code;
    }
    else{
        FILE *fileOut = fopen("output.ifj25", "w");
        if (fileOut == NULL) {
            perror("Failed to open output file");
            return ERROR_INTERNAL;
        }
        error_code = semantic_analyze(PROGRAM);
        if (error_code != NO_ERROR) {
            fprintf(stderr, "Error during semantic analysis: %d\n", error_code);
            return error_code;
        }
        fclose(fileOut);
    }
    
    return error_code;
}
