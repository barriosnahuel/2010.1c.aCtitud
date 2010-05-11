#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include "configuration.h"
#include "LdapWrapperHandler.h"

#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
#define REQUEST_TYPE_NEWSGROUP 1	/*	Indica que se esta solicitando el listado de newsgroups.	*/
#define REQUEST_TYPE_NEWS_LIST 2	/*	Indica que se esta solicitando el listado de noticias para un newsgroup especifico. 	*/
#define REQUEST_TYPE_NEWS 3			/*	Indica que se esta solicitando una noticia en particular.	*/

/*int thr_create(void *stack_base
 , size_t stack_size
 , void *(*start_routine)(void *)
 , void *arg
 , long flags
 , thread_t* new_thread
 );*/

/**
 * Estructura que contiene los parametros para cada nuevo thread.
 */
typedef struct stThreadParameters {
	int ficheroCliente; /*	el file descriptor de la conexion con el nuevo cliente.	*/

	/*	OpenDS/LDAP	*/
	PLDAP_SESSION* pstPLDAPSession; /*	La sesion de OpenDS/LDAP */
	PLDAP_SESSION_OP* pstPLDAPSessionOperations; /*	Permite realizar operaciones sobre la sesino, ej,
	 insertar/modificar/eliminar entries		*/
} stThreadParameters;

/************************************************************************************************************
 *	Aca comienzan las declaraciones de las funciones														*
 ************************************************************************************************************/
/**
 * Se conecta a OpenDS/PLDAP y deja establecida la conexion para usarla con el LDAPWrapper
 */
int crearConexionLDAP(stConfiguracion* stConf, PLDAP_CONTEXT* pstPLDAPContext,
		PLDAP_CONTEXT_OP* pstPLDAPContextOperations,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations);
/**
 * Crea el socket y lo deja listo para escuchar conexiones entrantes.
 */
int crearConexionConSocket(stConfiguracion* stConf, int* ficheroServer,
		struct sockaddr_in* server);
/**
 * Esta funcion es la funcion principal desde la que "arranca" cada nuevo
 * thread encargado de procesar una conexion entrante.
 */
void* procesarRequestFuncionThread(void* parametro);
void
		liberarRecursos(int ficheroServer, PLDAP_CONTEXT stPLDAPContext,
				PLDAP_CONTEXT_OP stPLDAPContextOperations,
				PLDAP_SESSION stPLDAPSession,
				PLDAP_SESSION_OP stPLDAPSessionOperations);
/**
 * Procesa un request del tipo newsgroup. Es decir, el listado de newsgroup disponibles
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeNewsgroup(char* sRecursoPedido,
		stThreadParameters* pstParametros);
/**
 * Procesa un request del tipo news list. Es decir, busca las noticias pertenecientes a un newsgroup en particular
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeNewsList(char* sRecursoPedido,
		stThreadParameters* pstParametros);
/**
 * Procesa un request del tipo news. Es decir, busca una noticia en particular en base al sCriterio
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeNews(char* sRecursoPedido,
		stThreadParameters* pstParametros);
/**
 * Busca la noticia en la cache, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnCache(stArticle* pstArticulo, char* sURL);
/**
 * Busca la noticia en la BD, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnBD(stArticle* pstArticulo, char* sURL,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations);
/**
 * Guarda la noticia que se le pasa como parametro en cache.
 */
void guardarNoticiaEnCache(stArticle stArticulo);
/**
 * Formatea la noticia la noticia a un char* en formato HTML para poder enviarle eso al cliente
 * y visualizar bien la pagina HTML.
 */
char* formatearArticuloAHTML(stArticle stArticulo);
/**
 * Esta funcion es la que se ejecuta cuando se crea un nuevo thread.
 */
void* procesarRequestFuncionThread(void* threadParameters);

/************************************************************************************************************
 *	Aca comienzan las definiciones de las funciones															*
 ************************************************************************************************************/
