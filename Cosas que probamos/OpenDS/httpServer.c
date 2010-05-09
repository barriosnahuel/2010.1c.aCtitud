#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include "configuration.h"
#include "LdapWrapperHandler.h"

#define PORT 15000 /* El puerto que ser� abierto */
#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
#define OPENDS_LOCATION "ldap://localhost:4444"	/*	TODO: Esto vamos a tener que pasarlo por archivo de configuracion */

/*int thr_create(void *stack_base
					, size_t stack_size
					, void *(*start_routine)(void *)
					, void *arg
					, long flags
					, thread_t* new_thread
				);*/
int crearContextoPLDAP(  stConfiguracion*	stConf
					, PLDAP_CONTEXT*	pstPLDAPContext
					, PLDAP_CONTEXT_OP* pstPLDAPContextOperations);
int crearSesionPLDAP( PLDAP_SESSION*	pstPLDAPSession
					, PLDAP_CONTEXT*	pstPLDAPContext
					, PLDAP_CONTEXT_OP*	pstPLDAPContextOperations);
int crearConexionConSocket(	  stConfiguracion*		stConf
							, int*					ficheroServer
							, struct sockaddr_in*	server);
void* procesarRequestFuncionThread(void* parametro);
void liberarRecursos( int 				ficheroServer
					, PLDAP_CONTEXT		context
					, PLDAP_CONTEXT_OP	ctxOp);

/**
 * Estructura que contiene los parametros para cada nuevo thread.
 */
typedef struct stThreadParameters {
    int				 	ficheroCliente;				/*	el file descriptor de la conexion con el nuevo cliente.	*/

    /*	OpenDS/LDAP	*/
    PLDAP_CONTEXT*		pstPLDAPContext;			/*	El contexto de OpenDS/LDAP para poder crear una nueva sesion
													si es necesario. Es decir, cuando no se encuentre la inf. en cache.	*/
    PLDAP_CONTEXT_OP*	pstPLDAPContextOperations;	/*	Permite realizar operaciones sobre el contexto, ej, crear nuevas sesiones	*/

} stThreadParameters;


/********************************************************************************
 *	Aca comienzan las definiciones de las funciones								*
 *******************************************************************************/

void* procesarRequestFuncionThread(void* threadParameters) {
	printf("Bienvenido a la funcion del nuevo thread\n");
	stThreadParameters stParametros= *((stThreadParameters*)threadParameters);

	char* msg = "Hola mundo!";
	int len, bytesEnviados;
	len = strlen(msg);

	printf("---------------- Procesando thread xD -----------------\n");

					/********************************************************************************
					 *	A partir de aca es de prueba de OpenDS										*
					 ********************************************************************************/
						PLDAP_SESSION stPLDAPSession;
						if(!crearSesionPLDAP(&stPLDAPSession, stParametros.pstPLDAPContext, stParametros.pstPLDAPContextOperations))
							printf("No se pudo crear una sesion con PLDAP\n");
						printf("Se creo la sesion con PLDAP\n");
						PLDAP_SESSION_OP stPLDAPSessionOperations = newLDAPSessionOperations();	/*	Me permite operar sobre una sesion	*/

						/*	Estos dos, son exclusivos para algunas operaciones	*/
						PLDAP_ENTRY_OP entryOp= 		newLDAPEntryOperations();
						PLDAP_ATTRIBUTE_OP attribOp=	newLDAPAttributeOperations();

						stArticle article;
						article.sBody= "body probando hibernate!";
						article.sHead= "head probando hibernate!";
						article.sNewsgroup= "blablabla.com";
						article.uiArticleID= 6969;

						selectEntries(stPLDAPSession, stPLDAPSessionOperations);
						insertEntry(stPLDAPSession, stPLDAPSessionOperations, entryOp, attribOp, article);
						selectEntries(stPLDAPSession, stPLDAPSessionOperations);

						article.sBody= "cambie el body para probar hibernate";
						updateEntry(stPLDAPSession, stPLDAPSessionOperations, entryOp, attribOp, article);
						selectEntries(stPLDAPSession, stPLDAPSessionOperations);

						deleteEntry(stPLDAPSession, stPLDAPSessionOperations, entryOp, article.uiArticleID);
						selectEntries(stPLDAPSession, stPLDAPSessionOperations);

						/*	Libero recursos.	*/
						stPLDAPSessionOperations->endSession(stPLDAPSession);
						freeLDAPSessionOperations(stPLDAPSessionOperations);
						freeLDAPSession(stPLDAPSession);
					/********************************************************************************
					 *	Hasta aca es la prueba														*
					 *******************************************************************************/

/*
	if (TODO: Si no esta en el cache) {
		TODO: Lo busco en la DB.

		TODO: Guardo en cache.
	}
	Para este momento ya tengo el response seteado.

	TODO: Formateo la respuesta a HTML.
*/

	printf("Pruebo enviarle algo a mi amigo el cliente... \n");
	if((bytesEnviados = send(stParametros.ficheroCliente, msg, len, 0)) == -1) {
		printf("El send no funco\n");
	}
	printf("El cliente recibio %d bytes\n", bytesEnviados);
	
	printf("Voy a cerrar la conexion con el cliente\n");
	close(stParametros.ficheroCliente);

	printf("Cerre la conexion con el cliente y ahora Exit al thread\n");
	thr_exit(0);/*	Termino el thread.*/

	return 0;
}

