/*
 * httpServer.c
 *
 *  Created on: Apr 24, 2010
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 15000 /* El puerto que será abierto */
#define BACKLOG 3 /* El número de conexiones permitidas */

//void* gestionarSolicitudDelClienteThread(void* parametro) {
//	if (/*//	TODO: No esta en el cache*/true) {
//	TODO: Lo busco en la DB.

//	TODO: Guardo en cache.

//	TODO: Formateo la respuesta a HTML.

//	TODO: Devuelvo la respuesta. (Ver como hacer esto, que paremetro es "parametro".. capaz tengo que pasar el socket como parametro para asi poder responderle!!)

//	TODO: Termino el thread. (Chequear si no hay que hacer algo con el socket, o el medio por el cual devuelvo la rta).
//	}

//	return parametro;
//}

int crearSocketYDejarEscuchando(int ficheroServer, int ficheroCliente,
		struct sockaddr_in server, struct sockaddr_in client, int sin_size) {
	if ((ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error al crear el socket.\n");
		exit(-1);
	}
	printf("Socket creado.\n");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra dirección IP automáticamente */
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
	printf("Si queres probar la conexion abri tu firefox y escribi %d:%d\n", INADDR_ANY, PORT);

	sin_size = sizeof(struct sockaddr_in);
	if ((ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client,
			&sin_size)) == -1) {
		printf("Error al aceptar conexión.\n");
		return -1;
	}
	printf("Se obtuvo una conexión desde %s...\n", inet_ntoa(client.sin_addr));
	return 1;
}

int main(void) {
	int ficheroServer, ficheroCliente; /* los ficheros descriptores */
	int sin_size;
	struct sockaddr_in server; /* para la información de la dirección del servidor */
	struct sockaddr_in client; /* para la información de la dirección del cliente */

	//	TODO: Crear un socket para escuchar las peticiones que van a venir del browser.
	crearSocketYDejarEscuchando(ficheroServer, ficheroCliente, server, client,
			sin_size);

	//	TODO: Conectarme a la DB openDS.

	//while (true) {//	While true porque tengo que escuchar todo el tiempo conexiones entrantes. En realidad hay que ver como cerrar el programa de una manera "linda".
	//	TODO: Acepto una conexion nueva.

	//	TODO: Creo un nuevo thread para atender esa conexion. (Seria la funcion: gestionarSolicitudDelClienteThread)
	//}

	//	TODO: Me desconecto de la DB.

	printf("Te pudiste conectar amigowww!!!\n");
	printf("Estoy por cerrar los sockets\n");
	close(ficheroCliente);
	close(ficheroServer);

	//	TODO: Libero la lo ultimo que pueda llegar a quedar de memoria pedida.

	return 0;
}
