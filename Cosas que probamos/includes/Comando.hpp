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
    char* comando;

public:
	Comando(void);
	~Comando(void);

	void setComando(char* comando);
	char* getComando(void);

};

#endif /* COMANDO_H_ */
