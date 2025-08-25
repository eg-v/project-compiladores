#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST *make_node(NodeType type, char *name, int ival, char *bval,
               char op, AST *left, AST *right) {
    AST *n = malloc(sizeof(AST));
    n->type = type;
    n->name = name ? strdup(name) : NULL;
    n->ival = ival;
    n->bval = bval ? strdup(bval) : NULL;
    n->op = op;
    n->eval_type = TYPE_UNKNOWN;
    n->left = left;
    n->right = right;
    n->extra = NULL;
    n->next = NULL;
    return n;
}

void print_ast(AST *node, int depth, int is_last) {
    if (!node) return;

    for (int i = 0; i < depth-1; i++) printf("│   ");
    if (depth > 0) printf(is_last ? "└── " : "├── ");

    switch (node->type) {
        case NODE_INT:    printf("INT(%d)\n", node->ival); break;
        case NODE_BOOL:   printf("BOOL(%s)\n", node->bval); break;
        case NODE_ID:     printf("ID(%s)\n", node->name); break;
        case NODE_BINOP:  printf("BINOP(%c)\n", node->op); break;
        case NODE_UNOP:   printf("UNOP(%c)\n", node->op); break;
        case NODE_DECL:   printf("DECL(%s)\n", node->name); break;
        case NODE_ASSIGN: printf("ASSIGN(%s)\n", node->name); break;
        case NODE_RETURN: printf("RETURN\n"); break;
        case NODE_FUNCTION: printf("FUNCTION(%s)\n", node->name); break;
        case NODE_BLOCK:  printf("BLOCK\n"); break;
        default:          printf("NODE(%d)\n", node->type);
    }

    AST* children[3] = {node->left, node->right, node->extra};
    int n = 0;
    for (int i = 0; i < 3; i++) if (children[i]) n++;

    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (!children[i]) continue;
        count++;
        print_ast(children[i], depth + 1, count == n);
    }

    if (node->next) print_ast(node->next, depth, is_last);
}
