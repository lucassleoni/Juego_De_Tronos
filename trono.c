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

// Pre C.:  Recibe dos strings.
// Post C.: Los imprime en el centro de la pantalla, dejando visible únicamente el mensaje a transmitir.
void centrar_mensaje(char mensaje_1[], char mensaje_2[]){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\t\t\t\t %s \n\t\t\t\t\t\t\t\t\t %s \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", mensaje_1, mensaje_2);
}

// Pre C.:  El reino debe haber sido correctamente inicializado para que esta función sea invocada.
// Post C.: Imprime el menú de ayudas/explicación del juego.
void mostrar_menu_ayudas(){
	system("clear");
	printf("\t\t\t\t\t\t\t¡Bienvenido al simulador del Juego De Tronos!\n\n");
	printf("Aquí podrás inventar el paso del tiempo en el reino para conocer quién ocupa el Trono de Hierro luego de un lapso establecido...\n\n");
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

// Pre C.:  Recibe dos strings.
// Post C.: Imprime un mensaje de error, según los parámetros recibidos.
void imprimir_mensaje_error(char error[MAX_NOMBRE], char nombre[MAX_NOMBRE]){
	if(strcmp(error, ARCHIVO) == 0){
		printf("\n\nHubo un error al abrir el archivo '%s'\n", nombre);
		printf("No se cargaron las casas.\n\n\n");
	}
	else if(strcmp(error, CREAR) == 0){
		printf("Hubo un error al crear la estructura '%s'\n", nombre);
	}
}

// Pre C.:  Recibe dos punteros a elementos (casas, en este caso).
// Post C.: Compara las casas por nombre y devuelve un código según si la primera es mayor, menor o igual a la segunda.
int comparar_casas(void* casa_1, void* casa_2){
    if((casa_1 == NULL) || (casa_2 == NULL)){
		return ERROR;
    }
    
    if(strcmp((((casa_t*)casa_1)->nombre), (((casa_t*)casa_2)->nombre)) > 0){
		return PRIMERO_ES_MAYOR;
    }
    else if(strcmp((((casa_t*)casa_1)->nombre), (((casa_t*)casa_2)->nombre)) < 0){
		return PRIMERO_ES_MENOR;
    }

    return IGUALES;
}

// Pre C.:  Recibe un puntero a una casa.
// Post C.: La destruye, liberando todos los campos que esta ocupa en el Heap.
void destruir_casa(void* casa){
	if(casa != NULL){
		free(((casa_t*)casa)->nombre);
		lista_destruir(((casa_t*)casa)->lista_personas);
		free(((casa_t*)casa));
	}
}

// Pre C.:  Recibe un puntero a una persona.
// Post C.: La destruye, liberando todos los campos que esta ocupa en el Heap.
void destruir_persona(void* persona){
    if(persona != NULL){
    	free(((persona_t*)persona)->nombre);
    	free((persona_t*)persona);
    }
}

// Pre C.:  ---
// Post C.: Crea la estructura del reino (ABB y cola de casas extintas) e inicializa el nombre de la casa gobernador en NULL.
// 			Devuelve un puntero a la estructura creada o NULL en caso de error.
reino_t* inicializar_reino(){
	reino_t* reino = malloc(sizeof(reino_t));
	if(reino == NULL){
		centrar_mensaje(" Hubo un error al crear el reino", "No se pudo comenzar la simulación");
		return NULL;
	}

	reino->arbol_casas = arbol_crear(comparar_casas, destruir_casa);
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

	reino->casa_gobernadora = NULL;

	return reino;
}

// Pre C.:  Recibe los datos de una persona.
// Post C.: Crea la estructura de una persona con los datos recibidos.
// 			Devuelve un puntero a la estructura creada o NULL en caso de error.
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

// Pre C.:  Recibe una lista de personas con los datos del integrante, y la cantidad de personas de la lista.
// Post C.: Inserta la nueva persona en la lista y devuelve el código de éxito '0' o error '-1' al finalizar.
int agregar_persona(lista_t* lista_personas, char* nombre_persona, size_t edad, size_t* cant_personas){
	if(lista_personas == NULL){
		return ERROR;
	}

	int estado = EXITO;
	persona_t* persona = crear_persona(strdup(nombre_persona), edad);
	if(persona == NULL){
		return ERROR;
	}

	estado = lista_insertar(lista_personas, (void*)persona);
	(*cant_personas) = lista_elementos(lista_personas);

	return estado;
}

// Pre C.:  Recibe los datos de una casa.
// Post C.: Crea la estructura de una casa con los datos recibidos.
// 			Devuelve un puntero a la estructura creada o NULL en caso de error.
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

// Pre C.:  Recibe un puntero a un archivo y a un ABB de casas.
// Post C.: Lee el archivo y ejecuta la acción correspondiente en caso de haber leído una casa o un indiviuo.
//			Inserta las casas en un ABB y las personas en la lista de la casa a la que pertenece.
//			Devuelve el código de éxito '0' o error '-1' al finalizar.
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
			estado = agregar_persona(casa->lista_personas, nombre_persona, edad, &(casa->cant_personas));
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

// Pre C.:  Recibe un puntero a un ABB.
// Post C.: Abre el archivo de casas/personas e invoca a la funcion que los inserta en el ABB.
//			Devuelve el código de éxito '0' o error '-1' al finalizar.
int agregar_casas(abb_t* arbol_casas){
	system("clear");

	char f_nombre[MAX_NOMBRE];
	printf("Ingrese el nombre del archivo donde se encuentra la casa a cargar: ");
	scanf("%s", f_nombre);
	
	FILE* f_casas = fopen(f_nombre, M_LECTURA);
	if(f_casas == NULL){
		imprimir_mensaje_error(ARCHIVO, f_nombre);
		return ERROR;
	}

	int estado = insertar_casas(f_casas, arbol_casas);

	if(estado == EXITO){
		printf("¡Archivo cargado con éxito!\n\n");
	}

	fclose(f_casas);

	return estado;
}

// Pre C.:  Recibe punteros a la cola de casas extintas y a una casa.
// Post C.: En caso de recibir una casa sin integrantes, la marca como 'casa extinta' y encola una copia de la misma.
void actualizar_casas_extintas(cola_t* casas_extintas, casa_t* casa){
	if((casa->cant_personas) == 0){
		casa->es_casa_extinta = true;
		casa_t* casa_copia = crear_casa(strdup(casa->nombre), casa->factor_env, casa->factor_nac);
		cola_encolar(casas_extintas, casa_copia);
	}
}

// Pre C.:  Recibe un puntero a una lista de personas y un vector de posiciones a borrar (junto con su tope).
// Post C.: Borra de la lista los elementos de las posiciones que posee el vector.
void actualizar_decesos(lista_t* lista_personas, size_t* posiciones_a_borrar, size_t tope_borrados){
	for(int i = 0; i < tope_borrados; i++){
		lista_borrar_de_posicion(lista_personas, posiciones_a_borrar[i] - i);
	}
}

// Pre C.:  Recibe un puntero a una lista de personas y un factor de envejecimiento.
// Post C.: Suma la edad correspondiente a cada persona, crea un vector con las posiciones de las que
//			superan la edad máxima establecida e invoca a la función que los elimina de la lista.
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

// Pre C.:  Recibe un puntero a la cola de casas extinstas y al vector de casas junto con su tope.
// Post C.: Invoca a la funciones que actualizan la lista de personas de cada casa y las casas extintas.
//			Actualiza también la cantidad de personas de cada casa.
void actualizar_casas(cola_t* casas_extintas, casa_t** casas, int tope_array){
	for(size_t i = 0; i < tope_array; i++){
		actualizar_personas(casas[i]->lista_personas, casas[i]->factor_env);
		casas[i]->cant_personas = lista_elementos(casas[i]->lista_personas);
		actualizar_casas_extintas(casas_extintas, casas[i]);
	}
}

// Pre C.:  Recibe un puntero al ABB y al vector de casas (junto con su tope).
// Post C.: Borra las casas extintas del vector y del ABB, modificando el tope correctamente.
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

// Pre C.:  Recibe un vector de nombres vacío.
// Post C.: Abre un archivo de nombres y llena el vector con todos los que encuentre, devolviendo una posicion random del vector.
int generar_nombre(char nombres[CANT_NOMBRES][MAX_NOMBRE]){
	FILE* f_nombres = fopen(F_NOMBRES, M_LECTURA);
	if(nombres == NULL){
		imprimir_mensaje_error(ARCHIVO, F_NOMBRES);
		return ERROR;
	}

	for(int i = 0; i < CANT_NOMBRES; i++){
		fscanf(f_nombres, FORMATO_NOMBRES, nombres[i]);
	}
	
	fclose(f_nombres);
	
	return (rand() % CANT_NOMBRES);
}

// Pre C.:  Recibe un vector de casas junto con su tope.
// Post C.: Crea e inserta tantas personas (con un nombre aleatorio) como la cantidad a nacer indica.
void actualizar_nacimientos(casa_t** casas, int tope_array){
	char nombres[CANT_NOMBRES][MAX_NOMBRE];
	size_t cantidad_a_nacer;
	
	for(int i = 0; i < tope_array; i++){
		cantidad_a_nacer = (casas[i]->cant_personas) / (casas[i]->factor_nac);

		for(int j = 0; j < cantidad_a_nacer; j++){
			int pos_nombre = generar_nombre(nombres);
			persona_t* persona = crear_persona(strdup(nombres[pos_nombre]), 0);
			lista_insertar(casas[i]->lista_personas, persona);
		}
	}
}

// Pre C.:  Recibe un vector de casas junto con su tope y el nombre de la casa gobernadora (por referencia)
// Post C.: Modifica el nombre recibido con el de la casa que posea más integrantes.
void determinar_casa_gobernadora(casa_t** casas, char** casa_gobernadora, int tope_array){
	size_t max_personas = 0;

	for(int i = 0; i < tope_array; i++){
		if((casas[i]->cant_personas) > max_personas){
			max_personas = (casas[i]->cant_personas);
			(*casa_gobernadora) = casas[i]->nombre;
		}
		casas[i]->cant_personas = lista_elementos(casas[i]->lista_personas);
	}
}

// Pre C.:  Recibe un puntero a la estructura del reino y la cantidad de años simulados por el usuario.
// Post C.: Crea un vector con las casas presentes en el ABB e invoca (una vez por cada año) a las funciones
//			encargadas de actualizar el estado del reino (casas y casas extintas) para finalmente determinar la
//			casa gobernadora al cabo de los años simulados. Devuelve el código de éxito '0' o error '-1' al finalizar.
int simular_tiempo(reino_t* reino, size_t anios_simulados){
	int tope_array = arbol_cantidad(reino->arbol_casas);
	casa_t* casas[tope_array];
	
	int elementos_recorridos = arbol_recorrido_inorden(reino->arbol_casas, (void**)casas, tope_array);
	if(elementos_recorridos != tope_array){
		return ERROR;
	}

	for(size_t i = 0; i < anios_simulados; i++){
		actualizar_casas(reino->casas_extintas, casas, tope_array);
		actualizar_vector_casas(reino->arbol_casas, casas, &tope_array);
		actualizar_nacimientos(casas, tope_array);
	}

	determinar_casa_gobernadora(casas, &(reino->casa_gobernadora), tope_array);

	return EXITO;
}

// Pre C.:  Recibe un puntero a la estructura del reino.
// Post C.: Pide al usuario la cantidad de años a simular y simula dicho tiempo.
//			Imprime por pantalla la casa que gobierna el reino luego de los años simulados.
int iniciar_simulacion(reino_t* reino){
	if(arbol_vacio(reino->arbol_casas)){
		system("clear");
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
			system("clear");
			printf("\nQuien controla Trono de Hierro ahora es la casa %s\n\n", (reino->casa_gobernadora));
		}
		else{
			system("clear");
			printf("\n\n\n\n\n\nNo hay ninguna casa presente en el reino.\n\n\n\n\n\n");
		}
	}

	return EXITO;
}

