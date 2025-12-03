/**
 * @file main.c
 * @author xcernoj00,xmikusm00
 * @brief Entry point of the compiler.
 */

#include "ast.h"
#include "error.h"
#include "expr_ast.h"
#include "expr_precedence_parser.h"
#include "generator.h"
#include "parser.h"
#include "scanner.h"
#include "semantic.h"
#include "symtable.h"
#include <stdio.h>

/**
 * @brief Main function of interpret
 * @return int Error code
 */
int main() {
    FILE *source_file;
    source_file = stdin;

    set_source_file(source_file); ///< Set the source file for the scanner
    ASTNode *PROGRAM =
        create_ast_node(AST_PROGRAM, NULL); ///< Create the root AST node

    int error_code = parser(PROGRAM); ///< Call the parser
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM); ///< Free the AST tree on error
        fclose(source_file);
        return error_code;
    }
    FILE *fileOut; ///< Output file pointer
    fileOut = stdout;

    error_code = semantic_analyze(PROGRAM); ///< Call the semantic analyzer
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM); ///< Free the AST tree on error
        fclose(source_file);
        fclose(fileOut);
        return error_code;
    }
    error_code = generate_code(PROGRAM, fileOut); ///< Call the code generator
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM); ///< Free the AST tree on error
        fclose(source_file);
        fclose(fileOut);
        return error_code;
    }
    free_ast_tree(PROGRAM); ///< Free the AST tree on success
    fclose(source_file);
    fclose(fileOut);

    return error_code;
}