int main() {
	/********************************************************************************
	 *	Cargo el archivo de configuracion											*
	 *******************************************************************************/
	stConfiguracion stConf;

	if(!CargaConfiguracion("config.conf\0", &stConf)) {
		printf("Archivo de configuracion no valido.\n");
		return -1;
	}
	else{
		printf("Archivo de configuracion cargado correctamente:\n");
		printf("\tPuerto de la aplicacion: %d\n", stConf.uiAppPuerto);
		printf("\tPuerto de OpenDS: %d\n", stConf.uiBDPuerto);
		printf("\tIP OpenDS: %s\n", stConf.czBDServer);
	}

	/********************************************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper									*
	 *******************************************************************************/
	PLDAP_CONTEXT 		stPLDAPContext = newLDAPContext();
	PLDAP_CONTEXT_OP	stPLDAPContextOperations = newLDAPContextOperations();	/*	Me permite operar sobre un contexto	*/
	if(!crearContextoPLDAP(&stConf, &stPLDAPContext, &stPLDAPContextOperations)){
		printf("No se pudo conectar a OpenDS.");
		return -1;
	}
	else
		printf("Conectado a OpenDS en: IP=%s; Port=%d\n", stConf.czBDServer, stConf.uiBDPuerto);


	/********************************************************************************
	 *	Creo la conexion con el socket y lo dejo listo								*
	 *******************************************************************************/
	int ficheroServer;			/* los ficheros descriptores */
	struct sockaddr_in server;	/* para la informacion de la direccion del servidor */
	if(!crearConexionConSocket(&stConf, &ficheroServer, &server))
		return -1;
	else
		printf("Aplicacion levantada en: IP=%s; Port=%d\n\nEscuchando conexiones entrantes...\n", "ver como obtener esta ip!!", stConf.uiAppPuerto);

	/********************************************************************************
	 *	Itero de manera infinita??? recibiendo conexiones de != clientes			*
	 *******************************************************************************/
/*	while (1) {*/
		int sin_size = sizeof(struct sockaddr_in);
		struct sockaddr_in client; /* para la informaci�n de la direcci�n del cliente */

		int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client, &sin_size);
		if (ficheroCliente != -1) {
			/*	Si no hubo errores aceptando la conexion, entonces la gestiono. */

			thread_t threadProcesarRequest;/*	Declaro un nuevo thread. */

			/*	Le seteo los parametros al nuevo thread. ME PARECE QUE HAY QUE CREAR UNA SESION POR CADA THREAD!!	*/
			stThreadParameters stParameters;
			stParameters.ficheroCliente= ficheroCliente;
			stParameters.pstPLDAPContext= &stPLDAPContext;
			stParameters.pstPLDAPContextOperations= &stPLDAPContextOperations;

			if (thr_create(0, 0, (void*)&procesarRequestFuncionThread, (void*)&stParameters, 0, &threadProcesarRequest) != 0)
				printf("No se pudo crear un nuevo thread para procesar el request.\n");
		}
		else
			printf("Error al aceptar la conexion\n");
		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
