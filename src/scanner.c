/**
 * @file scanner.c
 * @author xcernoj00
 * @brief Lexical analysis and token generator
 */

#include "scanner.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *source_file;      // Source file that will be scanned
DynamicString d_string; // Dynamic string that will be written into

void set_source_file(FILE *f) { source_file = f; }

static int check_keyword(DynamicString *d_string, Token *token) {
    if (!d_string || !d_string->str) {
        return ERROR_INTERNAL;
    }

    // Check for keywords
    if (d_string_cmp(d_string, "class") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_CLASS;
    } else if (d_string_cmp(d_string, "if") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_IF;
    } else if (d_string_cmp(d_string, "else") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_ELSE;
    } else if (d_string_cmp(d_string, "is") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_IS;
    } else if (d_string_cmp(d_string, "null") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_NULL_L;
    } else if (d_string_cmp(d_string, "return") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_RETURN;
    } else if (d_string_cmp(d_string, "var") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_VAR;
    } else if (d_string_cmp(d_string, "while") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_WHILE;
    } else if (d_string_cmp(d_string, "Ifj") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_IFJ;
    } else if (d_string_cmp(d_string, "static") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_STATIC;
    } else if (d_string_cmp(d_string, "import") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_IMPORT;
    } else if (d_string_cmp(d_string, "for") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_FOR;
    } else if (d_string_cmp(d_string, "Num") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_NUM;
    } else if (d_string_cmp(d_string, "String") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_STRING;
    } else if (d_string_cmp(d_string, "Null") == 0) {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KEYWORD_NULL_C;
    } else {
        // Not a keyword, check if it's a global variable or identifier
        if (d_string->length >= 2 && d_string->str[0] == '_' &&
            d_string->str[1] == '_') {
            token->type = TOKEN_GLOBAL_VAR;
        } else {
            if (d_string->str[0] == '_') {
                return SCANNER_ERROR; // Invalid identifier starting with single
                                      // _
            }
            token->type = TOKEN_IDENTIFIER;
        }

        // Allocate and copy dynamic string for identifiers and global vars
        token->value.string = malloc(sizeof(DynamicString));
        if (token->value.string == NULL) {
            return ERROR_INTERNAL;
        }

        if (d_string_alloc(token->value.string) != NO_ERROR) {
            free(token->value.string);
            token->value.string = NULL;
            return ERROR_INTERNAL;
        }

        if (d_string_copy(d_string, token->value.string) != NO_ERROR) {
            d_string_free(token->value.string);
            free(token->value.string);
            token->value.string = NULL;
            return ERROR_INTERNAL;
        }
    }
    return NO_ERROR;
}

