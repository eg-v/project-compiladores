#ifndef TS_H
#define TS_H


typedef enum {
  TIPO_INDEFINIDO,
  TIPO_ENTERO,
  TIPO_BOOLEANO,
  TIPO_VOID
} TipoInfo;

typedef enum {
  FUNCION,
  VARIABLE
} Flag;

typedef struct Simbolo {
  char* nombre;
  TipoInfo tipo_info;
  Flag flag;
  struct Simbolo* siguiente;
} Simbolo;

typedef struct Nivel {
  Simbolo* simbolos;
  struct Nivel* anterior;
} Nivel;

void abrirNivel(void);
void cerrarNivel(void);
Simbolo* insertarSimbolo(char* nombre, TipoInfo tipo_info, Flag flag);
Simbolo* buscarSimbolo(char* nombre);
Simbolo* buscarSimboloEnNivelSuperior(char* nombre);
void imprimirTS(void);

#endif
