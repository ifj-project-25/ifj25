/**
 * @file generator.c
 * @author xklusaa00
 * @brief Code generator implementation
 */

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void print_convert_string(const char* input) {
    // Convert string to IFJcode25 format
    // Replace special chars with \XXX (3-digit decimal ASCII)
    // Example: "hello\n" -> "hello\010"
    // Spaces: \032, newlines: \010, etc.
    for (int i = 0; i < strlen(input); i++)
    {
        if ((input[i] > 65 && input[i] <= 90) || (input[i] >= 97 && input[i] <= 122))
        {
            fprintf(stdout, "%c", input[i]);
        }
        if (input[i] == '\\')
        {
            i ++;
            switch (input[i])
            {
                case 'n':
                    fprintf(stdout, "\\010");
                    break;
                case 't':
                    fprintf(stdout, "\\009");
                    break;
                case 's':
                    fprintf(stdout, "\\032");
                    break;
                case '\\':
                    fprintf(stdout, "\\092");
                    break;
                case 'x':
                    {
                        char hex[3] = {input[i+1], input[i+2], '\0'};
                        long decimal = strtol(hex, NULL, 16);  // 16 = base 16 (hex)
                        fprintf(stdout, "\\%03ld", decimal);
                        i +=2;
                    }
                    break;
                default:
                    break;
            }
        }
        
        
    }
    
}



int read_str_func(ASTNode *node, FILE *output) {
    fprintf(output, "\n");
    return 0;
}


// Code generation helper functions


// REMAKE
int identifier (ASTNode *node, FILE *output) {
    if(node->type != AST_IDENTIFIER)
    {
        fprintf(stderr, "[GENERATOR] Expected identifier in variable declaration.\n");
        return -1; // Error: invalid AST structure
    }

    if (node->name && node->name[0]=='_' && node->name[1] == '_')
    {
        fprintf(output, "GF@%s\n", node->name);
    }
    else
    {
        fprintf(output, "LF@%s\n", node->name);
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
    if (node->right) {
        return next_step(node->right, output);
    }
    return 0;
}

// Code generation function
int generate_code(ASTNode *root, FILE *output) {
    if (!root || !output) return -1;
    
    // 1. Write IFJcode25 header
    fprintf(output, ".IFJcode25\n");
    
    // 2. Define built-in function labels
    fprintf(output, "JUMP $$main\n");
    
    // 3. Generate built-in functions (write, read_num, etc.)
    generate_builtin_functions(output);
    
    // 4. Generate main label
    fprintf(output, "LABEL $$main\n");
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");
    
    // 5. Traverse AST
    if (root->type == AST_PROGRAM) {
        // Process global variables (left child)
        if (root->left) {
            next_step(root->left, output);
        }
        // Process main/functions (right child)
        if (root->right) {
            next_step(root->right, output);
        }
    }
    
    // 6. Exit program
    fprintf(output, "POPFRAME\n");
    fprintf(output, "CLEARS\n");
    fprintf(output, "EXIT int@0\n");
    
    return 0;
}

int next_step(ASTNode *node, FILE *output) {
    if (!node) return 0;
    
    switch(node->type) {
        case AST_VAR_DECL:
            return var_decl(node, output);
        case AST_ASSIGN:
            return assign(node, output);
        case AST_FUNC_DEF:
            return func_def(node, output);
        case AST_MAIN_DEF:
            return main_def(node, output);
        case AST_FUNC_CALL:
            return func_call(node, output);
        case AST_IF:
            return if_stmt(node, output);
        case AST_WHILE:
            return while_loop(node, output);
        case AST_RETURN:
            return return_stmt(node, output);
        case AST_BLOCK:
            return block(node, output);
        case AST_EXPRESSION:
            return expression(node, output);
        // Add other cases...
        default:
            fprintf(stderr, "[GENERATOR] Unknown AST node type: %d\n", node->type);
            return -1;
    }
}