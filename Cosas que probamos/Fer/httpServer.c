#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>

#define PORT 15000 /* El puerto que ser� abierto */
#define BACKLOG 3 /* El numero de conexiones permitidas */ //	TODO: Aca no tendriamos que poner por lo menos 20?
int thr_create(void *stack_base, size_t stack_size, void *(*start_routine) (void *), void *arg, long flags, thread_t *new_thread); 

int procesarRequestFuncionThread(int ficheroCliente) {
	printf(
			"Lalala, estoy en la funcion del thread con el fichero del cliente nro: %d\n",
			ficheroCliente);
	return 1;
}

int main() {
	int ficheroServer; /* los ficheros descriptores */
	int sin_size;//	TODO: Esto hace falta declararlo aca? Que es?
	struct sockaddr_in server; /* para la informacion de la direccion del servidor */
	printf("Acabo de entrar al main\n");

	if ((ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error al crear el socket.\n");
		exit(-1);
	}
	printf("Cree el socket bien\n");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra direcci�n IP autom�ticamente */
	server.sin_port = htons(PORT); /* htons transforma el short de maquina a short de red */
	bzero(&(server.sin_zero), 8); /* escribimos ceros en el reto de la estructura*/

	if (bind(ficheroServer, (struct sockaddr *) &server,
			sizeof(struct sockaddr)) == -1) {
		printf("Error al asociar el puerto al socket.\n");
		exit(-1);
	}
	printf("Pude bindear bien el socket\n");

	if (listen(ficheroServer, BACKLOG) == -1) {
		printf("Error al escuchar por el puerto.\n");
		exit(-1);
	}
	printf("Escuchando conexiones en el puerto %d.\n", PORT);

	sin_size = sizeof(struct sockaddr_in);

	while (1) {
		struct sockaddr_in client; /* para la informaci�n de la direcci�n del cliente */

		//	1. Obtengo una conexion que esta pendiente en la cola de entrada, y obtengo un descriptor de fichero de socket nuevo
		//	exclusivo de esa conexion.
		//	2. Declaro recien en este momento a ficheroCliente, porque preciso uno por cada conexion. Estaba declarado en el main.
		int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client,
				&sin_size);
		if (ficheroCliente != -1) {
			//	Si no hubo errores aceptando la conexion, entonces la gestiono.

			thread_t threadProcesarRequest;//	Declaro un nuevo thread.
			//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread, si es que hay que setearle algo.

			//	En Solaris!!
			if (thr_create(0, 0, &procesarRequestFuncionThread,
					(void*) ficheroCliente, 0, threadProcesarRequest) != 0)
				printf(
						"No se pudo crear un nuevo thread para procesar el request.\n");
		}
		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(
				client.sin_addr));
	}

	printf("Chao chao!\n");
	return 1;
}
