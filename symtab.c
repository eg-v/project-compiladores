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


TypeInfo symtab_scope(SymTab *st, const char *name) {
    SymTab *scope = st;
    for (Symbol *s = scope->head; s != NULL; s = s->next) {
        if (strcmp(s->info->name, name) == 0) {
            return s->info->eval_type;
        }
    }
    return TYPE_ERROR;
}

void symtab_print(SymTab *st) {
    printf("=== Symbol Table ===\n");
    printf("Scope level 0:\n");
    for (Symbol *s = st->head; s != NULL; s = s->next) {
        printf("  Name: %s, Type: %s, value: %d\n",
            s->info->name ? s->info->name : "(null)",
            type_to_string(s->info->eval_type),
            (s->info->eval_type == TYPE_INT) ? s->info->ival : s->info->bval);
    }
    printf("====================\n");
}

TypeInfo symtab_lookup(SymTab *st, const char *name) {
    for (Symbol *s = st->head; s != NULL; s = s->next) {
        if (s->info->name && strcmp(s->info->name, name) == 0) {
            return s->info->eval_type;
        }
    }
    return TYPE_ERROR;
}

int symtab_get_value(SymTab *st, const char *name, int *found) {
    for (Symbol *s = st->head; s != NULL; s = s->next) {
        if (strcmp(s->info->name, name) == 0) {
            *found = 1;
            if (s->info->eval_type == TYPE_INT) {
                return s->info->ival;
            } else if (s->info->eval_type == TYPE_BOOL) {
                return s->info->bval;
            }
        }
    }
    *found = 0;
    return 0;
}

void symtab_set_value(SymTab *st, const char *name, int value) {
    for (Symbol *s = st->head; s != NULL; s = s->next) {
        if (strcmp(s->info->name, name) == 0) {
            if (s->info->eval_type == TYPE_INT) {
                s->info->ival = value;
            } else if (s->info->eval_type == TYPE_BOOL) {
                s->info->bval = value;
            }
            return;
        }
    }
    fprintf(stderr, "Error: assignment to undeclared variable '%s'\n", name);
}

void symtab_mark_initialized(SymTab *st, const char *name) {
    for (Symbol *s = st->head; s != NULL; s = s->next) {
        if (s->info->name && strcmp(s->info->name, name) == 0) {
            s->info->initialized = 1;
            return;
        }
    }
    fprintf(stderr, "Warning: trying to mark undeclared variable '%s' as initialized\n", name);
}

