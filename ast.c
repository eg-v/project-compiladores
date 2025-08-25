#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylineno;

AST *new_node(NodeKind kind, AST *l, AST *r, AST *t) {
    AST *n = malloc(sizeof(AST));
    n->kind = kind;
    n->name = NULL;
    n->value = 0;
    n->type = TYPE_UNKNOWN;
    n->lineno = yylineno;
    n->left = l; n->right = r; n->third = t; n->next = NULL;
    return n;
}

AST *new_leaf(NodeKind kind, char *name, TypeKind type) {
    AST *n = malloc(sizeof(AST));
    n->kind = kind;
    n->name = name ? strdup(name) : NULL;
    n->value = 0;
    n->type = type;
    n->lineno = yylineno;
    n->left = n->right = n->third = n->next = NULL;
    return n;
}

const char* type_to_str(TypeKind t) {
    switch(t) {
        case TYPE_INT: return "int";
        case TYPE_BOOL: return "bool";
        case TYPE_VOID: return "void";
        default: return "unknown";
    }
}

static const char* kind_to_str(NodeKind k) {
    switch(k) {
        case NODE_INT: return "INT";
        case NODE_BOOL: return "BOOL";
        case NODE_ID: return "ID";
        case NODE_ADD: return "+";
        case NODE_MUL: return "*";
        case NODE_AND: return "AND";
        case NODE_OR: return "OR";
        case NODE_NOT: return "NOT";
        case NODE_ASSIGN: return "ASSIGN";
        case NODE_DECL: return "DECL";
        case NODE_RETURN: return "RETURN";
        case NODE_FUNC_DEF: return "FUNC_DEF";
        case NODE_BLOCK: return "BLOCK";
        default: return "???";
    }
}

void print_ast(AST *n, int depth, int isLast) {
    if (!n) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("(%s", kind_to_str(n->kind));
    if (n->name) printf(" : %s", n->name);
    if (n->type != TYPE_UNKNOWN) printf(" : type=%s", type_to_str(n->type));
    printf(")\n");

    if (n->left) print_ast(n->left, depth+1, 0);
    if (n->right) print_ast(n->right, depth+1, 0);
    if (n->third) print_ast(n->third, depth+1, 0);
    if (n->next) print_ast(n->next, depth, 1);
}

