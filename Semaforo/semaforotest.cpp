/*
Testea la clase Semaforo
Uso de 1 sem�foro para sincronizar, o sea que no importa el orden de llegada, sino que se
ejecute primero uno y luego el otro.
Hace un fork() para crear 2 procesos. Inicialmente ambos procesos imprimir�n en pantalla
10 n�meros, sin estar sincronizados.
Luego se sincronizan con el sem�foro, y el primero que llegue imprimir� los 20 n�meros de
corrido, y luego lo har� el otro.
*/

#include <stdio.h>
#include "semaforo.hpp"

int main(){

	Semaforo unSem.Semaforo('S');

	int pid;
	int i;

    pid=fork();

	if (pid==0) { //proceso padre

		for (i = 0; i<10; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};

		sleep(2);

		unSem.Wait();
		for (i = 0; i<10; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};
		unSem.Signal();
	}
	else { //proceso hijo

		for (i = 0; i<10; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		}

		sleep(2);

		unSem.Wait();
		for (i = 0; i<10; i++) {
			cout << "Soy el proceso " << getpid() << " en " << i << endl;
			sleep (1);
		};
		unSem.Signal();
	}
}