int main() {
	/****************************************
	 *	Cargo el archivo de configuracion	*
	 ****************************************/
	stConfiguracion stConf;

	if (!CargaConfiguracion("config.conf\0", &stConf)) {
		printf("Archivo de configuracion no valido.\n");
		return -1;
	} else {
		printf("Archivo de configuracion cargado correctamente:\n");
		printf("\tPuerto de la aplicacion: %d\n", stConf.uiAppPuerto);
		printf("\tPuerto de OpenDS: %d\n", stConf.uiBDPuerto);
		printf("\tIP OpenDS: %s\n", stConf.czBDServer);
	}

	/****************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper		*
	 ****************************************************/
	PLDAP_CONTEXT stPLDAPContext = newLDAPContext();
	PLDAP_CONTEXT_OP stPLDAPContextOperations = newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
	PLDAP_SESSION stPLDAPSession;
	PLDAP_SESSION_OP stPLDAPSessionOperations = newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
	if (!crearConexionLDAP(&stConf, &stPLDAPContext, &stPLDAPContextOperations,
			&stPLDAPSession, &stPLDAPSessionOperations)) {
		printf("No se pudo conectar a OpenDS.");
		return -1;
	} else
		printf("Conectado a OpenDS en: IP=%s; Port=%d\n", stConf.czBDServer,
				stConf.uiBDPuerto);

	/********************************************************
	 *	Creo la conexion con el socket y lo dejo listo		*
	 ********************************************************/
	int ficheroServer; /* los ficheros descriptores */
	struct sockaddr_in server; /* para la informacion de la direccion del servidor */
	if (!crearConexionConSocket(&stConf, &ficheroServer, &server))
		return -1;
	else
		printf(
				"Aplicacion levantada en: IP=%s; Port=%d\n\nEscuchando conexiones entrantes...\n",
				"ver como obtener esta ip!!", stConf.uiAppPuerto);

	/********************************************************************************
	 *	Itero de manera infinita??? recibiendo conexiones de != clientes			*
	 *******************************************************************************/
	/*while (1) {*/
		int sin_size = sizeof(struct sockaddr_in);
		struct sockaddr_in client; /* para la informacion de la direccion del cliente */

		int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client,
				&sin_size);
		if (ficheroCliente != -1) {
			/*	Si no hubo errores aceptando la conexion, entonces la gestiono. */

			thread_t threadProcesarRequest;/*	Declaro un nuevo thread. */

			/*	Le seteo los parametros al nuevo thread.	*/
			stThreadParameters stParameters;
			stParameters.ficheroCliente = ficheroCliente;
			stParameters.pstPLDAPSession = &stPLDAPSession;
			stParameters.pstPLDAPSessionOperations = &stPLDAPSessionOperations;

			if (thr_create(0, 0, (void*) &procesarRequestFuncionThread,
					(void*) &stParameters, 0, &threadProcesarRequest) != 0)
				printf(
						"No se pudo crear un nuevo thread para procesar el request.\n");
		} else
			printf("Error al aceptar la conexion\n");
		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(
				client.sin_addr));
	/*}*/

	printf(
			"Le doy al thread 15 segundos para responderle al cliente antes que cierre todo... ;)\n");
	sleep(15);

	printf("Ahora cierro socket, db, etc...\n");
	liberarRecursos(ficheroServer, stPLDAPContext, stPLDAPContextOperations,
			stPLDAPSession, stPLDAPSessionOperations);

	/*	TODO: Libero lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
}

void* procesarRequestFuncionThread(void* threadParameters) {
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);
	printf("---------------- Procesando thread xD -----------------\n");
	int bytesRecibidos;
	char* sResponse;
	
	char sRecursoPedido[1024];/*	TODO: esto seria toda la url me parece, y en cada
	 funcion la parseo y creo el criterio por el que voy a buscar en OpenDS!!			*/
	char cadenaAUsarParaImprimirElRecv[1024];
	
	int lenRecursoPedido;
	lenRecursoPedido = strlen(sRecursoPedido);
	bytesRecibidos = recv(stParametros.ficheroCliente, sRecursoPedido, lenRecursoPedido, 0);
	
	snprintf(cadenaAUsarParaImprimirElRecv, bytesRecibidos, "###Recibi del cliente el siguiente texto: %s\n", sRecursoPedido);
	printf("Recibi %d bytes del usuario.\n", bytesRecibidos);
	printf("%s", cadenaAUsarParaImprimirElRecv);
	
	unsigned int uiOperation = REQUEST_TYPE_NEWS;/*	TODO: Esto hay que setearlo en base a lo que se pida en la URL	*/
	switch (uiOperation) {
	case REQUEST_TYPE_NEWSGROUP:
		printf("Proceso la obtencion del listado de newsgroup.\n");
		sResponse = processRequestTypeNewsgroup(sRecursoPedido, &stParametros);
		break;
	case REQUEST_TYPE_NEWS_LIST:
		printf(
				"Proceso la obtencion del listado de noticias para un newsgroup.\n");
		sResponse = processRequestTypeNewsList(sRecursoPedido, &stParametros);
		break;
	case REQUEST_TYPE_NEWS:
		printf("Proceso la obtencion de una noticia en particular.\n");
		sResponse = processRequestTypeNews(sRecursoPedido, &stParametros);
		break;
	default:
		printf("Por default proceso la obtencion del listado de newsgroup.\n");
		sResponse = processRequestTypeNewsgroup(sRecursoPedido, &stParametros);
		break;
	}

	int len, bytesEnviados;
	len = strlen(sResponse);

	printf("Le respondo al cliente con el html ya armado... \n");
	if ((bytesEnviados = send(stParametros.ficheroCliente, sResponse, len, 0))
			== -1) {
		printf("El send no funco\n");
	}
	printf("El cliente recibio %d bytes\n", bytesEnviados);

	printf("Voy a cerrar la conexion con el cliente\n");
	close(stParametros.ficheroCliente);

	printf("Cerre la conexion con el cliente y ahora Exit al thread\n");

	thr_exit(0);/*	Termino el thread.*/

	return 0;
}

