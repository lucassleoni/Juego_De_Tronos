#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "abb.h"
#include "lista.h"
#include "cola.h"
#include "trono.h"

int main(int argc, char *argv[]){
	srand((unsigned int)time(NULL));
	reino_t* reino = inicializar_reino();
	
	if(determinar_accion(reino) == ERROR){
		printf("Hubo un error al ejecutar el comando especificado, no se puede continuar.\n");
	}

	terminar_simulacion(reino);
	
	return 0;
}