/*
Testea la clase Semaforo
Uso de 1 sem�foro para sincronizar, o sea que no importa el orden de llegada, sino que se
ejecute primero uno y luego el otro.
Hace un fork() para crear 2 procesos. Inicialmente ambos procesos imprimir�n en pantalla
10 n�meros, sin estar sincronizados.
Luego se sincronizan con el sem�foro, y el primero que llegue imprimir� los 20 n�meros de
corrido, y luego lo har� el otro.
*/

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaforo.hpp"

using namespace std;

int main(){

    Semaforo unSem('S');
   
	int pid;
	int i;

    pid=fork();

	if (pid==0) { //proceso padre

		for (i = 0; i<3; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};

        unSem.Wait();

        cout << "- Valor actual unSem " << unSem.ValorActual() << endl;
        cout << "Proceso padre sale sincronizado..." << endl;

		for (i = 0; i<3; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};

        cout << "Terminado proceso padre" << endl;
        //cout << "- Valor actual unSem " << unSem.ValorActual() << endl;
        unSem.Signal();
	}
	else { //proceso hijo

        Semaforo otroSem('S'); //deber�a devolver el mismo

		for (i = 0; i<3; i++) {

			cout << " Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		}

        otroSem.Wait();
        cout << "- Valor actual otroSem " << otroSem.ValorActual() << endl;
        cout << "Proceso hijo sale sincronizado..." << endl;

		for (i = 0; i<3; i++) {
			cout << " Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};

        cout << "Terminado proceso hijo" << endl;
		otroSem.Signal();
        sleep(1);
        cout << "- Valor actual otroSem " << otroSem.ValorActual() << endl;
	}
}
