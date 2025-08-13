%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "calc-sintaxis.tab.h"

%}

%option noyywrap
%option yylineno

letra [a-zA-Z]      
digito [0-9]   
bool (true|false)
tipo (bool|int)
or (\|\|)
and (&&)
not (~)


%%
{digito}+                     { printf("INT: %d\n", atoi(yytext)); return INT; }
{bool}                        { printf("BOOL: %s\n", yytext); return BOOL; }
{or}                          { printf("OR: %s\n", yytext); return OR; }
{and}                         { printf("AND: %s\n", yytext); return AND; }
{not}                         { printf("NOT: %s\n", yytext); return NOT; }
{tipo}                        { printf("TIPO: %s\n", yytext); return TIPO; }
{letra}({letra}|{digito})*    { printf("ID: %s\n",yytext); return ID; }
"-"                           { printf("%s\n", yytext); return TMENOS; }    
[+*;()=]                      { printf("%s\n", yytext); return *yytext; }
.|\n                          ; /* ignore all the rest */
%%

void yyerror() {
	printf("%s%d\n","-> ERROR Sintactico en la linea: ",yylineno);
}

int main(int argc,char *argv[]){
	++argv,--argc;
	if (argc > 0)
		yyin = fopen(argv[0],"r");
	else
		yyin = stdin;

	yyparse();

}
