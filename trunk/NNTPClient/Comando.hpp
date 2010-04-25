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
	string cadenaIngresada;
    string parametro;
    string nombreComando;
    string respuestaAlUsuario;//	Pongo este atributo porque Comando es el recurso que voy a compartir entre los hilos.
    bool   llevaParametro;


    string vectorDeComandos[7];
    int    vectorDeParametros[7];

public:
	Comando(void);
	~Comando(void);

	/**
	 * Resetea los atributos del objeto. Lo usamos antes de empezar una nueva iteracion, para que no quede basura.
	 */
	void   reset(void);

	void   init(string cadena);
	string getCadenaIngresada(void);

	void   setNombreComando(string nombre);
	string getNombreComando(void);

	void   setRespuestaAlUsuario(string respuesta);
	string getRespuestaAlUsuario(void);

	bool getLlevaParametro(void);

	string getParametro(void);

    int parseaYValida(string cadenaIngresada);
	void inicializacionVector(void);
	string sacaEspaciosIzquierda(string cadena);
    void extraerNombreYParametro(string comandoEntero);
    int consumeEspaciosDesde(int posicion,string cadena);
    int validacion(void);
    string aMayusculas(string cadena);
};

#endif /* COMANDO_H_ */
