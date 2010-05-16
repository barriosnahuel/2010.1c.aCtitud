#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "conexion.h"

//------------------------------------------------------------------------------------
/* Genera el socket utilizado por el programa servidor
 * retorna:
 * 	socket del programa servidor (siempre > 0)
 * 	-1 en caso de error
 */
int AbrirSocketEscucha(char *czIp, int iPuerto) {
	struct sockaddr_in direccion;
	int iSocketAux;
	int iUno = 1;

	/* verifica el numero de ip */
	if (inet_addr(czIp) == INADDR_NONE) {
		return -1;
	}

	/* verificar puerto 
	if ((iPuerto < 0) || (iPuerto > 65535)) {
		return -1;
	}*/

	/* crea el canal de comunicacion */
	iSocketAux = socket(AF_INET, SOCK_STREAM, 0);

	if (iSocketAux == -1) {
		return -1;
	}

	if((setsockopt(iSocketAux, SOL_SOCKET, SO_REUSEADDR, &iUno, sizeof(int))) == -1) {
		//close(iSocketAux);
		return -1;
	}

	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = inet_addr(czIp);
	direccion.sin_port = htons(iPuerto);
	memset(&(direccion.sin_zero), '\0', 8);

	if (bind(iSocketAux, (struct sockaddr *)&direccion, sizeof(struct sockaddr)) == -1) {
		//No se puede atender el puerto de escucha
		close(iSocketAux);
		return -1;
	}

	/* empazar a atender el puerto */
	if (listen(iSocketAux, 255) == -1) {
		close(iSocketAux);
		return -1;
	}

	return iSocketAux;
}

/*
 * Se conecta a un programa servidor, retorna
 * socket para el programa cliente
 * o -1 en caso de error
 */
int AbrirSocketConexion(char *czIpRemoto, int iPuertoRemoto) {
	struct sockaddr_in direccion;
	int iSocket;
	int iUno=1;

	/* verifica el numero de ip */
	if (inet_addr(czIpRemoto) == INADDR_NONE) {
		return -1;
	}

	/* verificar puerto 
	if ((iPuertoRemoto < 0) || (iPuertoRemoto > 65535)) {
		return -1;
	}*/

	/* crea el canal de comunicacion */
	iSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (iSocket == -1) {
		return -1;
	}

	if(setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, &iUno, sizeof(int)) == -1) {
		//close(iSocket);
		return -1;
	}

	direccion.sin_family = AF_INET;
	direccion.sin_addr.s_addr = inet_addr(czIpRemoto);
	direccion.sin_port = htons(iPuertoRemoto);
	memset(&(direccion.sin_zero), '\0', 8);

	if (connect(iSocket, (struct sockaddr *)&direccion, sizeof(struct sockaddr)) == -1) {
		close(iSocket);
		return -1;
	}

	return iSocket;
}

/*
 * Lee datos del socket. Supone que se le pasa un buffer con hueco
 * suficiente para los datos. Devuelve el numero de bytes leidos o
 * 0 si se cierra fichero o -1 si hay error.
 */
int Lee_Socket (int iSocket, void *Datos, int Longitud) {
	int Leido = 0;
	int Aux = 0;

	/* Comprobacion de que los parametros de entrada son correctos */
	if ((iSocket < 0) || (Datos == NULL) || (Longitud < 1)) {
		return -1;
	}

	/* Mientras no hayamos leido todos los datos solicitados */
	while (Leido < Longitud) {
		Aux = read (iSocket, Datos + Leido, Longitud - Leido);

		if (Aux > 0) {
			/* Si hemos conseguido leer datos, incrementamos la variable
			* que contiene los datos leidos hasta el momento */
			Leido += Aux;
		}
		else {
			/* Si read devuelve 0, es que se ha cerrado el socket. Devolvemos
			* los caracteres leidos hasta ese momento */
			if (Aux == 0)
				return Leido;
			if (Aux == -1) {
				/* En caso de error, la variable errno nos indica el tipo
				* de error.
				* El error EINTR se produce si ha habido alguna
				* interrupcion del sistema antes de leer ningun dato. No
				* es un error realmente.
				* El error EGAIN significa que el socket no esta disponible
				* de momento, que lo intentemos dentro de un rato.
				* Ambos errores se tratan con una espera de 100 microsegundos
				* y se vuelve a intentar.
				* El resto de los posibles errores provocan que salgamos de
				* la funcion con error.
				*/
				switch (errno) {
					case EINTR:
					case EAGAIN:
						usleep (100);
						break;
					default:
						return -1;
				}
			}
		}
	}

	/* Se devuelve el total de los caracteres leidos */
	return Leido;
}

/*
 * Escribe dato en el socket. Devuelve numero de bytes escritos,
 * o -1 si hay error.
 */
int Escribe_Socket (int iSocket, void *Datos, int Longitud) {
	int Escrito = 0;
	int Aux = 0;

	/* Comprobacion de los parametros de entrada */
	if ((iSocket < 0) || (Datos == NULL) || (Longitud < 1))
		return -1;

	/* Bucle hasta que hayamos escrito todos los caracteres que nos han
	* indicado. */
	while (Escrito < Longitud) {
		Aux = write (iSocket, Datos + Escrito, Longitud - Escrito);
		if (Aux > 0) {
			/* Si hemos conseguido escribir caracteres, se actualiza la
			* variable Escrito */
			Escrito = Escrito + Aux;
		}
		else {
			/* Si se ha cerrado el socket, devolvemos el numero de caracteres
			* leidos. Si ha habido error, devolvemos -1 */
			if (Aux == 0)
				return Escrito;
			else
				return -1;
		}
	}

	/* Devolvemos el total de caracteres leidos */
	return Escrito;
}

/* Se le pasa un socket de servidor y acepta en el una conexion de cliente.
 * devuelve el descriptor del socket del cliente o -1 si hay problemas.
 */
int Acepta_Conexion_Cliente (int Descriptor) {
	socklen_t Longitud_Cliente;
	struct sockaddr Cliente;
	int iUno=1;
	int iSocketN;

	/*
	* La llamada a la funcion accept requiere que el parametro
	* Longitud_Cliente contenga inicialmente el tamano de la
	* estructura Cliente que se le pase. A la vuelta de la
	* funcion, esta variable contiene la longitud de la informacion
	* util devuelta en Cliente
	*/
	Longitud_Cliente = sizeof (Cliente);
	iSocketN = accept (Descriptor, &Cliente, &Longitud_Cliente);

	if (iSocketN == -1) {
		return -1;
	}

	if((setsockopt(iSocketN, SOL_SOCKET, SO_REUSEADDR, &iUno, sizeof(int))) == -1) {
		//close(iSocketN);
		return -1;
	}

	/* Se devuelve el descriptor en el que esta "enchufado" el cliente */
	return iSocketN;
}

