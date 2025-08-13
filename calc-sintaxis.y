%{

#include <stdlib.h>
#include <stdio.h>

%}
 
%token INT
%token BOOL
%token VOID
%token ID
%token TIPO
%token TMENOS
%token RETURN

%type expr
%type VALOR
    
%left OR
%left AND
%right NOT
%left '+' TMENOS 
%left '*'
 
%%
 
prog:
    | prog stmt
    ;

stmt:
      expr ';'
    | decl ';'
    | RETURN expr ';'
    | function_def
    ;

block:
    '{' prog '}'
    ;

function_def:
    TIPO ID '(' ')' block
    ;

decl: TIPO ID
    | TIPO ID '=' expr
    | ID '=' expr
  
expr: VALOR               
    | ID
    | expr '+' expr    
    | expr '*' expr
    | expr TMENOS expr  
    | '(' expr ')'      
    | expr OR expr
    | expr AND expr
    | NOT expr
    ;


VALOR : INT              
      | BOOL
       ;
 
%%


