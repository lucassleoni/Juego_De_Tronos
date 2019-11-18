#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "abb.h"
#include "lista.h"
#include "cola.h"
#include "trono.h"

int comparar_elementos(void* elemento_1, void* elemento_2){
    if((elemento_1 == NULL) || (elemento_2 == NULL)){
		return ERROR;
    }
    
    if(strcmp((((casa_t*)elemento_1)->nombre), (((casa_t*)elemento_2)->nombre)) > 0){
		return PRIMERO_ES_MAYOR;
    }
    else if(strcmp((((casa_t*)elemento_1)->nombre), (((casa_t*)elemento_2)->nombre)) < 0){
		return PRIMERO_ES_MENOR;
    }

    return IGUALES;
}

void destruir_elemento(void* elemento){
    if(elemento != NULL){
    	free(elemento);
    }
}

bool es_comando_valido(char accion){
	return ((accion == INICIAR) || (accion == AGREGAR) || (accion == LISTAR) || (accion == MOSTRAR) || (accion == AYUDAS));
}

void determinar_accion(abb_t* arbol_casas, cola_t* casas_extintas, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	int estado = EXITO;
	char accion;
	printf("Ingrese una letra: ");
	scanf("%c", &accion);

	while((accion != FINALIZAR) && (estado == EXITO)){
		switch(accion){
			case INICIAR: estado = iniciar_simulacion(arbol_casas, casas_extintas, casa_gobernadora, max_personas, destructor);
				break;

			case AGREGAR: estado = agregar_nueva_casa(arbol_casas, casa_gobernadora, max_personas, destructor);
				break;

			case LISTAR: estado = listar_casas(arbol_casas);
				break;

			case MOSTRAR: mostrar_casas_extintas(casas_extintas);
				break;

			case AYUDAS: mostrar_menu_ayudas();
				break;

			default: printf("Por favor, ingrese un comando válido: ");
		}
		if(es_comando_valido(accion)){
			printf("Ingrese su próxima acción a realizar o 'Q' para finalizar.\n");
			printf("Si desea mostrar el menú de ayudas, ingrese 'H': ");
		}
		scanf("%c", &accion);
	}

	if(estado == ERROR){
		printf("Hubo un error al ejecutar el comando especificado, no se puede continuar.\n");
	}
}


int main(int argc, char *argv[]){
	system("clear");
	srand((unsigned int)time(NULL));

	size_t max_personas = 0;
	char casa_gobernadora[MAX_NOMBRE];
	
	abb_t* arbol_casas = arbol_crear(comparar_elementos, destruir_elemento);
	if(arbol_casas == NULL){
		imprimir_mensaje_error("crear", "arbol_casas");
		return ERROR;
	}

	cola_t* casas_extintas = cola_crear(destruir_elemento);
	if(casas_extintas == NULL){
		arbol_destruir(arbol_casas);
		imprimir_mensaje_error("crear", "casas_extintas");
		return ERROR;
	}

	if(inicializar_reino(arbol_casas, casa_gobernadora, &max_personas, destruir_elemento) == EXITO){
		mostrar_introduccion();
		mostrar_menu_ayudas();

		determinar_accion(arbol_casas, casas_extintas, casa_gobernadora, &max_personas, destruir_elemento);

		terminar_simulacion(arbol_casas, casas_extintas);
	}
	else{
		centrar_mensaje("Hubo un error al inicializar el reino", "  No se pudo comenzar la simulación");
	}
	
	return 0;
}