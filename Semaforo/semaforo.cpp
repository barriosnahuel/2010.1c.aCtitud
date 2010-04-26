#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <iostream>
#include <stdio.h>
#include "semaforo.hpp"

using namespace std;

Semaforo::Semaforo(int intClave) {
    //genero clave única para este semáforo
    kClave = ftok("/bin/bash", intClave);

    //si falla al obtener la clave
    if(kClave < 0)
        perror("Error al generar la clave para el semáforo.");

	//obtenemos el semáforo deseado
	intIdSemaforo = semget( kClave, 0, 0);

    //si no existe o hubo problema, intentamos crearlo con permisos 0666 (rw-rw-rw-)
    if(!(intIdSemaforo > 0))
		intIdSemaforo = CrearSemaforo(kClave,1);

	//si no se pudo crear, se levanta un error
	if(intIdSemaforo < 0)
		perror("No se pudo crear el semáforo.");
};

Semaforo::Semaforo(int intClave, int intValor) {
    //genero clave única para este semáforo
    kClave = ftok("/bin/bash", intClave);

    //si falla al obtener la clave
    if(kClave < 0)
        perror("Error al generar la clave para el semáforo.");

    //obtenemos el semáforo deseado
	intIdSemaforo = semget( kClave, 0, 0);

	//si existe lo eliminamos e intentamos crearlo con
    //persimos 0666 (rw-rw-rw-)
    if(intIdSemaforo > 0) {
        semctl(intIdSemaforo, 0, IPC_RMID);
        intIdSemaforo = CrearSemaforo(kClave,intValor);
    };

	//si no se pudo crear, se levanta un error
	if(intIdSemaforo < 0)
		perror("No se pudo crear el semáforo.");
};

Semaforo::~Semaforo() {
	/* No hace nada.
     * El semáforo se elimina forzosamente si hace falta */
};

int Semaforo::EliminarSemaforo(void) {
    /* Elimino el semáforo.
     * no se hace en el destructor porque puede morir
     * la instancia, pero el semáforo seguir en uso
     * por el SO u otro proceso. */
    return semctl(intIdSemaforo, 0, IPC_RMID);
}

int Semaforo::CrearSemaforo(key_t kClaveNueva, int intValorInicial) {
	int intId; //identificador

    intId = semget(kClaveNueva, 1, 0666 | IPC_CREAT);

	//si se pudo crear, lo inicializo con intValorInicial
	if(!(intId < 0)) {
		stArgumentosSem.val = intValorInicial;
        //configuro el semáforo con índice 0 a 1
	    semctl(intId, 0, SETVAL, stArgumentosSem);
	}

	return intId;
};

int Semaforo::ValorActual(void) {
	/* devuelve el valor actual del semáforo con indice 0
    parece no funcionar bien porque no devuelve el valor
    actualizado */
	return semctl( intIdSemaforo, 0, GETVAL );
};

void Semaforo::Wait(void) {

	struct sembuf stAccion;

    stAccion.sem_num = 0; //índice del semáforo
    stAccion.sem_op = -1; //operación resta enteros al semáforo
    stAccion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho
    //arreglo. sería posible ejecutar varias acciones al mismo tiempo
    semop ( intIdSemaforo, &stAccion, 1 );
};

void Semaforo::Signal(void) {

	sembuf stAccion;

    stAccion.sem_num = 0; //índice del semáforo
    stAccion.sem_op = 1; //operación suma enteros al semáforo
    stAccion.sem_flg = 0;

    //identificador, arreglo de sembuf y cantidad de elementos de dicho
    //arreglo. sería posible ejecutar varias acciones al mismo tiempo
    semop ( intIdSemaforo, &stAccion, 1 );
};

