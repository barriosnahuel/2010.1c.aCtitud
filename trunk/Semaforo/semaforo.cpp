#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include "semaforo.hpp"

using namespace std;

Semaforo::Semaforo(int intClave) {
    //genero clave �nica para este sem�foro
    kClave = ftok("/bin/bash", intClave);

	//obtenemos el sem�foro deseado
	intIdSemaforo = semget( kClave, 0, 0);

	//si no existe o hubo problema, intentamos crearlo con persimos 0666 (rw-rw-rw-)
    if(!(intIdSemaforo > 0))
		intIdSemaforo = CrearSemaforo(kClave,1);

	//si no se pudo crear, se levanta un error
	if(!(intIdSemaforo > 0))
		perror("No se pudo crear el sem�foro %c.",intClave)
}

Semaforo::Semaforo(int intClave, int intValor) {
    //genero clave �nica para este sem�foro
    kClave = ftok("/bin/bash", intClave);

	//obtenemos el sem�foro deseado
	intIdSemaforo = semget( kClave, 0, 0);

	//si no existe o hubo problema, intentamos crearlo con persimos 0666 (rw-rw-rw-)
    if(!(intIdSemaforo > 0))
		intIdSemaforo = CrearSemaforo(kClave,intValor);

	//si no se pudo crear, se levanta un error
	if(!(intIdSemaforo > 0))
		perror("No se pudo crear el sem�foro %c.",intClave)
}

Semaforo::~Semaforo() {
	//eliminamos el sem�foro del SO
    semctl(intIdSemaforo, 0, IPC_RMID);
}

int Semaforo::CrearSemaforo(key_t kClaveNueva, int intValorInicial) {
	int intId; //identificador

	intId = semget(kClaveNueva, 1, 0666 | IPC_CREAT);

	//si se pudo crear, lo inicializo con intValorInicial
	if(intId >0) {
		stArgumentosSem.val = intValorInicial; //valor inicial para el sem�foro

		//configuro el sem�foro con �ndice 0 a 1
	    semctl(intId, 0, SETVAL, stArgumentosSem);
	}

	return intId;
}

int Semaforo::ValorActual(void) {
	//devuelve el valor actual del sem�foro con indice 0
	return semctl( intIdSemaforo, 0, GETVAL );
}
void Wait(void) {

	struct sembuf stAccion; 

    accion.sem_num = 0; //�ndice del sem�foro
    accion.sem_op = -1; //operaci�n se restan enteros al sem�foro
    accion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho 
    //arreglo. ser�a posible ejecutar varias acciones al mismo tiempo
    semop (intIdSemaforo, &stAccion, 1 );
}
void Signal(void) {

	sembuf stAccion; 

    accion.sem_num = 0; //�ndice del sem�foro
    accion.sem_op = 1; //operaci�n se suman enteros al sem�foro
    accion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho 
    //arreglo. ser�a posible ejecutar varias acciones al mismo tiempo
    semop (intIdSemaforo, &stAccion, 1 );
}