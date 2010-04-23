/*
 * Comando.h
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef COMANDO_H_
#define COMANDO_H_

class Comando {
	//	Lo privado
    char* nombreComando;
    bool  llevaParametro;

public:
	Comando(void);
	~Comando(void);

	void  setNombreComando(char* nombre);
	char* getNombreComando(void);

	bool getLlevaParametro(void);
	void setLlevaParametro(int valor);

	void funcionDePrueba(void);
};

#endif /* COMANDO_H_ */
