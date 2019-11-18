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

void mostrar_introduccion(){
	printf("\t\t\t\t\t\t\t¡Bienvenido al simulador del Juego De Tronos!\n\n");
	printf("Aquí podrás inventar el paso del tiempo para conocer quién ocupa el Trono de Hierro luego de un lapso establecido...\n\n");
	printf("A continuación, podrás visualizar el menú de ayudas para guiarte a lo largo de este juego.\n\n");
	printf("Para indicar la acción que quieras desarrollar, deberás escribir una de las siguientes letras (mayúsculas):\n\n\n");
}

void mostrar_menu_ayudas(){
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

persona_t* crear_persona(char nombre_persona[MAX_NOMBRE], size_t edad){
	persona_t* persona = malloc(sizeof(persona_t));
    if(persona == NULL){
    	imprimir_mensaje_error(CREAR, "persona_t persona");
		return NULL;
    }

    strcpy(persona->nombre, nombre_persona);
    persona->edad = edad;

    return persona;
}

int agregar_personas(FILE* reino, lista_t* lista_personas, size_t* cant_personas){
	persona_t* persona = NULL;
	char codigo, nombre_persona[MAX_NOMBRE];
	size_t edad;

	int estado = EXITO;
	int persona_leida = fscanf(reino, FORMATO_PERSONA, &codigo, nombre_persona, &edad);
	while((persona_leida = CAMPOS_PERSONA) && (estado == EXITO)){
		persona = crear_persona(nombre_persona, edad);
		estado = lista_insertar(lista_personas, persona);

		persona_leida = fscanf(reino, FORMATO_PERSONA, &codigo, nombre_persona, &edad);
	}

	(*cant_personas) = lista_elementos(lista_personas);

	return estado;
}

casa_t* crear_casa(char nombre_casa[MAX_NOMBRE], size_t factor_env, size_t factor_nac, lista_destructor_t destructor){
    casa_t* casa = malloc(sizeof(casa_t));
    if(casa == NULL){
		imprimir_mensaje_error(CREAR, "casa_t casa");
		return NULL;
    }

    strcpy(casa->nombre, nombre_casa);
    casa->factor_env = factor_env;
    casa->factor_nac = factor_nac;
    casa->cant_personas = 0;
	casa->lista_personas = lista_crear(destructor);

	if((casa->lista_personas) == NULL){
		free(casa);
		imprimir_mensaje_error(CREAR, "lista_personas");
		return NULL;
	}

    return casa;
}

int agregar_casas(FILE* archivo, abb_t* arbol_casas, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	casa_t* casa = NULL;
	char codigo, nombre_casa[MAX_NOMBRE];
	size_t factor_env, factor_nac;

	int estado = EXITO;
	int casa_leida = fscanf(archivo, FORMATO_CASAS, &codigo, nombre_casa, &factor_env, &factor_nac);
	while((casa_leida == CAMPOS_CASA) && (estado == EXITO)){
		casa = arbol_buscar(arbol_casas, nombre_casa);
		if(casa == NULL){
			casa = crear_casa(nombre_casa, factor_env, factor_nac, destructor);
		}

		estado = agregar_personas(archivo, casa->lista_personas, &(casa->cant_personas));
		if(estado == EXITO){
    		casa->es_casa_extinta = false;
			arbol_insertar(arbol_casas, casa);
		}
		
		if((casa->cant_personas) > (*max_personas)){
			(*max_personas) = (casa->cant_personas);
			strcpy(casa_gobernadora, casa->nombre);
		}

		casa_leida = fscanf(archivo, FORMATO_CASAS, &codigo, nombre_casa, &factor_env, &factor_nac);
	}

	if(estado == ERROR){
		arbol_destruir(arbol_casas);
		return ERROR;
	}

	return EXITO;
}

int inicializar_reino(abb_t* arbol_casas, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	FILE* reino = fopen(F_CASAS, M_LECTURA);
	if(reino == NULL){
		imprimir_mensaje_error(ARCHIVO, F_CASAS);
		return ERROR;
	}

	int estado = agregar_casas(reino, arbol_casas, casa_gobernadora, max_personas, destructor);
	fclose(reino);

	return estado;
}

int agregar_nueva_casa(abb_t* arbol_casas, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	system("clear");

	char f_nombre[MAX_NOMBRE];
	printf("Ingrese el nombre del archivo donde se encuentra la casa a cargar\n");
	scanf("%s", f_nombre);

	FILE* nueva_casa = fopen(f_nombre, M_LECTURA);
	if(nueva_casa == NULL){
		imprimir_mensaje_error(ARCHIVO, f_nombre);
		return ERROR;
	}

	int estado = agregar_casas(nueva_casa, arbol_casas, casa_gobernadora, max_personas, destructor);

	return estado;
}

void generar_nombre(char nombre[MAX_NOMBRE]){
	FILE* nombres = fopen(F_NOMBRES, M_LECTURA);
	if(nombres == NULL){
		imprimir_mensaje_error(ARCHIVO, F_NOMBRES);
		return;
	}

	char nombre_en_archivo[MAX_NOMBRE];
	int random = rand() % CANT_NOMBRES;

	for(int i = 0; i < random; i++){
		fscanf(nombres, FORMATO_NOMBRES, nombre_en_archivo);
		nombre = nombre_en_archivo;
	}
}

void actualizar_casas_extintas(abb_t* arbol_casas, cola_t* casas_extintas, casa_t* casa, lista_destructor_t destructor){
	casa_t* casa_copia = crear_casa(strdup(casa->nombre), casa->factor_env, casa->factor_nac, destructor);
	
	if((casa->cant_personas) == 0){
		casa->es_casa_extinta = true;
		cola_encolar(casas_extintas, casa_copia);
		arbol_borrar(arbol_casas, casa);
	}
}

void actualizar_decesos(lista_t* lista_personas, size_t* posiciones_a_borrar, size_t tope_borrados){
	for(int i = 0; i < tope_borrados; i++){
		lista_borrar_de_posicion(lista_personas, posiciones_a_borrar[i]);
	}
}

void actualizar_casas(casa_t** casas, abb_t* arbol_casas, cola_t* casas_extintas, persona_t* persona, int tope_array, lista_destructor_t destructor){
	lista_iterador_t* iterador = NULL;
	size_t* posiciones_a_borrar = NULL;
	size_t contador_posicion = 0, tope_borrados = 0;

	for(size_t i = 0; i < tope_array; i++){
		iterador = lista_iterador_crear(casas[i]->lista_personas);
	
		while(lista_iterador_tiene_siguiente(iterador)){
			persona = lista_iterador_siguiente(iterador);
			
			persona->edad += 1 + (casas[i]->factor_env);
			if((persona->edad) >= MAX_VIDA){
				posiciones_a_borrar[tope_borrados] = contador_posicion;
				tope_borrados++;
			}
			contador_posicion++;
		}

		lista_iterador_destruir(iterador);
		actualizar_decesos(casas[i]->lista_personas, posiciones_a_borrar, tope_borrados);
		casas[i]->cant_personas = lista_elementos(casas[i]->lista_personas);
		actualizar_casas_extintas(arbol_casas, casas_extintas, casas[i], destructor);
	}
}

void actualizar_vector_casas(casa_t** casas, int* tope_array){
	int i = 0, j = 0;

	while(i < (*tope_array)){
		if(casas[i]->es_casa_extinta){
			for(j = i; j < (*tope_array)-1; j++){
				casas[j] = casas[j + 1];
			}
			(*tope_array)--;
			i--;
		}
		i++;
	}
}

void actualizar_nacimientos(casa_t** casas, int tope_array){
	persona_t* persona = NULL;
	char nombre[MAX_NOMBRE];
	size_t cantidad_a_nacer;
	
	for(int i = 0; i < tope_array; i++){
		cantidad_a_nacer = (casas[i]->cant_personas) / (casas[i]->factor_nac);
		for(int j = 0; j < cantidad_a_nacer; j++){
			generar_nombre(nombre);
			persona = crear_persona(nombre, 0);
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

int simular_tiempo(abb_t* arbol_casas, cola_t* casas_extintas, size_t anios_simulados, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	casa_t** casas = NULL;
	persona_t* persona = NULL;
	int tope_array = arbol_cantidad(arbol_casas);

	int elementos_recorridos = arbol_recorrido_inorden(arbol_casas, (void**)casas, tope_array);

	if(elementos_recorridos != tope_array){
		return ERROR;
	}

	for(size_t i = 0; i < anios_simulados; i++){
		actualizar_casas(casas, arbol_casas, casas_extintas, persona, tope_array, destructor);
		actualizar_vector_casas(casas, &tope_array);
		actualizar_nacimientos(casas, tope_array);
	}	
	
	determinar_casa_gobernadora(casas, casa_gobernadora, tope_array, max_personas);
	
	return EXITO;
}

int iniciar_simulacion(abb_t* arbol_casas, cola_t* casas_extintas, char casa_gobernadora[MAX_NOMBRE], size_t* max_personas, lista_destructor_t destructor){
	size_t anios_simulados;
	
	printf("Ingresar cantidad de años a transcurrir: \n");
	scanf("%zu", &anios_simulados);

	if(simular_tiempo(arbol_casas, casas_extintas, anios_simulados, casa_gobernadora, max_personas, destructor) == ERROR){
		printf("Hubo una falla al simular el tiempo solicitado, no se puede continuar.\n");
		return ERROR;
	}

	printf("Quien controla Trono de Hierro ahora es la casa %s\n", casa_gobernadora);

	return EXITO;
}

void swap(void* elemento_1, void* elemento_2){
	void* elemento_aux;

	elemento_aux = elemento_1;
	elemento_1 = elemento_2;
	elemento_2 = elemento_aux;
}

void ordenar_descentemente(casa_t** casas, int tope_array){
	bool esta_ordenado = false;
	int i = 0;

	while((i < tope_array) && (!esta_ordenado)){
		esta_ordenado = true;

		for(int j = 0; j < (tope_array - i); j++){
			if(casas[j] < casas[j+1]){
				esta_ordenado = false;
				swap(casas[j], casas[j+1]);
			}
		}
		i++;
	}
}

int listar_casas(abb_t* arbol_casas){
	casa_t** casas = NULL;
	int tope_array = arbol_cantidad(arbol_casas);
	int elementos_recorridos = arbol_recorrido_inorden(arbol_casas, (void**)casas, tope_array);

	if(elementos_recorridos != tope_array){
		return ERROR;
	}

	ordenar_descentemente(casas, tope_array);

	printf("Casas listadas por cantidad de integrantes (en orden descendente): \n");
	for(int i = 0; i < tope_array; i++){
		printf("Casa N° %i: %s\n", (i+1), (casas[i]->nombre));
	}

	return EXITO;
}

void mostrar_casas_extintas(cola_t* casas_extintas){
	// Prototipo (desencolando)
	int i = 1;
	casa_t* casa_aux = NULL;

	printf("Las casas extintas (en el orden en que fueron desapareciendo) son: \n\n");

	while(!cola_vacia(casas_extintas)){
		casa_aux = cola_primero(casas_extintas);
		printf("Casa N° %i: %s\n", i, (casa_aux->nombre));
		cola_desencolar(casas_extintas);
		i++;
	}
}

void terminar_simulacion(abb_t* arbol_casas, cola_t* casas_extintas){
	casa_t** casas = NULL;
	int tope_array = arbol_cantidad(arbol_casas);

	arbol_recorrido_inorden(arbol_casas, (void**)casas, tope_array);
	
	for(int i = 0; i < tope_array; i++){
		lista_destruir(casas[i]->lista_personas);
	}

	arbol_destruir(arbol_casas);
	cola_destruir(casas_extintas);

	system("clear");
	centrar_mensaje("Simulación terminada", "");
}