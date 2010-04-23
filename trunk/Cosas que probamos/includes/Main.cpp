/*
 * Main.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 */

#include <iostream>
#include "Comando.hpp"
#include "Validator.h"
using namespace std;

int main(void){
	cout << "Loading..." << endl;

	Comando comando;
	comando.setComando("mira vos che!");

	cout << comando.getComando() << endl;

	cout << "ahora uso la clase Validator" << endl;

	Validator unValidator;
	cout << unValidator.validameEsta(2) << endl;

	cout << "Ending..." << endl;
	return 0;
}
