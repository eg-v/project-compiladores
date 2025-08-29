#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "ts.h"

ASTNodo* crear_nodo(TipoNodo tipo_nodo, ASTNodo* izq, ASTNodo* der) {
  ASTNodo* n = malloc(sizeof(ASTNodo));
  if (!n) {
      fprintf(stderr, "Error: sin memoria para AST.\n");
      exit(1);
  }
  n->tipo_nodo = tipo_nodo;
  n->tipo_info = TIPO_INDEFINIDO;
  n->simbolo = NULL;
  n->valor_entero = 0;
  n->valor_booleano = 0;
  n->hijo_izq = izq;
  n->hijo_der = der;
  n->siguiente = NULL;
  return n;
}

ASTNodo* crear_hoja_entero(int valor) {
  ASTNodo* n = crear_nodo(N_ENTERO, NULL, NULL);
  n->valor_entero = valor;
  n->tipo_info = TIPO_ENTERO;
  return n;
}

ASTNodo* crear_hoja_booleano(int valor) {
  ASTNodo* n = crear_nodo(N_BOOLEANO, NULL, NULL);
  n->valor_booleano = valor;
  n->tipo_info = TIPO_BOOLEANO;
  return n;
}


ASTNodo* crear_hoja_id(Simbolo* s) {
    ASTNodo* n = crear_nodo(N_ID, NULL, NULL);
    n->simbolo = s;
    if (s) {
      n->tipo_info = s->tipo_info;
    }
    return n;
}

void imprimir_ast(ASTNodo* nodo, int nivel) {
  if (!nodo) return;
  for (int i = 0; i < nivel; i++) printf("  ");

  switch(nodo->tipo_nodo) {
    case N_PROGRAMA:        printf("Programa\n"); break;
		case N_FUNCION:					printf("Funcion. ID: %s, Retorno: %s\n", nodo->simbolo->nombre, tipoToString(nodo->simbolo->tipo_info)); break;
    case N_BLOQUE:          printf("Bloque.\n");
    case N_DECLARACION:     printf("Declaracion. ID: %s, Tipo: %s\n", nodo->simbolo->nombre, tipoToString(nodo->simbolo->tipo_info)); break;
    case N_ASIGNACION:      printf("Asignacion. ID: %s\n", nodo->hijo_izq->simbolo->nombre); break;
    case N_RETORNO:         printf("Retorno\n"); break;
    case N_SUMA:            printf("+\n"); break;
    case N_RESTA:           printf("-\n"); break;
    case N_MULTIPLICACION:  printf("*\n"); break;
		case N_IGUALDAD:				printf("==\n"); break;
		case N_AND:							printf("AND\n"); break;
		case N_OR:							printf("OR\n"); break;
		case N_NEGACION:				printf("!\n"); break;
    case N_ID:              printf("ID: %s\n", nodo->simbolo->nombre); break;
    case N_ENTERO:          printf("Entero: %d\n", nodo->valor_entero); break;
    case N_BOOLEANO:        printf("Booleano: %s\n", nodo->valor_booleano ? "true" : "false"); break;
    case N_ERROR:           printf("<error>\n"); break;
    default:                printf("Nodo desconocido\n");
  }

  imprimir_ast(nodo->hijo_izq, nivel + 1);
  imprimir_ast(nodo->hijo_der, nivel + 1);
  imprimir_ast(nodo->siguiente, nivel);
}

void chequear(ASTNodo* raiz) {
  if (!raiz) {
    chequearTiposNodo(raiz, TIPO_INDEFINIDO);
  }
}

