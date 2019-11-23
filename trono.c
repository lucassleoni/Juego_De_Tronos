#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "abb.h"
#include "lista.h"
#include "cola.h"
#include "trono.h"

extern char* strdup(const char*);

void centrar_mensaje(char mensaje_1[], char mensaje_2[]){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t %s \n\t\t\t\t\t\t\t\t\t %s \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", mensaje_1, mensaje_2);
}

void mostrar_menu_ayudas(){
	//system("clear");
	printf("\t\t\t\t\t\t\t¡Bienvenido al simulador del Juego De Tronos!\n\n");
	printf("Aquí podrás inventar el paso del tiempo para conocer quién ocupa el Trono de Hierro luego de un lapso establecido...\n\n");
	printf("A continuación, podrás visualizar el menú de ayudas para guiarte a lo largo de este juego.\n\n");
	printf("Para indicar la acción que quieras desarrollar, deberás escribir una de las siguientes letras (mayúsculas):\n\n\n");
	printf("\t - 'S': Iniciará la simulación (para la que deberás indicar la cantidad de años a simular).\n");
	printf("\t\tFinalmente, sabrás quién ocupa el Trono de Hierro al cabo de los años transcurridos.\n\n");
	printf("\t - 'A': Podrás agregar una casa al árbol de casas existente (explicitando el nombre del archivo en\n\t\tel que se encuentra la información de la casa a cargar).\n");
	printf("\t\tEn caso de ya existir la casa solicitada, se sumará el número de integrantes de la misma.\n\n");
	printf("\t - 'L': Se listarán las casas por cantidad de integrantes de forma descendente.\n\n");
	printf("\t - 'E': Mostrará las casas extintas (en el orden en que fueron desapareciendo).\n\n");
	printf("\t - 'Q': Finalizará la ejecución del simulador.\n\n");
}

void imprimir_mensaje_error(char error[MAX_NOMBRE], char nombre[MAX_NOMBRE]){
	if(strcmp(error, ARCHIVO) == 0){
		printf("Hubo un error al abrir el archivo '%s'\n", nombre);
	}
	else if(strcmp(error, CREAR) == 0){
		printf("Hubo un error al crear la estructura '%s'\n", nombre);
	}

	printf("Finalizando programa...\n");
}

reino_t* inicializar_reino(){
	reino_t* reino = malloc(sizeof(reino_t));
	if(reino == NULL){
		centrar_mensaje(" Hubo un error al crear el reino", "No se pudo comenzar la simulación");
		return NULL;
	}

	reino->arbol_casas = arbol_crear(comparar_elementos, destruir_casa);
	if((reino->arbol_casas) == NULL){
		free(reino);
		centrar_mensaje(" Hubo un error al crear el reino", "No se pudo comenzar la simulación");
		return NULL;
	}

	reino->casas_extintas = cola_crear(destruir_casa);
	if((reino->casas_extintas) == NULL){ 
		arbol_destruir(reino->arbol_casas);
		free(reino);
		centrar_mensaje(" Hubo un error al crear el reino", "No se pudo comenzar la simulación");
		return NULL;
	}

	reino->casa_gobernadora = "Ninguna";

	return reino;
}


persona_t* crear_persona(char* nombre_persona, size_t edad){
	persona_t* persona = malloc(sizeof(persona_t));
    if(persona == NULL){
    	imprimir_mensaje_error(CREAR, "persona_t persona");
		return NULL;
    }

    persona->nombre = nombre_persona;
    persona->edad = edad;

    return persona;
}

int agregar_persona(FILE* reino, char* nombre_persona, size_t edad, lista_t* lista_personas, size_t* cant_personas){
	int estado = EXITO;
	persona_t* persona = crear_persona(strdup(nombre_persona), edad);
	if(persona == NULL){
		return ERROR;
	}

	estado = lista_insertar(lista_personas, (void*)persona);
	(*cant_personas) = lista_elementos(lista_personas);

	return estado;

}

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

void destruir_casa(void* casa){
	if(casa != NULL){
		free(((casa_t*)casa)->nombre);
		lista_destruir(((casa_t*)casa)->lista_personas);
		free(((casa_t*)casa));
	}
}

void destruir_persona(void* persona){
    if(persona != NULL){
    	free(((persona_t*)persona)->nombre);
    	free((persona_t*)persona);
    }
}

casa_t* crear_casa(char* nombre_casa, size_t factor_env, size_t factor_nac){
    casa_t* casa = malloc(sizeof(casa_t));
    if(casa == NULL){
		imprimir_mensaje_error(CREAR, "casa_t casa");
		return NULL;
    }

    casa->nombre = nombre_casa;
    casa->factor_env = factor_env;
    casa->factor_nac = factor_nac;
    casa->cant_personas = 0;
	casa->lista_personas = lista_crear(destruir_persona);

	if((casa->lista_personas) == NULL){
		free(casa);
		imprimir_mensaje_error(CREAR, "lista_personas");
		return NULL;
	}

    return casa;
}

