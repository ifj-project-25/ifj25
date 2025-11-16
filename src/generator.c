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

int get_scope_number(ASTNode *node) {
    int scope_number = 0;
    Scope *current_scope = node->current_scope;
    while (current_scope) {
        scope_number++;
        current_scope = current_scope->parent;
    }
    return scope_number;
}

int identifier (ASTNode *node, FILE *output) {
    if(node->type != AST_IDENTIFIER)
    {
        fprintf(stderr, "[GENERATOR] Expected identifier in variable declaration.\n");
        return -1; // Error: invalid AST structure
    }

    if (node->name && node->name[0]=='_' && node->name[1] == '_')
    {
        fprintf(output, "GF@%s", node->name);
    }
    else
    {
        
        fprintf(output, "LF@%s$%d", node->name, get_scope_number(node));
    } 
    
    return 0;
}

int var_decl (ASTNode *node, FILE *output) {
    
    fprintf(output, "DEFVAR ");
    identifier(node->left, output);
    fprintf(output, "\n");

    if (node->right) {
        return next_step(node->right, output);
    }
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

//IF statement
static int label_counter = 0;  // Global counter for unique labels

int if_stmt(ASTNode *node, FILE *output) {
    int if_id = label_counter++;
    
    // Evaluate condition
    if (expression(node->left, output) != 0) return -1;
    
    // Pop condition and jump if false
    fprintf(output, "PUSHS bool@false\n");
    fprintf(output, "JUMPIFEQS $else%d\n", if_id);
    
    // Generate 'then' block
    if (node->right && node->right->type == AST_BLOCK) {
        block(node->right, output);
    }
    node = node->right->right; // Move to next node (possibly else)

    
    // Check for else block
    if (node && node->type == AST_ELSE) {
        fprintf(output, "JUMP $endif%d\n", if_id);
        fprintf(output, "LABEL $else%d\n", if_id);
        node = node->right; // Move to next node
        if (node && node->type == AST_BLOCK) {
            block(node, output);
        }
    }
    else {
        fprintf(output, "LABEL $else%d\n", if_id);
    }
    next_step(node->right, output);
    return 0;
}

int while_loop(ASTNode *node, FILE *output) {
    int while_id = label_counter++;
    
    fprintf(output, "LABEL $while%d\n", while_id);
    
    // Evaluate condition
    if (expression(node->left, output) != 0) return -1;
    
    // Jump out if false
    fprintf(output, "PUSHS bool@true\n");
    fprintf(output, "JUMPIFEQS $endwhile%d\n", while_id);
    node = node->right;
    // Generate loop body
    if (node && node->type == AST_BLOCK) {
        block(node, output);
        node = node->right;
    }
    
    fprintf(output, "JUMP $while%d\n", while_id);
    fprintf(output, "LABEL $endwhile%d\n", while_id);
    next_step(node, output);
    return 0;
}

int block(ASTNode *node, FILE *output) {
    if (!node || node->type != AST_BLOCK) return -1;
    
    // Process statements inside the block
    if (node->left) {
        next_step(node->left, output);
    }
    return 0;
}

int func_def(ASTNode *node, FILE *output) {
    if (!node || !node->name) return -1;
    
    // Create function label
    fprintf(output, "JUMP $endfunc_%s\n", node->name);
    fprintf(output, "LABEL $func_%s\n", node->name);
    
    // Create new frame
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");
    
    // Handle parameters (node->left = AST_FUNC_ARG chain)
    int param_count = 0;
    ASTNode *param = node->left;
    while (param && param->type == AST_FUNC_ARG) {
        fprintf(output, "DEFVAR ");
        identifier(param->right, output);
        fprintf(output, "\n");
        fprintf(output, "POPS ");
        identifier(param->right, output);
        fprintf(output, "\n");
        param_count++;
        param = param->left;
    }
    
    // Generate function body
    if (node->right && node->right->type == AST_BLOCK) {
        block(node->right, output);
    }
    
    // Default return (if no explicit return)
    fprintf(output, "PUSHS nil@nil\n");
    fprintf(output, "POPFRAME\n");
    fprintf(output, "RETURN\n");
    
    fprintf(output, "LABEL $endfunc_%s\n", node->name);
    next_step(node->right->right, output);
    
    return 0;
}

int func_call(ASTNode *node, FILE *output) {
    if (!node || !node->name) return -1;
    
    // Check if it's a built-in function
    if (strcmp(node->name, "write") == 0) {
        return write_func(node, output);
    } else if (strcmp(node->name, "read_num") == 0) {
        return read_num_func(node, output);
    } else if (strcmp(node->name, "read_str") == 0) {
        return read_str_func(node, output);
    } else if (strcmp(node->name, "floor") == 0) {
        return floor_func(node, output);
    } else if (strcmp(node->name, "str") == 0) {
        return length_func(node, output);
    } else if (strcmp(node->name, "substring") == 0) {
        return substring_func(node, output);
    } else if (strcmp(node->name, "ord") == 0) {
        return ord_func(node, output);
    } else if (strcmp(node->name, "chr") == 0) {
        return chr_func(node, output);
    }
    
    
    // Push arguments in reverse order
    // Count arguments first
    int arg_count = 0;
    ASTNode *arg = node->left;
    while (arg && arg->type == AST_FUNC_ARG) {
        arg_count++;
        arg = arg->left;
    }
    
    // Store arguments in temporary array
    ASTNode **args = malloc(sizeof(ASTNode*) * arg_count);
    arg = node->left;
    for (int i = 0; i < arg_count; i++) {
        args[i] = arg->right;  // The expression
        arg = arg->left;
    }
    
    // Push arguments in reverse order
    for (int i = arg_count - 1; i >= 0; i--) {
        if (args[i]) {
            expression(args[i], output);
        }
    }
    free(args);
    
    fprintf(output, "CALL $func_%s\n", node->name);
    
    // Result is on stack
    
    return 0;
}

int return_stmt(ASTNode *node, FILE *output) {
    if (!node) return -1;
    
    // Evaluate return expression
    if (node->left) {
        expression(node->left, output);
    } else {
        // Return nil if no expression
        fprintf(output, "PUSHS nil@nil\n");
    }
    
    fprintf(output, "POPFRAME\n");
    fprintf(output, "RETURN\n");
    
    return 0;
}

int getter_def(ASTNode *node, FILE *output) {
    // Similar to func_def but no parameters
    if (!node || !node->name) return -1;
    
    // Create function label
    fprintf(output, "JUMP $endgetter_%s\n", node->name);
    fprintf(output, "LABEL $getter_%s\n", node->name);
    
    // Create new frame
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");
    
    // Generate function body
    if (node->right && node->right->type == AST_BLOCK) {
        block(node->right, output);
    }
    
    // Default return (if no explicit return)
    fprintf(output, "PUSHS nil@nil\n");
    fprintf(output, "POPFRAME\n");
    fprintf(output, "RETURN\n");
    
    fprintf(output, "LABEL $endgetter_%s\n", node->name);
    next_step(node->right->right, output);
    
    return 0;
}

int setter_def(ASTNode *node, FILE *output) {
    // Similar to func_def but no parameters
    if (!node || !node->name) return -1;
    
    // Create function label
    fprintf(output, "JUMP $endsetter_%s\n", node->name);
    fprintf(output, "LABEL $setter_%s\n", node->name);
    
    // Create new frame
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");
    
    // Generate function body
    if (node->right && node->right->type == AST_BLOCK) {
        block(node->right, output);
    }
    
    // Default return (if no explicit return)
    fprintf(output, "PUSHS nil@nil\n");
    fprintf(output, "POPFRAME\n");
    fprintf(output, "RETURN\n");
    
    fprintf(output, "LABEL $endsetter_%s\n", node->name);
    next_step(node->right->right, output);
    
    return 0;
}

int getter_call(ASTNode *node, FILE *output) {
    if (!node || !node->name) return -1;
    
    fprintf(output, "CALL $getter_%s\n", node->name);
    
    // Result is on stack
    
    return 0;
}

int setter_call(ASTNode *node, FILE *output) {
    if (!node || !node->name) return -1;
    
    // Evaluate value to set
    if (node->left) {
        expression(node->left, output);
    } else {
        fprintf(stderr, "[GENERATOR] Setter call missing value expression.\n");
        return -1;
    }
    
    fprintf(output, "CALL $setter_%s\n", node->name);
    
    // Result is on stack
    
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