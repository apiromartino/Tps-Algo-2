#include "abb.h"
#include <stdlib.h>
#include <string.h>
#include "pila.h"


struct abb_nodo {
	char* clave;
	void* dato;
	abb_nodo_t* padre;
	abb_nodo_t* izq;
	abb_nodo_t* der;
};

struct abb {
	size_t cant;
	abb_nodo_t* raiz;
	abb_destruir_dato_t destructor;
	abb_comparar_clave_t comparar;
};



typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);



abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){

	abb_t* arbol = malloc(sizeof(abb_t));
	if (!arbol)
		return NULL;
	arbol->raiz = NULL;
	arbol->cant = 0;
	arbol->destructor = destruir_dato;
	arbol->comparar = cmp;

	return arbol;


}

char *strdup(const char *old) {
	char *new = malloc(strlen(old) + 1);
	if (new == NULL){
		return NULL;
	}
	strcpy(new, old);
	return new;
}


abb_nodo_t* abb_crear_nodo (const char *clave, void *dato, abb_nodo_t* padre){
	abb_nodo_t* abb_nodo = malloc (sizeof(abb_nodo_t));
	if (!abb_nodo)
		return NULL;
	char* copia = strdup(clave);
		if (copia == NULL){
			return NULL;
		}	
	abb_nodo->clave = copia;
	abb_nodo->dato = dato;
	abb_nodo->padre = padre; 
	abb_nodo->izq = NULL;
	abb_nodo->der = NULL;


	return abb_nodo;
}

bool _abb_guardar (abb_t* arbol, abb_nodo_t* actual, const char* clave, void* dato){

	if (!actual->der && (arbol->comparar(actual->clave, clave) < 0)){
		abb_nodo_t* nuevo = abb_crear_nodo(clave, dato, actual);
		if (nuevo == NULL){
			return false;
		}										// NO REPETIR CODIGO
		actual->der = nuevo;
		arbol->cant++;
		return true;
	}
	if (!actual->izq && (arbol->comparar(actual->clave, clave) > 0)){
		abb_nodo_t* nuevo = abb_crear_nodo(clave, dato, actual);
		if (nuevo == NULL){
			return false;
		}
		actual->izq = nuevo;
		arbol->cant++;
		return true;
	}
	if (arbol->comparar(actual->clave, clave) == 0){
		if(arbol->destructor){
			arbol->destructor(actual->dato);
		}
		actual->dato = dato;
		return true;
	}
	if (arbol->comparar(actual->clave, clave) > 0){
		return _abb_guardar(arbol, actual->izq, clave, dato);
	}
	else {
		return _abb_guardar(arbol, actual->der, clave, dato);	
	} 

	return false;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	
	if (arbol->cant == 0){
		abb_nodo_t* nodo_nuevo = abb_crear_nodo(clave, dato, NULL);
		arbol->raiz = nodo_nuevo;
		arbol->cant++;
		return true;
	}
	return _abb_guardar(arbol, arbol->raiz, clave, dato);
}



void destruir_nodo(abb_t* arbol, abb_nodo_t* actual){

	free(actual->clave);
	if (arbol->destructor != NULL){
		arbol->destructor(actual->dato);
	}
	free(actual);
	return;
}


void* abb_borrar_con_un_hijo (abb_t* arbol, abb_nodo_t* actual, abb_nodo_t* hijo, abb_nodo_t* padre){

	void* dato = actual->dato;
	if (padre != NULL){
		hijo->padre = padre;	
		if (arbol->comparar(actual->clave, padre->clave) >= 0)
			padre->der = hijo;
		if (arbol->comparar(actual->clave, padre->clave) < 0)
			padre->izq = hijo;
	}
	else arbol->raiz = hijo;
	
	destruir_nodo(arbol, actual);
	arbol->cant--;
	return dato;
}

abb_nodo_t* abb_buscar_hijo_mas_chico(abb_nodo_t* actual){
	if(actual->izq == NULL){
		return actual;
	}
	return abb_buscar_hijo_mas_chico(actual->izq);

}



