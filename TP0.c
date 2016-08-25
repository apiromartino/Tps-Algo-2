#include "tp0.h"


void swap (int* x, int* y){

	int aux=*x;
	*x=*y;
	*y=aux;	
}	


int maximo(int vector[], int n){
	if (n==0){
		return -1;
    }	
	else {int max=vector[0];
	      int pos=0;
	      for (int i=0; i<n;i++){
		if (vector[i]>max){
			max=vector[i];
			pos=i;
		}			
		}
	return pos;
	}			
}		

int comparar(int vector1[], int n1, int vector2[], int n2){

	for(int i=0;i<n1 && i<n2;i++){	
		if (vector1[i]>vector2[i]){
			return 1;
		} 
		else if (vector1[i]<vector2[i]){
					return -1;
			 }	
	}
	if (n1>n2){
		return 1;
	}	
	else if (n1<n2){
			return -1;
		}
		else {
			return 0;
		}
}


void seleccion(int vector[], int n){
	int aux;
	for (int i=0;i<n-1;i++){
		for(int j=i+1;j<n;j++){
			if(vector[i]>vector[j]){
				aux=vector[i];
				vector[i]=vector[j];
				vector[j]=aux;
			}	
	}
		}
}
