#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include "configuration.h"
#include "LdapWrapper.h"

/* #define PORT 15000 /* El puerto que ser� abierto */
#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
/*#define OPENDS_LOCATION "ldap://localhost:4444"		TODO: Esto vamos a tener que pasarlo por archivo de configuracion */

/*Esto tiene que estar?????int thr_create(void *stack_base
					, size_t stack_size
					, void *(*start_routine)(void *)
					, void *arg
					, long flags
					, thread_t *new_thread
				);*/
int procesarRequestFuncionThread(int ficheroCliente);
int conectarAOpenDS(  stConfiguracion*	stConf
					, PLDAP_SESSION* 	session
					, PLDAP_CONTEXT*	context
					, PLDAP_CONTEXT_OP* ctxOp
					, PLDAP_SESSION_OP* sessionOp);
int crearConexionConSocket(	  stConfiguracion*		stConf
							, int*					ficheroServer
							, struct sockaddr_in*	server);
void liberarRecursos( int 				ficheroServer
					, PLDAP_SESSION 	session
					, PLDAP_CONTEXT		context
					, PLDAP_CONTEXT_OP	ctxOp
					, PLDAP_SESSION_OP	sessionOp);

int main() {
	/*	Cargo el archivo de configuracion	*/
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


	/* Conecto a OpenDS por medio del LDAP Wrapper*/
	PLDAP_SESSION session;
	PLDAP_CONTEXT context = newLDAPContext();
	PLDAP_CONTEXT_OP ctxOp = newLDAPContextOperations();
	PLDAP_SESSION_OP sessionOp = newLDAPSessionOperations();
	PLDAP_RESULT_SET resultSet;
	LDAP_ITERATOR iterator;
	LDAP_RECORD_OP recordOp;
	PLDAP_ENTRY entry;
	LDAP_ENTRY_OP entryOp;
	LDAP_ATTRIBUTE_OP attribOp;
	if(!conectarAOpenDS(&stConf, &session, &context, &ctxOp, &sessionOp)){
		printf("No se pudo conectar a OpenDS.");
		return -1;
	}
	else
		printf("Conectado a OpenDS en: IP=%s; Port=%d\n", stConf.czBDServer, stConf.uiBDPuerto);


	/*	Creo la conexion con el socket que va a estar escuchando conexiones entrantes	*/
	int ficheroServer;			/* los ficheros descriptores */
	struct sockaddr_in server;	/* para la informacion de la direccion del servidor */
	if(!crearConexionConSocket(&stConf, &ficheroServer, &server))
		return -1;
	else
		printf("Aplicacion levantada en: IP=%s; Port=%d\n\nEscuchando conexiones entrantes...\n", "ver como obtener esta ip!!", stConf.uiAppPuerto);

	/*int sin_size = sizeof(struct sockaddr_in);
	while (1) {
		struct sockaddr_in client; /* para la informacion de la direccion del cliente */
		
	/*	int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client, &sin_size);
		printf("Acepte una conexion y ficheroCliente vale: %d.\n", ficheroCliente);
		/*	Despues de este printf es cuando me tira el segmentation fault	*/
	/*	printf("1- Imprimo algo para ver si anda\n");
		printf("2- Imprimo algo para ver si anda\n");
		printf("3- Imprimo algo para ver si anda\n");
		printf("4- Imprimo algo para ver si anda\n");
		printf("5- Imprimo algo para ver si anda\n");
		if (ficheroCliente != -1) {
			/*	Si no hubo errores aceptando la conexion, entonces la gestiono. */
	/*		printf("1- Entre al if\n");
			printf("Voy a declarar el neuvo thread");
			thread_t threadProcesarRequest;	/*	Declaro un nuevo thread. */
			/*	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread, si es que hay que setearle algo. */

	/*		printf("Ahora llamo al thr_create");
			if (thr_create(0, 0, (void*)&procesarRequestFuncionThread, (void*) ficheroCliente, 0, (void *)threadProcesarRequest) != 0)
				printf("No se pudo crear un nuevo thread para procesar el request.\n");
		}
		else
			printf("Error al aceptar la conexion\n");

		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
	}*/
	printf("Voy a buscar un entry.\n");
	/*resultSet = sessionOp->searchEntry(session, "ou=so,dn=utn,dn=edu",	"utnArticleID=*");*/
	/*resultSet->iterator;
	iterator->hasNext(resultSet);
	PLDAP_RECORD record = iterator->next(resultSet);
	PLDAP_FIELD field = recordOp->nextField(record);
	field->name;*/
	/*printf("El resultset es: %s", resultSet);*/
	entry = entryOp->createEntry();
	(*entry)->dn = "utnurlID=00009,ou=so,dc=utn,dc=edu";
	(*entryOp)->addAttribute(entry, attribOp->createAttribute("objectclass", 2, "top", "utnUrl"));
	(*sessionOp)->addEntry(session, entry);
	printf("Cree el entry!!\n");

	printf("Ahora cierro socket, db, etc...\n");
	liberarRecursos(ficheroServer, session, context, ctxOp, sessionOp);
	
	return 1;
}

