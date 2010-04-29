#include <sys/socket.h>
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
	
	return 1;
}
