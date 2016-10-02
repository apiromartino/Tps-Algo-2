#include "hash.h"
#include <stdlib.h>
#include <string.h>


#define TAM_INI 97
#define INITIAL_VALUE 0
#define M 31
typedef enum {BORRADO, LLENO, FINAL, VACIO} estado_t;

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
	hash_destruir_dato_t destructor; //Por ahi es puntero.
};



size_t Hash(const char *clave, size_t tam) {
   size_t hash = INITIAL_VALUE;
   for(size_t i = 0; i < tam; ++i)
      hash = M * hash + clave[i];
   return hash % tam;
}

typedef void (*hash_destruir_dato_t)(void *);



hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc (sizeof(hash_t));
	if (hash == NULL){
		return NULL;
	}
	hash->tabla = malloc(TAM_INI * sizeof(celda_t));
	if (hash->tabla == NULL){
		free(hash);
		return NULL;
	}
	hash->tam = TAM_INI;
	hash->cantidad = 0;
	hash->cantidad_borrado = 0;
	hash->destructor = destruir_dato;
	for (int i = 0; i < (TAM_INI - 1); i++){
		hash->tabla[i].estado = VACIO;
	}
	hash->tabla[TAM_INI - 1].estado = FINAL;
	return hash;
}


bool pasar_datos(hash_t* hash, hash_t* hash_viejo){
	for(int i = 0; i < hash->tam; i++){
		if (hash_viejo->tabla[i].estado == LLENO){
			hash_guardar(hash, hash_viejo->tabla[i].clave, hash_viejo->tabla[i].dato);
		}
	}
	return true;	
}


hash_t* redimensionar_hash (size_t tam_nuevo, hash_t* hash_viejo){
	hash_t* hash = malloc (sizeof(hash_t));
	if (hash == NULL){
		return NULL;
	}
	hash->tabla = malloc(tam_nuevo * sizeof(celda_t));
	if (hash->tabla == NULL){
		free(hash);
		return NULL;
	}
	hash->tam = tam_nuevo;
	hash->cantidad = 0;
	hash->cantidad_borrado = 0;
	hash->destructor = hash_viejo->destructor; //Por ahi pasar destruir dato por parametro y asignarlo al nuevo hash.
	hash->tabla[tam_nuevo - 1].estado = FINAL;
	for (int i = 0; i < (tam_nuevo - 1); i++){
		hash->tabla[i].estado = VACIO;
	}
	pasar_datos(hash, hash_viejo);
	hash_destruir(hash_viejo);
	return hash;
}


char *strdup(const char *old) {
	char *new;
	if((new = malloc(strlen(old) + 1)) == NULL)
	return NULL;
	strcpy(new, old);
	return new;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	size_t pos = Hash(clave, hash->tam);
	bool terminado = false;
	while (!terminado){

	if (hash->tabla[pos].estado == VACIO){
		char* copia = strdup(clave);	
		hash->tabla[pos].clave = copia;
		hash->tabla[pos].dato = dato;
		hash->cantidad++;
		hash->tabla[pos].estado = LLENO;
		terminado = true;
	} 
	else if (hash->tabla[pos].estado == FINAL){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && hash->tabla[pos].clave != clave)){
				pos++;
			}		
			else {  hash->tabla[pos].dato = hash->destructor;
					hash->tabla[pos].dato = dato;
					hash->cantidad++;
					terminado = true;
			}
	}

	size_t factor_carga = (hash->cantidad + hash->cantidad_borrado) / hash->tam;
	if (factor_carga >= 70){
		hash = redimensionar_hash (hash->tam * 2, hash);
	}
	return true;
}
	

void *hash_borrar(hash_t *hash, const char *clave){
	
	if(!hash_pertenece(hash, clave)){
		return NULL;
	}
	void* aux;	
	size_t pos = Hash(clave, hash->tam);
	bool encontrado = false;
	while (!encontrado){
		if (hash->tabla[pos].estado == FINAL){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && hash->tabla[pos].clave != clave)){
				pos++;
			}
			else if (hash->tabla[pos].clave == clave){
				 	aux = hash->tabla[pos].dato;
					hash->tabla[pos].estado = BORRADO;
					hash->tabla[pos].dato = hash->destructor;
					hash->cantidad--;
					hash->cantidad_borrado++;
				 	encontrado = true;
				 }	
	}

	size_t factor_carga = (hash->cantidad + hash->cantidad_borrado) / hash->tam;
	if (factor_carga <= 15){
		hash = redimensionar_hash (hash->tam/2, hash);
	}
	
	return aux; 
}
	



void *hash_obtener(const hash_t *hash, const char *clave){
	if(!hash_pertenece(hash, clave)){
		return NULL;
	}
	size_t pos = Hash(clave, hash->tam);
	bool encontrado = false;
	while (!encontrado){
	if (hash->tabla[pos].estado == FINAL){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && hash->tabla[pos].clave != clave)){
				pos++;
			}
			else {
				 if (hash->tabla[pos].clave == clave){
				 	encontrado = true;
				 }

			}
	}
	return hash->tabla[pos].dato;		

}
	

bool hash_pertenece(const hash_t *hash, const char *clave){
	if (hash->cantidad == 0){
		return false;
	}
	size_t pos = Hash(clave, hash->tam);
	bool encontrado = false;
	while (!encontrado){
	if (hash->tabla[pos].clave == clave){
		encontrado = true;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && hash->tabla[pos].clave != clave)){
				pos++;
		}
			else if (hash->tabla[pos].estado == VACIO){
					return false;
			}
	}	
	return true;
}
	

size_t hash_cantidad(const hash_t *hash){

	return hash->cantidad;
}



void hash_destruir(hash_t *hash){
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
	if (iter->hash->cantidad == 0){
		iter->pos = iter->hash->tam - 1; 
	} else iter->pos = 0;
	iter->actual = iter->hash->tabla + iter->pos;
	
	return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter))
		return false;

	iter->pos++;
	iter->actual = iter->hash->tabla + iter->pos;
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if (hash_iter_al_final(iter)){
		return NULL;
	}
	return iter->actual->clave;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	if (iter->actual->estado == FINAL){
		return true;
	}
	return false;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}