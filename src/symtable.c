/**
 * @file symtable.c
 * @author xcernoj00
 * @brief Table of symbols presented as binary tree
 */

#include "symtable.h"
#include <stdlib.h>
#include <string.h>

// ---------- Internal helpers ----------

// strdup replacement (not in C standard)
char *my_strdup(const char *s) {
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (!copy)
        return NULL;
    memcpy(copy, s, len);
    return copy;
}

// Return node height
static int node_height(SNode *n) { return n ? n->height : 0; }

// Return max of two integers
static int max(int a, int b) { return (a > b) ? a : b; }

// Create new node
static SNode *create_node(const char *key, SymTableData *data) {
    SNode *node = malloc(sizeof(SNode));
    if (!node)
        return NULL;

    node->key = my_strdup(key); // copy key
    if (!node->key) {
        free(node);
        return NULL;
    }

    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Free node and its data
static void free_node(SNode *n) {
    if (!n)
        return;

    if (n->data) {
        switch (n->data->type) {
        case NODE_VAR:
            free(n->data->data.var_data);
            break;
        case NODE_FUNC: {
            Param *p = n->data->data.func_data->parameters;
            while (p) {
                Param *next = p->next;
                free(p->name);
                free(p);
                p = next;
            }
            free(n->data->data.func_data);
            break;
        }
        case NODE_GETTER:
            free(n->data->data.getter_data);
            break;
        case NODE_SETTER:
            free(n->data->data.setter_data);
            break;
        }
        free(n->data);
    }

    if (n->key) {
        free(n->key);
    }
    free(n);
}

// AVL balance helpers
static int get_balance(SNode *n) {
    return n ? (node_height(n->left) - node_height(n->right)) : 0;
}

static SNode *rotate_right(SNode *y) {
    SNode *x = y->left;
    SNode *temp = x->right;

    x->right = y;
    y->left = temp;

    y->height = max(node_height(y->left), node_height(y->right)) + 1;
    x->height = max(node_height(x->left), node_height(x->right)) + 1;

    return x;
}

static SNode *rotate_left(SNode *x) {
    SNode *y = x->right;
    SNode *temp = y->left;

    y->left = x;
    x->right = temp;

    x->height = max(node_height(x->left), node_height(x->right)) + 1;
    y->height = max(node_height(y->left), node_height(y->right)) + 1;

    return y;
}

// ---------- Recursive insert + balance ----------
static SNode *insert_node(SNode *node, const char *key, SymTableData *data,
                          bool *inserted) {
    if (!node) {
        *inserted = true;
        return create_node(key, data);
    }

    int cmp = strcmp(key, node->key);
    if (cmp < 0) {
        node->left = insert_node(node->left, key, data, inserted);
    } else if (cmp > 0) {
        node->right = insert_node(node->right, key, data, inserted);
    } else {
        // key already exists
        *inserted = false;
        return node;
    }

    node->height = 1 + max(node_height(node->left), node_height(node->right));

    int balance = get_balance(node);

    // LL
    if (balance > 1 && strcmp(key, node->left->key) < 0)
        return rotate_right(node);

    // RR
    if (balance < -1 && strcmp(key, node->right->key) > 0)
        return rotate_left(node);

    // LR
    if (balance > 1 && strcmp(key, node->left->key) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    // RL
    if (balance < -1 && strcmp(key, node->right->key) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

// Search node
static SymTableData *search_node(SNode *node, const char *key) {
    if (!node)
        return NULL;

    int cmp = strcmp(key, node->key);
    if (cmp == 0)
        return node->data;
    if (cmp < 0)
        return search_node(node->left, key);
    return search_node(node->right, key);
}

// Free whole subtree
static void free_subtree(SNode *node) {
    if (!node)
        return;
    free_subtree(node->left);
    free_subtree(node->right);
    free_node(node);
}

// ---------- Public API ----------

void symtable_init(SymTable *table) { table->root = NULL; }

void symtable_free(SymTable *table) {
    free_subtree(table->root);
    table->root = NULL;
}

SymTableData *symtable_search(SymTable *table, const char *key) {
    return search_node(table->root, key);
}

bool symtable_insert(SymTable *table, const char *key, SymTableData *data) {
    bool inserted = false;
    table->root = insert_node(table->root, key, data, &inserted);
    return inserted;
}

bool symtable_delete(SymTable *table, const char *key) {
    // Optional implementation if needed
    (void)table;
    (void)key;
    return false;
}

// ---------- Factory functions ----------

SymTableData *make_variable(DataType type, bool defined, bool initialized) {
    SymTableData *d = malloc(sizeof(SymTableData));
    if (!d)
        return NULL;
    d->type = NODE_VAR;
    d->data.var_data = malloc(sizeof(VariableData));
    if (!d->data.var_data) {
        free(d);
        return NULL;
    }
    d->data.var_data->data_type = type;
    d->data.var_data->defined = defined;
    d->data.var_data->initialized = initialized;
    return d;
}

// Create a new parameter node
Param *make_param(const char *name, DataType type) {
    Param *p = malloc(sizeof(Param));
    if (!p)
        return NULL;

    p->name = my_strdup(name); // copy identifier
    if (!p->name) {
        free(p);
        return NULL;
    }
    p->data_type = type;
    p->next = NULL;
    return p;
}

// Append parameter to list (returns new head)
Param *append_param(Param *head, Param *new_param) {
    if (!head)
        return new_param;
    Param *curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = new_param;
    return head;
}

SymTableData *make_function(int param_count, Param *params, bool defined, DataType return_type) {
    SymTableData *d = malloc(sizeof(SymTableData));
    if (!d)
        return NULL;
    d->type = NODE_FUNC;
    d->data.func_data = malloc(sizeof(FunctionData));
    if (!d->data.func_data) {
        free(d);
        return NULL;
    }
    d->data.func_data->param_count = param_count;
    d->data.func_data->parameters = params;
    d->data.func_data->defined = defined;
    d->data.func_data->return_type = return_type;
    return d;
}

SymTableData *make_getter(DataType return_type, bool defined) {
    SymTableData *d = malloc(sizeof(SymTableData));
    if (!d)
        return NULL;
    d->type = NODE_GETTER;
    d->data.getter_data = malloc(sizeof(GetterData));
    if (!d->data.getter_data) {
        free(d);
        return NULL;
    }
    d->data.getter_data->return_type = return_type;
    d->data.getter_data->defined = defined;
    return d;
}

SymTableData *make_setter(DataType param_type, bool defined) {
    SymTableData *d = malloc(sizeof(SymTableData));
    if (!d)
        return NULL;
    d->type = NODE_SETTER;
    d->data.setter_data = malloc(sizeof(SetterData));
    if (!d->data.setter_data) {
        free(d);
        return NULL;
    }
    d->data.setter_data->param_type = param_type;
    d->data.setter_data->defined = defined;
    return d;
}
