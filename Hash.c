#include "hash.h"
#include <stdlib.h>
#include <string.h>

#define POSICION_VACIA 1
#define POSICION_OCUPADA 2
#define TAM_INI 337

// Alumnos: Ariel Piro Martino (Padron: 99469)
//	    Juan Cruz OPizzi (Padron: 99807)
//Ayudante: Miguel Alfaro	
		

typedef enum {BORRADO, LLENO, VACIO} estado_t;

typedef struct celda_hash{
	char* clave;
	void* dato;
	estado_t estado; 
}celda_t;

struct hash{
	celda_t* tabla;
	size_t tam;
	size_t cantidad;
	size_t cantidad_borrado;
	hash_destruir_dato_t destructor; 
};


//
size_t hashing(const char *key, size_t largo){
    size_t hash, i;
    for(hash = i = 0; i < strlen(key); ++i){
    size_t aux = (size_t)(key[i]);
        hash += aux;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    hash %= largo;
    return hash;
}


typedef void (*hash_destruir_dato_t)(void *);

// Crea la tabla donde van a estar los datos, claves y estados de cada posicion dentro del hash.
// Pre: Se recibe por parametro el tamaño del hash para crear la tabla de dicho tamaño.
// Post: se devolvio la tabla para el hash. 
celda_t* crear_tabla (size_t tam){
	celda_t* tabla = malloc(tam * sizeof(celda_t));
	return tabla;
}

// Inicializa el hash con 0 elementos y del tamaño que recibe por parametro, tambien recibe el destructor que se le pasa por parametro. 
// Pre: el hash fue creado.
// Post: el hash tiene todo sus nodos sin datos adentro (con el estado en vacio).
void hash_inicializar (hash_t* hash, size_t tam, hash_destruir_dato_t destruir_dato){
	hash->tam = tam;
	hash->cantidad = 0;
	hash->cantidad_borrado = 0;
	hash->destructor = destruir_dato;
	for (int i = 0; i < tam; i++){
		hash->tabla[i].estado = VACIO;
	}
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc (sizeof(hash_t));
	if (hash == NULL){
		return NULL;
	}
	celda_t* tabla = crear_tabla(TAM_INI);
	if (tabla == NULL){
		free (hash);
		return NULL;
	}
	hash->tabla = tabla;
	hash_inicializar(hash, TAM_INI, destruir_dato);
	return hash;
}

// Recorre la tabla destruyendo las posiciones de la tabla del hash en caso de que las posiciones esten ocupadas por algun dato.
// Pre: la tabla fue creada
void tabla_destruir (celda_t* tabla, size_t tam){
	for(int i = 0; i < tam; i++){
		if(tabla[i].estado == LLENO){
			free(tabla[i].clave);
		}
	}
	free(tabla);
}

//
bool redimensionar_hash (size_t tam_nuevo, hash_t* hash){
	celda_t* tabla_nueva = crear_tabla (tam_nuevo);
	if (tabla_nueva == NULL){
		return false;
	}

	size_t tam = hash->tam;
	size_t cantidad = hash->cantidad;
	size_t cantidad_borrado = hash->cantidad_borrado;
	celda_t* aux = hash->tabla;
	hash->tabla = tabla_nueva;
	hash_inicializar(hash, tam_nuevo, hash->destructor);
	
	for(int i = 0; i < tam; i++){
		if (aux[i].estado != LLENO) continue;
		
		if (!hash_guardar(hash, aux[i].clave, aux[i].dato)){
			hash->tam = tam;
			hash->cantidad = cantidad;
			hash->cantidad_borrado = cantidad_borrado;
			hash->tabla = aux;
			tabla_destruir(tabla_nueva, tam_nuevo);
			return false;
		}
	}

	tabla_destruir(aux, tam);
	return true;
}

//
char *strdup(const char *old) {
	char *new = malloc(strlen(old) + 1);
	if (new == NULL){
		return NULL;
	}
	strcpy(new, old);
	return new;
}

// Busca en el hash la clave que recibe por parametro y devuelve si la posicion esta vacio u ocupada por un dato, VER 
// Pre: el hash fue creado
// Post: se devolvio el estado de la posicion donde esta la clave que se busca.
int hash_localizar_clave(const hash_t *hash, const char *clave, size_t* posicion){
	size_t pos = 0;
	if (*clave != '\0')
		pos = hashing(clave, hash->tam);
		
	bool vacio = false;
	while (!vacio){
		if (pos == hash->tam){
			pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && strcmp(hash->tabla[pos].clave, clave) != 0)){
						pos++;
			}
			else if (hash->tabla[pos].estado == VACIO){
					*posicion = pos;
					vacio = true;
				}	
				else if (hash->tabla[pos].estado == LLENO && strcmp (hash->tabla[pos].clave, clave) == 0){
				  		*posicion = pos;
				  		return POSICION_OCUPADA;
					}
	}
	return POSICION_VACIA;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	size_t factor_carga = ((hash->cantidad + hash->cantidad_borrado) * 100) / hash->tam;
	if (factor_carga >= 72){
		if(!redimensionar_hash (hash->tam * 2, hash)){
			return false;
		}
	}

	if (clave == NULL){
		return false;
	}
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == POSICION_VACIA){
		char* copia = strdup(clave);
		if (copia == NULL){
			return false;
		}	
		hash->tabla[pos].estado = LLENO;
		hash->tabla[pos].clave = copia;	
		hash->tabla[pos].dato = dato;
		hash->cantidad++;
	}
	if (loc == POSICION_OCUPADA){
		if (hash->destructor != NULL){
			hash->destructor(hash->tabla[pos].dato);
		}
		hash->tabla[pos].dato = dato;
	}

	return true;
}
	

