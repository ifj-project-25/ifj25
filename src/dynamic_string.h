/**
 * @file dynamic_string.h
 * @author xcernoj00
 * @brief Dynamic, resizable string utility used across the project.
 *
 * The DynamicString type provides a small growable buffer for building
 * and copying character sequences. It is a minimal wrapper around a
 * heap-allocated char array and keeps track of used and allocated size.
 */

#ifndef _DYNAMIC_STRING_H
#define _DYNAMIC_STRING_H

#include <stddef.h>

#define INITIAL_ALLOC_SIZE 8

/**
 * @brief Growable string container.
 *
 * - `str` points to a NUL-terminated buffer (heap allocated by the API).
 * - `length` is the number of characters stored (excluding terminating NUL).
 * - `max_length` is the total buffer size including the terminating NUL.
 */
typedef struct d_str {
    char *str;
    unsigned int length;
    unsigned int max_length;
} DynamicString;

/**
 * @brief Reset the string to empty (keeps allocated buffer).
 *
 * @param s Pointer to an initialized DynamicString.
 */
void d_string_clear(DynamicString *s);

/**
 * @brief Allocate initial storage for a DynamicString.
 *
 * The function initializes the internal buffer and fields. The caller
 * must call `d_string_free` when the string is no longer needed.
 *
 * @param s Pointer to DynamicString to initialize.
 * @return NO_ERROR on success or ERROR_INTERNAL on allocation failure.
 */
int d_string_alloc(DynamicString *s);

/**
 * @brief Append a single character to the dynamic string.
 *
 * The buffer will be grown automatically if needed.
 *
 * @param s Pointer to DynamicString.
 * @param c Character to append.
 * @return NO_ERROR on success or ERROR_INTERNAL on allocation failure.
 */
int d_string_add_char(DynamicString *s, char c);

/**
 * @brief Append a NUL-terminated C string to the dynamic string.
 *
 * The buffer will be grown automatically to fit the appended data.
 *
 * @param s Target DynamicString.
 * @param const_str NUL-terminated source string to append.
 * @return NO_ERROR on success or ERROR_INTERNAL on allocation failure.
 */
int d_string_add_str(DynamicString *s, const char *const_str);

/**
 * @brief Compare the dynamic string with a C string.
 *
 * @param s DynamicString to compare.
 * @param const_str NUL-terminated string to compare with.
 * @return 0 if equal, <0 or >0 like strcmp otherwise.
 */
int d_string_cmp(DynamicString *s, const char *const_str);

/**
 * @brief Copy content from source dynamic string into destination.
 *
 * The destination will be resized if necessary.
 *
 * @param src Source DynamicString.
 * @param dest Destination DynamicString (must be initialized or allocated).
 * @return NO_ERROR on success or ERROR_INTERNAL on allocation failure.
 */
int d_string_copy(DynamicString *src, DynamicString *dest);

/**
 * @brief Free internal buffer of the dynamic string.
 *
 * After this call the `str` pointer is freed and must not be used.
 *
 * @param s DynamicString to free.
 */
void d_string_free(DynamicString *s);

#endif // _DYNAMIC_STRING_H