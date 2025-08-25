#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symtab.h"

SymTab *symtab_new(void) {
    SymTab *st = calloc(1, sizeof(SymTab));
    return st;
}

void symtab_insert(SymTab *st, const char *name, TypeInfo t) {
    Symbol *s = calloc(1, sizeof(Symbol));
    s->name = strdup(name);
    s->type = t;
    s->next = st->head;
    st->head = s;
}

TypeInfo symtab_lookup(SymTab *st, const char *name) {
    for (Symbol *s = st->head; s; s = s->next) {
        if (strcmp(s->name, name) == 0)
            return s->type;
    }
    return TYPE_ERROR;
}