int get_token(Token *token) {
    char c;                  // Current character
    int state = STATE_START; // Current state of the scanner
    int hex_count = 0;       // Counter for hexadecimal digits
    int hex_value = 0;       // Value of hexadecimal escape sequence

    if (source_file == NULL) {
        return ERROR_INTERNAL;
    }

    // Initialize d_string
    if (d_string_alloc(&d_string) != NO_ERROR) {
        return ERROR_INTERNAL;
    }

    token->type = TOKEN_UNDEFINED;
    d_string_clear(&d_string); // Clear the string for new token

    while (1) {
        c = fgetc(source_file);

        switch (state) {
        case STATE_START:
            if (c == '\n') {
                // Skip multiple newlines, return only one EOL token
                do {
                    c = fgetc(source_file);
                } while (c == '\n');
                ungetc(c, source_file); // push back the first non-newline
                token->type = TOKEN_EOL;
                return NO_ERROR;
            } else if (isspace(c)) { // Skip whitespace except newline
                continue;
            } else if (c == EOF) {
                token->type = TOKEN_EOF;
                return NO_ERROR;
            } else if (isalpha(c) || c == '_') {
                d_string_add_char(&d_string, c);
                state = STATE_IDENTIFY_WORD;
            } else if (c == '0') {
                d_string_add_char(&d_string, c);
                state = STATE_NUMBER; // Will handle 0x prefix in STATE_NUMBER
            } else if (isdigit(c)) {
                d_string_add_char(&d_string, c);
                state = STATE_NUMBER;
            } else if (c == '"') {
                // Check for multiline string (""")
                char c2 = fgetc(source_file);
                if (c2 == '"') {
                    char c3 = fgetc(source_file);
                    if (c3 == '"') {
                        state = STATE_MULTILINE_STRING;
                    } else {
                        // Put back characters and treat as regular string
                        ungetc(c3, source_file);
                        ungetc(c2, source_file);
                        state = STATE_STRING;
                    }
                } else {
                    ungetc(c2, source_file);
                    state = STATE_STRING;
                }
            } else if (c == '+') {
                token->type = TOKEN_PLUS;
                return NO_ERROR;
            } else if (c == '-') {
                c = fgetc(source_file);
                if (isdigit(c)) {
                    // Negative number
                    d_string_add_char(&d_string, '-');
                    d_string_add_char(&d_string, c);
                    state = STATE_NUMBER;
                } else {
                    ungetc(c, source_file);
                    token->type = TOKEN_MINUS;
                    return NO_ERROR;
                }
            } else if (c == '*') {
                token->type = TOKEN_MULTIPLY;
                return NO_ERROR;
            } else if (c == '/') {
                state = STATE_SLASH; // Check for comments
            } else if (c == '=') {
                state = STATE_EQUAL;
            } else if (c == '!') {
                state = STATE_NOT;
            } else if (c == '<') {
                state = STATE_LESSER;
            } else if (c == '>') {
                state = STATE_GREATER;
            } else if (c == '(') {
                token->type = TOKEN_LPAREN;
                return NO_ERROR;
            } else if (c == ')') {
                token->type = TOKEN_RPAREN;
                return NO_ERROR;
            } else if (c == '{') {
                token->type = TOKEN_LCURLY;
                return NO_ERROR;
            } else if (c == '}') {
                token->type = TOKEN_RCURLY;
                return NO_ERROR;
            } else if (c == '.') {
                token->type = TOKEN_DOT;
                return NO_ERROR;
            } else if (c == ',') {
                token->type = TOKEN_COMMA;
                return NO_ERROR;
            } else {
                return SCANNER_ERROR; // Unknown character
            }
            break;

        case STATE_IDENTIFY_WORD:
            if (isalnum(c) || c == '_') {
                d_string_add_char(&d_string, c);
            } else { // check if word is 'ifj' and is followed by dot
                if (d_string_cmp(&d_string, "ifj") == 0) {
                    if (c == '.') {
                        d_string_add_char(&d_string, c);
                    } else {
                        ungetc(c, source_file);
                        return check_keyword(&d_string, token);
                    }
                    state = STATE_IDENTIFY_WORD;
                    continue;
                }
                ungetc(c, source_file);
                return check_keyword(&d_string, token);
            }
            break;

        case STATE_NUMBER:
            if (isdigit(c)) {
                d_string_add_char(&d_string, c);
            } else if (c == 'x' && d_string.length == 1 &&
                       d_string.str[0] == '0') {
                // Hexadecimal number
                d_string_add_char(&d_string, c);
                state = STATE_HEXADECIMAL;
            } else if (c == '.') {
                d_string_add_char(&d_string, c);
                state = STATE_DECIMAL;
            } else if (c == 'e' || c == 'E') {
                d_string_add_char(&d_string, c);
                state = STATE_EXPONENT;
            } else {
                ungetc(c, source_file);
                // Convert to integer
                token->type = TOKEN_INTEGER;
                token->value.integer = atoi(d_string.str);
                return NO_ERROR;
            }
            break;

        case STATE_HEXADECIMAL:
            if (isdigit(c) || (c >= 'a' && c <= 'f') ||
                (c >= 'A' && c <= 'F')) {
                d_string_add_char(&d_string, c);
            } else {
                ungetc(c, source_file);
                if (d_string.length <= 2) { // Only "0x" without digits
                    return SCANNER_ERROR;
                }
                // Convert hexadecimal to integer
                token->type = TOKEN_INTEGER;
                token->value.integer = (int)strtol(d_string.str, NULL, 16);
                return NO_ERROR;
            }
            break;

        case STATE_DECIMAL:
            if (isdigit(c)) {
                d_string_add_char(&d_string, c);
            } else if (c == 'e' || c == 'E') {
                d_string_add_char(&d_string, c);
                state = STATE_EXPONENT;
            } else {
                ungetc(c, source_file);
                // Convert to double
                token->type = TOKEN_DOUBLE;
                token->value.decimal = atof(d_string.str);
                return NO_ERROR;
            }
            break;

        case STATE_EXPONENT:
            if (isdigit(c)) {
                d_string_add_char(&d_string, c);
                state = STATE_EXPONENT; // Stay in exponent state
            } else if ((c == '+' || c == '-') &&
                       (d_string.str[d_string.length - 1] == 'e' ||
                        d_string.str[d_string.length - 1] == 'E')) {
                d_string_add_char(&d_string, c);
            } else {
                ungetc(c, source_file);
                // Check if we have a valid exponent
                char last_char = d_string.str[d_string.length - 1];
                if (last_char == 'e' || last_char == 'E' || last_char == '+' ||
                    last_char == '-') {
                    return SCANNER_ERROR; // Incomplete exponent
                }
                token->type = TOKEN_DOUBLE;
                token->value.decimal = atof(d_string.str);
                return NO_ERROR;
            }
            break;

        case STATE_STRING:
            if (c == EOF) {
                return SCANNER_ERROR; // Unterminated string
            } else if (c == '"') {
                // End of string
                token->type = TOKEN_STRING;

                // Allocate and copy dynamic string for string literals
                token->value.string = malloc(sizeof(DynamicString));
                if (token->value.string == NULL) {
                    return ERROR_INTERNAL;
                }

                if (d_string_alloc(token->value.string) != NO_ERROR) {
                    free(token->value.string);
                    token->value.string = NULL;
                    return ERROR_INTERNAL;
                }

                if (d_string_copy(&d_string, token->value.string) != NO_ERROR) {
                    d_string_free(token->value.string);
                    free(token->value.string);
                    token->value.string = NULL;
                    return ERROR_INTERNAL;
                }
                return NO_ERROR;
            } else if (c == '\\') {
                state = STATE_ESCAPE_SEQ;
            } else if (c == '\n') {
                return SCANNER_ERROR; // Newline in string not allowed
            } else {
                d_string_add_char(&d_string, c);
            }
            break;

        case STATE_ESCAPE_SEQ:
            if (c == '"') {
                d_string_add_char(&d_string, '"');
                state = STATE_STRING;
            } else if (c == 'n') {
                d_string_add_char(&d_string, '\n');
                state = STATE_STRING;
            } else if (c == 'r') {
                d_string_add_char(&d_string, '\r');
                state = STATE_STRING;
            } else if (c == 't') {
                d_string_add_char(&d_string, '\t');
                state = STATE_STRING;
            } else if (c == '\\') {
                d_string_add_char(&d_string, '\\');
                state = STATE_STRING;
            } else if (c == 'x') {
                hex_count = 0;
                hex_value = 0;
                state = STATE_HEXADECIMAL2;
            } else {
                return SCANNER_ERROR; // Invalid escape sequence
            }
            break;

        case STATE_HEXADECIMAL2:
            if (isdigit(c) || (c >= 'a' && c <= 'f') ||
                (c >= 'A' && c <= 'F')) {
                int digit_value;
                if (isdigit(c)) {
                    digit_value = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    digit_value = c - 'a' + 10;
                } else {
                    digit_value = c - 'A' + 10;
                }
                hex_value = hex_value * 16 + digit_value;
                hex_count++;

                if (hex_count == 2) {
                    d_string_add_char(&d_string, (char)hex_value);
                    state = STATE_STRING;
                }
            } else {
                return SCANNER_ERROR; // Invalid hexadecimal escape
            }
            break;

        case STATE_MULTILINE_STRING:
            if (c == EOF) {
                return SCANNER_ERROR; // Unterminated multiline string
            } else if (c == '"') {
                // Check for closing """
                char c2 = fgetc(source_file);
                if (c2 == '"') {
                    char c3 = fgetc(source_file);
                    if (c3 == '"') {
                        // End of multiline string
                        token->type = TOKEN_STRING;

                        // Allocate and copy dynamic string for multiline
                        // string
                        token->value.string = malloc(sizeof(DynamicString));
                        if (token->value.string == NULL) {
                            return ERROR_INTERNAL;
                        }

                        if (d_string_alloc(token->value.string) != NO_ERROR) {
                            free(token->value.string);
                            token->value.string = NULL;
                            return ERROR_INTERNAL;
                        }

                        if (d_string_copy(&d_string, token->value.string) !=
                            NO_ERROR) {
                            d_string_free(token->value.string);
                            free(token->value.string);
                            token->value.string = NULL;
                            return ERROR_INTERNAL;
                        }

                        return NO_ERROR;
                    } else {
                        // Not closing, add chars to string
                        d_string_add_char(&d_string, c);
                        d_string_add_char(&d_string, c2);
                        ungetc(c3, source_file);
                    }
                } else {
                    d_string_add_char(&d_string, c);
                    ungetc(c2, source_file);
                }
            } else {
                d_string_add_char(&d_string, c);
            }
            break;

        case STATE_SLASH:
            if (c == '/') {
                // Line comment
                state = STATE_COMMENT;
            } else if (c == '*') {
                // Block comment - treat as whitespace
                int block_depth = 1;
                while (block_depth > 0) {
                    c = fgetc(source_file);
                    if (c == EOF) {
                        return SCANNER_ERROR; // Unterminated block comment
                    } else if (c == '/' && fgetc(source_file) == '*') {
                        block_depth++; // Nested block comment
                    } else if (c == '*' && fgetc(source_file) == '/') {
                        block_depth--; // End of block comment level
                    }
                }
                state = STATE_START; // Continue tokenizing
            } else {
                ungetc(c, source_file);
                token->type = TOKEN_DIVIDE;
                return NO_ERROR;
            }
            break;

        case STATE_COMMENT:
            if (c == '\n' || c == EOF) {
                ungetc(c, source_file); // Put back newline/EOF
                state = STATE_START;
            }
            // Skip all other characters in comment
            break;

        case STATE_EQUAL:
            if (c == '=') {
                token->type = TOKEN_LOGIC_EQUAL;
                return NO_ERROR;
            } else {
                ungetc(c, source_file);
                token->type = TOKEN_EQUAL;
                return NO_ERROR;
            }
            break;

        case STATE_NOT:
            if (c == '=') {
                token->type = TOKEN_NEQUAL;
                return NO_ERROR;
            } else {
                ungetc(c, source_file);
                token->type = TOKEN_NOT;
                return NO_ERROR;
            }
            break;

        case STATE_LESSER:
            if (c == '=') {
                token->type = TOKEN_LESSER_EQUAL;
                return NO_ERROR;
            } else {
                ungetc(c, source_file);
                token->type = TOKEN_LESSER;
                return NO_ERROR;
            }
            break;

        case STATE_GREATER:
            if (c == '=') {
                token->type = TOKEN_GREATER_EQUAL;
                return NO_ERROR;
            } else {
                ungetc(c, source_file);
                token->type = TOKEN_GREATER;
                return NO_ERROR;
            }
            break;

        default:
            return SCANNER_ERROR;
        }
    }
}

