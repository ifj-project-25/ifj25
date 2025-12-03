/**
 * @file dynamic_string.c
 * @author xcernoj00
 * @brief Dynamic string manipulation
 */

#include "dynamic_string.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

void d_string_clear(DynamicString *s) {
    s->length = 0;
    s->str[0] = '\0';
}

int d_string_alloc(DynamicString *s) {
    s->str = (char *)malloc(INITIAL_ALLOC_SIZE);
    if (!s->str)
        return ERROR_INTERNAL;
    d_string_clear(s);
    s->max_length = INITIAL_ALLOC_SIZE;

    return NO_ERROR;
}

int d_string_add_char(DynamicString *s, char c) {
    if (s->length + 1 >= s->max_length) {
        unsigned int new_size = s->length * 2;
        s->str = (char *)realloc(s->str, new_size);
        if (!s->str)
            return ERROR_INTERNAL;
        s->max_length = new_size;
    }
    s->str[s->length++] = c;
    s->str[s->length] = '\0';

    return NO_ERROR;
}

int d_string_add_str(DynamicString *s, const char *const_str) {
    unsigned int const_str_length = (unsigned int)strlen(const_str);

    if (s->length + const_str_length + 1 >= s->max_length) {
        unsigned int new_size = s->length + const_str_length + 1;
        s->str = (char *)realloc(s->str, new_size);
        if (!s->str)
            return ERROR_INTERNAL;
        s->max_length = new_size;
    }
    s->length += const_str_length;
    strcat(s->str, const_str);
    s->str[s->length] = '\0';

    return NO_ERROR;
}

int d_string_cmp(DynamicString *s, const char *const_str) {
    return strcmp(s->str, const_str);
}

int d_string_copy(DynamicString *src, DynamicString *dest) {
    unsigned int new_length = src->length;
    if (new_length >= dest->max_length) {
        dest->str = (char *)realloc(dest->str, new_length + 1);
        if (!dest->str)
            return ERROR_INTERNAL;
        dest->max_length = new_length + 1;
    }
    strcpy(dest->str, src->str);
    dest->length = new_length;

    return NO_ERROR;
}

void d_string_free(DynamicString *s) { free(s->str); }