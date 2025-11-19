/**
 * @file error.h
 * @author xcernoj00
 * @brief List of compiler errors
 */

#ifndef _ERROR_H
#define _ERROR_H

#define NO_ERROR 0      // no error
#define SCANNER_ERROR 1 // lexical analysis error
#define SYNTAX_ERROR 2  // syntax analysis error
// semantic analysis error - undefined variable or function
#define SEM_ERROR_UNDEFINED 3
// semantic analysis error - redefinition of variable or function
#define SEM_ERROR_REDEFINED 4
// semantic analysis error - incorrect parameter type in function call
#define SEM_ERROR_WRONG_PARAMS 5
// semantic analysis error - type incompatibility in expression
#define SEM_ERROR_TYPE_COMPATIBILITY 6
#define SEM_ERROR_OTHER 10 // semantic analysis error - other errors
// runtime error - wrong parameter in built-in function
#define SEM_RUNTIME_WRONG_PARAM 25
// runtime error - type incompatibility in expression
#define SEM_RUNTIME_TYPE_COMPATIBILITY 26
// internal compiler error (e.g., memory allocation error, ...)
#define ERROR_INTERNAL 99

#endif // _ERROR_H