#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "abb.h"

// Constantes:
    #define ABB_RECORRER_INORDEN   0
    #define ABB_RECORRER_PREORDEN  1
    #define ABB_RECORRER_POSTORDEN 2

    #define PRIMERO_ES_MENOR -1
    #define IGUALES 0
    #define PRIMERO_ES_MAYOR 1

    #define ERROR -1
    #define EXITO  0

    #define TAM_ARRAY 10

/*
 * Crea el arbol y reserva la memoria necesaria de la estructura.
 * Comparador se utiliza para comparar dos elementos.
 * Destructor es invocado sobre cada elemento que sale del arbol,
 * puede ser NULL indicando que no se debe utilizar un destructor.
 *
 * Devuelve un puntero al arbol creado o NULL en caso de error.
 */
abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
	if(comparador == NULL){
		return NULL;
	}

	abb_t* arbol = malloc(sizeof(abb_t));
	if(arbol == NULL){
		return NULL;
	}

	arbol->nodo_raiz = NULL;
	arbol->comparador = comparador;
	arbol->destructor = destructor;
    arbol->cantidad_elementos = 0;

	return arbol;
}

/*
 * Determina si el árbol está vacío.
 * Devuelve true si lo está, false en caso contrario.
 */
bool arbol_vacio(abb_t* arbol){
	return ((arbol == NULL) || (arbol->nodo_raiz == NULL) || ((arbol->cantidad_elementos) == 0));
}

// Pre C.: Recibe 3 punteros a la raiz, al nodo a insertar y a la función comparadora.
// Post C.: Inserta el nodo con su respectivo elemento en el lugar correspondiente según la siguiente convención...
//			Si el elemento a insertar es menor, va a izquierda.
//			Si el elemento a insertar es mayor o igual, va a derecha.
void arbol_insertar_recursiva(nodo_abb_t* raiz, nodo_abb_t* nuevo_nodo, abb_comparador comparador){
	if(raiz == NULL){
		return;
	}

	if((comparador(nuevo_nodo->elemento, raiz->elemento) == IGUALES) || (comparador(nuevo_nodo->elemento, raiz->elemento) == PRIMERO_ES_MAYOR)){
		if(raiz->derecha == NULL){
			raiz->derecha = nuevo_nodo;
		}
		else{
			arbol_insertar_recursiva(raiz->derecha, nuevo_nodo, comparador);
		}
	}
	else if(comparador(nuevo_nodo->elemento, raiz->elemento) == PRIMERO_ES_MENOR){
		if(raiz->izquierda == NULL){
			raiz->izquierda = nuevo_nodo;
		}
		else{
			arbol_insertar_recursiva(raiz->izquierda, nuevo_nodo, comparador);
		}
	}
}

/*
 * Inserta un elemento en el árbol.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 * El árbol admite elementos con valores repetidos.
 */
int arbol_insertar(abb_t* arbol, void* elemento){
	if((arbol == NULL) || (elemento == NULL)){
		return ERROR;
	}

	nodo_abb_t* nuevo_nodo = malloc(sizeof(nodo_abb_t));
	if(nuevo_nodo == NULL){
		return ERROR;
	}

	nuevo_nodo->elemento = elemento;

	if(arbol_vacio(arbol)){
		arbol->nodo_raiz = nuevo_nodo;
	}
	else{
		arbol_insertar_recursiva(arbol->nodo_raiz, nuevo_nodo, arbol->comparador);
	}

	nuevo_nodo->derecha = NULL;
	nuevo_nodo->izquierda = NULL;
    (arbol->cantidad_elementos)++;

	return EXITO;
}

// Pre C.: Recibe un puntero al nodo derecho del nodo que se busca eliminar.
// Post C.: Devuelve un puntero al nodo que contiene el elemento menor de los mayores
//          (con respecto al elemento del nodo a borrar).
nodo_abb_t* nodo_menor_de_los_mayores(nodo_abb_t* raiz){
    if(raiz->izquierda != NULL){
        return nodo_menor_de_los_mayores(raiz->izquierda);
    }

    return raiz;
}

