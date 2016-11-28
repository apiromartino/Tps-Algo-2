#include "heap.h"
#include <stdlib.h>
#include <string.h>

#define TAM_INI 100



void swap (void* arreglo[], size_t i, size_t pos){

	void * aux = arreglo[i];
	arreglo[i] = arreglo[pos];
	arreglo[pos] = aux;				
}

void downheap (void* arreglo[], size_t n, size_t i, cmp_func_t cmp){

	if (i >= n)
		return;
	size_t pos_h_izq = i * 2 + 1;
	size_t pos_h_der = i * 2 + 2;
	size_t pos_mayor = i;
	if (pos_h_izq < n && (cmp(arreglo[i], arreglo[pos_h_izq]) < 0 ))	
		pos_mayor  = pos_h_izq;
	if (pos_h_der < n && (cmp(arreglo[pos_mayor], arreglo[pos_h_der]) < 0 ))
		pos_mayor = pos_h_der;
	if (pos_mayor != i){
		swap(arreglo, i, pos_mayor);
		downheap (arreglo, n, pos_mayor, cmp);
	}

}


void heapify (void *elementos[], size_t cant, cmp_func_t cmp){
	for (size_t i = cant/2 - 1; i != -1; i--){		
		downheap(elementos, cant, i, cmp);
	}
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){

	heapify(elementos, cant, cmp);
	for (size_t i = cant; i > 0; i--){
		swap(elementos, 0, cant - 1);			
		cant--;
		downheap(elementos, cant, 0, cmp);
	}
}



struct heap{
	size_t cant;
	size_t tam; 
	void** datos;
    cmp_func_t cmp;
};


void upheap (void* arreglo[], size_t n, size_t i, cmp_func_t cmp){
	if (i == 0)
		return;
	size_t pos_padre = (i - 1) / 2;
	if (cmp(arreglo[i], arreglo[pos_padre]) > 0 ){			
		swap (arreglo, i, pos_padre);
		upheap (arreglo, n, pos_padre, cmp);
	}
}



heap_t* inicializar_heap (size_t tam, size_t cant, cmp_func_t cmp){

	heap_t* heap = malloc(sizeof(heap_t));
	if (heap == NULL) {
        return NULL;
    }
    heap->datos = malloc(tam * sizeof(void*));

    if (heap->datos == NULL) {
        free(heap);
        return NULL;
	}
    	    
    heap->tam = tam;
    heap->cant = cant;
    heap->cmp = cmp;
    return heap;
}

heap_t *heap_crear(cmp_func_t cmp){

	heap_t* heap = inicializar_heap(TAM_INI, 0, cmp);
	if (heap == NULL)
		return NULL;
    return heap;

}


heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){

	heap_t* heap = inicializar_heap(n, n, cmp);
	if(heap == NULL)
		return NULL;
	
   	for (int i = 0; i < n; i ++)
    	heap->datos[i] = arreglo[i];

    heapify(heap->datos, n, cmp);
	return heap;
}




void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	if (destruir_elemento != NULL){
		for (int i = 0; i < heap->cant; i++){
			destruir_elemento(heap->datos[i]);
		}
	}
	free(heap->datos);
	free(heap);
}


size_t heap_cantidad(const heap_t *heap){
	return heap->cant;
}



bool heap_esta_vacio(const heap_t *heap){
	return heap->cant == 0;
}



bool heap_redimensionar(heap_t* heap, size_t tam_nuevo){
	void** datos_nuevos = realloc(heap->datos, tam_nuevo * sizeof(void*));
		if(datos_nuevos == NULL){
			return false;
		}
	
	heap->tam = tam_nuevo;
	heap->datos = datos_nuevos;
	return true;
}


bool heap_encolar(heap_t *heap, void *elem){
	
	if (elem == NULL){
		return false;
	}

	if (heap->cant == heap->tam){
		heap_redimensionar(heap, heap->tam * 2);
	}							
	heap->datos[heap->cant] = elem;
	heap->cant++;
	upheap(heap->datos, heap->cant, heap->cant - 1, heap->cmp);  

	return true;

}



void *heap_ver_max(const heap_t *heap){
	
	if (heap_esta_vacio(heap))
		return NULL;

	return heap->datos[0];
}




void *heap_desencolar(heap_t *heap){
	
	if (heap_esta_vacio(heap))
		return NULL;			

	if (heap->cant == heap->tam / 4 && heap->tam > TAM_INI){
		heap_redimensionar(heap, heap->tam / 2);
	}
	void* aux = heap->datos[0];
	swap(heap->datos, 0, heap->cant - 1);			
	heap->cant--;
	downheap(heap->datos, heap->cant, 0, heap->cmp);
	return aux;

}