int crearConexionLDAP(stConfiguracion* stConf, PLDAP_CONTEXT* pstPLDAPContext,
		PLDAP_CONTEXT_OP* pstPLDAPContextOperations,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {

	/*	Seteo sOpenDSLocation bajo el formato:	ldap://localhost:4444	*/
	char *sOpenDSLocation;
	asprintf(&sOpenDSLocation, "ldap://%s:%d", (*stConf).czBDServer,
			(*stConf).uiBDPuerto);

	/* Inicializamos el contexto. */
	(*pstPLDAPContextOperations)->initialize(*pstPLDAPContext, sOpenDSLocation);
	(*pstPLDAPSession) = (*pstPLDAPContextOperations)->newSession(
			*pstPLDAPContext, "cn=Directory Manager", "password");

	/* Se inicia la session. Se establece la conexion con el servidor LDAP. */
	(*pstPLDAPSessionOperations)->startSession(*pstPLDAPSession);

	/*	TODO: Ver alguna forma de retornar false cuando no me pueda conectar bien a la BD	*/

	return 1;
}

/**
 * Crea el socket, lo bindea, y lo deja listo para escuchar conexiones entrantes.
 */
int crearConexionConSocket(stConfiguracion* stConf, int* ficheroServer,
		struct sockaddr_in* server) {

	if ((*ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Error al crear el socket.\n");
		exit(-1);
	}
	printf("Cree el socket bien\n");

	(*server).sin_family = AF_INET;
	(*server).sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra direccion IP automaticamente */
	(*server).sin_port = htons((*stConf).uiAppPuerto); /* htons transforma el short de maquina a short de red */
	bzero(&((*server).sin_zero), 8); /* Escribimos ceros en el resto de la estructura*/

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
void liberarRecursos(int ficheroServer, PLDAP_CONTEXT stPLDAPContext,
		PLDAP_CONTEXT_OP stPLDAPContextOperations,
		PLDAP_SESSION stPLDAPSession, PLDAP_SESSION_OP stPLDAPSessionOperations) {
	printf("Entro a liberar recursos\n");

	/*	Cierro/Libero lo relacionado a la BD	*/
	stPLDAPSessionOperations->endSession(stPLDAPSession);
	freeLDAPSessionOperations(stPLDAPSessionOperations);
	freeLDAPSession(stPLDAPSession);
	freeLDAPContext(stPLDAPContext);
	freeLDAPContextOperations(stPLDAPContextOperations);
	printf("Libere LDAP/OpenDS\n");

	/*	Cierro el socket	*/
	close(ficheroServer);
	printf("Libere el ficheroServer\n");
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sURL) {
	printf("Entro a buscar la noticia en Memcached\n");

	return 0;
}

int buscarNoticiaEnBD(stArticle* pstArticulo, char* sURL,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	printf("Entro a buscar la noticia en OpenDS\n");

	/*	Estos dos, son exclusivos para algunas operaciones	*/
	PLDAP_ENTRY_OP entryOp = newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP attribOp = newLDAPAttributeOperations();

	/********************************************************************************
	 *	A partir de aca es de prueba de OpenDS										*
	 ********************************************************************************/
	selectEntries(*pstPLDAPSession, *pstPLDAPSessionOperations, sURL);
	insertEntry(*pstPLDAPSession, *pstPLDAPSessionOperations, entryOp,
			attribOp, *pstArticulo);
	selectEntries(*pstPLDAPSession, *pstPLDAPSessionOperations, sURL);

	(*pstArticulo).sBody = "cambie el body para probar el update";
	updateEntry(*pstPLDAPSession, *pstPLDAPSessionOperations, entryOp,
			attribOp, *pstArticulo);
	selectEntries(*pstPLDAPSession, *pstPLDAPSessionOperations, sURL);

	deleteEntry(*pstPLDAPSession, *pstPLDAPSessionOperations, entryOp,
			(*pstArticulo).uiArticleID);
	selectEntries(*pstPLDAPSession, *pstPLDAPSessionOperations, sURL);
	/********************************************************************************
	 *	Hasta aca es la prueba														*
	 *******************************************************************************/

	return 1;
}

void guardarNoticiaEnCache(stArticle stArticulo) {
	printf("Entre a guardarNoticiaEnCache.\n");
	return;
}

char* formatearArticuloAHTML(stArticle stArticulo) {
	printf("Entro a formatear la noticia a HTML.\n");

	char* response;
	strcpy(
			response,
			"<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, pero aca tendria que estar devolviendo una noticia en particular</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>");

	return response;
}

char* processRequestTypeNews(char* sRecursoPedido,
		stThreadParameters* pstParametros) {

	/*	TODO: Aca tengo que parsear el recurso y armar el criterio	*/

	char* sCriterio = "utnArticleID=*"; /*	TODO: Aca tendria que parsear sURL y crear un filtro como corresponde. ahora esta hardocodeado!	*/

	stArticle stArticulo;
	stArticulo.sBody = "este es el body!";
	stArticulo.sHead = "este es el head!";
	stArticulo.sNewsgroup = "blablabla.com";
	stArticulo.uiArticleID = 12345;

	if (!buscarNoticiaEnCache(&stArticulo, sCriterio)) {
		/*	Como no encontre la noticia en Cache, la busco en la BD	*/
		buscarNoticiaEnBD(&stArticulo, sCriterio,
				(*pstParametros).pstPLDAPSession,
				(*pstParametros).pstPLDAPSessionOperations);

		/*	Como no la encontre en Cache, ahora la guardo en cache para que este la proxima vez.	*/
		guardarNoticiaEnCache(stArticulo);
	}
	/*	Para este momento ya tengo la noticia que tengo que responderle al cliente seteada	*/

	return formatearArticuloAHTML(stArticulo);
}

char* processRequestTypeNewsgroup(char* sRecursoPedido,
		stThreadParameters* pstParametros) {

	return "<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, aca tendria que haber devuelto el listado de grupos de noticias</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>";
}
char* processRequestTypeNewsList(char* sRecursoPedido,
		stThreadParameters* pstParametros) {

	return "<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, aca tendria que haber devuelto el listado de noticias para un grupo de noticias en particular.</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>";
}
