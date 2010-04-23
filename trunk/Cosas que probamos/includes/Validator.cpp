/*
 * Validator.cpp
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

#include "Validator.h"
#include "Comando.hpp"
#include <iostream>
using namespace std;


Validator::Validator() {
	// TODO Auto-generated constructor stub

}

Validator::~Validator() {
	// TODO Auto-generated destructor stub
}

bool Validator::validameEsta(int i){
	Comando unComando;
	unComando.setComando("unComando");
	cout << unComando.getComando() << endl;
	return true;
}