// Pre C.: Recibe un puntero a la raíz del árbol y al elemento a borrar, junto con el nodo padre (pasado por referencia).
//         A su vez, recibe también las funciones de comparación  destrucción.
// Post C.: En caso que se encuentre presente en el árbol, borra el elemento pedido y reorganiza los punteros correspondiente de forma recursiva.
int arbol_borrar_recursiva(nodo_abb_t* raiz, nodo_abb_t** nodo_padre, void* elemento_a_borrar, abb_comparador comparador, abb_liberar_elemento destructor){
    if(raiz == NULL){
        return ERROR;
    }

    if(comparador(elemento_a_borrar, raiz->elemento) == IGUALES){
        if((raiz->derecha == NULL) && (raiz->izquierda == NULL)){
            (*nodo_padre) = NULL;
            destructor(raiz->elemento);
            free(raiz);
        }
        else if((raiz->derecha == NULL) && (raiz->izquierda != NULL)){
            (*nodo_padre) = raiz->izquierda;
            destructor(raiz->elemento);
            free(raiz);
        }
        else if((raiz->derecha != NULL) && (raiz->izquierda == NULL)){
            (*nodo_padre) = raiz->derecha;
            destructor(raiz->elemento);
            free(raiz);
        }
        else if((raiz->derecha != NULL) && (raiz->izquierda != NULL)){
            nodo_abb_t* nodo_aux = nodo_menor_de_los_mayores(raiz->derecha);

            void* elemento_aux = raiz->elemento;
            raiz->elemento = nodo_aux->elemento;
            nodo_aux->elemento = elemento_aux;

            return arbol_borrar_recursiva(raiz->derecha, &(raiz->derecha), nodo_aux->elemento, comparador, destructor);
        }
    }
    else if(comparador(elemento_a_borrar, raiz->elemento) == PRIMERO_ES_MAYOR){
        return arbol_borrar_recursiva(raiz->derecha, &(raiz->derecha), elemento_a_borrar, comparador, destructor);
    }
    else if(comparador(elemento_a_borrar, raiz->elemento) == PRIMERO_ES_MENOR){
        return arbol_borrar_recursiva(raiz->izquierda, &(raiz->izquierda), elemento_a_borrar, comparador, destructor);
    }

    return EXITO;
}

/*
 * Busca en el arbol un elemento igual al provisto (utilizando la
 * funcion de comparación) y si lo encuentra lo quita del arbol.
 * Adicionalmente, si encuentra el elemento, invoca el destructor con
 * dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 en caso contrario.
 */
int arbol_borrar(abb_t* arbol, void* elemento){
    if(arbol_vacio(arbol)){
        return ERROR;
    }

    int estado = arbol_borrar_recursiva(arbol->nodo_raiz, &(arbol->nodo_raiz), elemento, arbol->comparador, arbol->destructor);
    (arbol->cantidad_elementos)--;

    return estado;
}

// Pre C.: Recibe 3 punteros a la raiz, al elemento buscado y a la función comparadora.
// Post C.: Recorre el árbol recursivamente y, en caso de encontrarlo, devuelve un puntero al elemento buscado.
void* arbol_buscar_recursiva(nodo_abb_t* raiz, void* elemento_buscado, abb_comparador comparador){
    if(raiz == NULL){
        return NULL;
    }

    if((comparador(elemento_buscado, raiz->elemento) == IGUALES)){
        return elemento_buscado;
    }
    else if((comparador(elemento_buscado, raiz->elemento) == PRIMERO_ES_MAYOR)){
        return arbol_buscar_recursiva(raiz->derecha, elemento_buscado, comparador);
    }
    else{
        return arbol_buscar_recursiva(raiz->izquierda, elemento_buscado, comparador);
    }
}

/*
 * Busca en el arbol un elemento igual al provisto (utilizando la
 * funcion de comparación).
 *
 * Devuelve el elemento encontrado o NULL si no lo encuentra.
 */
void* arbol_buscar(abb_t* arbol, void* elemento){
	if((arbol_vacio(arbol)) || (elemento == NULL)){
		return NULL;
	}

    return arbol_buscar_recursiva(arbol->nodo_raiz, elemento, arbol->comparador);
}