TipoInfo chequearTiposNodo(ASTNodo* nodo, TipoInfo tipoRetornoFuncion) {
  if (!nodo) return TIPO_INDEFINIDO;

  // Recorrido post orden, primero chequeamos hijos, despues nodo actual
  TipoInfo tipoIzq = TIPO_INDEFINIDO, tipoDer = TIPO_INDEFINIDO;

  if(nodo->tipo_nodo != N_FUNCION) {
    if (nodo->hijo_izq) tipoIzq = chequearTiposNodo(nodo->hijo_izq, tipoRetornoFuncion);
    if (nodo->hijo_der) tipoDer = chequearTiposNodo(nodo->hijo_der, tipoRetornoFuncion);
  }

  // Ahora chequeamos el nodo actual
  switch (nodo->tipo_nodo) {
    case N_ENTERO:
    case N_BOOLEANO:
    case N_ID:
      return nodo->tipo_info;

    case N_DECLARACION:
      if(nodo->hijo_izq) {
        if(tipoIzq != nodo->simbolo->tipo_info) {
          fprintf(stderr, "Error Semántico: tipo de inicialización para '%s' es incorrecto. Se esperaba '%s' pero se obtuvo '%s'.\n",
                    nodo->simbolo->nombre, tipoToString(nodo->simbolo->tipo_info), tipoToString(tipoIzq));
        }
      }
      return nodo->tipo_info = TIPO_INDEFINIDO;

    case N_ASIGNACION:
      if(tipoIzq != tipoDer) {
        fprintf(stderr, "Error Semántico: asignación de tipo incorrecto a '%s'. Se esperaba '%s' pero se obtuvo '%s'.\n",
                nodo->hijo_izq->simbolo->nombre, tipoToString(tipoIzq), tipoToString(tipoDer));
      }
      return nodo->tipo_info = tipoIzq;

    case N_SUMA:
    case N_RESTA:
    case N_MULTIPLICACION:
      if (tipoIzq != TIPO_ENTERO || tipoDer != TIPO_ENTERO) {
        fprintf(stderr, "Error Semántico: operaciones aritméticas requieren operandos de tipo 'int'.\n");
      }
      return nodo->tipo_info = TIPO_ENTERO;

    case N_IGUALDAD:
      if (tipoIzq != tipoDer) {
        fprintf(stderr, "Error Semántico: comparación de igualdad entre tipos incompatibles ('%s' y '%s').\n",
                tipoToString(tipoIzq), tipoToString(tipoDer));
      }
      return nodo->tipo_info = TIPO_BOOLEANO;

    case N_AND:
    case N_OR:
      if (tipoIzq != TIPO_BOOLEANO || tipoDer != TIPO_BOOLEANO) {
        fprintf(stderr, "Error Semántico: operaciones lógicas requieren operandos de tipo 'bool'.\n");
      }
      return nodo->tipo_info = TIPO_BOOLEANO;

    case N_NEGACION:
      if (tipoIzq != TIPO_BOOLEANO)
        fprintf(stderr, "Error Semántico: operador de negación '!' requiere un operando de tipo 'bool'.\n");
      return nodo->tipo_info = TIPO_BOOLEANO;

    case N_RETORNO: {
      TipoInfo tipoRetornoReal = nodo->hijo_izq ? tipoIzq : TIPO_VOID;
      if (tipoRetornoReal != tipoRetornoFuncion) {
        fprintf(stderr, "Error Semántico: tipo de retorno incorrecto. La función esperaba '%s' pero se retornó '%s'.\n",
                tipoToString(tipoRetornoFuncion), tipoToString(tipoRetornoReal));
      }
      return nodo->tipo_info = tipoRetornoReal;
    }

    case N_PROGRAMA:
    case N_BLOQUE: {
      ASTNodo* actual = nodo->hijo_izq;
      while (actual)
      {
        chequearTiposNodo(actual, tipoRetornoFuncion);
        actual = actual->siguiente;
      }
      break;
    }

    case N_FUNCION: {
      if (nodo->hijo_izq) {
        chequearTiposNodo(nodo->hijo_izq, nodo->simbolo->tipo_info);
      }
      break;
    }

    case N_ERROR:
      break;
  }

  // Chequeamos los nodos siguientes en una lista
  if(nodo->siguiente) {
    chequearTiposNodo(nodo->siguiente, tipoRetornoFuncion);
  }

  return nodo->tipo_info;
}


