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
 * - LEXICAL_ERROR (1): Invalid token structure
 * - SYNTAX_ERROR (2): Grammar rule violation
 * - SEMANTIC_ERROR_UNDEFINED (3): Undefined variable/function
 * - SEMANTIC_ERROR_REDEFINE (4): Redefinition of identifier
 * - SEMANTIC_ERROR_TYPE (5): Type mismatch or invalid operation
 * - SEMANTIC_ERROR_INFERENCE (6): Type inference failure
 * - SEMANTIC_ERROR_COMPATIBILITY (7): Incompatible types
 * - SEMANTIC_ERROR_OTHER (8): Other semantic errors
 * - INTERNAL_ERROR (99): Internal compiler error
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
