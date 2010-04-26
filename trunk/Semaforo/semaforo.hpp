#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
	// La union ya está definida en sys/sem.h
#else
	// Tenemos que definir la union
	union semun {
		int val;
		struct semid_ds *buf;
		unsigned short int *array;
		struct seminfo *__buf;
	};
#endif

class Semaforo {
	private:
		key_t kClave;
	    int intIdSemaforo;
		union semun stArgumentosSem;

		int CrearSemaforo(key_t kClaveNueva, int intValorInicial);
	public:
		Semaforo(int intClave);
		Semaforo(int intClave, int intValor);
		virtual ~Semaforo();
		void Wait(void);
		void Signal(void);
		int ValorActual(void);
        int EliminarSemaforo(void);
};

#endif /* SEMAFORO_H_ */
