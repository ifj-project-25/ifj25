/**
 * @file symtable.h
 * @author xcernoj00
 * @brief Symbol table (AVL tree) API and type declarations for IFJ25.
 *
 * The symbol table stores variables, functions, getters and setters
 * in an AVL-balanced binary search tree keyed by identifier name.
 * This header exposes the public data types used by the semantic
 * analysis phase and factory functions to create symbol entries.
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

/**
 * @brief Opaque scope type.
 *
 * The concrete definition is internal to the symbol table implementation.
 */
typedef struct Scope Scope;

/**
 * @brief Data types available in IFJ25.
 */
typedef enum {
    TYPE_UNDEF, /**< uninitialized */
    TYPE_NULL,
    TYPE_NUM,
    TYPE_STRING,
} DataType;

/**
 * @brief Kind of symbol stored in a table node.
 */
typedef enum {
    NODE_VAR,
    NODE_FUNC,
    NODE_GETTER,
    NODE_SETTER,
} NodeDataType;

/**
 * @brief Variable metadata stored in the symbol table.
 */
typedef struct VariableData {
    DataType data_type; /**< declared data type (if known) */
    bool defined;       /**< whether the variable is defined */
    bool initialized;   /**< whether the variable has an assigned value */
    Scope *scope;       /**< scope where the variable is declared */
} VariableData;

/**
 * @brief Function parameter description (linked list node).
 */
typedef struct Param {
    char *name;         /**< parameter identifier (heap allocated) */
    DataType data_type; /**< parameter type */
    struct Param *next; /**< next parameter in list or NULL */
} Param;

/**
 * @brief Function metadata stored in the symbol table.
 */
typedef struct FunctionData {
    int param_count;      /**< number of parameters */
    Param *parameters;    /**< linked list of parameters */
    bool defined;         /**< whether the function body is defined */
    DataType return_type; /**< return type of the function */
} FunctionData;

/**
 * @brief Getter metadata.
 */
typedef struct GetterData {
    DataType return_type; /**< type returned by the getter */
    bool defined;         /**< whether getter is defined */
} GetterData;

/**
 * @brief Setter metadata.
 */
typedef struct SetterData {
    DataType param_type; /**< single parameter type of setter */
    bool defined;        /**< whether setter is defined */
} SetterData;

/**
 * @brief Generic container for symbol-specific data.
 */
typedef struct SymTableData {
    NodeDataType type; /**< which of the union members is valid */
    union {
        FunctionData *func_data;
        VariableData *var_data;
        GetterData *getter_data;
        SetterData *setter_data;
    } data;
} SymTableData;

/**
 * @brief AVL tree node storing an identifier and its associated data.
 */
typedef struct SNode {
    char *key;           /**< identifier name (unique in scope) */
    SymTableData *data;  /**< pointer to symbol metadata */
    struct SNode *left;  /**< left child */
    struct SNode *right; /**< right child */
    int height;          /**< node height for AVL balancing */
} SNode;

/**
 * @brief Symbol table root structure.
 */
typedef struct {
    SNode *root; /**< root node of the AVL tree */
} SymTable;

/* ---------- Public API ---------- */

/**
 * @brief Initialize an empty symbol table.
 *
 * @param table Pointer to SymTable to initialize.
 */
void symtable_init(SymTable *table);

/**
 * @brief Free all resources held by the symbol table.
 *
 * @param table Pointer to SymTable to free.
 */
void symtable_free(SymTable *table);

/**
 * @brief Search for a symbol by key in the table.
 *
 * @param table Pointer to SymTable.
 * @param key NUL-terminated identifier name.
 * @return Pointer to SymTableData if found, or NULL if not present.
 */
SymTableData *symtable_search(SymTable *table, const char *key);

/**
 * @brief Insert a new symbol into the table.
 *
 * @param table Pointer to SymTable.
 * @param key Identifier name (NUL-terminated).
 * @param data Pointer to SymTableData (ownership semantics depend on
 * implementation).
 * @return true on success, false on failure (e.g. memory allocation or
 * duplicate key).
 */
bool symtable_insert(SymTable *table, const char *key, SymTableData *data);

/**
 * @brief Remove a symbol from the table.
 *
 * @param table Pointer to SymTable.
 * @param key Identifier name to remove.
 * @return true if node was removed, false otherwise.
 */
bool symtable_delete(SymTable *table, const char *key);

/* ---------- Factory functions ---------- */

/**
 * @brief Create a SymTableData for a variable.
 *
 * @param type Variable data type.
 * @param defined Whether the variable is declared/defined.
 * @param initialized Whether the variable has an assigned value.
 * @return Pointer to newly allocated SymTableData or NULL on failure.
 */
SymTableData *make_variable(DataType type, bool defined, bool initialized);

/**
 * @brief Create a SymTableData for a function.
 */
SymTableData *make_function(int param_count, Param *params, bool defined,
                            DataType return_type);

/**
 * @brief Create a SymTableData for a getter.
 */
SymTableData *make_getter(DataType return_type, bool defined);

/**
 * @brief Create a SymTableData for a setter.
 */
SymTableData *make_setter(DataType param_type, bool defined);

/**
 * @brief Create a parameter descriptor.
 *
 * The returned Param is heap allocated and may be linked into a list.
 */
Param *make_param(const char *name, DataType type);

/**
 * @brief Append a parameter node to a parameter list.
 *
 * @param head Head of the parameter list (may be NULL).
 * @param new_param New parameter node to append.
 * @return Pointer to head of the list (may be the new node if head was NULL).
 */
Param *append_param(Param *head, Param *new_param);

/**
 * @brief Duplicate a C string (simple replacement for strdup).
 *
 * @param s NUL-terminated source string.
 * @return Heap-allocated copy or NULL on allocation failure.
 */
char *my_strdup(const char *s);

#endif // SYMTABLE_H
