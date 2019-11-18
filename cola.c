#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cola.h"

#define FALLO -1
#define EXITO 0
#define COLA_VACIA 0
#define UNICO_NODO 1


/* 
 * Crea una cola reservando la memoria necesaria
 * para almacenar la estructura.
 * Devuelve un puntero a la estructura cola_t creada.
 */
cola_t* cola_crear(cola_destructor_t destructor){
	cola_t* cola = malloc(sizeof(cola_t));
	if(cola == NULL){
		return NULL;
	}

	cola->nodo_inicio = NULL;
	cola->nodo_fin = NULL;
	cola->cantidad = COLA_VACIA;
	cola->destructor = destructor;
	
	return cola;
}

// Pre C.: Se debe recibir un puntero al struct cola_t cola.
// Post C.: Devuelve 'true' si hay algún error que invalide la cola (es decir si no existe la cola, si la cantidad de elementos es menor a 0, etc).
bool hay_error(cola_t* cola){
	return ((cola == NULL) || ((cola->cantidad) < COLA_VACIA) ||
		   (((cola->nodo_inicio) == NULL) && ((cola->nodo_fin) != NULL)) ||
		   (((cola->nodo_inicio) != NULL) && ((cola->nodo_fin) == NULL)));
}

/* 
 * Determina si la cola está vacia.
 * Devuelve true si está vacía y false si no.
 * Si la cola no existe devolverá true.
 */
bool cola_vacia(cola_t* cola){
	if(hay_error(cola)){
		return true;
	}
	return (cola->cantidad == COLA_VACIA);
}

/* 
 * Encola un elemento.
 * Devuelve 0 si pudo encolar o -1 si no pudo.
 */
int cola_encolar(cola_t* cola, void* elemento){
	if(hay_error(cola)){
		return FALLO;
	}

	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(nodo == NULL){
		return FALLO;
	}

	if(cola_vacia(cola)){
		cola->nodo_inicio = nodo;
		cola->nodo_fin = nodo;
	}
	else{
		cola->nodo_fin->siguiente = nodo;
		cola->nodo_fin = nodo;
	}	
	
	nodo->siguiente = NULL;
	nodo->elemento = elemento;
	(cola->cantidad)++;

	return EXITO;
}

/* 
 * Desencola un elemento.
 * Devuelve 0 si pudo desencolar o -1 si no pudo.
 */
int cola_desencolar(cola_t* cola){
	if((hay_error(cola)) || cola_vacia(cola)){
		return FALLO;
	}

	nodo_t* nodo_aux = cola->nodo_inicio;
	if(nodo_aux == NULL){
		return FALLO;
	}

	cola->nodo_inicio = cola->nodo_inicio->siguiente;
	(cola->destructor)(nodo_aux->elemento);
	free(nodo_aux);

	if(cola->cantidad == UNICO_NODO){
		cola->nodo_fin = NULL;
	}
	(cola->cantidad)--;

	return EXITO;
}

/* 
 * Devuelve la cantidad de elementos almacenados en la cola.
 * Si la cola no existe devolverá 0.
 */
int cola_cantidad(cola_t* cola){
	if(hay_error(cola)){
		return COLA_VACIA;
	}
	return (cola->cantidad);
}

/*
 * Devuelve el primer elemento de la cola o NULL en caso de estar
 * vacía.
 * Si la cola no existe devolverá NULL.
 */
void* cola_primero(cola_t* cola){
	if((hay_error(cola)) || (cola_vacia(cola))){
		return NULL;
	}
	return (cola->nodo_inicio->elemento);
}

/* 
 * Destruye la cola liberando toda la memoria reservada por ella.
 */
void cola_destruir(cola_t* cola){
	if(hay_error(cola)){
		return;
	}

	while(!cola_vacia(cola)){
		cola_desencolar(cola);
	}
	free(cola);
}