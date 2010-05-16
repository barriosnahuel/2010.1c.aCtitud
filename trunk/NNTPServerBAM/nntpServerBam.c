#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "logger.h"
#include "configuration.h"
#include "conexion.h"

#define STDIN 0

#pragma pack(1)

char czNombreProceso[20];

int main(int argn, char *argv[]){

	int iSockServer, iSockClient=-1;
	int iFdmax=0, i=0, sigue=1, iBytes;
	char letra;
	char *czMsg;
	struct timeval tv; //tiempo de espera del select
	fd_set master;
	fd_set read_fds;

	stConfiguracion stConf;

    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso,"NNTP_Server_BAM\0");
    strcpy(argv[0],czNombreProceso);

    tv.tv_sec = 5;
	tv.tv_usec = 0;

	czMsg = (char *)malloc(100);
	memset(czMsg, 0, 100);
	
	//PantallaInicio();
    LoguearInformacion("Iniciando NNTP Server BAM...");

	if(!LoadConfiguration(argn, argv, &stConf)) {
		LoguearError("Configuracion no cargada.");
		return 0;
	}
	
	
    /*este socket para esperar conexiones */
    iSockServer = AbrirSocketEscucha(stConf.czLocalIP, stConf.uiLocalPort);
    if(iSockServer == -1) {
	    LoguearError("No se pudo crear el socket para aceptar conexiones.");
	    return 0;
    }

    /* se limpian la estructuras */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* se agrega el socket de teclado */
    FD_SET(STDIN,&master);

    /* se agrega el socket de escucha */
    FD_SET(iSockServer,&master);
    if(iFdmax<iSockServer)
	    iFdmax=iSockServer;
	
    /* TODO intenta conectarse con el servidor open ds */
	
	
    while(sigue) {

	    read_fds = master;

	    /* se bloquea el proceso actual hasta que surja un evento */
	    if(select(iFdmax+1,&read_fds,NULL,NULL,NULL) == -1) {
		    LoguearWarning("Falla la funcion select().");
		    LoguearError(strerror(errno));
	    }
	    else {
	        /* Los eventos que desbloquean la funci� pueden ser:
	         * 	1 - se conecta un nuevo cliente
	         * 	2 - un cliente nos envia un mensaje
	         * 	3 - se desconecta un cliente
	         * 	4 - atendemos el teclado
	         * TODO relacionado al servidor open ds
	         */

	        /* si tengo algo en el socket escucha: conexion de nuevo cliente */
	        //TODO: solo puedo aceptar un cliene por vez
	        if (FD_ISSET(iSockServer,&read_fds)) {
		        LoguearInformacion("Recibiendo conexion por el puerto de escucha.");
		        iSockClient = Acepta_Conexion_Cliente(iSockServer);
		        if(iSockClient == -1) {
			        LoguearInformacion("El cliente no se pudo conectar.");
		        } else {
		            LoguearInformacion("Cliente conectado correctamente.");
			        FD_SET(iSockClient,&master);
			        if(iFdmax<iSockClient)
				        iFdmax=iSockClient;
		        }
	        }

            /* si hay algo en el socket cliene: mandó comando */
	        if ((iSockClient != -1) && (FD_ISSET(iSockClient,&read_fds))) {
		        iBytes = Lee_Socket(iSockClient, czMsg, 5);
		        if (iBytes == 0) {
			        FD_CLR(iSockClient, &master); /* lo saco de la bolsa */
			        close(iSockClient); /* cierro el socket */
			        iSockClient = -1;
			        LoguearInformacion("Se desconecto el cliente.");
		        }
		        else {
		        	czMsg[iBytes] = '\0';
					printf("%s", czMsg);
                }
            }

	        /* si tengo algo en el teclado: puede ser una salida */
	        if (FD_ISSET(STDIN,&read_fds)) {
		        scanf("%c", &letra);
		        if(letra != '\n')
			        for(;getchar() != '\n';); // vaciar buffer
		        if (toupper(letra) == 'S')
			        sigue = 0;
	        }
	    } //if del select
    }
	
	
    // cerrar todo y liberar la memoria
    close(iSockServer);
    close(iSockClient);
    free(czMsg);
    //PantallaFin();
    LoguearInformacion("Terminacion exitosa.");

    return 1;
}
