/*
 * util.cpp
 *
 *  Created on: Apr 24, 2010
 *      Author: Barrios, Nahuel.
 */

#include "Semaforo.h"
using namespace std;


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