void* _abb_borrar(abb_t* arbol, abb_nodo_t* actual, abb_nodo_t* padre, const char* clave){

	if (actual == NULL)
		return NULL;
	if (arbol->comparar(actual->clave, clave) == 0){
		if(!actual->izq && !actual->der){
			void* dato = actual->dato;
			
			if (padre != NULL && arbol->comparar(actual->clave, padre->clave) >= 0){
				padre->der = NULL;
			}
			if (padre != NULL && arbol->comparar(actual->clave, padre->clave) < 0){
				padre->izq = NULL;
			}
			destruir_nodo(arbol, actual);
			arbol->cant--;
			if (arbol->cant == 0)
				arbol->raiz = NULL;
			return dato;
		}
		if(!actual->izq && actual->der){
			return abb_borrar_con_un_hijo(arbol, actual, actual->der, padre);
		}
		if(actual->izq && !actual->der){
			return abb_borrar_con_un_hijo(arbol, actual, actual->izq, padre);
		}
		if(actual->izq && actual->der){
			abb_nodo_t* nuevo_actual = abb_buscar_hijo_mas_chico(actual->der);
			void* aux = actual->dato;
			char* copia = strdup(nuevo_actual->clave);
			if (copia == NULL){
				return NULL;
			}	
			char* aux_2 = actual->clave;
			actual->clave = copia;
			free(aux_2);
			actual->dato = nuevo_actual->dato;
			nuevo_actual->dato = aux;
			return _abb_borrar(arbol, actual->der, actual, nuevo_actual->clave);
		}
	}
	if (arbol->comparar(actual->clave, clave) > 0){
		return _abb_borrar(arbol, actual->izq, actual, clave);
	}
	else {
		return _abb_borrar(arbol,actual->der, actual, clave);
	}

	return NULL;
}


void *abb_borrar(abb_t *arbol, const char *clave){

	return _abb_borrar(arbol, arbol->raiz, NULL, clave);
	
}


void *_abb_obtener(const abb_t* arbol, abb_nodo_t* actual, const char* clave){

	if (actual == NULL){
		return NULL;
	}
	if (arbol->comparar(actual->clave, clave) == 0){
		return actual->dato;
	}
	if (arbol->comparar(actual->clave, clave) > 0){
		return _abb_obtener(arbol, actual->izq, clave);
	}
	else{
		return _abb_obtener(arbol, actual->der, clave);
	}

	return NULL;

} 

void *abb_obtener(const abb_t *arbol, const char *clave){

	return _abb_obtener(arbol, arbol->raiz, clave);
}


bool _abb_pertenece(const abb_t* arbol, abb_nodo_t* actual, const char* clave){

	if (actual == NULL){
		return false;
	}
	if(arbol->comparar(actual->clave, clave) == 0){
		return true;
	}
	if (arbol->comparar(actual->clave, clave) > 0){
		return _abb_pertenece(arbol, actual->izq, clave);
	}
	else{
		return _abb_pertenece(arbol, actual->der, clave);
	}

	return false;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){

	return _abb_pertenece(arbol, arbol->raiz, clave);
}



size_t abb_cantidad(abb_t *arbol){
	return arbol->cant;
}



void _abb_destruir(abb_t* arbol, abb_nodo_t* actual){

	if (actual == NULL){
		return;
	}
	if (actual->izq){
		_abb_destruir(arbol, actual->izq);
	}
	if (actual->der){
		_abb_destruir(arbol, actual->der);
	}
	
	destruir_nodo(arbol, actual);
	return;

}

void abb_destruir(abb_t *arbol){

	_abb_destruir(arbol, arbol->raiz);
	free(arbol);
	return;
}




void _abb_in_order(abb_t *arbol, abb_nodo_t* actual, bool visitar(const char *, void *, void *), void *extra){

	if (actual == NULL)
		return;
	_abb_in_order(arbol, actual->izq, visitar, extra);
	if(!visitar(actual->clave, actual->dato, extra))
		return;
	_abb_in_order(arbol, actual->der, visitar, extra);

	return;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){

	_abb_in_order(arbol, arbol->raiz, visitar, extra);
	return;

}




struct abb_iter {
	pila_t* pila;
};


void abb_iter_apilar_hij_izq (abb_nodo_t* actual, pila_t* pila){

	if (actual == NULL)
		return;
	if(!pila_apilar(pila, actual))
		return;

	abb_iter_apilar_hij_izq(actual->izq, pila);
	return;
}


abb_iter_t *abb_iter_in_crear(const abb_t *arbol){

	abb_iter_t* iter = malloc (sizeof(abb_iter_t));
	if (iter == NULL)
		return NULL;

	pila_t* pila = pila_crear();
	abb_nodo_t* actual = arbol->raiz;
	abb_iter_apilar_hij_izq (actual, pila);

	iter->pila = pila;

	return iter;

}


bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)){
		return false;
	}
	abb_nodo_t* aux = pila_desapilar(iter->pila);
	abb_iter_apilar_hij_izq(aux->der, iter->pila);

	return true;

}



const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter)){
		return NULL;
	}
	abb_nodo_t* nodo_actual = pila_ver_tope(iter->pila);
	return nodo_actual->clave;
}





bool abb_iter_in_al_final(const abb_iter_t *iter){

	return pila_esta_vacia(iter->pila);
}





void abb_iter_in_destruir(abb_iter_t* iter){

	pila_destruir(iter->pila);
	free(iter);
}

