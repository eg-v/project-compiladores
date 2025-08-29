%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "ts.h"

extern int yylineno;
extern ASTNodo *raiz;
void yyerror(const char *s);
int yylex(void);

%}

%token INT BOOL VOID RETURN
%token IGUALDAD AND OR NOT SUMA RESTA MULTIPLICACION ASIGNAR
%token <int_val> INT_CONST
%token <bool_val> BOOL_CONST
%token <id> ID

%union {
  int int_val;
  int bool_val;
  char* id;
  struct ASTNodo* nodo;
  TipoInfo tipo_val;
}

/* Tipos de no terminales */
%type <nodo> Programa ListaDeclaracionesFunciones ListaSentencias Sentencia Declaracion Asignacion Retorno Expresion Funcion Bloque
%type <tipo_val> TipoRetorno

/* precedencias */
%left OR
%left AND
%nonassoc IGUALDAD
%left SUMA RESTA
%left MULTIPLICACION
%right NOT

%%

Programa
  : ListaDeclaracionesFunciones { raiz = crear_nodo(N_PROGRAMA, $1, NULL); }
  ;

ListaDeclaracionesFunciones
  : /* vacío */ { $$ = NULL; }
  | Declaracion ListaDeclaracionesFunciones {
      if ($2) { $1->siguiente = $2; $$ = $1; }
      else $$ = $1;
    }
  | Funcion ListaDeclaracionesFunciones {
      if ($2) { $1->siguiente = $2; $$ = $1; }
      else $$ = $1;
    }
  ;

Funcion
  : TipoRetorno ID '(' ')' {
        Simbolo* s = insertarSimbolo($2, $1, FUNCION);
        abrirNivel();
    } Bloque {
        cerrarNivel();
        Simbolo* s = buscarSimboloEnNivelSuperior($2);
        ASTNodo* fn = crear_nodo(N_FUNCION, $6, NULL);
        fn->simbolo = s;
        $$ = fn;
        free($2);
    }
  ;

Bloque
  : '{' ListaSentencias '}' { $$ = $2; }
  ;


ListaSentencias
  : /* vacío */ { $$ = NULL; }
  | Sentencia ListaSentencias {
      if ($2) { $1->siguiente = $2; $$ = $1; }
      else { $$ = $1; }
    }
  ;

Sentencia
  : Declaracion { $$ = $1; }
  | Asignacion  { $$ = $1; }
  | Retorno     { $$ = $1; }
  | Bloque      { $$ = $1; }
  ;

Declaracion
  : TipoRetorno ID ';' {
      Simbolo* s = insertarSimbolo($2, $1, VARIABLE);
      ASTNodo* decl_nodo = crear_nodo(N_DECLARACION, NULL, NULL);
      decl_nodo->simbolo = s;
      $$ = decl_nodo;
      free($2);
    }
  | TipoRetorno ID ASIGNAR Expresion ';' {
      Simbolo* s = insertarSimbolo($2, $1, VARIABLE);
      ASTNodo* decl_nodo = crear_nodo(N_DECLARACION, $4, NULL);
      decl_nodo->simbolo = s;
      $$ = decl_nodo;
      free($2);
    }
  ;

Asignacion
  : ID ASIGNAR Expresion ';' {
      Simbolo* s = buscarSimbolo($1);
      if (!s) {
        fprintf(stderr, "Error Semántico en línea %d: variable '%s' no declarada.\n", yylineno, $1);
        yyerror("Uso de variable no declarada.");
        $$ = crear_nodo(N_ERROR, NULL, NULL);
      } else {
        ASTNodo* id_nodo = crear_hoja_id(s);
        $$ = crear_nodo(N_ASIGNACION, id_nodo, $3);
      }
      free($1);
    }
  ;

Retorno
  : RETURN Expresion ';' { $$ = crear_nodo(N_RETORNO, $2, NULL); }
  | RETURN ';' { $$ = crear_nodo(N_RETORNO, NULL, NULL); }
  ;

Expresion
  : Expresion SUMA Expresion           { $$ = crear_nodo(N_SUMA, $1, $3); }
  | Expresion RESTA Expresion          { $$ = crear_nodo(N_RESTA, $1, $3); }
  | Expresion MULTIPLICACION Expresion { $$ = crear_nodo(N_MULTIPLICACION, $1, $3); }
  | Expresion IGUALDAD Expresion       { $$ = crear_nodo(N_IGUALDAD, $1, $3); }
  | Expresion AND Expresion            { $$ = crear_nodo(N_AND, $1, $3); }
  | Expresion OR Expresion             { $$ = crear_nodo(N_OR, $1, $3); }
  | NOT Expresion                      { $$ = crear_nodo(N_NEGACION, $2, NULL); }
  | '(' Expresion ')'                  { $$ = $2; }
  | INT_CONST                          { $$ = crear_hoja_entero($1); }
  | BOOL_CONST                         { $$ = crear_hoja_booleano($1); }
  | ID {
      Simbolo* s = buscarSimbolo($1);
      if (!s) {
        fprintf(stderr, "Error Semántico en línea %d: variable '%s' no declarada.\n", yylineno, $1);
        yyerror("Uso de variable no declarada.");
        $$ = crear_nodo(N_ERROR, NULL, NULL);
      } else {
        $$ = crear_hoja_id(s);
      }
      free($1);
    }
  ;

TipoRetorno
  : INT { $$ = TIPO_ENTERO; }
  | BOOL { $$ = TIPO_BOOLEANO; }
  | VOID { $$ = TIPO_VOID; }
  ;

%%

void yyerror(const char *s) {
  extern char *yytext;
  fprintf(stderr, "-> ERROR Sintáctico en la línea %d: %s. Token inesperado: '%s'\n",
    yylineno, s, yytext);
}

int main(int argc, char **argv) {
    if(argc<2) { fprintf(stderr,"Uso: %s <archivo>\n", argv[0]); return 1; }
    FILE* fp = fopen(argv[1],"r");
    if(!fp) { perror("No se puede abrir el archivo"); return 1; }

    extern FILE* yyin;
    yyin = fp;

    abrirNivel(); /* Nivel global */

    if(yyparse() == 0 && raiz) {
        chequear(raiz);
        printf("--- Chequeo semántico finalizado ---\n");
        imprimir_ast(raiz,0);
    }

    cerrarNivel();
    fclose(fp);
    return 0;
}
