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
//#include "util.h"
using namespace std;

int EXIT_OK= 1;
int EXIT_ERROR= 0;

int inicializarDAO(void){
	NNTPClientDAO dao;
	return EXIT_OK;
}


int inicializarBO(void){
	NNTPClientBO bo;
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

		//	NBarrios-TODO: aca hago el enterCritical que dijo Scarfiello para sincronizar hilos.
		//	En este espacio, solo este hilo de ejecucion puede acceder a comando.

//		(*comando).cargateDesdeUnString(cadenaIngresadaPorElUsuario);	//	Esto es el anterior parservalidator que ahora esta adentro de Comando.
//		cout << "Pruebo el parseo:" << endl;
//		cout << (*comando).getNombreComando() << endl;

		//	NBarrios-TODO: aca hago el exitCritical que dijo Scarfiello para sincronizar hilos.

		//	NBarrios-TODO: Muestro la respuesta.
//		cout << (*comando).getRespuestaAlUsuario() << endl;
		cout << "--------------------------------------------" << endl;
	}

	pthread_exit(comando);
}

bool crearThreadDeUI(Comando* param){
	pthread_t threadUI;//	Declaro un nuevo thread.
	//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread.

	return true;
//	return !pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, &param);
}


int main(void){
    Comando comando;//	Recurso que voy a compartir entre los threads.

    if(crearThreadDeUI(&comando)){
    	//	Si estoy aca es porque se pudo crear correctamente el nuevo thread.

    	inicializarDAO();
   	    //inicializarBO();//Hace falta?

   	    //	NBarrios-TODO: Conectar un servidor NNTP (ej: nntpd) por medio de un canal seguro (openSSL)



   	    //	NBarrios-TODO: aca hago el enterCritical que dijo Scarfiello para sincronizar hilos.
   	    string nombreDelComandoLeido=comando.getNombreComando();
   	    //	NBarrios-TODO: aca hago el exitCritical que dijo Scarfiello para sincronizar hilos.



   	    //	NBarrios-TODO: Ver si esta bien cortar el programa de esta manera cuando el usuario ingresa quit.
   	    //	Tener en cuenta que el comando se setea en el otro thread y va a estar sincronizado por eso andar. creo.
   	    while(nombreDelComandoLeido.compare("QUIT")!=0){

   	    	//	NBarrios-TODO: Envio el comando al nntp server y obtengo la respuesta al usuario.
   	    	string respuesta= "No existe esa noticia campeon";


   			//	NBarrios-TODO: aca hago el enterCritical que dijo Scarfiello para sincronizar hilos.
   			//	En este espacio, solo este hilo de ejecucion puede acceder a comando.
   	    	comando.setRespuestaAlUsuario(respuesta);

   			//	NBarrios-TODO: aca hago el exitCritical que dijo Scarfiello para sincronizar hilos.

   	    	nombreDelComandoLeido= comando.getNombreComando();
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
