#ifndef AST_H
#define AST_H

typedef enum {
    NODE_INT,
    NODE_BOOL,
    NODE_ID,
    NODE_BINOP,
    NODE_UNOP,
    NODE_DECL,
    NODE_ASSIGN,
    NODE_RETURN,
    NODE_FUNCTION,
    NODE_BLOCK,
    NODE_PROG
} NodeType;

typedef struct AST {
    NodeType type;
    char* name;
    int ival;
    char* bval;
    char op;
    struct AST *left;
    struct AST *right;
    struct AST *extra;
    struct AST *next;
} AST;

AST *new_node(NodeType type, AST *left, AST *right);
AST *new_int(int val);
AST *new_bool(char *val);
AST *new_id(char *name);
AST *new_binop(char op, AST *l, AST *r);
AST *new_unop(char op, AST *expr);

void print_ast(AST *n, int depth, int is_last);

#endif
