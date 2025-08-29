#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "ts.h"

static Nivel* nivelActual = NULL;

void abrirNivel(void) {
  Nivel* n = malloc(sizeof(Nivel));
  n->simbolos = NULL;
  n->anterior = nivelActual;
  nivelActual = n;
}

void cerrarNivel(void) {
  if (!nivelActual) return;

  Nivel* nivelABorrar = nivelActual;
  nivelActual = nivelABorrar->anterior;

  // Solo liberar símbolos locales de la función
  Simbolo* s = nivelABorrar->simbolos;
  while (s) {
    Simbolo* temp = s;
    s = s->siguiente;
    //if (s->flag == VARIABLE) { // Solo variables locales
    //    free(s->nombre);
    //    free(s);
    //}
  }
  free(nivelABorrar);
}


Simbolo* insertarSimbolo(char* nombre, TipoInfo tipo_info, Flag flag) {
  for (Simbolo* it = nivelActual->simbolos; it; it = it->siguiente) {
    if (strcmp(it->nombre, nombre) == 0) {
      fprintf(stderr, "Error Semántico: El identificador '%s' ya fue declarado en este ámbito.\n", nombre);
      return it;
    }
  }

  Simbolo* nuevo = malloc(sizeof(Simbolo));
  nuevo->nombre = strdup(nombre);
  nuevo->tipo_info = tipo_info;
  nuevo->flag = flag;
  nuevo->siguiente = nivelActual->simbolos;
  nivelActual->simbolos = nuevo;
  return nuevo;
}

Simbolo* buscarSimbolo(char* nombre) {
  for (Nivel* n = nivelActual; n; n = n->anterior) {
    for (Simbolo* s = n->simbolos; s; s = s->siguiente) {
      if (strcmp(s->nombre, nombre) == 0) return s;
    }
  }
  return NULL;
}

Simbolo* buscarSimboloEnNivelSuperior(char* nombre) {
    if (!nivelActual) return NULL;
    for (Simbolo* s = nivelActual->simbolos; s; s = s->siguiente) {
        if (strcmp(s->nombre, nombre) == 0) return s;
    }
    return NULL;
}

void imprimirTS(void) {
    int nivel = 0;
    for (Nivel* n = nivelActual; n; n = n->anterior) {
        printf("Nivel %d:\n", nivel++);
        for (Simbolo* s = n->simbolos; s; s = s->siguiente) {
            printf("  %s (tipo=%s)\n", s->nombre,
                s->tipo_info == TIPO_ENTERO ? "int" :
                s->tipo_info == TIPO_BOOLEANO ? "bool" : "void");
        }
    }
}
