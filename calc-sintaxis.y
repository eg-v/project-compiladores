%{

#include <stdlib.h>
#include <stdio.h>

%}
 
%token INT
%token BOOL
%token ID
%token TIPO
%token TMENOS

%type expr
%type VALOR
    
%left OR
%left AND
%right NOT
%left '+' TMENOS 
%left '*'
 
%%
 
prog:
      /* empty */
    | prog stmt
    ;

stmt:
      expr ';'
    | decl ';'
    ;

decl: TIPO ID
    | TIPO ID '=' expr
  
expr: VALOR               
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