// helper function to print token types for debugging (can be removed later)
void print_token_types() {
    Token token;
    int result;

    printf("List of token types:\n");

    while (1) {
        result = get_token(&token);

        if (result != NO_ERROR) {
            printf("Scanner error: %d\n", result);
            break;
        }

        if (token.type == TOKEN_EOF) {
            printf("EOF\n");
            break;
        }

        // Add token type names for easier debugging
        switch (token.type) {
        case TOKEN_UNDEFINED:
            printf("UNDEFINED");
            break;
        case TOKEN_EOL:
            printf("EOL");
            break;
        case TOKEN_GLOBAL_VAR:
            printf("GLOBAL_VAR=\"%s\"",
                   token.value.string ? token.value.string->str : "NULL");
            break;
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER=\"%s\"",
                   token.value.string ? token.value.string->str : "NULL");
            break;
        case TOKEN_KEYWORD:
            switch (token.value.keyword) {
            case KEYWORD_CLASS:
                printf("KEYWORD-CLASS");
                break;
            case KEYWORD_IF:
                printf("KEYWORD-IF");
                break;
            case KEYWORD_ELSE:
                printf("KEYWORD-ELSE");
                break;
            case KEYWORD_IS:
                printf("KEYWORD-IS");
                break;
            case KEYWORD_NULL_L:
                printf("KEYWORD-NULL_L");
                break;
            case KEYWORD_RETURN:
                printf("KEYWORD-RETURN");
                break;
            case KEYWORD_VAR:
                printf("KEYWORD-VAR");
                break;
            case KEYWORD_WHILE:
                printf("KEYWORD-WHILE");
                break;
            case KEYWORD_IFJ:
                printf("KEYWORD-IFJ");
                break;
            case KEYWORD_STATIC:
                printf("KEYWORD-STATIC");
                break;
            case KEYWORD_IMPORT:
                printf("KEYWORD-IMPORT");
                break;
            case KEYWORD_FOR:
                printf("KEYWORD-FOR");
                break;
            case KEYWORD_NUM:
                printf("KEYWORD-NUM");
                break;
            case KEYWORD_STRING:
                printf("KEYWORD-STRING");
                break;
            case KEYWORD_NULL_C:
                printf("KEYWORD-NULL_C");
                break;
            }
            break;
        case TOKEN_INTEGER:
            printf("INTEGER=%d", token.value.integer);
            break;
        case TOKEN_DOUBLE:
            printf("DOUBLE=%f", token.value.decimal);
            break;
        case TOKEN_STRING:
            printf("STRING=\"%s\"",
                   token.value.string ? token.value.string->str : "NULL");
            break;
        case TOKEN_PLUS:
            printf("PLUS");
            break;
        case TOKEN_MINUS:
            printf("MINUS");
            break;
        case TOKEN_MULTIPLY:
            printf("MULTIPLY");
            break;
        case TOKEN_DIVIDE:
            printf("DIVIDE");
            break;
        case TOKEN_EQUAL:
            printf("EQUAL");
            break;
        case TOKEN_NEQUAL:
            printf("NEQUAL");
            break;
        case TOKEN_LESSER:
            printf("LESSER");
            break;
        case TOKEN_GREATER:
            printf("GREATER");
            break;
        case TOKEN_LESSER_EQUAL:
            printf("LESSER_EQUAL");
            break;
        case TOKEN_GREATER_EQUAL:
            printf("GREATER_EQUAL");
            break;
        case TOKEN_NOT:
            printf("NOT");
            break;
        case TOKEN_LOGIC_EQUAL:
            printf("LOGIC_EQUAL");
            break;
        case TOKEN_LPAREN:
            printf("LPAREN");
            break;
        case TOKEN_RPAREN:
            printf("RPAREN");
            break;
        case TOKEN_LCURLY:
            printf("LCURLY");
            break;
        case TOKEN_RCURLY:
            printf("RCURLY");
            break;
        case TOKEN_DOT:
            printf("DOT");
            break;
        case TOKEN_COMMA:
            printf("COMMA");
            break;
        default:
            printf("UNKNOWN");
            break;
        }

        printf("\n");
    }
}
