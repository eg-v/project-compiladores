%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "calc-sintaxis.tab.h"

extern AST *root;
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

#include <stdio.h>
#include "ast.h"

extern int yyparse(void);
extern AST *root;

int main() {
    if (yyparse() == 0) {
        printf("\nÁrbol sintáctico con tipos:\n");
        print_ast(root, 0, 1);
    }
    return 0;
}


