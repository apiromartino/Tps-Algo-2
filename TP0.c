# Tps-Algo-2
#include "tp0.h"


void swap (int* x, int* y){

	int aux=*x;
	*x=*y;
	*y=aux;	
}	


int maximo(int vector[], int n){
	int pos;
	int max=0;
	if (n==0){
		return -1;
    }	
	else {for (int i=1; i<n-1;i++){
		if (vector[i]>max){
			max=vector[i];
			pos=i;
		}			
		}
	return pos;
	}			
}		
/* La función comparar recibe dos vectores y sus respectivas longitudes
 * y devuelve -1 si el primer vector es menor que el segundo; 0 si son
 * iguales; y 1 si el segundo es menor.
 *
 * Un vector es menor a otro cuando al compararlos elemento a elemento,
 * el primer elemento en el que difieren no existe o es menor.
 */
int comparar(int vector1[], int n1, int vector2[], int n2){

	for(int i=1;i<n1-1 && i<n2-1;i++){	
		
		
	}
}	



/* selection_sort() ordena el arreglo recibido mediante el algoritmo de
 * selección.
 */
void seleccion(int vector[], int n){
	int aux;
	for (int i=1;i<n-2;i++){
		for(int j=i+1;j<n-1;j++){
			if(vector[i]>vector[j]){
				aux=vector[i];
				vector[i]=vector[j];
				vector[j]=aux;
			}	
	}
		}
}	
