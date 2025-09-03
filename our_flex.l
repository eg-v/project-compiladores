%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "symtab.h"
#include "calc-sintaxis.tab.h"

extern AST *root;
void interpreter(AST* n, SymTab *st);
TypeInfo check_types(AST* n, SymTab *st);
void codegen(AST *n, SymTab *st);
%}

%option noyywrap
%option yylineno

letra   [a-zA-Z]      
digito  [0-9]   
bool    (true|false)
tipo    (bool|int|void)

%%
{digito}+                  { yylval.ival = atoi(yytext); return INT; }
{bool}                     { yylval.ival = ((strcmp(strdup(yytext), "true")) ? 0 : 1) ; return BOOL; }
"main"                     { return MAIN; }
"return"                   { return RETURN; }
"||"                       { return OR; }
"&&"                       { return AND; }
"!"                        { return NOT; }
{tipo}                     { yylval.sval = strdup(yytext); return TIPO; }
{letra}({letra}|{digito})* { yylval.sval = strdup(yytext); return ID; }
[+*\-;()={}]               { return *yytext; }
"//".* ;

[ \t\r\n]+ ;

. {
fprintf(stderr, "Lexical Error at line %d: Unrecognized character '%s'\n", yylineno, yytext);
}

%%

void yyerror(const char *s) {
    // Empty
}

int main(int argc,char *argv[]){
    ++argv,--argc;
    if (argc > 0)
        yyin = fopen(argv[0],"r");
    else
        yyin = stdin;

    if (yyparse() == 0) {
        print_ast(root, 0, 1);
        printf("\n--- Type Checking ---\n");
        SymTab *st = symtab_new();
        check_types(root, st);
        printf("\n--- Interpreter ---\n");
        interpreter(root, st);
        printf("\n--- Assembler ---\n");
        codegen(root, st);
    }
    return 0;
}
