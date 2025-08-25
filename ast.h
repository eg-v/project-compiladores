#ifndef AST_H
#define AST_H

typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_ID,
    TYPE_OP,
} DataType;

typedef struct {
    DataType dtype;
    char *name;
    int ival;      
    int bval;      
    char op;       
} Info;

typedef struct AST {
    Info info;
    struct AST *left;
    struct AST *right;
    struct AST *next;
} AST;

AST *new_int(int val);
AST *new_bool(int val);
AST *new_id(char *name);
AST *new_op(char op, AST *l, AST *r);

int check_types(AST *node);

void print_ast(AST *n, int depth, int is_last);

#endif