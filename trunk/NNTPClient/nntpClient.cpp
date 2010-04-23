/*
 * nntpClient.cpp
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

#include <iostream>
#include <pthread.h>
#include "Comando.hpp"
#include "ParseadorValidadorDeComandos.h"
#include "NNTPClientBO.h"
#include "NNTPClientDAO.h"
using namespace std;

int EXIT_SUCCESS= 1;
int EXIT_FAILURE= 0;

int inicializarDAO(void){
	//	NBarrios-TODO: Inicializar DAO.

	NNTPClientDAO dao;
	dao.funcionDePrueba();

	return EXIT_SUCCESS;
}


int inicializarBO(void){
	//	NBarrios-TODO: Inicializar BO.

	NNTPClientBO bo;
	bo.funcionDePrueba();

	return EXIT_SUCCESS;
}

void* funcionSegundoThread(void* unParametro){
	cout << "Bienvenido al hilo 2." << endl;
	cout << "No tenemos esa noticia maquina, ponete las pilas! (hardcoded, tengo que terminarlo)" <<endl;

	pthread_exit(unParametro);
}

int crearSegundoThread(Comando param){
	pthread_t thread;//	Declaro un nuevo thread.
	//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread.

	return true;
//	return !pthread_create(&thread, NULL, &funcionSegundoThread, &param);
}


int main(void){
	inicializarDAO();
    inicializarBO();

    cout << "Pruebo el parser" << endl;
    ParseadorValidadorDeComandos parser;
    parser.funcionDePrueba();
    cout << "Termine de probar el parser" << endl;

    cout << "Pruebo el comando" << endl;
    Comando comando;
    comando.funcionDePrueba();
    cout << "Termine de probar el comando" << endl;

//	Comando comandoIngresado;//= prueba();// = validarYConvertirAObjetoComando();
//	comandoIngresado.setNombreComando("ARTICLE");
//	cout << comandoIngresado.getNombreComando() << endl;
//    if(crearSegundoThread(comandoIngresado)){
//        cout << "Esto es del hilo uno, y no hubo errores al crear el hilo 2!" << endl;
//		return EXIT_SUCCESS;
//	}

	cout << "Se ha producido un error y la aplicacion no puede ejecutarse. Comuniquese con aCtitud." << endl;
	return EXIT_FAILURE;
}

//#include <iostream>
//#include "Comando.hpp"
//#include "Validator.h"
//using namespace std;
//
//int main(void){
//	cout << "Loading..." << endl;
//
//	Comando comando;
//	comando.setComando("mira vos che!");
//
//	cout << comando.getComando() << endl;
//
//	cout << "ahora uso la clase Validator" << endl;
//
//	Validator unValidator;
//	cout << unValidator.validameEsta(2) << endl;
//
//	cout << "Ending..." << endl;
//	return 0;
//}
