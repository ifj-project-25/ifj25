/**
 * @file scanner.h
 * @author xcernoj00
 * @brief Scanner (lexical analyzer) public interface for IFJ25.
 *
 * This header declares token types, scanner states and the minimal
 * API used by the rest of the compiler to obtain lexical tokens from
 * a source stream.
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include "dynamic_string.h"
#include "error.h"
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Internal scanner states used by the DFA.
 */
typedef enum {
    STATE_START,
    STATE_NUMBER,
    STATE_IDENTIFY_WORD,
    STATE_SLASH,
    STATE_STRING,
    STATE_LESSER,
    STATE_GREATER,
    STATE_EQUAL,
    STATE_NOT,
    STATE_COMMENT,
    STATE_DECIMAL,
    STATE_EXPONENT,
    STATE_ESCAPE_SEQ,
    STATE_HEXADECIMAL,      ///< 0x prefix for hex numbers
    STATE_HEXADECIMAL2,     ///< \xNN escape sequence parsing
    STATE_MULTILINE_STRING, ///< """..."""
} State;

/**
 * @brief Token types produced by the scanner.
 */
typedef enum {
    TOKEN_UNDEFINED,  ///< default/invalid
    TOKEN_EOF,        ///< end of file
    TOKEN_EOL,        ///< end of line
    TOKEN_GLOBAL_VAR, ///< global variable (starts with __)
    TOKEN_IDENTIFIER, ///< identifier
    TOKEN_KEYWORD,    ///< keyword

    TOKEN_INTEGER, ///< integer literal
    TOKEN_DOUBLE,  ///< floating-point literal
    TOKEN_STRING,  ///< string literal

    TOKEN_PLUS,     ///< +
    TOKEN_MINUS,    ///< -
    TOKEN_MULTIPLY, ///< *
    TOKEN_DIVIDE,   ///< /

    TOKEN_EQUAL,         ///< =
    TOKEN_NEQUAL,        ///< !=
    TOKEN_LESSER,        ///< <
    TOKEN_GREATER,       ///< >
    TOKEN_LESSER_EQUAL,  ///< <=
    TOKEN_GREATER_EQUAL, ///< >=
    TOKEN_NOT,           ///< !
    TOKEN_LOGIC_EQUAL,   ///< ==

    TOKEN_LPAREN, ///< '('
    TOKEN_RPAREN, ///< ')'
    TOKEN_LCURLY, ///< '{'
    TOKEN_RCURLY, ///< '}'
    TOKEN_DOT,    ///< '.'
    TOKEN_COMMA,  ///< ','
    TOKEN_DOLLAR  ///< '$' bottom-of-stack marker for operator stack
} TokenType;

/**
 * @brief Language keywords recognized by the scanner.
 */
typedef enum {
    KEYWORD_CLASS,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_IS,
    KEYWORD_NULL_L, ///< lowercase 'null'
    KEYWORD_RETURN,
    KEYWORD_VAR,
    KEYWORD_WHILE,
    KEYWORD_IFJ, ///< 'Ifj' namespace identifier
    KEYWORD_STATIC,
    KEYWORD_IMPORT,
    KEYWORD_FOR,
    KEYWORD_NUM,
    KEYWORD_STRING,
    KEYWORD_NULL_C ///< capitalized 'Null'
} Keyword;

/**
 * @brief Token payload. Only the field matching the token type is valid.
 *
 * - For identifiers, global variables and string literals the `string`
 *   field holds a pointer to a `DynamicString` with the textual value.
 */
typedef union {
    Keyword keyword; ///< for TOKEN_KEYWORD
    int integer;     ///< for TOKEN_INTEGER
    double decimal;  ///< for TOKEN_DOUBLE
    DynamicString
        *string; ///< for TOKEN_IDENTIFIER/TOKEN_GLOBAL_VAR/TOKEN_STRING
} TokenValue;

/**
 * @brief Scanner token structure.
 */
typedef struct Token {
    TokenType type;   /**< token kind */
    TokenValue value; /**< token payload */
} Token;

/**
 * @brief Set the source file used by the scanner.
 *
 * @param f Input file stream (e.g. stdin or fopen result).
 */
void set_source_file(FILE *f);

/**
 * @brief Simple debug helper that prints token types until EOF.
 *
 * Intended for development/testing; not used by the parser.
 */
void print_token_types(void);

/**
 * @brief Obtain the next token from the input stream.
 *
 * On success the function initializes the provided `Token` structure.
 * For token types that allocate a `DynamicString` (identifiers, global
 * variables, string literals) the caller is responsible for freeing
 * the `DynamicString` when it is no longer needed.
 *
 * @param token Pointer to Token structure to populate.
 * @return NO_ERROR on success, SCANNER_ERROR on lexical error or
 *         ERROR_INTERNAL on internal failures (allocation etc.).
 */
int get_token(Token *token);

#endif // _SCANNER_H
