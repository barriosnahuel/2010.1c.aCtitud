#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "../Logger/logger.hpp"
#include "Configuracion.hpp"
#include "Comando.hpp"
#include "NNTPClientDAO.hpp"

#define EXIT_OK 1
#define EXIT_ERROR 0

using namespace std;

/* semáforos globales para ui y dao */
pthread_mutex_t semUI = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semConexion = PTHREAD_MUTEX_INITIALIZER;

/* nombre del proceso */
char czNombreProceso[20];

void* threadInterfazDeUsuario(void* parametro);

/**
 * Función que ejecuta el thread de CLUI.
 */
void* threadInterfazDeUsuario(void* parametro){
    Logger logger;
    logger.LoguearDebugging("--> threadInterfazDeUsuario()", "NNTPClient");

    string strCadenaIngresada;

    //Casteo el parametro a Comando* y comparto el recurso entre los hilos
    Comando* comando = ((Comando*)parametro);

    do {
        cout << "[C] ";
        getline(cin, strCadenaIngresada);
        (*comando).init(strCadenaIngresada);
        pthread_mutex_unlock(&semConexion);
        pthread_mutex_lock(&semUI);
        cout << "[S] " << (*comando).respuestaObtenida() << endl << endl;
    } while ((*comando).indicaSalida() != 0);

    cout << "[Server closes connection.]" << endl;

    pthread_exit(comando);
}

int main(int argn, char *argv[]){
	Logger logger;

    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso, "NNTP_Client\0");
    strcpy(argv[0], czNombreProceso);

    pthread_t threadUI;
    char *rtaHilo = NULL;

    Configuracion confCliente;

    cout << "* Iniciando NNTPClient v1.0..." << endl;

    switch(argn) {
        case 1: // no se indicó archivo, se carga archivo por defecto (czNombreProceso)
            if(confCliente.cargarDefault() == 0) {
                cerr << "Archivo de configuración no válido.\n";
                return EXIT_FAILURE;
            }
            break;
        case 2: // único parámetro: nombre de archivo de configuración
            if(confCliente.cargarDesdeArchivo(argv[1]) == 0) {
                cerr << "Archivo de configuración no válido.\n";
                return EXIT_FAILURE;
            }
            break;
        case 3: // se pasaron los valores por parámetro
            if(confCliente.cargarDesdeParametros(argv[1],atoi(argv[2])) == 0) {
                cerr << "Parámetros no válidos.\n";
                return EXIT_FAILURE;
            }
            break;
        default:
            cerr << "Parámetros no válidos.\n";
            return EXIT_FAILURE;
            break;
    };
/*	ToDo: Terminar esta validacion, estan copiadas exactamente igual de las otras aplicaciones, pero aca no anda.
	Pincha porque encuentra mas de 3 "digitos" en la ultima parte de la IP, es decir, una vez que ya recorrio
	192.168.0.103 el strtok le devuelve uno mas.. y entonces pincha, como si recibiera 192.168.0.1034

	if(!confCliente.Valida_IP(confCliente.getServidor())){
		logger.LoguearError("Archivo de configuracion incorrecto, la IP del NNTP no esta bien formada.", "NNTPClient");
		cout << "ERROR: Archivo de configuracion incorrecto, la IP del NNTP no esta bien formada." << endl;

		cout << "\n\n--\nEl archivo de configuracion contiene errores, corrijalo y vuelva a iniciar la aplicacion." << endl;
		cout << "\n-------------------------------" << endl;
		cout << "-- Gracias por usar NNTPClient." << endl;
		sleep(10);
		return EXIT_FAILURE;
	}
*/


    NNTPClientDAO dao;
    dao.abrirConexion(confCliente.getServidor(), confCliente.getPuerto()); // Abrimos la conexion

    Comando comando; // Recurso que voy a compartir entre los threads.

    pthread_mutex_lock(&semConexion);
    pthread_mutex_lock(&semUI);

    if(pthread_create(&threadUI, NULL, &threadInterfazDeUsuario, (void*) &comando) != 0) {
        // No se pudo crear el thread
        perror("No se pudo crear el thread de UI.");
        pthread_mutex_unlock(&semConexion);
        pthread_mutex_unlock(&semUI);
        return EXIT_FAILURE;
    }

    //Se pudo crear correctamente el nuevo thread.

    do {
        pthread_mutex_lock(&semConexion);
        if(comando.validacion()) {
            dao.enviarMensaje(comando.cadenaIngresada());
            comando.setRespuestaObtenida(dao.recibirRespuesta());
            if(comando.respuestaObtenida() == "") {
            	cout << "Servidor ca�do" << endl;
				//Cierro la conexion.
				dao.cerrarConexion();

				cout << "\n-------------------------------" << endl;
				cout << "-- Gracias por usar NNTPClient." << endl;

				pthread_mutex_unlock(&semConexion);
				pthread_mutex_unlock(&semUI);
				return EXIT_SUCCESS;
            }
        }
        else
        	comando.setRespuestaObtenida("Comando invalido.");
        pthread_mutex_unlock(&semUI);
    } while (comando.indicaSalida() != 0);

    // espero a que termine el thread de la ui
    pthread_join (threadUI, (void **)&rtaHilo);

    //Cierro la conexion.
    dao.cerrarConexion();

    cout << "\n-------------------------------" << endl;
    cout << "-- Gracias por usar NNTPClient." << endl;

    pthread_mutex_unlock(&semConexion);
    pthread_mutex_unlock(&semUI);
    return EXIT_SUCCESS;
}