/*	}*/

printf("Le doy al thread 10 segundos para responderle al cliente antes que cierre todo... ;)\n");
sleep(10);
	


printf("Ahora cierro socket, db, etc...\n");
	liberarRecursos(ficheroServer, stPLDAPContext, stPLDAPContextOperations);

	/*	TODO: Libero lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
}

/**
 * Se conecta a OpenDS y deja establecida la conexion para usarla con el LDAPWrapper
 */
int crearContextoPLDAP(  stConfiguracion*	stConf
					, PLDAP_CONTEXT*	pstPLDAPContext
					, PLDAP_CONTEXT_OP* pstPLDAPContextOperations){


	/*	Seteo sOpenDSLocation bajo el formato:	ldap://localhost:4444	*/
	char *sOpenDSLocation;
	asprintf(&sOpenDSLocation, "ldap://%s:%d", (*stConf).czBDServer, (*stConf).uiBDPuerto);

	/* Inicializamos el contexto. */
	(*pstPLDAPContextOperations)->initialize(*pstPLDAPContext, sOpenDSLocation);
/*	(*session)= (*ctxOp)->newSession(*context, "cn=Directory Manager", "password");

	/* Se inicia la session. Se establece la conexion con el servidor LDAP. */
/*	(*sessionOp)->startSession(*session);

	/*	TODO: Ver alguna forma de retornar false cuando no me pueda conectar bien a la BD	*/

	return 1;
}

/**
 * Crea el socket, lo bindea, y lo deja listo para escuchar conexiones entrantes.
 */
int crearConexionConSocket(	  stConfiguracion*		stConf
							, int*					ficheroServer
							, struct sockaddr_in*	server){

	if ((*ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error al crear el socket.\n");
		exit(-1);
	}
	printf("Cree el socket bien\n");

	(*server).sin_family = AF_INET;
	(*server).sin_addr.s_addr = INADDR_ANY; 			/* INADDR_ANY coloca nuestra direccion IP automaticamente */
	(*server).sin_port = htons((*stConf).uiAppPuerto);	/* htons transforma el short de maquina a short de red */
	bzero(&((*server).sin_zero), 8); 					/* Escribimos ceros en el resto de la estructura*/

	if (bind(*ficheroServer, (const struct sockaddr *) &(*server),
			sizeof(struct sockaddr)) == -1) {
		printf("Error al asociar el puerto al socket.\n");
		exit(-1);
	}
	printf("Pude bindear bien el socket\n");

	if (listen(*ficheroServer, BACKLOG) == -1) {
		printf("Error al escuchar por el puerto.\n");
		exit(-1);
	}

	return 1;
}

/**
 * 1. Cierra el socket.
 * 2. Cierra contexto, sesion y demas "cosas" de OpenDS y LDAP Wrapper.
 */
void liberarRecursos(int 				ficheroServer
					, PLDAP_CONTEXT		context
					, PLDAP_CONTEXT_OP	ctxOp){

	/*	Cierro el socket	*/
	close(ficheroServer);	/*	TODO Descomentar esto, cuando se descomente y se trate la parte de los sockets.*/

	/*	Cierro/Libero lo relacionado a la BD (OpenDS)	*/
	freeLDAPContext(context);
	freeLDAPContextOperations(ctxOp);
}

int crearSesionPLDAP( PLDAP_SESSION*	pstPLDAPSession
					, PLDAP_CONTEXT*	pstPLDAPContext
					, PLDAP_CONTEXT_OP*	pstPLDAPContextOperations){

	/*	Creo una nueva sesion */
	*pstPLDAPSession= (*pstPLDAPContextOperations)->newSession(*pstPLDAPContext, "cn=Directory Manager", "password");

	/* Se inicia la session. Se establece la conexion con el servidor LDAP. */
	PLDAP_SESSION_OP stPLDAPSessionOperations = newLDAPSessionOperations();	/*	Me permite operar sobre una sesion	*/
	stPLDAPSessionOperations->startSession(*pstPLDAPSession);
	freeLDAPSessionOperations(stPLDAPSessionOperations);

	/*	TODO: Ver la forma de retornar false.	*/
	return 1;
}
