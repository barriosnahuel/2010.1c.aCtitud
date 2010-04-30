#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include "Comando.hpp"
#include "NNTPClientDAO.hpp"
#include "semaforo.hpp"

#define EXIT_OK 1
#define EXIT_ERROR 0

using namespace std;

void* threadInterfazDeUsuario(void* parametro);

/**
 * Función que ejecuta el thread de CLUI.
 */
void* threadInterfazDeUsuario(void* parametro){
	Semaforo semConexion('C');
	Semaforo semUI('U');
    string strCadenaIngresada;

	Comando* comando = ((Comando*)parametro);
    //Casteo el parametro a Comando* y comparto el recurso entre los hilos

	do {
		cout << "[C]: ";
		getline(cin, strCadenaIngresada);
		(*comando).init(strCadenaIngresada);
		semConexion.Signal();
        semUI.Wait();
		cout << "[S]: " << (*comando).respuestaObtenida() << endl << endl;
	} while ((*comando).indicaSalida() != 0);
	pthread_exit(comando);
}

int main(int argn, char *argv[]){
	cout << "* Iniciando NNTPClient v1.0..." << endl;

    pthread_t threadUI;
    char *rtaHilo = NULL;

    Semaforo semaforoConexion('C',0);
	Semaforo semaforoUI('U',0);

	NNTPClientDAO dao;
	dao.abrirConexion(); //Abrimos la conexion.

	Comando comando;//	Recurso que voy a compartir entre los threads.

    if(pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, (void*) &comando) != 0) {
        // No se pudo crear el thread
        //LogError("No se pudo crear el thread de UI.");
        perror("No se pudo crear el thread de UI.");
    	semaforoConexion.EliminarSemaforo();
    	semaforoUI.EliminarSemaforo();
	    return EXIT_FAILURE;
    }

    //Se pudo crear correctamente el nuevo thread.

	do {
		semaforoConexion.Wait();
    	dao.enviarMensaje(comando.cadenaIngresada());
		comando.setRespuestaObtenida(dao.recibirRespuesta());
		semaforoUI.Signal();
	} while (comando.indicaSalida() != 0);
    semaforoUI.Signal();

    // espero a que termine el thread de la ui
    pthread_join (threadUI, (void **)&rtaHilo);

	//Cierro la conexion.
	dao.cerrarConexion();

	cout << "\n-------------------------------" << endl;
	cout << "-- Gracias por usar NNTPClient." << endl;

    semaforoConexion.EliminarSemaforo();
	semaforoUI.EliminarSemaforo();
	return EXIT_SUCCESS;
}

