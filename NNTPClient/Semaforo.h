/*
 * util.h
 *
 *  Created on: Apr 24, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef SEMAFORO_H_
#define SEMAFORO_H_

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


#endif /* SEMAFORO_H_ */