/*
 * Devuelve el elemento almacenado como raiz o NULL si el árbol está
 * vacío.
 */
void* arbol_raiz(abb_t* arbol){
	if(arbol_vacio(arbol)){
		return NULL;
	}

	return (arbol->nodo_raiz->elemento);
}

int arbol_cantidad(abb_t* arbol){
    return (arbol->cantidad_elementos);
}

// Pre C.: Recibe punteros a la raíz del árbol y al array que se busca completar (junto con su tamaño).
//         Y además, una entero 'posición' inicializada en 0.
// Post C.: Recorre el árbol en el modo inorden de forma resuriva.
void arbol_recorrido_inorden_recursiva(nodo_abb_t* raiz, void** array, int* posicion_array, int tamanio_array){
    if((raiz == NULL) || (array == NULL)){
        return;
    }

    arbol_recorrido_inorden_recursiva(raiz->izquierda, array, posicion_array, tamanio_array);

    if((*posicion_array) < tamanio_array){
        array[*posicion_array] = raiz->elemento;
        (*posicion_array)++;
    }

    arbol_recorrido_inorden_recursiva(raiz->derecha, array, posicion_array, tamanio_array);
}

/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia inorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */
int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array){
    if((arbol_vacio(arbol)) || (array == NULL) || (tamanio_array <= 0)){
        return 0;
    }

    int posicion_array = 0;
    arbol_recorrido_inorden_recursiva(arbol->nodo_raiz, array, &posicion_array, tamanio_array);

    return posicion_array;
}

// Pre C.: Recibe punteros a la raíz del árbol y al array que se busca completar (junto con su tamaño).
//         Y además, una entero 'posición' inicializada en 0.
// Post C.: Recorre el árbol en el modo inorden de forma resuriva.
void arbol_recorrido_preorden_recursiva(nodo_abb_t* raiz, void** array, int* posicion_array, int tamanio_array){
    if((raiz == NULL) || (array == NULL)){
        return;
    }

    if((*posicion_array) < tamanio_array){
        array[*posicion_array] = raiz->elemento;
        (*posicion_array)++;
    }

    arbol_recorrido_preorden_recursiva(raiz->izquierda, array, posicion_array, tamanio_array);
    arbol_recorrido_preorden_recursiva(raiz->derecha, array, posicion_array, tamanio_array);
}

/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia preorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */
int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array){
    if((arbol_vacio(arbol)) || (array == NULL) || (tamanio_array <= 0)){
        return 0;
    }

    int posicion_array = 0;
    arbol_recorrido_preorden_recursiva(arbol->nodo_raiz, array, &posicion_array, tamanio_array);

    return posicion_array;
}

// Pre C.: Recibe punteros a la raíz del árbol y al array que se busca completar (junto con su tamaño).
//         Y además, una entero 'posición' inicializada en 0.
// Post C.: Recorre el árbol en el modo inorden de forma resuriva.
void arbol_recorrido_postorden_recursiva(nodo_abb_t* raiz, void** array, int* posicion_array, int tamanio_array){
    if((raiz == NULL) || (array == NULL)){
        return;
    }

    arbol_recorrido_postorden_recursiva(raiz->izquierda, array, posicion_array, tamanio_array);
    arbol_recorrido_postorden_recursiva(raiz->derecha, array, posicion_array, tamanio_array);

    if((*posicion_array) < tamanio_array){
        array[*posicion_array] = raiz->elemento;
        (*posicion_array)++;
    }
}

/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia postorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */
int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array){
    if((arbol_vacio(arbol)) || (array == NULL) || (tamanio_array <= 0)){
        return 0;
    }

    int posicion_array = 0;
    arbol_recorrido_postorden_recursiva(arbol->nodo_raiz, array, &posicion_array, tamanio_array);

    return posicion_array;
}

/*
 * Destruye el arbol liberando la memoria reservada por el mismo.
 * Adicionalmente invoca el destructor con cada elemento presente en
 * el arbol.
 */
