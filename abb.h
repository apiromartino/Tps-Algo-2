#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>



typedef struct abb_nodo abb_nodo_t;

typedef struct abb abb_t;


//tipo de funcion para comparar claves
typedef int (*abb_comparar_clave_t) (const char *, const char *);

// tipo de función para destruir dato
typedef void (*abb_destruir_dato_t) (void *);


// Crea una Arbol Binario de Busqueda.
// Pre: Recibe por parametro una funcion para comparar las claves de los elementos dentro del abb y un destructor para  destruir los datos dentro del abb de ser necesario.
// Post: devuelve un nuevo abb vacío.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda un elemento en el abb, si la clave ya se encuentra en la  estructura, la reemplaza, si no puede guardar en la clave lo guarda en la posicion mas proxima que pueda guardar manteniendo el orden del abb. De no poder guardarlo devuelve false.
// Pre: El abb fue creado.
// Post: Se almacenó el par (clave, dato).
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

//Borra un elemento del abb y devuelve el dato asociado. Devuelve NULL si el dato no estaba.
// Pre: El abb fue creado
// Post: El elemento fue borrado del abb y lo devuelve en caso de que estuviera en la estructura.
void *abb_borrar(abb_t *arbol, const char *clave);

// Obtiene el valor de un elemento del hash, si la clave no se encuentra
// devuelve NULL.
//Pre: El abb fue creado.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Determina si la clave existe o no en el abb, devuelve true si encuentra la clave, si no esta dentro del abb devuelve false.
// Pre: el abb fue creado.
bool abb_pertenece(const abb_t *arbol, const char *clave);

size_t abb_cantidad(abb_t *arbol);

// Destruye la estructura liberando la memoria pedida y llamando a la función destruir para cada par (clave, dato).
//Pre: El abb fue creado.
//Post: El abb fue destruido.
void abb_destruir(abb_t *arbol);


// ITERADOR EXTERNO


typedef struct abb_iter abb_iter_t;

// Crea un iterador para el abb.
// Post: devuelve un iterador colocado en el primer elemento del abb si es que existe,
// si el abb esta vacio el iterador apunta a NULL.
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza una posicion al iterador dentro del abb de forma in order devolviendo true,
// devuelve false si ya no puede avanzar. 
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve el elemento del abb al que esta apuntando el iterador en ese momento.
// Devuelve NULL en caso de que el abb este vacio.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Devuelve false si el iterador puede seguir avanzando dentro del abb, devuelve true si si el iterador ya no puede avanzar dentro del abb.
// Pre: el iterador fue creado.
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador si este ya llego al final del abb.
// Pre: el iterador fue creado.
void abb_iter_in_destruir(abb_iter_t* iter);


// ITERADOR INTERNO

// Recorre el abb entero usando el metodo in order y permite utilizar los elementos del abb sin modificarlos.
// Pre: visitar es una funcion que se recibe por parametro la cual recibe un dato del abb
// y un elemento extra con el que hara lo que quiera con el dato de la lista. El abb fue creado.
// Post: Itera de forma automatica. 
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);