void *hash_borrar(hash_t *hash, const char *clave){
	
	size_t factor_carga = ((hash->cantidad * 100)) / hash->tam;
	if (factor_carga <= 17 && hash->tam > TAM_INI){
		redimensionar_hash (hash->tam/2, hash);
	}

	if (clave == NULL){
		return false;
	}
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == POSICION_VACIA){
		return NULL;
	}
	if (loc == POSICION_OCUPADA){
		free(hash->tabla[pos].clave);
		hash->tabla[pos].estado = BORRADO;
		hash->cantidad--;
		hash->cantidad_borrado++;
	}
	
	return hash->tabla[pos].dato;
}

// Pre: el hash fue creado 
// Post: Devuelve true si se encontro la posicion de la clave que recibe por paramatro y dicha posicion tiene un dato adentro, devuelve false si no se encontro la posicion o si la posicion esta vacia.
bool encontrado_o_vacio(const hash_t *hash, const char *clave, size_t* pos){
	int loc = hash_localizar_clave(hash, clave, pos);
	if (loc == POSICION_VACIA){
		return false;
	}
	if (loc == POSICION_OCUPADA){
		return true;		
	}
	return false;
}
	
void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = 0;
	if (encontrado_o_vacio(hash, clave, &pos)){
		return hash->tabla[pos].dato;
	}
	return NULL;
}
	

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = 0;
	if (encontrado_o_vacio(hash, clave, &pos)){
		return true;
	}
	return false;
}

	

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}



void hash_destruir(hash_t *hash){
	for(int i = 0; i < hash->tam; i++){
		if(hash->tabla[i].estado == LLENO){
			free(hash->tabla[i].clave);
			if(hash->destructor != NULL)
				hash->destructor(hash->tabla[i].dato);
		}
	}
	free(hash->tabla);
	free(hash);
}


struct hash_iter{
	celda_t* actual;
	const hash_t* hash;
	size_t pos;
};


hash_iter_t *hash_iter_crear(const hash_t *hash){

	hash_iter_t* iter = malloc (sizeof(hash_iter_t));
	if (iter == NULL){
		return NULL;
	}

	iter->hash = hash;
	size_t pos = 0;
	while (pos < hash->tam && hash->tabla[pos].estado != LLENO){
		pos++;
	}
	iter->pos = pos;
	iter->actual = iter->hash->tabla + iter->pos;
	
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	iter->pos++;
	bool encontrado = false;
	while (!encontrado){
		if (hash_iter_al_final(iter)){
			return false;
		}
		else if (iter->hash->tabla[iter->pos].estado == LLENO){
				iter->actual = iter->hash->tabla + iter->pos;
				encontrado = true;
		} else iter->pos++;
			
	}
	
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter)){
		return NULL;
	}
	return iter->actual->clave;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	return iter->pos >= iter->hash->tam;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