// Pre C.:  Recibe un vector de casas junto con su tope.
// Post C.: Lo ordena descentemente según la cantidad de integrante de cada una.
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

// Pre C.:  Recibe un puntero a un ABB.
// Post C.: Imprime por pantalla una lista de la casas por orden descendente de cantidad de integrantes.
int listar_casas(abb_t* arbol_casas){
	if(arbol_vacio(arbol_casas)){
		system("clear");
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
			printf("Casa N° %i: %s (%zu integrantes)\n", (i+1), (casas[i]->nombre), casas[i]->cant_personas);
		}

		printf("\n");
	}

	return EXITO;
}

// Pre C.:  Recibe un puntero a la cola de casas extintas.
// Post C.: Imprime por pantalla las casas extintas en el orden en que fueron desapareciendo.
void mostrar_casas_extintas(cola_t* casas_extintas){
	if(cola_vacia(casas_extintas)){
		system("clear");
		printf("\n\n\n\n\n\nNo hay casas extintas por listar.\n\n\n\n\n\n");
	}
	else{
		int i = 1;
		int cant_casas = cola_cantidad(casas_extintas);

		printf("\nLas casas extintas (en el orden en que fueron desapareciendo) son: \n\n");

		while(cant_casas > 0){
			casa_t* casa_aux = cola_primero(casas_extintas);
			casa_t* casa_copia = crear_casa(strdup(casa_aux->nombre), casa_aux->factor_env, casa_aux->factor_nac);
			printf("Casa N° %i: %s\n", i, (casa_aux->nombre));
			cola_encolar(casas_extintas, casa_copia);
			cola_desencolar(casas_extintas);
			i++;
			cant_casas--;
		}

		printf("\n");
	}
}

// Pre C.:  Recibe un puntero a la estructura del reino.
// Post C.: Libera toda la memeoria ocupada por la el juego, imprimiendo por pantalla que la simulación ha terminado.
void terminar_simulacion(reino_t* reino){
	if(reino != NULL){
		arbol_destruir(reino->arbol_casas);
		cola_destruir(reino->casas_extintas);
		free(reino);
	}

	centrar_mensaje("Simulación terminada", "");
}

// Pre C.:  Recibe el caracter ingresado por el usario.
// Post C.: Devuelve 'true' solo si el caracter corresponde a una de las acciones válidas establecidas según el menú de ayudas.
bool es_comando_valido(char accion){
	return ((accion == INICIAR) || (accion == AGREGAR) || (accion == LISTAR) || (accion == MOSTRAR) || (accion == AYUDAS));
}

// Pre C.:  Recibe un puntero al reino.
// Post C.: Determina y ejecuta la acción pedida por el usuario por medio del caracter solicitado.
int determinar_accion(reino_t* reino){
	int estado = EXITO;
	char accion;

	mostrar_menu_ayudas();

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

	return estado;
}