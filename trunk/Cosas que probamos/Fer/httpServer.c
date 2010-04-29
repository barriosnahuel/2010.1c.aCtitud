#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>

#define PORT 15000 /* El puerto que ser� abierto */
#define BACKLOG 3 /* El numero de conexiones permitidas */ //	TODO: Aca no tendriamos que poner por lo menos 20?
int main() {
	int ficheroServer;
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

	return 1;
}
