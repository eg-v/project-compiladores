#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

typedef struct Symbol {
    char *name;
    TypeInfo type;
    struct Symbol *next;
} Symbol;

typedef struct {
    Symbol *head;
} SymTab;

SymTab *symtab_new(void);
void symtab_insert(SymTab *st, const char *name, TypeInfo t);
TypeInfo symtab_lookup(SymTab *st, const char *name);

#endif
