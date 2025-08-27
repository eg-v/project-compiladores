#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symtab.h"

SymTab *symtab_new(void) {
    SymTab *st = calloc(1, sizeof(SymTab));
    st->level = 0;
    return st;
}

void symtab_insert(SymTab *st, Info *info) {
    Symbol *s = calloc(1, sizeof(Symbol));
    // s->flag = flag;
    struct SymTab *parent;
    s->info = info;
    s->next = st->head;
    st->head = s;
}

TypeInfo symtab_lookup(SymTab *st, const char *name) {
    for (SymTab *scope = st; scope != NULL; scope = scope->parent) {
        for (Symbol *s = scope->head; s != NULL; s = s->next) {
            if (strcmp(s->info->name, name) == 0) {
                return s->info->eval_type;
            }
        }
    }
    return TYPE_ERROR;
}

void symtab_print(SymTab *st) {
    printf("=== Symbol Table ===\n");
    int scope_level = 0;
    for (SymTab *scope = st; scope != NULL; scope = scope->parent) {
        printf("Scope level %d:\n", scope_level++);
        for (Symbol *s = scope->head; s != NULL; s = s->next) {
            printf("  Name: %s, Type: %d\n", 
                   s->info->name ? s->info->name : "(null)",
                   s->info->eval_type);
        }
    }
    printf("====================\n");
}

