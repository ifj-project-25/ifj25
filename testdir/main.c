/**
 * @file main.c
 * @author xcernoj00, xmikusm00, xmalikm00
 * @brief Entry point of the IFJ25 compiler.
 * @details
 * This module serves as the main entry point for the IFJ25 language compiler.
 * It orchestrates the compilation pipeline consisting of three main phases:
 * 1. Lexical and Syntactic Analysis (Scanner + Parser)
 * 2. Semantic Analysis
 * 3. Code Generation
 *
 * The compiler reads source code from standard input and outputs IFJcode25
 * (a variant of IFJ instruction set) to standard output.
 *
 * Compilation Pipeline:
 * - Input: IFJ25 source code (stdin)
 * - Scanner: Tokenizes the source code
 * - Parser: Builds Abstract Syntax Tree (AST) using recursive descent
 * - Semantic Analyzer: Validates types, scopes, and semantic rules
 * - Code Generator: Produces IFJcode25 instructions
 * - Output: IFJcode25 executable code (stdout)
 *
 * Error Handling:
 * The compiler follows a fail-fast approach. If any phase encounters an error,
 * resources are cleaned up and an appropriate error code is returned:
 * - NO_ERROR (0): Successful compilation
 * - SCANNER_ERROR (1): Lexical analysis error - invalid token structure
 * - SYNTAX_ERROR (2): Syntax analysis error - grammar rule violation
 * - SEM_ERROR_UNDEFINED (3): Undefined variable or function
 * - SEM_ERROR_REDEFINED (4): Redefinition of variable or function
 * - SEM_ERROR_WRONG_PARAMS (5): Incorrect parameter type in function call
 * - SEM_ERROR_TYPE_COMPATIBILITY (6): Type incompatibility in expression
 * - SEM_ERROR_OTHER (10): Other semantic errors
 * - SEM_RUNTIME_WRONG_PARAM (25): Runtime error - wrong built-in function
 * parameter
 * - SEM_RUNTIME_TYPE_COMPATIBILITY (26): Runtime error - type incompatibility
 * - ERROR_INTERNAL (99): Internal compiler error (memory allocation, etc.)
 */

#include "ast.h"
#include "error.h"
#include "expr_ast.h"
#include "expr_parser.h"
#include "generator.h"
#include "parser.h"
#include "scanner.h"
#include "semantic.h"
#include "symtable.h"
#include <stdio.h>

/**
 * @brief Main entry point of the IFJ25 compiler.
 * @details
 * Executes the complete compilation pipeline:
 * 1. Initializes the scanner with stdin as the source
 * 2. Creates the root AST node (PROGRAM)
 * 3. Invokes the parser to build the AST
 * 4. Performs semantic analysis on the AST
 * 5. Generates IFJcode25 instructions to stdout
 * 6. Cleans up all allocated resources
 *
 * @return Error code indicating compilation result:
 *         - NO_ERROR (0) on successful compilation
 *         - Non-zero error code if compilation fails at any stage
 */
int main() {
    // Initialize source file (stdin)
    FILE *source_file = stdin;
    set_source_file(source_file);

    // Create root AST node for the program
    ASTNode *PROGRAM = create_ast_node(AST_PROGRAM, NULL);

    // Phase 1: Lexical and Syntactic Analysis
    // Parse the source code and build the Abstract Syntax Tree
    int error_code = parser(PROGRAM);
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM);
        fclose(source_file);
        return error_code;
    }

    FILE *fileOut = stdout;

    // Phase 2: Semantic Analysis
    // Validate types, scopes, function signatures, and semantic rules
    error_code = semantic_analyze(PROGRAM);
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM);
        fclose(source_file);
        fclose(fileOut);
        return error_code;
    }

    // Phase 3: Code Generation
    // Generate IFJcode25 instructions from the validated AST
    error_code = generate_code(PROGRAM, fileOut);
    if (error_code != NO_ERROR) {
        free_ast_tree(PROGRAM);
        fclose(source_file);
        fclose(fileOut);
        return error_code;
    }

    // Cleanup: Free all allocated resources
    free_ast_tree(PROGRAM);
    fclose(source_file);
    fclose(fileOut);

    return error_code;
}
