#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST *make_node(NodeType type, char *name, int ival, int bval,
               char op, AST *left, AST *right) {
    AST *n = malloc(sizeof(AST));
    n->type = type;
    n->info = malloc(sizeof(Info));
    n->info->name = name ? strdup(name) : NULL;
    n->info->ival = ival;
    n->info->bval = bval;
    n->info->op = op;
    n->info->eval_type = TYPE_UNKNOWN;
    n->info->initialized = 0;
    n->left = left;
    n->right = right;
    n->next = NULL;
    return n;
}

void print_ast(AST *node, int depth, int is_last) {
    if (!node) return;

    for (int i = 0; i < depth-1; i++) printf("│   ");
    if (depth > 0) printf(is_last ? "└── " : "├── ");

    switch (node->type) {
        case NODE_INT:    printf("INT(%d)\n", node->info->ival); break;
        case NODE_BOOL:   printf("BOOL(%s)\n", (node->info->bval) ? "TRUE" : "FALSE"); break;
        case NODE_ID:     printf("ID(%s)\n", node->info->name); break;
        case NODE_BINOP:  printf("BINOP(%c)\n", node->info->op); break;
        case NODE_UNOP:   printf("UNOP(%c)\n", node->info->op); break;
        case NODE_DECL:   printf("DECL(%s)\n", node->info->name); break;
        case NODE_ASSIGN: printf("ASSIGN\n"); break;
        case NODE_RETURN: printf("RETURN\n"); break;
        case NODE_FUNCTION: printf("FUNCTION(%s)\n", node->info->name); break;
        default:          printf("NODE(%d)\n", node->type);
    }

    AST* children[2] = {node->left, node->right};
    int n = 0;
    for (int i = 0; i < 2; i++) if (children[i]) n++;

    int count = 0;
    for (int i = 0; i < 2; i++) {
        if (!children[i]) continue;
        count++;
        print_ast(children[i], depth + 1, count == n);
    }

    if (node->next) print_ast(node->next, depth, is_last);
}

const char* type_to_string(TypeInfo t) {
    switch (t) {
        case TYPE_INT: return "int";
        case TYPE_BOOL: return "bool";
        case TYPE_UNKNOWN: return "unknown";
        case TYPE_ERROR: return "error";
        default: return "invalid";
    }
}
