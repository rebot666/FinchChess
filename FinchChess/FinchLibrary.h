#ifndef FINCHLIBLINUX_H
#define FINCHLIBLINUX_H
/**********************************************************************************************
***********************************************************************************************

										Re-Bot

***********************************************************************************************
***********************************************************************************************
Libreria de apoyo Robot Finch
Esta libreria contiene los metodos para interactuar con el robot Finch.
Esta libreria es construida como apoyo a la libreria principal Finch.h desarrollada por los
fabricantes.
Algunos metodos pueden o no estar asi como ser diferentes dentro de esta libreria en
comparacion con Finch.h
***********************************************************************************************
**********************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Finch.h"
#include "FinchLibrary.h"

/**********************************************************************************************
***********************************************************************************************
Estructura utilizada para la lectura del acelerometro
	La llamada de esta estructura es de la siguiente manera:
		Acelerometro datos;
	Para acceder a los valores de la estructura es de la siguiente manera:
		datos.x;
		datos.y;
		datos.z;
		datos.presionado;
		datos.sacudido;
***********************************************************************************************
**********************************************************************************************/
typedef struct acelerometro Acelerometro;
struct acelerometro{
	float x;
	float y;
	float z;
	int presionado;
	int sacudido;
};

/**********************************************************************************************
***********************************************************************************************
Estructura utilizada para guardar las coordenadas de la lectura del acelerometro
	La llamada de esta estructura es de la siguiente manera:
		Punto datos;
	Para acceder a los valores de la estructura es de la siguiente manera:
		datos.x;
		datos.y;
		datos.z;
***********************************************************************************************
**********************************************************************************************/
typedef struct punto Punto;
struct punto{
	float x;
	float y;
	float z;
};

/**********************************************************************************************
***********************************************************************************************
Definicion de prototipos para metodos de la libreria
***********************************************************************************************
**********************************************************************************************/
int sacudido();
int presionado();
int detectarLuz(void);
int atras(int segundos);
int obtenerLuz(int ojo);
int terminarConexion(void);
int adelante(int segundos);
int detectarObstaculo(void);
int inicializarConexion(void);
int obtenerObstaculo(int ojo);
float obtenerTemperatura(void);
struct punto pocision();
int obtenerVelocidadMotor(int motor);
int luz(int rojo, int verde, int azul);
int buzzer(int segundos, int frecuencia);
int vuelta(int duracion, int motor1, int motor2);
int motorContinuo(int duracion, int motor1, int motor2);
int motorBloqueante(int duracion, int motor1, int motor2);

/**********************************************************************************************
***********************************************************************************************
Implementacion de la libreria

Estructura:
	Explicacion del Metodo
		Nombre del Metodo
		Descripcion del Metodo
		Valores de Entrada del Metodo
			Parametro y descripcion del valor de entrada
		Valores de Salida del Metodos
			Parametro y descripcion del valor de salida
	Implementacion del Metodo

***********************************************************************************************
**********************************************************************************************/


/**********************************************************************************************
***********************************************************************************************
inicializarConexion(void)

Metodo que inicializa una conexion con el Finch.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int inicializarConexion(void){
	return Fin_Init();
}

/**********************************************************************************************
***********************************************************************************************
terminarConexion(void)

Metodo que termina una conexion con el Finch.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int terminarConexion(void){
	return Fin_Exit();
}

/**********************************************************************************************
***********************************************************************************************
luz(int rojo, int verde, int azul)

Metodo que configura la luz del frente del Finch.

Entrada:
	@param rojo intensidad de rojo para el led (0 a 255) (0 es apagado)
	@param verde intensidad de verde para el led (0 a 255) (0 es apagado)
	@param azul intensidad de azul para el led (0 a 255) (0 es apagado)

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int luz(int rojo, int verde, int azul){
	return Fin_LED(rojo,verde,azul);
}

/**********************************************************************************************
***********************************************************************************************
motor(int duracion, int motor1, int motor2)

Metodo que realiza desplazamiento del motor basado en la velocidad introducida a cada motor en
un tiempo dado.

Entrada:
	@param tiempo de duracion de la rotacion de los motores (en segundos)
	@param motor1 velocidad del motor Izquierdo (-255 a 255) (0 es detenido)
	@param motor2 velocidad del motor Derecho (-255 a 255) (0 es detenido)

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int motor(int duracion, int motor1, int motor2){
	Fin_Move(duracion, motor1, motor2);
	return luz(255,255,255);
}

/**********************************************************************************************
***********************************************************************************************
detectarObstaculo(void)

Metodo que devuelve si hay un obstaculo frente al Finch que haya sido detectado por ambos
sensores de deteccion de obstaculos.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
	@return 1 hay un obstaculo
	@return 0 no hay un obstaculo
***********************************************************************************************
**********************************************************************************************/
int detectarObstaculo(void){
	int izq, der;

	if(Fin_Obstacle(&izq, &der) <0 ){
		return -1;
	}else{
		if(izq == 1 && der == 1){
			return 1;
		}else{
			return 0;
		}
	}
}

/**********************************************************************************************
***********************************************************************************************
obtenerObstaculo(int ojo)

Metodo que devuelve si hay un obstaculo frente al sensores de deteccion de obstaculos
introducido como parametro.

Entrada:
	@param ojo sensor de obstaculo (0 - derecho y 1 izquierdo)

Regreso:
	@return -1 si hay errores
	@return 1 hay un obstaculo
	@return 0 no hay un obstaculo
***********************************************************************************************
**********************************************************************************************/
int obtenerObstaculo(int ojo){
	int izq, der;
	if(Fin_Obstacle(&izq, &der) <0 ){
		return -1;
	}else{
		if(ojo == 1 ){
			return izq;
		}else if(ojo == 0){
			return der;
		}
	}
}

