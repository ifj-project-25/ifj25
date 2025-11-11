/**
 * @file generator.c
 * @author xklusaa00
 * @brief Code generator implementation
 */

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void convert_string(const char* input) {
    // Implementation of string conversion to IFJcode25 format
    // This is a placeholder function
    //!!!!!!!!!!!!!!!!!!!! TODO !!!!!!!!!!!!!!!!!
}



int read_str_func(ASTNode *node, FILE *output) {
    fprintf(output, "\n");
    return 0;
}


// Code generation helper functions
int identifier (ASTNode *node, FILE *output) {
    if(node->type != AST_IDENTIFIER)
    {
        fprintf(stderr, "[GENERATOR] Expected identifier in variable declaration.\n");
        return -1; // Error: invalid AST structure
    }
    else if (node->string_val[0]=="_" && node->string_val[1] == "_")
    {
        fprintf(output, "GF@%s\n", node->string_val);
    }
    else
    {
        fprintf(output, "LF@%s\n", node->string_val);
    } 
    
    return 0;
}

int var_decl (ASTNode *node, FILE *output) {
    
    fprintf(output, "DEFVAR ");
    identifier(node->left, output);
    fprintf(output, "\n");
    return 0;
}

int assign (ASTNode *node, FILE *output) {
    // assign->left = AST_EQUALS, assign->right = next statement
    node = node->left; // AST_EQUALS
    if(node->type != AST_EQUALS)
    {
        fprintf(stderr, "[GENERATOR] Expected equals in assignment.\n");
        return -1; // Error: invalid AST structure
    }
    fprintf(output, "MOVE ");
    identifier(node->left, output);
    expression(node->right, output);
    fprintf(output, "\n");
    return 0;
}

// Code generation function
int generate_code(ASTNode *root, FILE *output) {
    if (!root || !output) {
        return -1; // Error: invalid arguments
    }
    
    // Code generation logic goes here
    
    return 0; // Success
}