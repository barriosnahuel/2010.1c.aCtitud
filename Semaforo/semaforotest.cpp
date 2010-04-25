/*
Testea la clase Semaforo
Uso de 1 semáforo para sincronizar, o sea que no importa el orden de llegada, sino que se
ejecute primero uno y luego el otro.
Hace un fork() para crear 2 procesos. Inicialmente ambos procesos imprimirán en pantalla
10 números, sin estar sincronizados.
Luego se sincronizan con el semáforo, y el primero que llegue imprimirá los 20 números de
corrido, y luego lo hará el otro.
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
