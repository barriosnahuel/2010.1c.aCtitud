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
#include <stdlib.h>
#include "util.h"
using namespace std;

int EXIT_OK= 1;
int EXIT_ERROR= 0;

Semaforo semaforoConexion;
Semaforo semaforoUI;


int inicializarDAO(void){
	NNTPClientDAO dao;
	return EXIT_OK;
}


/**
 * Esta funcion es la que seria el "main" del nuevo thread que creamos encargado de la interfaz de usuario.
 */
void* threadInterfazDeUsuario(void* parametro){
	 Comando* comando = (Comando*)parametro;//	Casteo el parametro a Comando* asi comparto el recurso entre los hilos.

	while(true){//	loop infinito porque necesito que corra todo el tiempo y nunca llegue al final de la funcion.
		cout << "Ingrese un comando:" << endl;
		string cadenaIngresadaPorElUsuario;
		cin >> cadenaIngresadaPorElUsuario;

		semaforoUI.setEstasOcupado(true);	//	enterCritical.

		//	Aca trabajo con los recursos compartidos.
//			(*comando).cargateDesdeUnString(cadenaIngresadaPorElUsuario);	//	Esto es el anterior parservalidator que ahora esta adentro de Comando.
		cout << "Pruebo el parseo:" << endl;
		cout << (*comando).getNombreComando() << endl;

		semaforoUI.setEstasOcupado(false);//	exitCritical.
		//	Espero dos segundos, porque es un tiempo considerable (creo) para que la cpu comience a trabajar con el otro hilo
		//	y este otro hilo setee bloquee el semaforo.
		sleep(2);


		while(semaforoConexion.estasOcupado())
			;//	Con este while y la sentencia nula me quedo esperando hasta poder acceder al recurso compartido.
		semaforoUI.setEstasOcupado(true);	//	enterCritical.

		cout << (*comando).getRespuestaAlUsuario() << endl;
		cout << "--------------------------------------------" << endl;
	}

	pthread_exit(comando);
}

bool crearThreadDeUI(Comando* param){
	pthread_t threadUI;//	Declaro un nuevo thread.
	//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread.

	return false;
//	return !pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, &param);
}


int main(void){
	cout << "* Iniciando NNTPClient v0.4..." << endl;
	sleep(1);
	cout << "." << endl;
	sleep(1);
	cout << "." << endl;
	sleep(1);
	cout << "." << endl;

    Comando comando;//	Recurso que voy a compartir entre los threads.
    semaforoUI.setEstasOcupado(true);//	Seteando este semaforo como ocupado, primero voy a poder leer los datos por consola.

    if(crearThreadDeUI(&comando)){
    	//	Si estoy aca es porque se pudo crear correctamente el nuevo thread.

    	inicializarDAO();
   	    //inicializarBO();//Hace falta?

   	    //	NBarrios-TODO: Conectar un servidor NNTP (ej: nntpd) por medio de un canal seguro (openSSL)

    	while(semaforoUI.estasOcupado())
    		;//	Con este while y la sentencia nula me quedo esperando hasta poder acceder al recurso compartido.
		semaforoConexion.setEstasOcupado(true);	//	enterCritical.

   	    //	NBarrios-TODO: Ver si esta bien cortar el programa de esta manera cuando el usuario ingresa quit.
   	    //	Tener en cuenta que el comando se setea en el otro thread y va a estar sincronizado por eso andar. creo.
   	    while(dosStringsSonIguales(comando.getNombreComando(), "QUIT")){

   	    	//	NBarrios-TODO: Envio el comando al nntp server y obtengo la respuesta al usuario.
   	    	string respuesta= "No existe esa noticia campeon";

			comando.setRespuestaAlUsuario(respuesta);
			semaforoConexion.setEstasOcupado(false);
			sleep(2);

			while(semaforoUI.estasOcupado())
				;
			semaforoConexion.setEstasOcupado(true);
   	    }

   	    //	NBarrios-TODO: Cierro la conexion.
   	    //	NBarrios-TODO: Libero la memoria que haya pedido.

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