/**********************************************************************************************
***********************************************************************************************
obtenerVelocidadMotor(int motor)

Metodo que devuelve la velocidad actual del motor dado como parametro.

Entrada:
	@param motor (0 - derecho y 1 izquierdo)

Regreso:
	@return -1 si hay errores
	@return (-255 a 255) (0 es detenido)
***********************************************************************************************
**********************************************************************************************/
int obtenerVelocidadMotor(int motor){
	int izq, der;
	if(Fin_Obstacle(&izq, &der) <0 ){
		return -1;
	}else{
		if(motor == 1 ){
			return izq;
		}else if(motor == 0){
			return der;
		}
	}
}

/**********************************************************************************************
***********************************************************************************************
obtenerLuz(int ojo)

Metodo que devuelve la intensidad actual del sensor de luz del Finch.

Entrada:
	@param ojo sensor de luz (0 - derecho y 1 izquierdo)

Regreso:
	@return -1 si hay errores
	@return (0 a 255) (0 es oscuro)
***********************************************************************************************
**********************************************************************************************/
int obtenerLuz(int ojo){
	int izq, der;
	if(Fin_Lights(&izq, &der) <0 ){
		return -1;
	}else{
		if(ojo == 1 ){
			return izq;
		}else if(ojo == 0){
			return der;
		}
	}
}


/**********************************************************************************************
***********************************************************************************************
buzzer(int milisegundos, int frecuencia)

Metodo que activa el buzzer del Finch.

Entrada:
	@param milisegundos tiempo que se reproducira el buzzer
	@param frecuencia la frecuencia del buzzer dada en hz


Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int buzzer(int milisegundos, int frecuencia){
	return Fin_Buzzer(milisegundos, frecuencia);
}

/**********************************************************************************************
***********************************************************************************************
obtenerTemperatura(void)

Metodo que devuelve la temperatura obtenida por el sensor de temperatura del Finch.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
	@return temperatura devuelta en grados celsius (1/1000 unidades)
***********************************************************************************************
**********************************************************************************************/
float obtenerTemperatura(void){
	float temp;
	Fin_Temp(&temp);
	return temp;

}

/**********************************************************************************************
***********************************************************************************************
presionado(void)

Metodo que devuelve el valor del sensor de toque del Finch, este se encuentra localizado en
la parte superior del mismo.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
	@return bandera de presion del sensor (1 - presionado y 0 no presionado)
***********************************************************************************************
**********************************************************************************************/
int presionado(void){
	struct acelerometro valores;

	Fin_Accel(&valores.x, &valores.y, &valores.z, &valores.presionado, &valores.sacudido);
	return valores.presionado;
}

/**********************************************************************************************
***********************************************************************************************
sacudido(void)

Metodo que devuelve el valor del acelerometro de toque del Finch, este se encuentra localizado en
la parte superior del mismo.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
	@return bandera de aceleracion del sensor (1 - sacudido y 0 no sacudido)
***********************************************************************************************
**********************************************************************************************/
int sacudido(void){
	struct acelerometro valores;

	Fin_Accel(&valores.x, &valores.y, &valores.z, &valores.presionado, &valores.sacudido);
	return valores.sacudido;
}

/**********************************************************************************************
***********************************************************************************************
struct acelerometro pocision(void)

Metodo que devuelve los valores actuales de coordenadas del Finch para el sensor del
acelerometro.

Entrada:
	Sin valores de entrada

Regreso:
	@return -1 si hay errores
	@return valores.x Coordenada x de la aceleracion
	@return valores.y Coordenada y de la aceleracion
	@return valores.z Coordenada z de la aceleracion
***********************************************************************************************
**********************************************************************************************/
struct punto pocision(void){
	struct acelerometro valores;

	Fin_Accel(&valores.x, &valores.y, &valores.z, &valores.presionado, &valores.sacudido);
	struct punto finales;
	finales.x = valores.x;
	finales.y = valores.y;
	finales.z = valores.z;
	return finales;
}


/**********************************************************************************************
***********************************************************************************************
adelante(int segundos)

Metodo que mueve hacia adelante al Finch en un determinado tiempo dado, la velocidad de los
motores esta dada en 255.

Entrada:
	@param segundos tiempo de avance del Finch.

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int adelante(int segundos){
	return motor(segundos, 255, 255);
}

/**********************************************************************************************
***********************************************************************************************
atras(int segundos)

Metodo que mueve hacia atras al Finch en un determinado tiempo dado, la velocidad de los
motores esta dada en 255.

Entrada:
	@param segundos tiempo de avance del Finch.

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int atras(int segundos){
	return motor(segundos, -255, -255);
}

/**********************************************************************************************
***********************************************************************************************
motor(int duracion, int motor1, int motor2)

Metodo que realizar vueltas con el Finch, esta delimitado de la misma manera que el metodo de
motor, explicado anteriormente.

Entrada:
	@param tiempo de duracion de la rotacion de los motores (en segundos)
	@param motor1 velocidad del motor Izquierdo (-255 a 255) (0 es detenido)
	@param motor2 velocidad del motor Derecho (-255 a 255) (0 es detenido)

Regreso:
	@return -1 si hay errores
***********************************************************************************************
**********************************************************************************************/
int vuelta(int duracion, int motor1, int motor2){
	return motor(duracion, motor1, motor2);
}

#endif

