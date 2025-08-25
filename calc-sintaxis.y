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
%left '-'
%left '*'
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
  

expr: INT        { $$ = new_int($1); }
    | BOOL       { $$ = new_bool($1); }
    | ID         { $$ = new_id($1); }
    | expr '+' expr { $$ = new_op('+', $1, $3); if (!check_types($$)) yyerror("Error de tipos en suma"); }
    | expr '*' expr { $$ = new_op('*', $1, $3); if (!check_types($$)) yyerror("Error de tipos en multiplicación"); }
    | expr AND expr { $$ = new_op('&', $1, $3); if (!check_types($$)) yyerror("Error de tipos en &&"); }
    | expr OR expr  { $$ = new_op('|', $1, $3); if (!check_types($$)) yyerror("Error de tipos en ||"); }
    | NOT expr      { $$ = new_op('!', $2, NULL); if (!check_types($$)) yyerror("Error de tipos en !"); }
    ;

VALOR : INT                     { $$ = new_int($1); } 
      | BOOL                    { $$ = new_bool($1); }
       ;
 
%%


#include <stdlib.h>
#include <string.h>

AST *new_int(int val) {
    AST *n = malloc(sizeof(AST));
    n->info.dtype = TYPE_INT;
    n->info.ival = val;
    n->info.name = NULL;
    n->info.bval = 0;
    n->info.op = 0;
    n->left = n->right = n->next = NULL;
    return n;
}

AST *new_bool(int val) {
    AST *n = malloc(sizeof(AST));
    n->info.dtype = TYPE_BOOL;
    n->info.bval = val;
    n->info.name = NULL;
    n->info.ival = 0;
    n->info.op = 0;
    n->left = n->right = n->next = NULL;
    return n;
}

AST *new_id(char *name) {
    AST *n = malloc(sizeof(AST));
    n->info.dtype = TYPE_ID;
    n->info.name = strdup(name);
    n->info.ival = 0;
    n->info.bval = 0;
    n->info.op = 0;
    n->left = n->right = n->next = NULL;
    return n;
}

AST *new_op(char op, AST *l, AST *r) {
    AST *n = malloc(sizeof(AST));
    n->info.dtype = TYPE_OP;
    n->info.op = op;
    n->info.name = NULL;
    n->info.ival = 0;
    n->info.bval = 0;
    n->left = l;
    n->right = r;
    n->next = NULL;
    return n;
}
int check_types(AST *node) {
    if (!node) return 1;
    if (node->info.dtype == TYPE_OP) {
        if (node->info.op == '+' || node->info.op == '*' || node->info.op == '-') {
            return node->left->info.dtype == TYPE_INT && node->right->info.dtype == TYPE_INT;
        }
        if (node->info.op == '&' || node->info.op == '|') {
            return node->left->info.dtype == TYPE_BOOL && node->right->info.dtype == TYPE_BOOL;
        }
        if (node->info.op == '!') {
            return node->left->info.dtype == TYPE_BOOL;
        }
    }
    return 1;
}