int procesarRequestFuncionThread(int ficheroCliente) {
	printf("entre a la funcion del thread.\n");
	char *msg = "Hola mundo!";
	int len, bytesEnviados;
	len = strlen(msg);
	printf("---------------- Procesando thread xD -----------------\n");

/*
	if (TODO: Si no esta en el cache) {
		TODO: Lo busco en la DB.

		TODO: Guardo en cache.
	}
	Para este momento ya tengo el response seteado.

	TODO: Formateo la respuesta a HTML.
*/

	printf("Pruebo enviarle algo a mi amigo el cliente... \n");
	if((bytesEnviados = send(ficheroCliente, msg, len, 0)) == -1) {
		printf("El send no funco\n");
	}
	printf("El cliente recibio %d bytes\n", bytesEnviados);

	printf("Voy a cerrar la conexion del socket\n");
	close(ficheroCliente); /*	�COMO CHOTA SE CIERRA UN SOCKET? */
	printf("Cerre el socket\n");
	printf("Exit al thread\n");
	thr_exit(0);/*	Termino el thread.*/
	return 0;
}


/**
 * Se conecta a OpenDS y deja establecida la conexion para usarla con el LDAPWrapper
 */
int conectarAOpenDS(  stConfiguracion*	stConf
					, PLDAP_SESSION* 	session
					, PLDAP_CONTEXT*	context
					, PLDAP_CONTEXT_OP* ctxOp
					, PLDAP_SESSION_OP* sessionOp){


	/*	Seteo sOpenDSLocation bajo el formato:	ldap://localhost:4444	*/
	char *sOpenDSLocation;
	asprintf(&sOpenDSLocation, "ldap://%s:%d", (*stConf).czBDServer, (*stConf).uiBDPuerto);

	/* Inicializamos el contexto. */
	(*ctxOp)->initialize(*context, sOpenDSLocation);
	(*session)= (*ctxOp)->newSession(*context, "cn=Directory Manager", "password");

	/* Se inicia la session. Se establece la conexion con el servidor LDAP. */
	(*sessionOp)->startSession(*session);
	printf("Inicie la sesion de OpenDS joya\n");

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

	if (bind(*ficheroServer, (struct sockaddr *) server,
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
					, PLDAP_SESSION 	session
					, PLDAP_CONTEXT		context
					, PLDAP_CONTEXT_OP	ctxOp
					, PLDAP_SESSION_OP	sessionOp){

	/*	Cierro el socket	*/
	close(ficheroServer);

	/*	Cierro/Libero lo relacionado a la BD (OpenDS)	*/
	sessionOp->endSession(session);
	freeLDAPSession(session);
	freeLDAPContext(context);
	freeLDAPContextOperations(ctxOp);
	freeLDAPSessionOperations(sessionOp);
}
