/*
 * nntpClient.cpp
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

#include <iostream>
#include <pthread.h>
#include <stdlib.h>

#include "Comando.hpp"
#include "NNTPClientDAO.hpp"
#include "semaforo.hpp"

using namespace std;

int   inicializarDAO(void);
bool  crearThreadDeUI(Comando* param);
void* threadInterfazDeUsuario(void* parametro);

int EXIT_OK= 1;
int EXIT_ERROR= 0;

bool crearThreadDeUI(Comando* param){
	pthread_t threadUI;//	Declaro un nuevo thread.
	//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread.

	return !pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, (void*)param);
}


/**
 * Esta funcion es la que seria el "main" del nuevo thread que creamos encargado de la interfaz de usuario.
 */
void* threadInterfazDeUsuario(void* parametro){
	Semaforo semConexion('C');
	Semaforo semUI('U');

	Comando* comando = ((Comando*)parametro);//	Casteo el parametro a Comando* asi comparto el recurso entre los hilos.

	semUI.Wait();
	while(true){//	loop infinito porque necesito que corra todo el tiempo y nunca llegue al final de la funcion.
		//	Aca trabajo con los recursos compartidos.

		cout << "Ingrese un comando:" << endl;
		string cadenaIngresadaPorElUsuario;
		getline(cin, cadenaIngresadaPorElUsuario, '\n'); //	Ya que cin corta la cadena

		(*comando).init(cadenaIngresadaPorElUsuario);//	Parseo la cadena, la valido, y seteo los atributos correspondientes del Comando.

		semConexion.Signal();
		semUI.Wait();

		cout << (*comando).getRespuestaAlUsuario() << endl;
		cout << "--------------------------------------------" << endl;
		(*comando).reset();//	Reseteo los atributos antes de iniciar una nueva vuelta asi no queda basura.
	}

	pthread_exit(comando);
}

int main(void){
	cout << "* Iniciando NNTPClient v0.4..." << endl;

	Semaforo semaforoConexion('C',0);
	Semaforo semaforoUI('U',1); //la ui ejecuta primero


	NNTPClientDAO dao;
	//Abrimos la conexion.
	dao.abrirConexion();

	Comando comando;//	Recurso que voy a compartir entre los threads.

    if(crearThreadDeUI(&comando)){
    	//	Si estoy aca es porque se pudo crear correctamente el nuevo thread.

		semaforoConexion.Wait(); /****** enter critical ******/
		while(comando.getNombreComando().compare("QUIT")!=0){

    		//	NBarrios-TODO: Envio el comando al nntp server y obtengo la respuesta al usuario.
    		string respuesta= dao.enviarMensaje(comando.getCadenaIngresada());

			comando.setRespuestaAlUsuario(respuesta);

			semaforoUI.Signal(); /****** exit critical ******/
			semaforoConexion.Wait();
		}


		//Cierro la conexion.
		dao.cerrarConexion();
		//	NBarrios-TODO: Libero la memoria que haya pedido.

		cout << "\n-------------------------------" << endl;
		cout << "-- Gracias por usar NNTPClient." << endl;
		semaforoConexion.EliminarSemaforo();
		semaforoUI.EliminarSemaforo();
		return EXIT_SUCCESS;//	Termino la aplicacion.
    }

    //	Si llego a este punto, es porque no se pudo crear el thread (ver si hay algun otro motivo)
	cout << "Se ha producido un error y la aplicacion no puede ejecutarse. Comuniquese con aCtitud." << endl;
	semaforoConexion.EliminarSemaforo();
	semaforoUI.EliminarSemaforo();
	return EXIT_FAILURE;
}



//--------------------------------------------------------------------------------------------------
//	Este main es para probar funciones y demas cosas que no tengan que ver con toda la aplicacion. |
//	Se le cambia el nombre al main original asi no tira error y se prueba con este Main.		   |
//--------------------------------------------------------------------------------------------------

//#include <iostream>
//#include "Comando.hpp"
//#include "Validator.h"
//using namespace std;
//
int main1(void){
	Comando comando;
	cout << "Ingresa el comando:" << endl;
	string  loQueEscribio;
	getline(cin, loQueEscribio, '\n'); //Ya que cin corta la cadena

	comando.init(loQueEscribio);

	cout << "La cadena es: " << comando.getCadenaIngresada() << endl;
	cout << "El nombre del comando es: " << comando.getNombreComando() << endl;
	cout << "El parametro es: " << comando.getParametro() << endl;

	cout << "Ending..." << endl;
	return 0;
}
