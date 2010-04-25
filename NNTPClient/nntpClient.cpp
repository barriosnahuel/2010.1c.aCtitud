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
#include "NNTPClientDAO.h"
#include <stdlib.h>
#include "Semaforo.h"
#include "NNTPClientDAO.hpp"
using namespace std;

int inicializarDAO(void);
bool crearThreadDeUI(Comando* param);
void* threadInterfazDeUsuario(void* parametro);

int EXIT_OK= 1;
int EXIT_ERROR= 0;

Semaforo semaforoConexion;
Semaforo semaforoUI;


bool crearThreadDeUI(Comando* param){
	pthread_t threadUI;//	Declaro un nuevo thread.
	//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread.

	return !pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, (void*)param);
}



/**
 * Esta funcion es la que seria el "main" del nuevo thread que creamos encargado de la interfaz de usuario.
 */
void* threadInterfazDeUsuario(void* parametro){
	 Comando* comando = ((Comando*)parametro);//	Casteo el parametro a Comando* asi comparto el recurso entre los hilos.

	while(true){//	loop infinito porque necesito que corra todo el tiempo y nunca llegue al final de la funcion.
		semaforoUI.setEstasOcupado(true);	//	enterCritical.

		//	Aca trabajo con los recursos compartidos.
		cout << "Ingrese un comando:" << endl;
		string cadenaIngresadaPorElUsuario;
		getline(cin, cadenaIngresadaPorElUsuario, '\n'); //Ya que cin corta la cadena

		(*comando).init(cadenaIngresadaPorElUsuario);
//		cout << "La cadena es: " << (*comando).getCadenaIngresada() << endl;
//		cout << "El nombre del comando es: " << (*comando).getNombreComando() << endl;
//		cout << "El parametro es: " << (*comando).getParametro() << endl;

		semaforoUI.setEstasOcupado(false);//	exitCritical.
		//	Espero dos segundos, porque es un tiempo considerable (creo) para que la cpu comience a trabajar con el otro hilo
		//	y este otro hilo setee bloquee el semaforo.
		sleep(1);

		while(semaforoConexion.estasOcupado())
			;//	Con este while y la sentencia nula me quedo esperando hasta poder acceder al recurso compartido.
		semaforoUI.setEstasOcupado(true);	//	enterCritical.

		cout << (*comando).getRespuestaAlUsuario() << endl;
		cout << "--------------------------------------------" << endl;
		(*comando).reset();
	}

	pthread_exit(comando);
}

int main(void){
//	cout << "* Iniciando NNTPClient v0.4..." << endl;
//	sleep(1);

	NNTPClientDAO dao;
	Comando comando;//	Recurso que voy a compartir entre los threads.

    semaforoUI.setEstasOcupado(true);//	Seteando este semaforo como ocupado, primero voy a poder leer los datos por consola.

    if(crearThreadDeUI(&comando)){
    	//	Si estoy aca es porque se pudo crear correctamente el nuevo thread.

   	    //	NBarrios-TODO: Conectar un servidor NNTP (ej: nntpd) por medio de un canal seguro (openSSL)

    	while(semaforoUI.estasOcupado())
    		;//	Con este while y la sentencia nula me quedo esperando hasta poder acceder al recurso compartido.
		semaforoConexion.setEstasOcupado(true);	//	enterCritical.
   	    while(comando.getNombreComando().compare("QUIT")!=0){

   	    	//	NBarrios-TODO: Envio el comando al nntp server y obtengo la respuesta al usuario.
//   	    	string respuesta= dao.enviarMensaje(comando.getCadenaIngresada());
   	    	string respuesta= "Aca se supone que mande el request, me respondio el server, y lo que estoy mostrando ahora es la rta.";

			comando.setRespuestaAlUsuario(respuesta);
			semaforoConexion.setEstasOcupado(false);
			sleep(1);

			while(semaforoUI.estasOcupado())
				;
			semaforoConexion.setEstasOcupado(true);
   	    }

   	    //	NBarrios-TODO: Cierro la conexion.
   	    //	NBarrios-TODO: Libero la memoria que haya pedido.

   	    cout << "\n-------------------------------" << endl;
   	    cout << "-- Gracias por usar NNTPClient." << endl;
		return EXIT_SUCCESS;//	Termino la aplicacion.
    }

    //	Si llego a este punto, es porque no se pudo crear el thread (ver si hay algun otro motivo)
	cout << "Se ha producido un error y la aplicacion no puede ejecutarse. Comuniquese con aCtitud." << endl;
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
