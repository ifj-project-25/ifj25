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
#include "generator.h"


int main() {

    FILE *source_file;
    source_file = stdin;
 
    set_source_file(source_file);
    ASTNode* PROGRAM = create_ast_node(AST_PROGRAM, NULL);
    
    int error_code = parser(PROGRAM);

    if (error_code != NO_ERROR || PROGRAM == NULL) {
        fprintf(stderr, "Error during parsing: %d\n", error_code);
        return error_code;
    }
    else{
        // FILE *fileOut;
        // fileOut = stdout;
        FILE *fileOut;
        fileOut = fopen("output.ifj25", "w");
        if (fileOut == NULL) {
            perror("Failed to open output file");
            return ERROR_INTERNAL;
        }
        error_code = semantic_analyze(PROGRAM);
        if (error_code != NO_ERROR) {
            fprintf(stderr, "Error during semantic analysis: %d\n", error_code);
            return error_code;
        }
        error_code = generate_code(PROGRAM, fileOut);
        if (error_code != NO_ERROR) {
            fprintf(stderr, "Error during code generation: %d\n", error_code);
            return error_code;
        }
        fclose(source_file);
        fclose(fileOut);
    }
    
    return error_code;
}