int insertar_casas(FILE* archivo, abb_t* arbol_casas){
	casa_t* casa = NULL;
	char codigo;
	char nombre_casa[MAX_NOMBRE], nombre_persona[MAX_NOMBRE];
	size_t edad, factor_env, factor_nac;

	int estado = EXITO;

	int leido = fscanf(archivo, "%c;", &codigo);
	while((leido == 1) && (estado == EXITO)){
		if(codigo == C_CASA){
			fscanf(archivo, FORMATO_CASAS, nombre_casa, &factor_env, &factor_nac);
			casa = crear_casa(strdup(nombre_casa), factor_env, factor_nac);
			casa_t* casa_buscada = arbol_buscar(arbol_casas, casa);

			if(casa_buscada == NULL){
				casa->es_casa_extinta = false;
				arbol_insertar(arbol_casas, casa);
			}
			else{
				destruir_casa(casa);
				casa = casa_buscada;
			}

		}
		else if(codigo == C_PERSONA){
			fscanf(archivo, FORMATO_PERSONA, nombre_persona, &edad);
			estado = agregar_persona(archivo, nombre_persona, edad, casa->lista_personas, &(casa->cant_personas));
		}

		leido = fscanf(archivo, "%c;", &codigo);
	}

	if(estado == ERROR){
		arbol_destruir(arbol_casas);
		printf("No se pudo agregar una casa. Destruyendo árbol...\n");
		return ERROR;
	}

	return EXITO;
}

int agregar_casas(abb_t* arbol_casas){
	//system("clear");

	char f_nombre[MAX_NOMBRE] = "casas.txt";
	/*
	printf("Ingrese el nombre del archivo donde se encuentra la casa a cargar: ");
	scanf("%s", f_nombre);
	*/

	FILE* f_casas = fopen(f_nombre, M_LECTURA);
	if(f_casas == NULL){
		imprimir_mensaje_error(ARCHIVO, f_nombre);
		return ERROR;
	}

	int estado = insertar_casas(f_casas, arbol_casas);

	fclose(f_casas);

	return estado;
}

void actualizar_casas_extintas(cola_t* casas_extintas, casa_t* casa){
	casa_t* casa_copia = crear_casa(strdup(casa->nombre), casa->factor_env, casa->factor_nac);
	
	if((casa->cant_personas) == 0){
		casa->es_casa_extinta = true;
		cola_encolar(casas_extintas, casa_copia);
	}
}

void actualizar_decesos(lista_t* lista_personas, size_t* posiciones_a_borrar, size_t tope_borrados){
	for(int i = 0; i < tope_borrados; i++){
		lista_borrar_de_posicion(lista_personas, posiciones_a_borrar[i]);
	}
}

void actualizar_personas(lista_t* lista_personas, size_t factor_env){
	size_t contador_posicion = 0, tope_borrados = 0;
	size_t posiciones_a_borrar[lista_elementos(lista_personas)];
	lista_iterador_t* iterador = lista_iterador_crear(lista_personas);
	
	while(lista_iterador_tiene_siguiente(iterador)){
		persona_t* persona = lista_iterador_siguiente(iterador);
		
		persona->edad += factor_env;
		if((persona->edad) >= MAX_VIDA){
			posiciones_a_borrar[tope_borrados] = contador_posicion;
			tope_borrados++;
		}
		contador_posicion++;
	}

	lista_iterador_destruir(iterador);
	actualizar_decesos(lista_personas, posiciones_a_borrar, tope_borrados);
}

void actualizar_casas(reino_t* reino, casa_t** casas, int tope_array){
	for(size_t i = 0; i < tope_array; i++){
		actualizar_personas(casas[i]->lista_personas, casas[i]->factor_env);
		casas[i]->cant_personas = lista_elementos(casas[i]->lista_personas);
		actualizar_casas_extintas(reino->casas_extintas, casas[i]);
	}
}

void actualizar_vector_casas(abb_t* arbol_casas, casa_t** casas, int* tope_array){
	int i = 0, j = 0;

	while(i < (*tope_array)){
		if(casas[i]->es_casa_extinta){
			arbol_borrar(arbol_casas, casas[i]);
			
			for(j = i; j < (*tope_array)-1; j++){
				casas[j] = casas[j + 1];
			}
			(*tope_array)--;
			i--;
		}
		i++;
	}
}

char* generar_nombre(){
	FILE* nombres = fopen(F_NOMBRES, M_LECTURA);
	if(nombres == NULL){
		imprimir_mensaje_error(ARCHIVO, F_NOMBRES);
		return NULL;
	}

	char* nombre_en_archivo;
	int random = rand() % CANT_NOMBRES;
	
	while(random > 0){
		fscanf(nombres, FORMATO_NOMBRES, nombre_en_archivo);
		random--;
	}
	
	fclose(nombres);

	return nombre_en_archivo;
}

void actualizar_nacimientos(casa_t** casas, int tope_array){
	//char* nombre;
	size_t cantidad_a_nacer;
	
	for(int i = 0; i < tope_array; i++){
		cantidad_a_nacer = (casas[i]->cant_personas) / (casas[i]->factor_nac);
		for(int j = 0; j < cantidad_a_nacer; j++){
			//nombre = generar_nombre();
			
				persona_t* persona = crear_persona(strdup("Klaus"), 0);
				lista_insertar(casas[i]->lista_personas, persona);
		}
	}
}

