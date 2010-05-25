#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../Logger/logger.h"
#include "configuration.h"
#include "conexion.h"
#include "funciones.h"
#include "../LDAP/LdapWrapperHandler.h"

#define STDIN 0
#define BUFFERSIZE 1024

#pragma pack(1)

char czNombreProceso[20];

int main(int argn, char *argv[]){
	char* sLogMessage;	/*	Es la variable que uso para generar el msj para el log.	*/

	int iFdmax=0, i=0, sigue=1, iBytes;
	char letra;
	char czMsg[BUFFERSIZE];
	fd_set master;
	fd_set read_fds;
	SSL_CTX *ctx;  //contexto ctx
	SSL *ssl;  // ssl state

	stConfiguracion stConf;

    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso,"NNTP_Server_BAM\0");
    strcpy(argv[0],czNombreProceso);

	memset(czMsg, 0, BUFFERSIZE);
	
	PantallaInicio();
    LoguearInformacion("Iniciando NNTP Server BAM...");

	if(!LoadConfiguration(argn, argv, &stConf)) {
		LoguearError("Configuracion no cargada.");
		return 0;
	}

    /* iniciar contexto ctx para SSL */
	ctx = IniciarCTX();
    if(ctx == NULL) {
	    LoguearError("No se pudo iniciar CTX.");
	    return 0;
    } else {
        LoguearInformacion("Iniciado contexto SSL.");
    }
    
    /* cargo los certificados */
    if(CargarCertificados(ctx, "servercert.pem", "serverprivatekey.pem") == -1) {
	    LoguearError("No se pudo cargar el certificado correctamente.");
	    return 0;
    } else {
        LoguearInformacion("Certificados cargados.");
    }

    /*este socket para esperar conexiones */
    stConf.iSockServer = AbrirSocketEscucha(stConf.czLocalIP, stConf.uiLocalPort);
    if(stConf.iSockServer == -1) {
	    LoguearError("No se pudo crear el socket para aceptar conexiones.");
	    return 0;
    }

    /* se limpian la estructuras */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* se agrega el socket de teclado */
    FD_SET(STDIN,&master);

    /* se agrega el socket de escucha */
    FD_SET(stConf.iSockServer,&master);
    if(iFdmax<stConf.iSockServer)
	    iFdmax=stConf.iSockServer;
	
	/****************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper		*
	 ****************************************************/
	PLDAP_CONTEXT stPLDAPContext = newLDAPContext();
	PLDAP_CONTEXT_OP stPLDAPContextOperations = newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
	PLDAP_SESSION stPLDAPSession;
	PLDAP_SESSION_OP stPLDAPSessionOperations = newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
	if (!crearConexionLDAP(stConf.czOpenDSServer, stConf.uiOpenDSPort, &stPLDAPContext, &stPLDAPContextOperations,
			&stPLDAPSession, &stPLDAPSessionOperations)) {
		LoguearError("No se pudo conectar a OpenDS.");
		return -1;
	}
	asprintf(&sLogMessage, "Conectado a OpenDS en: IP=%s; Port=%d.", stConf.czOpenDSServer, stConf.uiOpenDSPort);
	LoguearInformacion(sLogMessage);

	
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
	        if ((stConf.iSockClient == -1) && (FD_ISSET(stConf.iSockServer,&read_fds))) {
		        LoguearInformacion("Recibiendo conexion por el puerto de escucha.");
		        stConf.iSockClient = Acepta_Conexion_Cliente(stConf.iSockServer);
		        if(stConf.iSockClient == -1) {
			        LoguearInformacion("El cliente no se pudo conectar.");
		        } else {
		            printf("Recibiendo conexión cliente...\n");
		            LoguearInformacion("Cliente conectado correctamente.");
			        FD_SET(stConf.iSockClient,&master);
			        if(iFdmax<stConf.iSockClient)
				        iFdmax=stConf.iSockClient;

/*------------- TODO extraer lo que sigue en algun procedimiento --------------*/
				    ssl = SSL_new(ctx);     	/* get new SSL state with context */
                    SSL_set_fd(ssl, stConf.iSockClient);	/* set connection socket to SSL state */
                    if ( SSL_accept(ssl) == -1 ) {	/* do SSL-protocol accept */
                        ERR_print_errors_fp(stdout);
                        LoguearWarning("No se pudo establecer la comunicacion via SSL.");
                        FD_CLR(stConf.iSockClient, &master); /* lo saco de la bolsa */
                        SSL_free(ssl); /* release SSL state */
	    		        close(stConf.iSockClient); /* cierro el socket */
    			        stConf.iSockClient = -1;
                    } else {
                        LoguearInformacion("Aceptada comunicacion via SSL.");
                        printf("Cliente conectado.\nServidor ocupado.\n");
                    }
/*------------- ------------------------------------------ --------------*/
		        }
	        }

            /* si hay algo en el socket cliene: mandó comando */
	        if ((stConf.iSockClient != -1) && (FD_ISSET(stConf.iSockClient,&read_fds))) {
		        /*iBytes = Lee_Socket(stConf.iSockClient, czMsg, 5);*/
		        iBytes = SSL_read(ssl, czMsg, BUFFERSIZE);
		        if (iBytes == 0) {
			        FD_CLR(stConf.iSockClient, &master); /* lo saco de la bolsa */
                    SSL_free(ssl); /* release SSL state */
			        close(stConf.iSockClient); /* cierro el socket */
			        stConf.iSockClient = -1;
			        LoguearInformacion("Se desconecto el cliente.");
			        printf("Cliente desconectado.\nServidor disponible.\n");
		        }
		        else {
		        	czMsg[iBytes] = '\0';
					printf("- Mensaje recibido: %s\n", czMsg);

					/*	De todo el comando que recibimos (ej: ARTICLE clarin@1) obtenemos el parametro,
						es decir: "clarin@1".
						Se hace +1 Asi no queda el espacio adelante de todo.
					*/
					char* sParametroDelComando;
					substringFrom(&sParametroDelComando, czMsg, strcspn(czMsg, " "));

					char* sResponse;
					switch(SeleccionarComando(czMsg)) {
                	    case 0:  /* LIST 	o	LIST NEWSGROUPS	*/
                	    	processListNewsgroupsCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations);
                	    	break;
                	    case 1:  /* QUIT */
                	    	printf("1	Hay que hacer algo con quit????\n");
                	    	break;
                   	    case 2:  /* LISTGROUP */
                   	    	processListGroupCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations, sParametroDelComando);
                   	    	break;
                   	    case 3:  /* ARTICLE */
                       	    processArticleCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations, sParametroDelComando);
                       	    break;
                   	    case 4:  /* STAT */
                       	    printf("4\n");
printf("Esto esta puesto aca solo para probar, porque no me anda cuando ingreso el comando list newsgroups. chequear el cliente!!!!!!    una vez que el cliente acepte el comando LIST NEWSGROUPS la parte del server ya esta hecha y probada casi toda.\n");
                       	    processListNewsgroupsCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations);
                       	    break;
                   	    case 5:  /* HEAD */
                       	    processHeadCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations, sParametroDelComando);
                       	    break;
                   	    case 6:  /* BODY */
                       	    processBodyCommand(&sResponse, stPLDAPSession, stPLDAPSessionOperations, sParametroDelComando);
                       	    break;
                   	    case 7:  /* GROUP 		Este no va, son 7 nomas!!?? */
                   	    	printf("7\n");
                   	    	break;
                	}

					SSL_write(ssl, sResponse, strlen(sResponse));
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
    close(stConf.iSockServer);
    if (stConf.iSockClient != -1)
        close(stConf.iSockClient);
    SSL_CTX_free(ctx);	/* release context */

    PantallaFin();
    LoguearInformacion("Terminacion exitosa.");

    return 1;
}
