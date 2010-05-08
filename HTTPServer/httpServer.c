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
					, thread_t *new_thread
				);*/
int conectarAOpenDS(  stConfiguracion*	stConf
					, PLDAP_SESSION* 	session
					, PLDAP_CONTEXT*	context
					, PLDAP_CONTEXT_OP* ctxOp
					, PLDAP_SESSION_OP* sessionOp);

void* procesarRequestFuncionThread(void* parametro) {
	int* ficheroCliente= (int*)parametro;

	printf("Entro a la f del nuevo thread\n");
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
	if((bytesEnviados = send(*ficheroCliente, msg, len, 0)) == -1) {
		printf("El send no funco\n");
	}
	printf("El cliente recibio %d bytes\n", bytesEnviados);
	
	printf("Voy a cerrar la conexion del socket\n");
	close(*ficheroCliente); /*	�COMO CHOTA SE CIERRA UN SOCKET? */
	printf("Cerre el socket\n");
	printf("Exit al thread\n");
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
/*	PLDAP_SESSION session;
	PLDAP_CONTEXT context = newLDAPContext();
	PLDAP_CONTEXT_OP ctxOp = newLDAPContextOperations();	/*	Me permite operar sobre un contexto	*/
/*	PLDAP_SESSION_OP sessionOp = newLDAPSessionOperations();/*	Me permite operar sobre una sesion	*/
/*	if(!conectarAOpenDS(&stConf, &session, &context, &ctxOp, &sessionOp)){
		printf("No se pudo conectar a OpenDS.");
		return -1;
	}
	else
		printf("Conectado a OpenDS en: IP=%s; Port=%d\n", stConf.czBDServer, stConf.uiBDPuerto);

	PLDAP_ENTRY_OP entryOp= 		newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP attribOp=	newLDAPAttributeOperations();
*/
					/********************************************************************************
					 *	A partir de aca es de prueba de OpenDS										*
					 ********************************************************************************/
					/*	stArticle article;
						article.sBody= "body probando hibernate!";
						article.sHead= "head probando hibernate!";
						article.sNewsgroup= "blablabla.com";
						article.uiArticleID= 6969;

						selectEntries(session, sessionOp, entryOp, attribOp);
						insertEntry(session, sessionOp, entryOp, attribOp, article);
						selectEntries(session, sessionOp, entryOp, attribOp);

						article.sBody= "cambie el body para probar hibernate";
						updateEntry(session, sessionOp, entryOp, attribOp, article);
						selectEntries(session, sessionOp, entryOp, attribOp);

						deleteEntry(session, sessionOp, entryOp, attribOp, article.uiArticleID);
						selectEntries(session, sessionOp, entryOp, attribOp);*/
					/********************************************************************************
					 *	Hasta aca es la prueba														*
					 *******************************************************************************/

	/********************************************************************************
	 *	Creo la conexion con el socket y lo dejo listo								*
	 *******************************************************************************/
	int ficheroServer; /* los ficheros descriptores */
	/* int sin_size; TODO: Esto hace falta declararlo aca? Que es? */
	struct sockaddr_in server; /* para la informacion de la direccion del servidor */
	printf("Acabo de entrar al main\n");
	
	/* Inicializamos el contexto. VER DONDE ESTA LA BASE DE DATOS!! */
/*	ctxOp->initialize(context, OPENDS_LOCATION);
/*	session = ctxOp->newSession(context, "cn=Directory Manager", "password");
	/* se inicia la session. Se establece la	conexi�n con el servidor LDAP. */
/*	sessionOp->startSession(session);*/

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

	/* sin_size = sizeof(struct sockaddr_in); */

	while (1) {
		int sin_size = sizeof(struct sockaddr_in);
		struct sockaddr_in client; /* para la informaci�n de la direcci�n del cliente */

		int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client, &sin_size);
		if (ficheroCliente != -1) {
			/*	Si no hubo errores aceptando la conexion, entonces la gestiono. */

			thread_t threadProcesarRequest;/*	Declaro un nuevo thread. */
			/*	NBarrios-TODO: Seteo todo lo que tenga que setearle al thread, si es que hay que setearle algo. */
	printf("Despues de esto rompe!!\n");

			if (thr_create(	  0
							, 0
							, (void*)&procesarRequestFuncionThread
							, (void*)ficheroCliente
							, 0
							, (void *)threadProcesarRequest)
								!=0)
				printf("No se pudo crear un nuevo thread para procesar el request.\n");
		}
		else {
			printf("Error al aceptar la conexion\n");
		}
		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(
				client.sin_addr));
	}

	printf("Chao chao!\n");
	close(ficheroServer);
	
	/*sessionOp->endSession(session);
	freeLDAPSession(session);
	freeLDAPContext(context);
	freeLDAPContextOperations(ctxOp);
	freeLDAPSessionOperations(sessionOp);*/
	
	/*	TODO: Libero la lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
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

	/*	TODO: Ver alguna forma de retornar false cuando no me pueda conectar bien a la BD	*/

	return 1;
}