// TipoInfo chequearTipos(ASTNodo* nodo) {
//   if (!nodo) return TIPO_INDEFINIDO;

//   TipoInfo tipoIzq, tipoDer;

//   switch (nodo->tipo_nodo) {
//     case N_ENTERO:
//       return nodo->tipo_info = TIPO_ENTERO;

//     case N_BOOLEANO:
//       return nodo->tipo_info = TIPO_BOOLEANO;

//     case N_ID: {
//       if(nodo->simbolo) {
//         return nodo->tipo_info = nodo->simbolo->tipo_info;
//       } else {
//         fprintf(stderr, "Error: ID no declarado.\n");
//         return TIPO_INDEFINIDO;
//       }
//     }

//     case N_DECLARACION:
//       if (nodo->hijo_izq) {
//         TipoInfo tipoIzq = chequearTipos(nodo->hijo_izq);
//         if (tipoIzq != nodo->simbolo->tipo_info) {
//              fprintf(stderr, "Error Semántico: El tipo de la expresión de inicialización no coincide con el tipo de la variable '%s'. Se esperaba '%s' pero se obtuvo '%s'.\n",
//                         nodo->simbolo->nombre,
//                         tipoToString(nodo->simbolo->tipo_info),
//                         tipoToString(tipoIzq));
//         }
//       }
//       return TIPO_INDEFINIDO;

//     case N_ASIGNACION:
//       tipoDer = chequearTipos(nodo->hijo_izq);
//       if(nodo->simbolo && nodo->simbolo->tipo_info != tipoDer) {
//           fprintf(stderr, "Error: asignacion de tipo diferente a '%s' en '%s'.\n",
//                   tipoToString(nodo->simbolo->tipo_info), nodo->simbolo->nombre);
//       }
//       return nodo->simbolo ? nodo->simbolo->tipo_info : TIPO_INDEFINIDO;

//     case N_SUMA:
//     case N_RESTA:
//     case N_MULTIPLICACION:
//         tipoIzq = chequearTipos(nodo->hijo_izq);
//         tipoDer = chequearTipos(nodo->hijo_der);
//         if(tipoIzq != TIPO_ENTERO || tipoDer != TIPO_ENTERO)
//             fprintf(stderr, "Error: operaciones aritméticas requieren enteros.\n");
//         return TIPO_ENTERO;

//     case N_IGUALDAD:
//         tipoIzq = chequearTipos(nodo->hijo_izq);
//         tipoDer = chequearTipos(nodo->hijo_der);
//         if(tipoIzq != tipoDer)
//             fprintf(stderr, "Error: comparacion entre tipos distintos.\n");
//         return TIPO_BOOLEANO;

//     case N_AND:
//     case N_OR:
//         tipoIzq = chequearTipos(nodo->hijo_izq);
//         tipoDer = chequearTipos(nodo->hijo_der);
//         if(tipoIzq != TIPO_BOOLEANO || tipoDer != TIPO_BOOLEANO)
//             fprintf(stderr, "Error: operacion logica requiere booleanos.\n");
//         return TIPO_BOOLEANO;

//     case N_NEGACION:
//         tipoIzq = chequearTipos(nodo->hijo_izq);
//         if(tipoIzq != TIPO_BOOLEANO)
//             fprintf(stderr, "Error: negacion requiere booleano.\n");
//         return TIPO_BOOLEANO;

//     case N_RETORNO:
//         if(nodo->hijo_izq) return chequearTipos(nodo->hijo_izq);
//         else return TIPO_VOID;

//     case N_PROGRAMA:
//     case N_FUNCION:
//         chequearTipos(nodo->hijo_izq);
//       return TIPO_VOID;

//     default:
//       return TIPO_INDEFINIDO;
//   }
// }

const char* tipoToString(TipoInfo t) {
  switch (t) {
    case TIPO_ENTERO: return "int";
    case TIPO_BOOLEANO: return "bool";
    case TIPO_VOID: return "void";
    default: return "indefinido";
  }
}

ASTNodo *raiz = NULL;

