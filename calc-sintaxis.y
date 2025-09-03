%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

AST *root = NULL;
void yyerror(const char *s);
int yylex(void);
%}

%union {
    int ival;
    char *sval;
    struct AST *ast;
}
 
%token <ival> INT
%token <ival> BOOL
%token <sval> ID
%token <sval> TIPO
%token RETURN
%token MAIN
%token OR AND NOT

%type <ast> prog stmt expr valor decl def_main

%left OR
%left AND
%left '+'
%left '*'
%left '-'
%right NOT
%start prog
%%
 
prog:
      { $$ = NULL; root = NULL; }
    | prog stmt {
        if ($1 == NULL) {
            $$ = $2;
            root = $2;
        } else {
            AST *p = $1;
            while (p->next) p = p->next;
            p->next = $2;
            $$ = $1;
            root = $1;
        }
    }
    ;

stmt:
      expr ';'                  { $$ = $1; }
    | decl ';'                  { $$ = $1; }
    | RETURN expr ';'           { $$ = make_node(NODE_RETURN, NULL, 0, 0, 0, $2, NULL); }
    | def_main              { $$ = $1; }
    ;

def_main:
    TIPO MAIN '(' ')' '{' prog '}' {
        TypeInfo t;
        if (strcmp($1, "int") == 0) t = TYPE_INT;
        else if (strcmp($1, "bool") == 0) t = TYPE_BOOL;
        else t = TYPE_UNKNOWN;

        AST *fn = make_node(NODE_FUNCTION, "main", 0, 0, 0, $6, NULL);
        fn->info->eval_type = t;
        $$ = fn;
    }
;


decl:
      TIPO ID {
          TypeInfo t;
          if (strcmp($1, "int") == 0) t = TYPE_INT;
          else if (strcmp($1, "bool") == 0) t = TYPE_BOOL;
          else t = TYPE_UNKNOWN;

          $$ = make_node(NODE_DECL, $2, 0, 0, 0, NULL, NULL);
          $$->info->eval_type = t;   /* store declared type */
      }
    | ID '=' expr {
        AST *id_node = make_node(NODE_ID, $1, 0, 0, 0, NULL, NULL);
        $$ = make_node(NODE_ASSIGN, NULL, 0, 0, 0, id_node, $3);
    }
    | TIPO ID '=' expr {
        TypeInfo t;
        if (strcmp($1, "int") == 0) t = TYPE_INT;
        else if (strcmp($1, "bool") == 0) t = TYPE_BOOL;
        else t = TYPE_UNKNOWN;

        AST *id_node = make_node(NODE_ID, $2, 0, 0, 0, NULL, NULL);
        id_node->info->eval_type =  t;
        $$ = make_node(NODE_DECL, $2, 0, 0, 0, id_node, $4);
        $$->info->eval_type =  t;
    }
    ;
  
expr:
      valor                     { $$ = $1; }  
    | ID                        { $$ = make_node(NODE_ID, $1, 0, 0, 0, NULL, NULL); }
    | expr '+' expr             { $$ = make_node(NODE_BINOP, NULL, 0, 0, '+', $1, $3); }
    | expr '*' expr             { $$ = make_node(NODE_BINOP, NULL, 0, 0, '*', $1, $3); } 
    | expr '-' expr             { $$ = make_node(NODE_BINOP, NULL, 0, 0, '-', $1, $3); }
    | '(' expr ')'              { $$ = $2; }
    | expr OR expr              { $$ = make_node(NODE_BINOP, NULL, 0, 0, '|', $1, $3); }
    | expr AND expr             { $$ = make_node(NODE_BINOP, NULL, 0, 0, '&', $1, $3); }
    | NOT expr                  { $$ = make_node(NODE_UNOP, NULL, 0, 0, '!', $2, NULL); }
    ;


valor : INT                     { $$ = make_node(NODE_INT, NULL, $1, 0, 0, NULL, NULL); } 
      | BOOL                    { $$ = make_node(NODE_BOOL, NULL, 0, $1, 0, NULL, NULL); }
       ;
 
%%
