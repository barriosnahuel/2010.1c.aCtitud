/*
 * Comando.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 */

#include "Comando.hpp"
#include <iostream>
using namespace std;



	//	Constructor
	Comando::Comando(void)
	{
		cout << "Holaaaaaaaaaaaasomando.Comando(void)" << endl;

		comando= "chau";

		cout << comando << endl;
		cout << "\n" << endl;
	}

	//	Desctructor
	Comando::~Comando(void)
	{
//		cout << "Destructor: Comando.~Comando(void)" << endl;
	}


	void Comando::setComando(char* unComando){
		comando= unComando;
	}

	char* Comando::getComando(){
		return comando;
	}
