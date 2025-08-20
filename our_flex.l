%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

#include "calc-sintaxis.tab.h"

extern AST *root;
void print_ast(AST *n, int indent, int is_last);

%}

%option noyywrap
%option yylineno

letra [a-zA-Z]      
digito [0-9]   
bool (true|false)
tipo (bool|int|void)


%%
{digito}+                     { yylval.ival = atoi(yytext); return INT; }
{bool}                        { yylval.sval = strdup(yytext); return BOOL; }
"return"                      { return RETURN; }
"||"                          { return OR; }
"&&"                          { return AND; }
"~"                           { return NOT; }
{tipo}                        { yylval.sval = strdup(yytext); return TIPO; }
{letra}({letra}|{digito})*    { yylval.sval = strdup(yytext); return ID; }
[+*-;()={}]                    { return *yytext; }
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

        if (yyparse() == 0) {
            print_ast(root, 0, 1);
        }
        return 0;

}
