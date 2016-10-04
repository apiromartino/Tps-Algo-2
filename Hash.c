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
	for (size_t i = 0; i < TAM_INI; i++){
		hash->tabla[i].estado = VACIO;
	}
	return hash;
}


bool pasar_datos(hash_t* hash, hash_t* hash_viejo){
		
	for(size_t i = 0; i < hash_viejo->tam; i++){
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
	hash->destructor = hash_viejo->destructor;
	for (size_t i = 0; i < tam_nuevo; i++){
		hash->tabla[i].estado = VACIO;
	}
	pasar_datos(hash, hash_viejo);
	hash_destruir(hash_viejo);
	return hash;
}


char *strdup(const char *old) {
	char *new = malloc(strlen(old) + 1);
	if (new == NULL){
		return NULL;
	}
	strcpy(new, old);
	return new;
}


bool hash_guardar(hash_t *hash, const char *clave, void *dato){

	size_t pos = Hash(clave, hash->tam);
	bool terminado = false;
	while (!terminado){
		if (pos >= hash->tam){
				pos = 0;
		}
		else if (hash->tabla[pos].estado == VACIO){
				char* copia = strdup(clave);
				if (copia == NULL){
					return false;
				}	
				hash->tabla[pos].estado = LLENO;
				hash->tabla[pos].clave = copia;	
				hash->tabla[pos].dato = dato;
				hash->cantidad++;
				terminado = true;
	
			}else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) != *clave)){
						pos++;
				  }	else if (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) == *clave){
							if (hash->destructor != NULL)
								hash->destructor(hash->tabla[pos].dato);
								hash->tabla[pos].dato = dato;
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
	size_t pos = Hash(clave, hash->tam);
	bool encontrado = false;
	while (!encontrado){
		if (pos >= hash->tam){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) != *clave)){
				pos++;
			}
			else if (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) == *clave){
					hash->tabla[pos].estado = BORRADO;
					hash->cantidad--;
					hash->cantidad_borrado++;
				 	encontrado = true;
				 }
				 else if (hash->tabla[pos].estado == VACIO){
				 	return NULL;
				 }
	}

	size_t factor_carga = (hash->cantidad + hash->cantidad_borrado) / hash->tam;
	if (factor_carga <= 15 && hash->tam > TAM_INI){
		hash = redimensionar_hash (hash->tam/2, hash);
	}
	
	return hash->tabla[pos].dato; 
}
	
void *hash_obtener(const hash_t *hash, const char *clave){
	size_t pos = Hash(clave, hash->tam);
	bool encontrado = false;
	while (!encontrado){
	if (pos >= hash->tam){
				pos = 0;
		}
		else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) != *clave)){
				pos++;
			}
			else {
				 if (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) == *clave){
				 	encontrado = true;
				 }
				 else if (hash->tabla[pos].estado == VACIO){
				 	return NULL;
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
		if (pos >= hash->tam){
			pos = 0;
		} else	if (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) == *clave){
					encontrado = true;
				}	else if ((hash->tabla[pos].estado == BORRADO) || (hash->tabla[pos].estado == LLENO && *(hash->tabla[pos].clave) != *clave)){
								pos++;
						}	else if (hash->tabla[pos].estado == VACIO){
									return false;
								}		
	}
	return true;
}
	

size_t hash_cantidad(const hash_t *hash){

	return hash->cantidad;
}



void hash_destruir(hash_t *hash){
	for(int i = 0; i < hash->tam; i++){
		if(hash->tabla[i].estado == LLENO || hash->tabla[i].estado == BORRADO){
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
