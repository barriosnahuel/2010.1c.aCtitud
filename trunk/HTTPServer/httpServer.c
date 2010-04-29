/*
 * httpServer.c
 *
 *  Created on: Apr 24, 2010
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>

#define PORT 15000 /* El puerto que ser� abierto */
#define BACKLOG 3 /* El n�mero de conexiones permitidas */ //	TODO: Aca no tendriamos que poner por lo menos 20?

/**
 * Nota: Para Solaris el header se llama asi (thread.h). A diferencia de pthread, los hilos pueden ser gestionados
 * por el kernel directamente.
 */


//	---------------------------
//	Declaraciones de funciones.

void*	procesarRequestFuncionThread(void* parametro);
int		crearSocketYDejarEscuchando(int ficheroServer, struct sockaddr_in server, int sin_size);

//	--------------------------
//	Definiciones de funciones.

int crearSocketYDejarEscuchando(int ficheroServer, struct sockaddr_in server, int sin_size) {
	if ((ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error al crear el socket.\n");
		exit(-1);
	}
	printf("Socket creado.\n");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra direcci�n IP autom�ticamente */
	server.sin_port = htons(PORT); /* htons transforma el short de maquina a short de red */
	bzero(&(server.sin_zero), 8); /* escribimos ceros en el reto de la estructura*/

	if (bind(ficheroServer, (struct sockaddr *) &server,
			sizeof(struct sockaddr)) == -1) {
		printf("Error al asociar el puerto al socket.\n");
		exit(-1);
	}
	printf("Puerto abierto.\n");
	if (listen(ficheroServer, BACKLOG) == -1) {
		printf("Error al escuchar por el puerto.\n");
		exit(-1);
	}
	printf("Escuchando conexiones en el puerto %d.\n", PORT);



	//	TODO: Segun lo que entiendo de la Guia Beej, primero tengo que aceptar, y despues crear el hilo. Con eso alcanza porque
	//			a la funcion le paso el fd y ya puedo hacer el send() que necesito para responderle al cliente.


	//	Aca creo un while para crear threads y aceptar conexiones.

	//	sin_size "Es un puntero a una variable local int a la que deberías asignar el valor de sizeof(struct sockaddr_in)".
	//	Joya, pero que carajo es?
	sin_size = sizeof(struct sockaddr_in);

	while(true){	//	NBarrios-TODO: Check this!
		struct sockaddr_in client; /* para la informaci�n de la direcci�n del cliente */

		//	1. Obtengo una conexion que esta pendiente en la cola de entrada, y obtengo un descriptor de fichero de socket nuevo
		//	exclusivo de esa conexion.
		//	2. Declaro recien en este momento a ficheroCliente, porque preciso uno por cada conexion. Estaba declarado en el main.
		int ficheroCliente= accept(ficheroServer, (struct sockaddr *) &client, &sin_size);
		if(ficheroCliente!=-1){
			//	Si no hubo errores aceptando la conexion, entonces la gestiono.

			pthread_t threadProcesarRequest;//	Declaro un nuevo thread.
			//	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread, si es que hay que setearle algo.

			//	En Solaris!!
			if(thr_create(0, 0, &procesarRequestFuncionThread, (void*)ficheroCliente, 0, threadProcesarRequest)!=0)
				printf("No se pudo crear un nuevo thread para procesar el request.");

			//	En Debian seria asi:
//			pthread_create(&threadProcesarRequest, NULL, &procesarRequestFuncionThread, (void*)ficheroCliente);


			printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
		}
		else
			printf("No se pudo aceptar la conexion.");
	}




	//	Esto es lo que estaba!!
//	sin_size = sizeof(struct sockaddr_in);
//	if ((ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client,
//			&sin_size)) == -1) {
//		printf("Error al aceptar conexi�n.\n");
//		return -1;
//	}

//	printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
	return 1;
}

/**
 * Esta funcion es el "main" del thread que se encarga de procesar el request.
 */
void* procesarRequestFuncionThread(void* parametro){
	int* ficheroCliente= (int*)parametro;
	//	Con este fichero deberia alcanzar para poder responderle al cliente. En realidad esto lo estamos analizando.

	String response= null;
//	if (/*//	TODO: No esta en el cache*/) {
		//	TODO: Lo busco en la DB.

		//	TODO: Guardo en cache.
//	}

	//	Para este momento ya tengo el response seteado.

	//	TODO: Formateo la respuesta a HTML.

	//	TODO: Devuelvo la respuesta por medio del "send(*ficheroCliente,...)" (Ver si en Solaris se llama igual)

	close(*ficheroCliente); //	NBarrios-TODO: Chequear si se llama igual en Solaris! (Estoy cerrando el socket).

	thr_exit(0);//	Termino el thread.
//	return parametro;	//	NBarrios-TODO: Esto hace falta? o con llamar thr_exit() alcanza?
}

int main(void) {
	int ficheroServer; /* los ficheros descriptores */
	int sin_size;//	TODO: Esto hace falta declararlo aca? Que es?
	struct sockaddr_in server; /* para la informaci�n de la direcci�n del servidor */

	//	TODO: Conectarme a la DB openDS.

	//	TODO: Crear un socket para escuchar las peticiones que van a venir del browser.
	crearSocketYDejarEscuchando(ficheroServer, ficheroCliente, server, sin_size);

	//	TODO: Me desconecto de la DB, pero una vez que cerre el socket que esta escuchando. Ver como averiguar eso.

	printf("Te pudiste conectar amigowww!!!\n");
	printf("Estoy por cerrar los sockets\n");
	close(ficheroServer);

	//	TODO: Libero la lo ultimo que pueda llegar a quedar de memoria pedida.

	return 0;
}
