#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include "Comando.hpp"
#include "NNTPClientDAO.hpp"
#include "semaforo.hpp"

#define EXIT_OK 1
#define EXIT_ERROR 0

using namespace std;

int crearThreadDeUI (Comando* param);
void* threadInterfazDeUsuario(void* parametro);


int crearThreadDeUI (Comando* param) {

    pthread_t threadUI;

    return pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, (void*)param);
}


/**
 * Función que ejecuta el thread de CLUI.
 */
void* threadInterfazDeUsuario(void* parametro){
	Semaforo semConexion('C');
	Semaforo semUI('U');
    string strCadenaIngresada;

	Comando* comando = ((Comando*)parametro);
    //Casteo el parametro a Comando* y comparto el recurso entre los hilos

	while(true){
		
		cout << "[C]: ";
		getline(cin, strCadenaIngresada);

        semUI.Wait();
		(*comando).init(strCadenaIngresada);
        semConexion.Signal();

		semUI.Wait();
		cout << "[S]: " << (*comando).respuestaObtenida() << endl << endl;
        (*comando).reset();
        semConexion.Signal();
	}

	pthread_exit(comando);
}

int main(void){
	cout << "* Iniciando NNTPClient v1.0..." << endl;

	Semaforo semaforoConexion('C',0);
	Semaforo semaforoUI('U',1); //la ui ejecuta primero


	NNTPClientDAO dao;
	//Abrimos la conexion.
	dao.abrirConexion();

	Comando comando;//	Recurso que voy a compartir entre los threads.

    if(!crearThreadDeUI(&comando)) {
        // No se pudo crear el thread
        LogError("No se pudo crear el thread de UI.");
        perror("No se pudo crear el thread de UI.");
    	semaforoConexion.EliminarSemaforo();
    	semaforoUI.EliminarSemaforo();
	    return EXIT_FAILURE;
    }

    //Se pudo crear correctamente el nuevo thread.

	semaforoConexion.Wait();
	while(comando.getNombreComando().compare("QUIT")!=0){

    	string respuesta= dao.enviarMensaje(comando.getCadenaIngresada());

		comando.setRespuestaAlUsuario(respuesta);

		semaforoUI.Signal(); 
		semaforoConexion.Wait();
	}


	//Cierro la conexion.
	dao.cerrarConexion();

	cout << "\n-------------------------------" << endl;
	cout << "-- Gracias por usar NNTPClient." << endl;

    semaforoConexion.EliminarSemaforo();
	semaforoUI.EliminarSemaforo();
	return EXIT_SUCCESS;
}

