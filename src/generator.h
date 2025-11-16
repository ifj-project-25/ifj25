/**
 * @file generator.h
 * @author xklusaa00
 * @brief Header file for code generator
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "ast.h"
#include "symtable.h"
#include <stdio.h>

//---------- Function declarations ----------

//---------- Helper functions ----------

/**
 * @brief Converts a string to a IFJcode25 compatible format and prints it.
 * @param input Input string to convert.
 */
void print_convert_string(const char* input);

//---------- Built-in functions ----------

/**
 * @brief Generates code for the Ifj.read_str() built-in function.
 * @param node AST node representing the function call.
 */
int read_str_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.read_num() built-in function.
 * @param node AST node representing the function call.
 */
int read_num_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.print() built-in function.
 * @param node AST node representing the function call.
 */
int write_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.floor() built-in function.
 * @param node AST node representing the function call.
 */
int floor_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.str() built-in function.
 * @param node AST node representing the function call.
 */
int length_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.substring() built-in function.
 * @param node AST node representing the function call.
 */
int substring_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.strcmp() built-in function.
 * @param node AST node representing the function call.
 */
int strcmp_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.ord() built-in function.
 * @param node AST node representing the function call.
 */
int ord_func(ASTNode *node, FILE *output);

/**
 * @brief Generates code for the Ifj.chr() built-in function.
 * @param node AST node representing the function call.
 */
int chr_func(ASTNode *node, FILE *output);





//---------- AST Types ----------

//variable ast types
int identifier (ASTNode *node, FILE *output);
int var_decl (ASTNode *node, FILE *output);

//assignments ast types
int assign (ASTNode *node, FILE *output);


/**
 * TODO 
 * how do difirentiate between static unicorn / static unicorn=(val) / static unicorn()
 * make variables not from LF but used in a function be known as GF variables
 */


//funkcion ast types
int funkc_call (ASTNode *node, FILE *output);
int func_def (ASTNode *node, FILE *output);
int block (ASTNode *node, FILE *output);

//definitions ast types
int main_def (ASTNode *node, FILE *output);
int func_def (ASTNode *node, FILE *output);
int getter_def (ASTNode *node, FILE *output);
int setter_def (ASTNode *node, FILE *output);

//statements ast types
int if_stmt (ASTNode *node, FILE *output);
int else_stmt (ASTNode *node, FILE *output);
int while_loop (ASTNode *node, FILE *output);
int return_stmt (ASTNode *node, FILE *output);

//expressions ast types
int expression (ASTNode *node, FILE *output);
//todo Mato has to check and fix/add/remove that


/**
 * @brief Function to handle the next step in code generation.
 * @param node Current AST node.
 * @param output File pointer to write the generated code.
 * @return 0 on success, non-zero error code on failure.
 */
int next_step(ASTNode *node, FILE *output);


/**
 * @brief Generates code from the AST and writes it to the output file.
 * @param root Root node of the AST.
 * @param output File pointer to write the generated code.
 * @return 0 on success, non-zero error code on failure.
 */
int generate_code(ASTNode *root, FILE *output);

#endif // GENERATOR_H