void arbol_destruir(abb_t* arbol){
    if(arbol == NULL){
	    return;
	}
	while(!arbol_vacio(arbol)){
	    arbol_borrar(arbol, arbol->nodo_raiz->elemento);
	}

    free(arbol);
}



/* ******************************************************************************************************* */



// Nuevas funciones de los 3 recorridos para el iterador interno:

// Pre C.: Recibe punteros a la raíz y a la funcion booleana.
// Post C.: Recorre el árbol siguiendo el recorrido Inorden
void arbol_inorden(nodo_abb_t* raiz, bool (*funcion)(void*, void*), void* extra, bool* elemento_encontrado){
    if((raiz == NULL) || (funcion == NULL) || (*elemento_encontrado)){
        return;
    }

    if(!(*elemento_encontrado)){
        arbol_inorden(raiz->izquierda, funcion, extra, elemento_encontrado);
    }

    if(!(*elemento_encontrado)){
        if(funcion(raiz->elemento, extra)){
            (*elemento_encontrado) = true;
            return;
        }
    }

    if(!(*elemento_encontrado)){
        arbol_inorden(raiz->derecha, funcion, extra, elemento_encontrado);
    }
}

// Pre C.: Recibe punteros a la raíz y a la funcion booleana.
// Post C.: Recorre el árbol siguiendo el recorrido Preorden
void arbol_preorden(nodo_abb_t* raiz, bool (*funcion)(void*, void*), void* extra, bool* elemento_encontrado){
    if((raiz == NULL) || (funcion == NULL) || (*elemento_encontrado)){
        return;
    }

    if(!(*elemento_encontrado)){
        if(funcion(raiz->elemento, extra)){
            (*elemento_encontrado) = true;
            return;
        }
    }

    if(!(*elemento_encontrado)){
        arbol_preorden(raiz->izquierda, funcion, extra, elemento_encontrado);
    }
    if(!(*elemento_encontrado)){
        arbol_preorden(raiz->derecha, funcion, extra, elemento_encontrado);
    }
}

// Pre C.: Recibe punteros a la raíz y a la funcion booleana.
// Post C.: Recorre el árbol siguiendo el recorrido Postorden
void arbol_postorden(nodo_abb_t* raiz, bool (*funcion)(void*, void*), void* extra, bool* elemento_encontrado){
    if((raiz == NULL) || (funcion == NULL) || (*elemento_encontrado)){
        return;
    }

    if(!(*elemento_encontrado)){
        arbol_postorden(raiz->izquierda, funcion, extra, elemento_encontrado);
    }
    if(!(*elemento_encontrado)){
        arbol_postorden(raiz->derecha, funcion, extra, elemento_encontrado);
    }
    
    if(!(*elemento_encontrado)){
        if(funcion(raiz->elemento, extra)){
            (*elemento_encontrado) = true;
            return;
        }
    }
}

/*
 * Iterador interno. Recorre el arbol e invoca la funcion con cada
 * elemento del mismo. El puntero 'extra' se pasa como segundo
 * parámetro a la función. Si la función devuelve true, se finaliza el
 * recorrido aun si quedan elementos por recorrer. Si devuelve false
 * se sigue recorriendo mientras queden elementos.
 * El recorrido se realiza de acuerdo al recorrido solicitado.
 * Los recorridos válidos son:
 * ABB_RECORRER_INORDEN = 0;
 * ABB_RECORRER_PREORDEN = 1;
 * ABB_RECORRER_POSTORDEN = 2.
*/
void abb_con_cada_elemento(abb_t* arbol, int recorrido, bool (*funcion)(void*, void*), void* extra){
    if(arbol_vacio(arbol)){
        return;
    }

    bool elemento_encontrado = false;

    if(recorrido == ABB_RECORRER_INORDEN){
        arbol_inorden(arbol->nodo_raiz, funcion, extra, &elemento_encontrado);
    }
    else if(recorrido == ABB_RECORRER_PREORDEN){
        arbol_preorden(arbol->nodo_raiz, funcion, extra, &elemento_encontrado);
    }
    else if(recorrido == ABB_RECORRER_POSTORDEN){
        arbol_postorden(arbol->nodo_raiz, funcion, extra, &elemento_encontrado);
    }
    
    return;
}