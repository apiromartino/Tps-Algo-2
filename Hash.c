#include "hash.h"
#include <stdlib.h>
#include <string.h>


#define TAM_INI 97

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



size_t Hash(const char *str, size_t tam){
    
    size_t hash = 5381;
    int c = *str;
	while (c == *str++)
        hash = hash * 33 + c; 

    return hash % tam;
}


typedef void (*hash_destruir_dato_t)(void *);

celda_t* crear_tabla (size_t tam){
	celda_t* tabla = malloc(tam * sizeof(celda_t));
	if (tabla == NULL){
		return NULL;
	}
	return tabla;
}

bool hash_inicializar (hash_t* hash, size_t tam, hash_destruir_dato_t destruir_dato){
	hash->tam = tam;
	hash->cantidad = 0;
	hash->cantidad_borrado = 0;
	hash->destructor = destruir_dato;
	for (int i = 0; i < tam; i++){
		hash->tabla[i].estado = VACIO;
	}
	return true;
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





void tabla_destruir (celda_t* tabla, size_t tam){
	for(int i = 0; i < tam; i++){
		if(tabla[i].estado == LLENO){
			free(tabla[i].clave);
		}
	}
	free(tabla);
}

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
		if (aux[i].estado == LLENO){
			if (!hash_guardar(hash, aux[i].clave, aux[i].dato)){
				hash->tam = tam;
				hash->cantidad = cantidad;
				hash->cantidad_borrado = cantidad_borrado;
				hash->tabla = aux;
				tabla_destruir(tabla_nueva, tam_nuevo);
				return false;
			}	
		}
	}

	tabla_destruir(aux, tam);
	return true;
}


char *strdup(const char *old) {
	char *new = malloc(strlen(old) + 1);
	if (new == NULL){
		return NULL;
	}
	strcpy(new, old);
	return new;
}

int hash_localizar_clave(const hash_t *hash, const char *clave, size_t* posicion){
	size_t pos = 0;
	if (*clave != '\0')
		pos = Hash(clave, hash->tam);
	bool vacio = false;
	while (!vacio){
		if (pos >= hash->tam){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && !(strcmp(hash->tabla[pos].clave, clave) == 0))){
						pos++;
			}
			else if (hash->tabla[pos].estado == VACIO){
					*posicion = pos;
					vacio = true;
				}	
				else if (hash->tabla[pos].estado == LLENO && strcmp (hash->tabla[pos].clave, clave) == 0){
				  		*posicion = pos;
				  		return 2;
					}
	}
	return 1;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	if (clave == NULL){
		return false;
	}
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == 1){
		char* copia = strdup(clave);
		if (copia == NULL){
			return false;
		}	
		hash->tabla[pos].estado = LLENO;
		hash->tabla[pos].clave = copia;	
		hash->tabla[pos].dato = dato;
		hash->cantidad++;
	}
	if (loc == 2){
		if (hash->destructor != NULL){
			hash->destructor(hash->tabla[pos].dato);
		}
		hash->tabla[pos].dato = dato;
	}

	size_t factor_carga = ((hash->cantidad + hash->cantidad_borrado) * 100) / hash->tam;
	if (factor_carga >= 70){
		if(!redimensionar_hash (hash->tam * 2, hash)){
			return false;
		}
	}
	return true;
}
	

void *hash_borrar(hash_t *hash, const char *clave){
	if (clave == NULL){
		return false;
	}
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == 1){
		return NULL;
	}
	if (loc == 2){
		free(hash->tabla[pos].clave);
		hash->tabla[pos].estado = BORRADO;
		hash->cantidad--;
		hash->cantidad_borrado++;
	}
	
	size_t factor_carga = ((hash->cantidad * 100)) / hash->tam;
	if (factor_carga <= 15 && hash->tam > TAM_INI){
		if(!redimensionar_hash (hash->tam/2, hash)){
			return NULL;
		}
	}
	
	return hash->tabla[pos].dato; 
}
	
void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == 1){
		return NULL;
	}
	if (loc == 2){
		return hash->tabla[pos].dato;		
	}
	return NULL;
}
	

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t pos = 0;
	int loc = hash_localizar_clave(hash, clave, &pos);
	if (loc == 1){
		return false;
	}
	if (loc == 2){
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
	if (iter->hash->cantidad == 0){
		iter->pos = iter->hash->tam; 
	}
	else { iter->pos = 0;
		   bool encontrado = false;
		   while (!encontrado){
		   		if (iter->hash->tabla[iter->pos].estado == LLENO){
		   			encontrado = true;
		   		}
		   		else iter->pos++;
		   }
		}
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
	if (iter->pos >= iter->hash->tam){
		return true;
	}
	return false;
}


void hash_iter_destruir(hash_iter_t* iter){
	free(iter);
}
