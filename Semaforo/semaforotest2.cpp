/*
Testea la clase Semaforo
Uso de 2 semáforos para sincronizar, o sea que importa el orden de llegada.
Hace un fork() para crear 2 procesos. Imprimen sincronizadamente los nros
del 0 al 2 3 veces, empezando el proceso hijo.
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

    Semaforo semPadre('P',0);
    Semaforo semHijo('H',1);
   
	int pid;
	int i,j;

    cout << "semPadre " << semPadre.ValorActual() << endl;
    cout << "semHijo " << semHijo.ValorActual() << endl;

    pid=fork();

	if (pid==0) { //proceso padre

		for (i = 0; i<3; i++) {
            semPadre.Wait();
            for (j = 0; j<3; j++) {
    			cout << "Soy el padre " << getpid() << " - " << j << endl;
    			sleep (1);
            };
            semHijo.Signal();
		};
        semHijo.EliminarSemaforo();
	}
	else { //proceso hijo

		for (i = 0; i<3; i++) {
            semHijo.Wait();
            for (j = 0; j<3; j++) {
	    		cout << " Soy el hijo " << getpid() << " - " << j << endl;
    			sleep (1);
            };
            semPadre.Signal();
		};
        semPadre.EliminarSemaforo();
	}
}
