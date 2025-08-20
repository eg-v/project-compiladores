%{

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

AST *root = NULL;
%}

%union {
    int ival;
    char *sval;
    struct AST *ast;
}
 
%token <ival> INT
%token <sval> BOOL
%token <sval> ID
%token <sval> TIPO
%token RETURN
%token OR AND NOT

%type <ast> prog stmt expr VALOR decl block function_def

%left OR
%left AND
%left '+' TMENOS
%left '*'
%left '-'
%right NOT
%start prog
%%
 
prog:
                                { $$ = NULL; }
    | prog stmt                 { 
                                 if ($1 == NULL) $$ = $2;
                                 else {
                                     AST *p = $1;
                                     while (p->next) p = p->next;
                                     p->next = $2;
                                     root = $1;
                                     $$ = $1;
                                 }
                              }
    ;

stmt:
      expr ';'                  { $$ = $1; }
    | decl ';'                  { $$ = $1; }
    | RETURN expr ';'           { $$ = new_node(NODE_RETURN, $2, NULL); }
    | function_def              { $$ = $1; }
    ;

block:
    '{' prog '}'                { $$ = new_node(NODE_BLOCK, $2, NULL); }
    ;

function_def:
    TIPO ID '(' ')' block       {
                                    AST *fn = new_node(NODE_FUNCTION, $5, NULL);
                                    fn->name = strdup($2);
                                    $$ = fn;
                                }
    ;

decl: TIPO ID                   { $$ = new_node(NODE_DECL, new_id($2), NULL); }
    | TIPO ID '=' expr          { $$ = new_node(NODE_DECL, new_id($2), $4); }
    | ID '=' expr               { $$ = new_node(NODE_ASSIGN, new_id($1), $3); }
  
expr: VALOR                     { $$ = $1; }  
    | ID                        { $$ = new_id($1); }
    | expr '+' expr             { $$ = new_binop('+', $1, $3); }
    | expr '*' expr             { $$ = new_binop('*', $1, $3); } 
    | expr '-' expr             { $$ = new_binop('-', $1, $3); }
    | '(' expr ')'              { $$ = $2; }
    | expr OR expr              { $$ = new_binop('|', $1, $3); }
    | expr AND expr             { $$ = new_binop('&', $1, $3); }
    | NOT expr                  { $$ = new_unop('!', $2); }
    ;


VALOR : INT                     { $$ = new_int($1); } 
      | BOOL                    { $$ = new_bool($1); }
       ;
 
%%


#include <stdlib.h>
#include <string.h>

AST *new_node(NodeType type, AST *left, AST *right) {
    AST *n = malloc(sizeof(AST));
    n->type = type;
    n->name = NULL;
    n->ival = 0;
    n->bval = NULL;
    n->op = 0;
    n->left = left;
    n->right = right;
    n->extra = NULL;
    n->next = NULL;
    return n;
}

AST *new_int(int val) {
    AST *n = new_node(NODE_INT, NULL, NULL);
    n->ival = val;
    return n;
}

AST *new_bool(char *val) {
    AST *n = new_node(NODE_BOOL, NULL, NULL);
    n->bval = strdup(val);
    return n;
}

AST *new_id(char *name) {
    AST *n = new_node(NODE_ID, NULL, NULL);
    n->name = strdup(name);
    return n;
}

AST *new_binop(char op, AST *l, AST *r) {
    AST *n = new_node(NODE_BINOP, l, r);
    n->op = op;
    return n;
}

AST *new_unop(char op, AST *expr) {
    AST *n = new_node(NODE_UNOP, expr, NULL);
    n->op = op;
    return n;
}

void print_ast(AST *node, int depth, int is_last) {
    if (!node) return;

    for (int i = 0; i < depth-1; i++) {
        printf("│   ");
    }
    if (depth > 0) {
        printf(is_last ? "└── " : "├── ");
    }

    switch (node->type) {
        case NODE_INT:    printf("INT(%d)\n", node->ival); break;
        case NODE_BOOL:   printf("BOOL(%s)\n", node->bval); break;
        case NODE_ID:     printf("ID(%s)\n", node->name); break;
        case NODE_BINOP:  printf("BINOP(%c)\n", node->op); break;
        case NODE_UNOP:   printf("UNOP(%c)\n", node->op); break;
        case NODE_DECL:   printf("DECL\n"); break;
        case NODE_ASSIGN: printf("ASSIGN\n"); break;
        case NODE_RETURN: printf("RETURN\n"); break;
        case NODE_FUNCTION: printf("FUNCTION(%s)\n", node->name); break;
        case NODE_BLOCK:  printf("BLOCK\n"); break;
        default:          printf("NODE(%d)\n", node->type);
    }

    AST* children[3] = {node->left, node->right, node->extra};
    int n = 0;
    for (int i = 0; i < 3; i++)
        if (children[i]) n++;

    int count = 0;
    for (int i = 0; i < 3; i++) {
        if (!children[i]) continue;
        count++;
        print_ast(children[i], depth + 1, count == n);
    }

    if (node->next)
        print_ast(node->next, depth, is_last);
}

