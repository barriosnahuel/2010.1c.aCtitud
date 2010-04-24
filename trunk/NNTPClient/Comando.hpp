/*
 * Comando.h
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef COMANDO_H_
#define COMANDO_H_
#include<vector>
#include <iostream>	//	Necesario para poder usar el struct string.
using namespace std;//	Necesario para poder usar el struct string.

class Comando {
	//	Lo privado
    string nombreComando;
    string respuestaAlUsuario;//	Pongo este atributo porque Comando es el recurso que voy a compartir entre los hilos.
    bool   llevaParametro;
    vector<string> vectorDeComandos;
    vector<int> vectorDeParametros;

public:
	Comando(void);
	~Comando(void);

	void  setNombreComando(string nombre);
	string getNombreComando(void);

	void  setRespuestaAlUsuario(string respuesta);
	string getRespuestaAlUsuario(void);

	bool getLlevaParametro(void);
	void setLlevaParametro(int valor);

	void funcionDePrueba(void);
};

#endif /* COMANDO_H_ */
