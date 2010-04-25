#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include "semaforo.hpp"

using namespace std;

Semaforo::Semaforo(int intClave) {
    //genero clave única para este semáforo
    kClave = ftok("/bin/bash", intClave);

	//obtenemos el semáforo deseado
	intIdSemaforo = semget( kClave, 0, 0);

	//si no existe o hubo problema, intentamos crearlo con persimos 0666 (rw-rw-rw-)
    if(!(intIdSemaforo > 0))
		intIdSemaforo = CrearSemaforo(kClave,1);

	//si no se pudo crear, se levanta un error
	if(!(intIdSemaforo > 0))
		perror("No se pudo crear el semáforo %c.",intClave)
}

Semaforo::Semaforo(int intClave, int intValor) {
    //genero clave única para este semáforo
    kClave = ftok("/bin/bash", intClave);

	//obtenemos el semáforo deseado
	intIdSemaforo = semget( kClave, 0, 0);

	//si no existe o hubo problema, intentamos crearlo con persimos 0666 (rw-rw-rw-)
    if(!(intIdSemaforo > 0))
		intIdSemaforo = CrearSemaforo(kClave,intValor);

	//si no se pudo crear, se levanta un error
	if(!(intIdSemaforo > 0))
		perror("No se pudo crear el semáforo %c.",intClave)
}

Semaforo::~Semaforo() {
	//eliminamos el semáforo del SO
    semctl(intIdSemaforo, 0, IPC_RMID);
}

int Semaforo::CrearSemaforo(key_t kClaveNueva, int intValorInicial) {
	int intId; //identificador

	intId = semget(kClaveNueva, 1, 0666 | IPC_CREAT);

	//si se pudo crear, lo inicializo con intValorInicial
	if(intId >0) {
		stArgumentosSem.val = intValorInicial; //valor inicial para el semáforo

		//configuro el semáforo con índice 0 a 1
	    semctl(intId, 0, SETVAL, stArgumentosSem);
	}

	return intId;
}

int Semaforo::ValorActual(void) {
	//devuelve el valor actual del semáforo con indice 0
	return semctl( intIdSemaforo, 0, GETVAL );
}
void Wait(void) {

	struct sembuf stAccion; 

    accion.sem_num = 0; //índice del semáforo
    accion.sem_op = -1; //operación se restan enteros al semáforo
    accion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho 
    //arreglo. sería posible ejecutar varias acciones al mismo tiempo
    semop (intIdSemaforo, &stAccion, 1 );
}
void Signal(void) {

	sembuf stAccion; 

    accion.sem_num = 0; //índice del semáforo
    accion.sem_op = 1; //operación se suman enteros al semáforo
    accion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho 
    //arreglo. sería posible ejecutar varias acciones al mismo tiempo
    semop (intIdSemaforo, &stAccion, 1 );
}