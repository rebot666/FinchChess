#include <stdio.h>
#include <stdlib.h>

#include "Finch.h"
#include "FinchLibrary.h"
#ifdef _LINUX_
#include <pthread.h>
#else
#include <conio.h>
#include <windows.h>
#endif

/*
	RE-BOT
	Recreo 1: Laberintos
	
	Para poder mover al robot se pueden usar alguno de los siguientes metodos:
	Descripciones:
		motor(segundos,velocidadIzquierda,velocidadDerecha);
		adelante(segundos);
		atras(segundos);

	Ejemplos:
		atras(5);
		adelante( 15 );
		motor(10, 100,200);

	Para ver los metodos y la descripcion de los mismo asi como los valores que reciben
	y su significado abrir el archivo FinchLibrary.h
*/

void rutina(void){
	//TO-DO  Escribir la rutina para el laberinto en este espacio	
	adelante(5);














}


/*
	Metodo principal del programa, no borrar.
*/

int main(){

	//Metodos de inicializacion del Finch
	//NO BORRAR
	if (inicializarConexion() < 0) exit(1);

	//Se ejecuta la rutina del Finch
	rutina();

	//Metodos de finalizacion del Finch
	//NO BORRAR
	terminarConexion();

	//Finalizacion del programa
	return 0;
}
