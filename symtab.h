#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

typedef enum {
    FUNCTION,
    VARIABLE,
    PARAMETER
} flags;

typedef struct Symbol {
    Info *info;
    struct Symbol *next;
} Symbol;

typedef struct SymTab {
    struct SymTab *parent;
    int level;
    Symbol *head;
} SymTab;

SymTab *symtab_new(void);
void symtab_insert(SymTab *st, Info *info);
TypeInfo symtab_lookup(SymTab *st, const char *name);
TypeInfo symtab_scope(SymTab *st, const char *name);
void symtab_print(SymTab *st);
int symtab_get_value(SymTab *st, const char *name, int *found);
void symtab_set_value(SymTab *st, const char *name, int value);

#endif
