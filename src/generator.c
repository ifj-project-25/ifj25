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

int expr_identifier (ExprNode *node, FILE *output) {
    if(node->type != EXPR_IDENTIFIER)
    {
        fprintf(stderr, "[GENERATOR] Expected identifier in variable declaration.\n");
        return -1; // Error: invalid AST structure
    }

    if (node->data.identifier_name && node->data.identifier_name[0]=='_' && node->data.identifier_name[1] == '_')
    {
        fprintf(output, "GF@%s", node->data.identifier_name);
    }
    else
    {
        
        fprintf(output, "LF@%s$%d", node->data.identifier_name, get_scope_number(node->data.current_scope));
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
    
    // Handle parameter (node->left = identifier)
    fprintf(output, "DEFVAR ");
    identifier(node->left, output);
    fprintf(output, "\n");

    // Pop argument into parameter variable
    fprintf(output, "POPS ");
    identifier(node->left, output);
    fprintf(output, "\n");
    
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

int expr_getter_call(char* name, FILE *output) {
    if (!name) return -1;
    
    fprintf(output, "CALL $getter_%s\n", name);
    
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
    
    return next_step(node->right, output);
}

int write_func(ASTNode *node, FILE *output) {
    // node->left = argument chain
    ASTNode *arg = node->left;
    printf(output, "CREATEFRAME\n");
    printf(output, "PUSHFRAME\n");

    while (arg && arg->type == AST_FUNC_ARG) {
        // Evaluate argument expression
        if (arg->right) {
            expression(arg->right, output);
        }
        fprintf(output, "DEFVAR LF@tmp\n");
        // Pop and write to output
        fprintf(output, "POPS LF@tmp\n");
        fprintf(output, "WRITE LF@tmp\n");
        
        arg = arg->left;  // Next argument
    }
    printf(output, "POPFRAME\n");
    return 0;
}

int str_func(ASTNode *node, FILE *output) {
    // Get argument
    if (node->left && node->left->right) {
        expression(node->left->right, output);
    }
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@tmp\n");
    fprintf(output, "DEFVAR LF@result\n");

    fprintf(output, "POPS LF@tmp\n");
    // Convert to string
    fprintf(output, "FLOAT2STR LF@result LF@tmp\n");
    fprintf(output, "PUSHS LF@result\n");
    
    fprintf(output, "POPFRAME\n");
    return 0;
}

int read_num_func(ASTNode *node, FILE *output) {
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@tmp_read\n");

    fprintf(output, "READ LF@tmp_read float\n");
    fprintf(output, "PUSHS LF@tmp_read\n");
    return 0;
}

int substring_func(ASTNode *node, FILE *output) {
    // Arguments: string s, start index i, end index j
    ASTNode *arg = node->left;
    int func_id = label_counter++;
    
    // Evaluate all three arguments (pushed in order: s, i, j)
    if (arg && arg->right) expression(arg->right, output);  // string s
    arg = arg->left;
    if (arg && arg->right) expression(arg->right, output);  // start i
    arg = arg->left;
    if (arg && arg->right) expression(arg->right, output);  // end j
    
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    // Define local variables
    fprintf(output, "DEFVAR LF@str\n");
    fprintf(output, "DEFVAR LF@start\n");
    fprintf(output, "DEFVAR LF@end\n");
    fprintf(output, "DEFVAR LF@len\n");
    fprintf(output, "DEFVAR LF@result\n");
    fprintf(output, "DEFVAR LF@idx\n");
    fprintf(output, "DEFVAR LF@char\n");
    fprintf(output, "DEFVAR LF@start_int\n");
    fprintf(output, "DEFVAR LF@end_int\n");
    fprintf(output, "DEFVAR LF@start_type\n");
    fprintf(output, "DEFVAR LF@end_type\n");

    // Pop arguments (reverse order)
    fprintf(output, "POPS LF@end\n");
    fprintf(output, "POPS LF@start\n");
    fprintf(output, "POPS LF@str\n");
    
    // Check if i and j are numeric (not string) - error 6 if string
    fprintf(output, "TYPE LF@start_type LF@start\n");
    fprintf(output, "TYPE LF@end_type LF@end\n");
    fprintf(output, "JUMPIFEQ $substr_type_error%d LF@start_type string@string\n", func_id);
    fprintf(output, "JUMPIFEQ $substr_type_error%d LF@end_type string@string\n", func_id);
    
    // Check if i and j are integers (whole numbers) using ISINT
    fprintf(output, "ISINT LF@result LF@start\n");
    fprintf(output, "JUMPIFEQ $substr_type_error%d LF@result bool@false\n", func_id);
    fprintf(output, "ISINT LF@result LF@end\n");
    fprintf(output, "JUMPIFEQ $substr_type_error%d LF@result bool@false\n", func_id);
    
    // Convert to int
    fprintf(output, "FLOAT2INT LF@start_int LF@start\n");
    fprintf(output, "FLOAT2INT LF@end_int LF@end\n");
    fprintf(output, "JUMP $substr_validations%d\n", func_id);
    
    // Type error label
    fprintf(output, "LABEL $substr_type_error%d\n", func_id);
    fprintf(output, "EXIT int@6\n");
    
    fprintf(output, "LABEL $substr_validations%d\n", func_id);
    
    // Get string length
    fprintf(output, "STRLEN LF@len LF@str\n");
    
    // Validation: i < 0 → return null
    fprintf(output, "LT LF@result LF@start_int int@0\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    
    // Validation: j < 0 → return null
    fprintf(output, "LT LF@result LF@end_int int@0\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    
    // Validation: i > j → return null
    fprintf(output, "GT LF@result LF@start_int LF@end_int\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    
    // Validation: i >= length(s) → return null
    fprintf(output, "GT LF@result LF@start_int LF@len\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    fprintf(output, "EQ LF@result LF@start_int LF@len\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    
    // Validation: j > length(s) → return null
    fprintf(output, "GT LF@result LF@end_int LF@len\n");
    fprintf(output, "JUMPIFEQ $substr_return_null%d LF@result bool@true\n", func_id);
    
    // All validations passed - extract substring
    fprintf(output, "MOVE LF@result string@\n");  // Initialize empty result string
    fprintf(output, "MOVE LF@idx LF@start_int\n");
    
    // Loop: while idx < end
    fprintf(output, "LABEL $substr_loop%d\n", func_id);
    fprintf(output, "LT LF@char LF@idx LF@end_int\n");
    fprintf(output, "JUMPIFEQ $substr_done%d LF@char bool@false\n", func_id);
    
    // Get character at index idx
    fprintf(output, "GETCHAR LF@char LF@str LF@idx\n");
    
    // Append character to result
    fprintf(output, "CONCAT LF@result LF@result LF@char\n");
    
    // Increment idx
    fprintf(output, "ADD LF@idx LF@idx int@1\n");
    fprintf(output, "JUMP $substr_loop%d\n", func_id);
    
    // Return result
    fprintf(output, "LABEL $substr_done%d\n", func_id);
    fprintf(output, "PUSHS LF@result\n");
    fprintf(output, "JUMP $substr_end%d\n", func_id);
    
    // Return null
    fprintf(output, "LABEL $substr_return_null%d\n", func_id);
    fprintf(output, "PUSHS nil@nil\n");
    
    fprintf(output, "LABEL $substr_end%d\n", func_id);
    fprintf(output, "POPFRAME\n");
    return 0;
}

int length_func(ASTNode *node, FILE *output) {

    // Note: Assuming this is string length, not str conversion
    // Get argument (string)

    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@tmp\n");
    fprintf(output, "DEFVAR LF@result\n");
    fprintf(output, "DEFVAR LF@type\n");
    if (node->left && node->left->right) {
        expression(node->left->right, output);
    }
    label_counter++;
    fprintf(output, "POPS LF@tmp\n");
    //if not str then we convert to str
    fprintf(output, "TYPE LF@type LF@tmp\n");
    fprintf(output, "JUMPIFEQ $is_str%d LF@type string@string\n", label_counter);
    //convert to str
    fprintf(output, "FLOAT2STR LF@tmp LF@tmp\n");
    fprintf(output, "LABEL $is_str%d\n", label_counter);

    fprintf(output, "STRLEN LF@result LF@tmp\n");
    fprintf(output, "PUSHS LF@result\n");
    
    fprintf(output, "POPFRAME\n");
    return 0;
}

int read_str_func(ASTNode *node, FILE *output) {
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@tmp_read\n");
    
    fprintf(output, "READ LF@tmp_read string\n");
    fprintf(output, "PUSHS LF@tmp_read\n");
    return 0;
}

int floor_func(ASTNode *node, FILE *output) {
    // Get argument
    if (node->left && node->left->right) {
        expression(node->left->right, output);
    }
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@tmp\n");
    fprintf(output, "DEFVAR LF@tmp_int\n");

    
    // Floor operation (convert to int and back)
    fprintf(output, "POPS LF@tmp\n");
    fprintf(output, "FLOAT2INT LF@tmp_int LF@tmp\n");
    fprintf(output, "INT2FLOAT LF@tmp LF@tmp_int\n");
    fprintf(output, "PUSHS LF@tmp\n");
    
    return 0;
}

int ord_func(ASTNode *node, FILE *output) {
    // Get character at index
    // Arguments: string, index
    ASTNode *arg = node->left;
    
    if (arg && arg->right) expression(arg->right, output);  // string
    arg = arg->left;
    if (arg && arg->right) expression(arg->right, output);  // index
    
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");
    fprintf(output, "DEFVAR LF@str\n");
    fprintf(output, "DEFVAR LF@index\n");
    fprintf(output, "DEFVAR LF@result\n");

    fprintf(output, "POPS LF@index\n");
    fprintf(output, "POPS LF@str\n");
    fprintf(output, "STRI2INT LF@result LF@str LF@index\n");
    fprintf(output, "PUSHS LF@result\n");
    
    fprintf(output, "POPFRAME\n");
    return 0;
}

int chr_func(ASTNode *node, FILE *output) {
    // Convert ASCII value to character
    if (node->left && node->left->right) {
        expression(node->left->right, output);
    }
    
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@ascii\n");
    fprintf(output, "DEFVAR LF@result\n");

    fprintf(output, "POPS LF@ascii\n");
    fprintf(output, "INT2CHAR LF@result LF@ascii\n");
    fprintf(output, "PUSHS LF@result\n");
    
    fprintf(output, "POPFRAME\n");
    return 0;
}

int strcmp_func(ASTNode *node, FILE *output) {
    // Compare two strings
    // Arguments: string1, string2
    ASTNode *arg = node->left;
    label_counter++;
    if (arg && arg->right) expression(arg->right, output);  // string1
    arg = arg->left;
    if (arg && arg->right) expression(arg->right, output);  // string2
    
    fprintf(output, "CREATEFRAME\n");
    fprintf(output, "PUSHFRAME\n");

    fprintf(output, "DEFVAR LF@str1\n");
    fprintf(output, "DEFVAR LF@str2\n");
    fprintf(output, "DEFVAR LF@result\n");

    fprintf(output, "POPS LF@str2\n");
    fprintf(output, "POPS LF@str1\n");

    fprintf(output, "LT LF@result LF@str1 LF@str2\n");
    fprintf(output, "JUMPIFEQ $strcmp_less%d LF@result bool@true\n", label_counter);
    fprintf(output, "GT LF@result LF@str1 LF@str2\n");
    fprintf(output, "JUMPIFEQ $strcmp_greater%d LF@result bool@true\n", label_counter);
    // Equal
    fprintf(output, "MOVE LF@result float@0.0\n");
    fprintf(output, "JUMP $strcmp_end%d\n", label_counter);
    // Less than
    fprintf(output, "LABEL $strcmp_less%d\n", label_counter);
    fprintf(output, "MOVE LF@result float@-1.0\n");
    fprintf(output, "JUMP $strcmp_end%d\n", label_counter);
    // Greater than
    fprintf(output, "LABEL $strcmp_greater%d\n", label_counter);
    fprintf(output, "MOVE LF@result float@1.0\n");
    // End
    fprintf(output, "LABEL $strcmp_end%d\n", label_counter);

    fprintf(output, "PUSHS LF@result\n");
    
    fprintf(output, "POPFRAME\n");
    return 0;
}

int generate_expression_code(ExprNode *expr, FILE *output) {
    if (!expr) return -1;
    
    switch(expr->type) {
        case EXPR_NUM_LITERAL:
            // Push numeric literal to stack
            fprintf(output, "PUSHS float@%a\n", expr->data.num_literal);
            break;
            
        case EXPR_STRING_LITERAL:
            // Push string literal to stack
            fprintf(output, "PUSHS string@");
            print_convert_string(expr->data.string_literal);
            fprintf(output, "\n");
            break;
            
        case EXPR_NULL_LITERAL:
            // Push nil to stack
            fprintf(output, "PUSHS nil@nil\n");
            break;
            
        case EXPR_IDENTIFIER:
            // Push variable value to stack
            fprintf(output, "PUSHS ");
            // Check if it's a global variable (starts with __)
            expr_identifier(expr, output);
            fprintf(output, "\n");
            break;
            
        case EXPR_BINARY_OP:
            // Recursively generate code for operands (postfix order)
            // First push left operand
            if (generate_expression_code(expr->data.binary.left, output) != 0) {
                return -1;
            }
            // Then push right operand
            if (generate_expression_code(expr->data.binary.right, output) != 0) {
                return -1;
            }
            
            // Apply operation (operands are on stack)
            // Create temporary frame for type checking
            int op_id = label_counter++;
            
            fprintf(output, "CREATEFRAME\n");
            fprintf(output, "PUSHFRAME\n");
            fprintf(output, "DEFVAR LF@op1\n");
            fprintf(output, "DEFVAR LF@op2\n");
            fprintf(output, "DEFVAR LF@type1\n");
            fprintf(output, "DEFVAR LF@type2\n");
            fprintf(output, "DEFVAR LF@result\n");
            
            switch(expr->data.binary.op) {
                case OP_ADD:
                    // Addition: can be numeric + numeric OR string + string (concatenation)
                    fprintf(output, "POPS LF@op2\n");
                    fprintf(output, "POPS LF@op1\n");
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");
                    
                    // Check for bool type (not allowed)
                    fprintf(output, "JUMPIFEQ $add_type_error_%d LF@type1 string@bool\n", op_id);
                    fprintf(output, "JUMPIFEQ $add_type_error_%d LF@type2 string@bool\n", op_id);
                    
                    // Check if both are strings
                    fprintf(output, "JUMPIFEQ $add_check_string_%d LF@type1 string@string\n", op_id);
                    
                    // Not strings, must be numeric
                    fprintf(output, "JUMPIFEQ $add_numeric_%d LF@type1 string@float\n", op_id);
                    fprintf(output, "LABEL $add_type_error_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    
                    fprintf(output, "LABEL $add_numeric_%d\n", op_id);
                    fprintf(output, "JUMPIFEQ $add_both_numeric_%d LF@type2 string@float\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    
                    fprintf(output, "LABEL $add_both_numeric_%d\n", op_id);
                    fprintf(output, "PUSHS LF@op1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "ADDS\n");
                    fprintf(output, "POPFRAME\n");
                    fprintf(output, "JUMP $add_end_%d\n", op_id);
                    
                    // String concatenation path
                    fprintf(output, "LABEL $add_check_string_%d\n", op_id);
                    fprintf(output, "JUMPIFEQ $add_both_string_%d LF@type2 string@string\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    
                    fprintf(output, "LABEL $add_both_string_%d\n", op_id);
                    fprintf(output, "CONCAT LF@result LF@op1 LF@op2\n");
                    fprintf(output, "PUSHS LF@result\n");
                    fprintf(output, "POPFRAME\n");
                    
                    fprintf(output, "LABEL $add_end_%d\n", op_id);
                    break;
                    
                case OP_SUB:
                    // Subtraction: both must be numeric
                    fprintf(output, "POPS LF@op2\n");
                    fprintf(output, "POPS LF@op1\n");
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");
                    
                    // Check for bool type
                    fprintf(output, "JUMPIFEQ $sub_type_error_%d LF@type1 string@bool\n", op_id);
                    fprintf(output, "JUMPIFEQ $sub_type_error_%d LF@type2 string@bool\n", op_id);
                    
                    fprintf(output, "JUMPIFEQ $sub_check2_%d LF@type1 string@float\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    fprintf(output, "LABEL $sub_check2_%d\n", op_id);
                    fprintf(output, "JUMPIFEQ $sub_ok_%d LF@type2 string@float\n", op_id);
                    fprintf(output, "LABEL $sub_type_error_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    fprintf(output, "LABEL $sub_ok_%d\n", op_id);
                    fprintf(output, "PUSHS LF@op1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "SUBS\n");
                    fprintf(output, "POPFRAME\n");
                    break;
                    
                case OP_MUL:
                    // Multiplication: numeric * numeric OR string * int
                    fprintf(output, "POPS LF@op2\n");
                    fprintf(output, "POPS LF@op1\n");
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");
                    
                    // Check for bool type
                    fprintf(output, "JUMPIFEQ $mul_type_error_%d LF@type1 string@bool\n", op_id);
                    fprintf(output, "JUMPIFEQ $mul_type_error_%d LF@type2 string@bool\n", op_id);
                    
                    // Check if left is string (string iteration)
                    fprintf(output, "JUMPIFEQ $mul_string_iter_%d LF@type1 string@string\n", op_id);
                    
                    // Not string, must be numeric multiplication
                    fprintf(output, "JUMPIFEQ $mul_check2_%d LF@type1 string@float\n", op_id);
                    fprintf(output, "JUMP $mul_type_error_%d\n", op_id);
                    
                    fprintf(output, "LABEL $mul_check2_%d\n", op_id);
                    fprintf(output, "JUMPIFEQ $mul_numeric_%d LF@type2 string@float\n", op_id);
                    fprintf(output, "JUMP $mul_type_error_%d\n", op_id);
                    
                    fprintf(output, "LABEL $mul_numeric_%d\n", op_id);
                    fprintf(output, "PUSHS LF@op1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "MULS\n");
                    fprintf(output, "POPFRAME\n");
                    fprintf(output, "JUMP $mul_end_%d\n", op_id);
                    
                    // String iteration: string * int
                    fprintf(output, "LABEL $mul_string_iter_%d\n", op_id);
                    // Check if right operand is numeric and integer
                    fprintf(output, "JUMPIFEQ $mul_check_int_%d LF@type2 string@float\n", op_id);
                    fprintf(output, "JUMP $mul_type_error_%d\n", op_id);
                    
                    fprintf(output, "LABEL $mul_check_int_%d\n", op_id);
                    fprintf(output, "ISINT LF@result LF@op2\n");
                    fprintf(output, "JUMPIFEQ $mul_type_error_%d LF@result bool@false\n", op_id);
                    
                    // Convert to int
                    fprintf(output, "DEFVAR LF@count\n");
                    fprintf(output, "DEFVAR LF@iter\n");
                    fprintf(output, "DEFVAR LF@temp_str\n");
                    fprintf(output, "FLOAT2INT LF@count LF@op2\n");
                    
                    // Check if count < 0
                    fprintf(output, "LT LF@result LF@count int@0\n");
                    fprintf(output, "JUMPIFEQ $mul_type_error_%d LF@result bool@true\n", op_id);
                    
                    // Initialize result and iterator
                    fprintf(output, "MOVE LF@result string@\n");
                    fprintf(output, "MOVE LF@iter int@0\n");
                    
                    // Loop: concatenate string count times
                    fprintf(output, "LABEL $mul_iter_loop_%d\n", op_id);
                    fprintf(output, "LT LF@temp_str LF@iter LF@count\n");
                    fprintf(output, "JUMPIFEQ $mul_iter_done_%d LF@temp_str bool@false\n", op_id);
                    fprintf(output, "CONCAT LF@result LF@result LF@op1\n");
                    fprintf(output, "ADD LF@iter LF@iter int@1\n");
                    fprintf(output, "JUMP $mul_iter_loop_%d\n", op_id);
                    
                    fprintf(output, "LABEL $mul_iter_done_%d\n", op_id);
                    fprintf(output, "PUSHS LF@result\n");
                    fprintf(output, "POPFRAME\n");
                    fprintf(output, "JUMP $mul_end_%d\n", op_id);
                    
                    fprintf(output, "LABEL $mul_type_error_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");
                    fprintf(output, "LABEL $mul_end_%d\n", op_id);
                    break;
                    
                case OP_DIV:
                    // Division: both must be numeric, divisor cannot be zero
                    fprintf(output, "POPS LF@op2\n");
                    fprintf(output, "POPS LF@op1\n");
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");
                    
                    // Check for bool type
                    fprintf(output, "JUMPIFEQ $div_type_error_%d LF@type1 string@bool\n", op_id);
                    fprintf(output, "JUMPIFEQ $div_type_error_%d LF@type2 string@bool\n", op_id);
                    
                    fprintf(output, "JUMPIFEQ $div_check2_%d LF@type1 string@float\n", op_id);
                    fprintf(output, "LABEL $div_type_error_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    fprintf(output, "LABEL $div_check2_%d\n", op_id);
                    fprintf(output, "JUMPIFEQ $div_check_zero_%d LF@type2 string@float\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    
                    // Check for division by zero
                    fprintf(output, "LABEL $div_check_zero_%d\n", op_id);
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "PUSHS float@0x0p+0\n");
                    fprintf(output, "EQS\n");
                    fprintf(output, "PUSHS bool@true\n");
                    fprintf(output, "JUMPIFEQS $div_by_zero_%d\n", op_id);
                    
                    fprintf(output, "PUSHS LF@op1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "DIVS\n");
                    fprintf(output, "POPFRAME\n");
                    fprintf(output, "JUMP $div_end_%d\n", op_id);
                    
                    fprintf(output, "LABEL $div_by_zero_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Division by zero error
                    fprintf(output, "LABEL $div_end_%d\n", op_id);
                    break;
                    
                case OP_LT:
                case OP_GT:
                case OP_LTE:
                case OP_GTE:
                    // Relational operators: both must be same type (numeric or string, not bool)
                    fprintf(output, "POPS LF@op2\n");
                    fprintf(output, "POPS LF@op1\n");
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");
                    
                    // Check for bool type (not allowed in relational comparisons)
                    fprintf(output, "JUMPIFEQ $rel_type_error_%d LF@type1 string@bool\n", op_id);
                    fprintf(output, "JUMPIFEQ $rel_type_error_%d LF@type2 string@bool\n", op_id);
                    
                    fprintf(output, "JUMPIFEQ $rel_same_type_%d LF@type1 LF@type2\n", op_id);
                    fprintf(output, "LABEL $rel_type_error_%d\n", op_id);
                    fprintf(output, "EXIT int@26\n");  // Type error
                    fprintf(output, "LABEL $rel_same_type_%d\n", op_id);
                    fprintf(output, "PUSHS LF@op1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    
                    if (expr->data.binary.op == OP_LT) {
                        fprintf(output, "LTS\n");
                    } else if (expr->data.binary.op == OP_GT) {
                        fprintf(output, "GTS\n");
                    } else if (expr->data.binary.op == OP_LTE) {
                        fprintf(output, "GTS\n");
                        fprintf(output, "NOTS\n");
                    } else {  // OP_GTE
                        fprintf(output, "LTS\n");
                        fprintf(output, "NOTS\n");
                    }
                    fprintf(output, "POPFRAME\n");
                    break;
                    
                case OP_EQ:
                case OP_NEQ:
                    // Equality operators: can compare any types
                    fprintf(output, "EQS\n");
                    if (expr->data.binary.op == OP_NEQ) {
                        fprintf(output, "NOTS\n");
                    }
                    break;
                    
                case OP_IS:
                    // IS operator - type checking operator
                    // Left: any expression, Right: type keyword (Num, String, or Null)
                    fprintf(output, "POPS LF@op2\n");  // This is unused (type keyword)
                    fprintf(output, "POPS LF@op1\n");  // The value to check
                    fprintf(output, "TYPE LF@type1 LF@op1\n");
                    fprintf(output, "TYPE LF@type2 LF@op2\n");

                    fprintf(output, "JUMPIFEQ $is_correct_type_%d LF@type2 string@string\n", op_id);
                    //the second operand is not 'string' so we cannot compare so we error
                    fprintf(output, "EXIT int@26\n");

                    //the second operand is  'strign' so we can compare
                    fprintf(output, "LABEL $is_correct_type_%d\n", op_id);
                    fprintf(output, "PUSHS LF@type1\n");
                    fprintf(output, "PUSHS LF@op2\n");
                    fprintf(output, "EQS\n");

                    fprintf(output, "POPFRAME\n");
                    break;
                    
                default:
                    fprintf(stderr, "[GENERATOR] Unknown binary operator: %d\n", expr->data.binary.op);
                    return -1;
            }
            break;
            
        case EXPR_GETTER_CALL:
            // Generate code for getter call
            if (expr_getter_call(expr->data.identifier_name, output) != 0) {
                return -1;
            }
            break;
            
        default:
            fprintf(stderr, "[GENERATOR] Unknown expression type: %d\n", expr->type);
            return -1;
    }
    
    return 0;
}


int expression(ASTNode *node, FILE *output) {
    if (!node) return -1;
    
    // Check if it's a function call
    if (node->left && node->left->type == AST_FUNC_CALL) {
        return func_call(node->left, output);
    }
    
    // Otherwise, evaluate expression tree
    ExprNode *expr = node->expr;
    if (!expr) {
        fprintf(stderr, "[GENERATOR] Expression node has no expression tree\n");
        return -1;
    }
    
    // Generate code that pushes result to stack
    return generate_expression_code(expr, output);
}

// Generate builtin function setup
void generate_builtin_functions(FILE *output) {
    // Built-in functions are implemented inline in func_call
    // No global temporary variables needed for expression type checking
    // Each operation creates its own temporary frame
    fprintf(output, "\n");
}

// Main function definition
int main_def(ASTNode *node, FILE *output) {
    if (!node) return -1;
    
    // Main body is in the block (right child)
    if (node->right && node->right->type == AST_BLOCK) {
        return block(node->right, output);
    }
    
    // Continue with next node (other functions)
    if (node->right) {
        return next_step(node->right->right, output);
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
            func_call(node, output);
            return next_step(node->right, output);
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
