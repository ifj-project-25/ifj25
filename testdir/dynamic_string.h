/**
 * @file dynamic_string.h
 * @author xcernoj00
 * @brief Header file for dynamic string
 */

#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#define INITIAL_ALLOC_SIZE 8

// dynamic string struct
typedef struct d_str {
    char *str;
    unsigned int length;
    unsigned int max_length;
} DynamicString;

void d_string_clear(DynamicString *s);

int d_string_alloc(DynamicString *s);

int d_string_add_char(DynamicString *s, char c);

int d_string_add_str(DynamicString *s, const char *const_str);

int d_string_cmp(DynamicString *s, const char *const_str);

int d_string_copy(DynamicString *src, DynamicString *dest);

void d_string_free(DynamicString *s);

#endif // _DYNAMIC_STRING_H