void determinar_casa_gobernadora(casa_t** casas, char casa_gobernadora[MAX_NOMBRE], int tope_array, size_t* max_personas){
	for(int i = 0; i < tope_array; i++){
		if((casas[i]->cant_personas) > (*max_personas)){
			(*max_personas) = (casas[i]->cant_personas);
			strcpy(casa_gobernadora, casas[i]->nombre);
		}
	}
}

int simular_tiempo(reino_t* reino, size_t anios_simulados){
	int tope_array = arbol_cantidad(reino->arbol_casas);
	casa_t* casas[tope_array];
	
	int elementos_recorridos = arbol_recorrido_inorden(reino->arbol_casas, (void**)casas, tope_array);
	if(elementos_recorridos != tope_array){
		return ERROR;
	}

	for(size_t i = 0; i < anios_simulados; i++){
		actualizar_casas(reino, casas, tope_array);
		actualizar_vector_casas(reino->arbol_casas, casas, &tope_array);
		actualizar_nacimientos(casas, tope_array);
	}

	size_t max_personas = 0;
	determinar_casa_gobernadora(casas, reino->casa_gobernadora, tope_array, &max_personas);
	
	return EXITO;
}

int iniciar_simulacion(reino_t* reino){
	if(arbol_vacio(reino->arbol_casas)){
		//system("clear");
		printf("\n\n\n\n\n\nNo hay ninguna casa presente en el reino.\n\n\n\n\n\n");
	}

	else{
		size_t anios_simulados = 0;
		
		printf("Ingresar cantidad de años a transcurrir: ");
		scanf("%zu", &anios_simulados);

		if(simular_tiempo(reino, anios_simulados) == ERROR){
			printf("Hubo una falla al simular el tiempo solicitado, no se puede continuar.\n");
			return ERROR;
		}

		if(arbol_cantidad(reino->arbol_casas)){
			//system("clear");
			printf("\nQuien controla Trono de Hierro ahora es la casa %s\n\n", reino->casa_gobernadora);
		}
		else{
			//system("clear");
			printf("\n\n\n\n\n\nNo hay ninguna casa presente en el reino.\n\n\n\n\n\n");
		}
	}

	return EXITO;
}

void ordenar_descentemente(casa_t** casas, int tope_array){
	bool esta_ordenado = false;

	for(int i = 0; i < tope_array; i++){
		int max_integrantes = casas[i]->cant_personas;
		int posicion_swap = -1;
		
		for(int j = i + 1; j < tope_array; j++){
			if(casas[j]->cant_personas > max_integrantes){
				max_integrantes = casas[j]->cant_personas;
				posicion_swap = j;
			}
		}

		if(posicion_swap != -1){
			casa_t* temp = casas[i];
			casas[i] = casas[posicion_swap];
			casas[posicion_swap] = temp;
		}
	}
}

int listar_casas(abb_t* arbol_casas){
	if(arbol_vacio(arbol_casas)){
		//system("clear");
		printf("\n\n\n\n\n\nNo hay ninguna casa presente en el reino.\n\n\n\n\n\n");
	}

	else{
		casa_t* casas[arbol_cantidad(arbol_casas)];
		int tope_array = arbol_cantidad(arbol_casas);
		int elementos_recorridos = arbol_recorrido_inorden(arbol_casas, (void**)casas, tope_array);

		if(elementos_recorridos != tope_array){
			return ERROR;
		}
		ordenar_descentemente(casas, tope_array);
		printf("\n\nCasas listadas por cantidad de integrantes (en orden descendente): \n");
		for(int i = 0; i < tope_array; i++){
			printf("Casa N° %i: %s (%zu integrantes)\n", (i+1), (casas[i]->nombre), (casas[i]->cant_personas));
		}
	}

	return EXITO;
}

void mostrar_casas_extintas(cola_t* casas_extintas){
	if(cola_vacia(casas_extintas)){
		//system("clear");
		printf("\n\n\n\n\n\nNo hay casas extintas por listar.\n\n\n\n\n\n");
	}
	else{
		int i = 1;
		int cant_casas = cola_cantidad(casas_extintas);

		printf("\n\nLas casas extintas (en el orden en que fueron desapareciendo) son: \n\n");

		while(cant_casas > 0){
			casa_t* casa_aux = cola_primero(casas_extintas);
			casa_t* casa_copia = crear_casa(strdup(casa_aux->nombre), casa_aux->factor_env, casa_aux->factor_nac);
			printf("Casa N° %i: %s\n", i, (casa_aux->nombre));
			cola_encolar(casas_extintas, casa_copia);
			cola_desencolar(casas_extintas);
			i++;
			cant_casas--;
		}
	}
}

void terminar_simulacion(reino_t* reino){
	arbol_destruir(reino->arbol_casas);
	cola_destruir(reino->casas_extintas);
	free(reino);

	//centrar_mensaje("Simulación terminada", "");
	printf("\nSimulación terminada\n\n");
}