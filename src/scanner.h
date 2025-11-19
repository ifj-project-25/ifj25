/**
 * @file scanner.h
 * @author xcernoj00
 * @brief Header file for scanner
 */

#ifndef _SCANNER_H
#define _SCANNER_H

#include "dynamic_string.h"
#include "error.h"
#include <stdbool.h>
#include <stdio.h>

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
    STATE_HEXADECIMAL,      // 0x prefix for hex numbers
    STATE_HEXADECIMAL2,     // \xNN escape sequence
    STATE_MULTILINE_STRING, // """..."""
} State;

typedef enum {
    TOKEN_UNDEFINED,  // default state
    TOKEN_EOF,        // end of file
    TOKEN_EOL,        // end of line
    TOKEN_GLOBAL_VAR, // global variable (starts with __)
    TOKEN_IDENTIFIER, // identifier
    TOKEN_KEYWORD,    // keyword

    TOKEN_INTEGER, // integer
    TOKEN_DOUBLE,  // double
    TOKEN_STRING,  // string

    TOKEN_PLUS,     // plus +
    TOKEN_MINUS,    // minus -
    TOKEN_MULTIPLY, // multiplication *
    TOKEN_DIVIDE,   // division /

    TOKEN_EQUAL,         // equal =
    TOKEN_NEQUAL,        // not equal !=
    TOKEN_LESSER,        // lesser than <
    TOKEN_GREATER,       // greater than >
    TOKEN_LESSER_EQUAL,  // lesser or equal <=
    TOKEN_GREATER_EQUAL, // greater or equal >=
    TOKEN_NOT,           // not !
    TOKEN_LOGIC_EQUAL,   // ==

    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )
    TOKEN_LCURLY, // {
    TOKEN_RCURLY, // }
    TOKEN_DOT,    // .
    TOKEN_COMMA,  // ,
} TokenType;

typedef enum {
    KEYWORD_CLASS,  // class
    KEYWORD_IF,     // if
    KEYWORD_ELSE,   // else
    KEYWORD_IS,     // is
    KEYWORD_NULL_L, // null (lowercase)
    KEYWORD_RETURN, // return
    KEYWORD_VAR,    // var
    KEYWORD_WHILE,  // while
    KEYWORD_IFJ,    // Ifj
    KEYWORD_STATIC, // static
    KEYWORD_IMPORT, // import
    KEYWORD_FOR,    // for
    KEYWORD_NUM,    // Num
    KEYWORD_STRING, // String
    KEYWORD_NULL_C, // Null (Capitalized)
} Keyword;

// Value of the token
typedef union {
    Keyword keyword;
    int integer;
    double decimal;
    DynamicString *string;
} TokenValue;

// Token structure
typedef struct Token {
    TokenType type;
    TokenValue value;
} Token;

void set_source_file(FILE *f);

void print_token_types();

#endif // _SCANNER_H