#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "abb.h"
#include "lista.h"
#include "cola.h"
#include "trono.h"

// Pre C.:  Recibe el caracter ingresado por el usario.
// Post C.: Devuelve 'true' solo si el caracter corresponde a una de las acciones válidas establecidas según el menú de ayudas.
bool es_comando_valido(char accion){
	return ((accion == INICIAR) || (accion == AGREGAR) || (accion == LISTAR) || (accion == MOSTRAR) || (accion == AYUDAS));
}

int main(int argc, char *argv[]){
	srand((unsigned int)time(NULL));

	reino_t* reino = inicializar_reino();
	mostrar_menu_ayudas();
	
	int estado = EXITO;
	char accion;

	printf("Ingrese una letra: ");
	scanf(" %c", &accion);

	while((accion != FINALIZAR) && (estado == EXITO)){
		switch(accion){
			case INICIAR: estado = iniciar_simulacion(reino);
				break;

			case AGREGAR: estado = agregar_casas(reino->arbol_casas);
				break;

			case LISTAR: estado = listar_casas(reino->arbol_casas);
				break;

			case MOSTRAR: mostrar_casas_extintas(reino->casas_extintas);
				break;

			case AYUDAS: mostrar_menu_ayudas();
				break;
			
			default: printf("\nPor favor, ingrese un comando válido: ");
				break;
		}

		if(es_comando_valido(accion)){
			printf("Ingrese su próxima acción a realizar o 'Q' para finalizar;\n");
			printf("si desea mostrar el menú de ayudas, ingrese 'H': ");
		}
		
		scanf(" %c", &accion);
	}

	if(estado == ERROR){
		printf("Hubo un error al ejecutar el comando especificado, no se puede continuar.\n");
	}

	terminar_simulacion(reino);
	
	return 0;
}