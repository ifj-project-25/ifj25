/**
 * @file symtable.h
 * @author xcernoj00
 * @brief Header file for symbol table (AVL tree implementation)
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

// ---------- Data types for IFJ25  ----------

typedef enum {
    TYPE_UNDEF, // uninitialized
    TYPE_NULL,
    TYPE_NUM,
    TYPE_STRING,
} DataType;

typedef enum {
    NODE_VAR,
    NODE_FUNC,
    NODE_GETTER,
    NODE_SETTER,
} NodeDataType;

// ---------- Structures for different symbol kinds ----------

// Variable data
typedef struct VariableData {
    DataType data_type;
    bool defined;     // declared/defined
    bool initialized; // has a value assigned
} VariableData;

// Parameter for functions
typedef struct Param {
    char *name; // identifier name
    DataType data_type;
    struct Param *next; // linked list
} Param;

// Function data
typedef struct FunctionData {
    int param_count;
    Param *parameters;
    bool defined; // function has been defined
    DataType return_type; // function return type
} FunctionData;

// Getter data
typedef struct GetterData {
    DataType return_type;
    bool defined;
} GetterData;

// Setter data
typedef struct SetterData {
    DataType param_type;
    bool defined;
} SetterData;

// Generic symbol data
typedef struct SymTableData {
    NodeDataType type;
    union {
        FunctionData *func_data;
        VariableData *var_data;
        GetterData *getter_data;
        SetterData *setter_data;
    } data;
} SymTableData;

// ---------- AVL tree node ----------
typedef struct SNode {
    char *key;          // identifier name (unique in scope)
    SymTableData *data; // symbol data
    struct SNode *left;
    struct SNode *right;
    int height; // height for AVL balancing
} SNode;

// Symbol table (root of AVL tree)
typedef struct {
    SNode *root;
} SymTable;

// ---------- Public API ----------

// Table management
void symtable_init(SymTable *table);
void symtable_free(SymTable *table);

// Insert/search/delete
SymTableData *symtable_search(SymTable *table, const char *key);
bool symtable_insert(SymTable *table, const char *key, SymTableData *data);
bool symtable_delete(SymTable *table, const char *key); // optional

// ---------- Factory functions ----------

// Symbol creators
SymTableData *make_variable(DataType type, bool defined, bool initialized);
SymTableData *make_function(int param_count, Param *params, bool defined, DataType return_type);
SymTableData *make_getter(DataType return_type, bool defined);
SymTableData *make_setter(DataType param_type, bool defined);

// Parameter creators
Param *make_param(const char *name, DataType type);
Param *append_param(Param *head, Param *new_param);

// strdup replacement (not in C standard)
char *my_strdup(const char *s);

#endif // SYMTABLE_H
