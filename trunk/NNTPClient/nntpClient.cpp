/*
 * Main.cpp
 *
 *  Created on: Apr 19, 2010
 *      Author: Barrios, Nahuel.
 */

#include <iostream>
#include <pthread.h>
using namespace std;

int EXIT_SUCCESS= 1;
int EXIT_FAILURE= 0;

//	Esta clase no es necesaria, pero no se como pasar un string (Struct) como (void*). De todas formas se puede usar agregandole despues un vector de opciones y demas.
class Comando {
	//	Lo privado
    string comando;


public:
	string getComando(void){
		return comando;
	}
	void setComando(string cadena){
//		command= newCadena; //	NBarrios-TODO:Ver como trabajar con los string, tira siempre segmentation fault.
	}

	//	Constructor
	Comando(void)
	{
		cout << "Constructor: Comando.Comando(void)" << endl;
	}

	//	Desctructor
	~Comando(void)
	{
		cout << "Destructor: Comando.~Comando(void)" << endl;
	}
};

int inicializarDAO(void){
	//	NBarrios-TODO: Inicializar DAO.

	return EXIT_SUCCESS;
}

int inicializarBO(void){
	//	NBarrios-TODO: Inicializar BO.

	return EXIT_SUCCESS;
}

int obtenerComandoDelUsuario(string* comando){
	cout << "Ingrese un comando: " << endl;
	cin >> *comando;

	//	NBarrios-TODO: Aca tendríamos que ver validar errores, tiempo, y varias cosas mas que en este momento no se me ocurren, y ver si tengo que retornar por bien o por mal

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

	return !pthread_create(&thread, NULL, &funcionSegundoThread, &param);
}

int main(){
	string comando;

	if(inicializarDAO())
		if(inicializarBO())
			if(obtenerComandoDelUsuario(&comando)){
				Comando comandoIngresado;
				comandoIngresado.setComando(comando);
				//	NBarrios-TODO: Aca también tengo que setear en el objeto command todas las opciones y demas. Pero primero ver si no hay algo ya hecho!

				if(crearSegundoThread(comandoIngresado)){
					cout << "Esto es del hilo uno, y no hubo errores al crear el hilo 2!" << endl;

					return EXIT_SUCCESS;
				}
			}

	cout << "Se ha producido un error y la aplicacion no puede ejecutarse. Comuniquese con aCtitud." << endl;
	return EXIT_FAILURE;
}
