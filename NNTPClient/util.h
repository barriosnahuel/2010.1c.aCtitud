/*
 * util.h
 *
 *  Created on: Apr 24, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
using namespace std;


class Semaforo {
	bool ocupado;
public:
	Semaforo();
	virtual ~Semaforo();

	bool estasOcupado(void);
	void setEstasOcupado(bool);
};


//	Funciones utilitarias.
bool dosStringsSonIguales(string cadena1,string cadena2);




#endif /* UTIL_H_ */
