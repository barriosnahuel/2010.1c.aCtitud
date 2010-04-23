/*
 * Comando.h
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef COMANDO_H_
#define COMANDO_H_

#include <iostream>	//	Necesario para poder usar el struct string.
using namespace std;//	Necesario para poder usar el struct string.

class Comando {
	//	Lo privado
    string nombreComando;
    bool  llevaParametro;

public:
	Comando(void);
	~Comando(void);

	void  setNombreComando(string nombre);
	string getNombreComando(void);

	bool getLlevaParametro(void);
	void setLlevaParametro(int valor);

	void funcionDePrueba(void);
};

#endif /* COMANDO_H_ */
