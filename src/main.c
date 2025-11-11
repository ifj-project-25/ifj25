/**
 * @file main.c
 * @author xcernoj00,xmikusm00
 * @brief Main function of interpret
 */

#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include "parser.h"
#include "symtable.h"
#include "expr_parser.h"
#include "expr_ast.h"
#include "ast.h"

// print all symbols in the symbol table (for debugging)
void print_all_symbols(ASTNode *node);

// Convert AST node type to string for debugging
const char * ast_node_type_to_string(ASTNodeType type);

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

const char * ast_node_type_to_string(ASTNodeType type) {
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
        //case AST_FUNC_PARAM: return "FUNC_PARAM";
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

int main() {

    FILE *source_file;
    source_file = stdin;

    set_source_file(source_file);
    ASTNode* PROGRAM = NULL;
    int error_code = parser(&PROGRAM);

    if (PROGRAM) {
        print_ast_tree(PROGRAM);
    } else {
        printf("AST not created (PROGRAM is NULL)\n");
    }


    if (error_code != NO_ERROR) {
        printf("Parsing failed with error code: %d\n", error_code);
        return error_code;
    }
    else{
        printf("Parsing completed successfully.\n");
    }
    
    return NO_ERROR;
}
