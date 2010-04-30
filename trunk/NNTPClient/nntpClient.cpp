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
        cout << "-----ui 1" << endl;
		cout << "[C]: ";
		getline(cin, strCadenaIngresada);
        cout << "-----ui 2" << endl;
		(*comando).init(strCadenaIngresada);
        cout << "-----ui 3" << endl;
		semConexion.Signal();
		cout << "-----ui 4" << endl;
        semUI.Wait();
        cout << "-----ui 5" << endl;
		cout << "[S]: " << (*comando).respuestaObtenida() << endl << endl;
        cout << "-----ui 6  ----- VUELTA UI" << endl;
	} while ((*comando).indicaSalida() != 0);
    cout << "-----ui 7 ---- CHAU UI" << endl;
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
        cout << "--conex 1" << endl;
		semaforoConexion.Wait();
        cout << "--conex 2" << endl;
    	dao.enviarMensaje(comando.cadenaIngresada());
        cout << "--conex 3" << endl;
		comando.setRespuestaObtenida(dao.recibirRespuesta());
        cout << "--conex 4" << endl;
		semaforoUI.Signal();
        cout << "--conex 5 ------- VUELTA CONEX" << endl;
	} while (comando.indicaSalida() != 0);
    cout << "--conex 6" << endl;
    semaforoUI.Signal();
    cout << "--conex 7" << endl;
    pthread_join (threadUI, (void **)&rtaHilo);
    cout << "--conex 8 --------CHAU CONEX" << endl;

	//Cierro la conexion.
	dao.cerrarConexion();

	cout << "\n-------------------------------" << endl;
	cout << "-- Gracias por usar NNTPClient." << endl;

    semaforoConexion.EliminarSemaforo();
	semaforoUI.EliminarSemaforo();
	return EXIT_SUCCESS;
}

