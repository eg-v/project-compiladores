#ifndef AST_H
#define AST_H

#include "ts.h"

typedef enum {
  N_PROGRAMA,
  N_FUNCION,
  N_BLOQUE,
  N_DECLARACION,
  N_ASIGNACION,
  N_RETORNO,
  N_EXPRESION,
  N_ID,
  N_ENTERO,
  N_BOOLEANO,
  N_SUMA,
  N_RESTA,
  N_MULTIPLICACION,
  N_IGUALDAD,
  N_AND,
  N_OR,
  N_NEGACION,
  N_ERROR
} TipoNodo;

typedef struct ASTNodo {
  TipoNodo tipo_nodo;
  TipoInfo tipo_info;
  Simbolo* simbolo;
  int valor_entero;
  int valor_booleano;
  struct ASTNodo *hijo_izq;
  struct ASTNodo *hijo_der;
  struct ASTNodo* siguiente;
} ASTNodo;

ASTNodo* crear_nodo(TipoNodo tipo, ASTNodo *izq, ASTNodo *der);
ASTNodo* crear_hoja_entero(int valor);
ASTNodo* crear_hoja_booleano(int valor);
ASTNodo* crear_hoja_id(Simbolo* s);

void imprimir_ast(ASTNodo *nodo, int nivel);
// TipoInfo chequearTipos(ASTNodo* nodo);
void chequear(ASTNodo* raiz);
TipoInfo chequearTiposNodo(ASTNodo* nodo, TipoInfo tipoRetornoFuncion);
const char* tipoToString(TipoInfo t);

extern ASTNodo *raiz;
extern int yylineno;

#endif
