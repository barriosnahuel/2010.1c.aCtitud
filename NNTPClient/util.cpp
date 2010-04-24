/*
 * util.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: Barrios, Nahuel.
 */

#include "util.h"
using namespace std;


/**
 * Retorna 1 (verdadero) cuando dos strings son iguales, y 0 (falso) cuando no.
 */
bool dosStringsSonIguales(string cadena1,string cadena2){
	return cadena1.compare(cadena2)==0;
}


//	-------------------------------------------------------------
//	Esto es de la clase Semaforo.
Semaforo::Semaforo() {
	setEstasOcupado(false);
}

Semaforo::~Semaforo() {
}

bool Semaforo::estasOcupado(void){
	return ocupado;
}

void Semaforo::setEstasOcupado(bool nuevoEstado){
	ocupado= nuevoEstado;
}
//	-------------------------------------------------------------
