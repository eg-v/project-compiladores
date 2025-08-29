%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "calc-sintaxis.tab.h"

%}

%option noyywrap
%option yylineno

letra [a-zA-Z]
digito [0-9]

%%

"int" { return INT; }
"bool" { return BOOL; }
"void" { return VOID; }
"return" { return RETURN; }

"true" { yylval.bool_val = 1; return BOOL_CONST; }
"false" { yylval.bool_val = 0; return BOOL_CONST; }

{digito}+ { yylval.int_val = atoi(yytext); return INT_CONST; }

"==" { return IGUALDAD; }
"AND" { return AND; }
"OR" { return OR; }
"!" { return NOT; }

"+" { return SUMA; }
"-" { return RESTA; }
"*" { return MULTIPLICACION; }
"=" { return ASIGNAR; }

{letra}({letra}|{digito})* { yylval.id = strdup(yytext); return ID; }

[\;(){}] { return *yytext; }

"//".* ;

[ \t\r\n]+ ;

. {
  fprintf(stderr, "Error Léxico en línea %d: Caracter no reconocido '%s'\n", yylineno, yytext);
}

%%
