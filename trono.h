#ifndef __TRONO_H__
#define __TRONO_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define ERROR -1
#define EXITO  0

#define PRIMERO_ES_MENOR -1
#define IGUALES 0
#define PRIMERO_ES_MAYOR  1

#define ANIO_INICIAL   -2
#define ARBOL_INORDEN   0
#define CAMPOS_PERSONA  3
#define CAMPOS_CASA     4
#define MAX_NOMBRE     15
#define CANT_NOMBRES   30
#define MAX_VIDA 	   60

#define INICIAR   'S'
#define AGREGAR   'A'
#define LISTAR    'L'
#define MOSTRAR   'E'
#define FINALIZAR 'Q'
#define AYUDAS    'H'

#define C_PERSONA 'I'
#define C_CASA 	  'C'

#define M_LECTURA "r"
#define M_AGREGAR "a"
#define F_CASAS   "casas.txt"
#define F_NOMBRES "nombres.txt"
#define CREAR 	  "crear"
#define ARCHIVO   "archivo"

#define FORMATO_CASAS   "%[^;];%zu;%zu\n"
#define FORMATO_PERSONA "%[^;];%zu\n"
#define FORMATO_NOMBRES "%s\n"


typedef struct persona{
	char* nombre;
	size_t edad;
} persona_t;

typedef struct casa{
	char* nombre;
	bool es_casa_extinta;
	size_t  factor_env;
	size_t  factor_nac;
	size_t  cant_personas;
	lista_t* lista_personas;
} casa_t;

void destruir_casa(void* casa);

void imprimir_mensaje_error(char error[MAX_NOMBRE], char nombre[MAX_NOMBRE]);

void mostrar_introduccion();

void mostrar_menu_ayudas();

int iniciar_simulacion(abb_t* arbol_casas, cola_t* casas_extintas);

int agregar_nueva_casa(abb_t* arbol_casas);

int listar_casas(abb_t* arbol_casas);

void mostrar_casas_extintas(cola_t* casas_extintas);

void terminar_simulacion(abb_t* arbol_casas, cola_t* casas_extintas);

void centrar_mensaje(char mensaje_1[], char mensaje_2[]);

#endif /* __TRONO_H__ */