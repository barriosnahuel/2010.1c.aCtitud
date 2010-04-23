/*
 * Comando.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 *      Nota:   Cualquier funcion y/o método que se agregue, ver si hay que agregarlo en los dos lugares (.h/.hpp y .cpp) y tienen que
 *      		respetar el formato que tiene la funcionDePrueba. Eso es:	ClaseALaQuePerteneceElMetodo::nombreMetodo.
 */

#include "Comando.hpp"
#include <iostream>
using namespace std;

	Comando::Comando(void)
	{
		cout << "Comando::Comando(void)" << endl;
	}

	Comando::~Comando(void)
	{
		cout << "Comando::~Comando(void)" << endl;
	}

	void Comando::setNombreComando(char* nombre){
		nombreComando= nombre;
	}

	char* Comando::getNombreComando(void){
		return nombreComando;
	}

    bool Comando::getLlevaParametro(void){
        return llevaParametro;
    }

    void Comando::setLlevaParametro(int valor){
        llevaParametro = valor;
    }

    void Comando::funcionDePrueba(void){
    	cout << "Entro bien a la funcion: void Comando::funcionDePrueba(void)" << endl;
    }
