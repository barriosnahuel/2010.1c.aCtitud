#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include "../Logger/logger.h"
#include "configuration.h"
#include "LdapWrapperHandler.h"

#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
#define APP_NAME_FOR_LOGGER "HTTPServer"
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
void liberarRecursos(int ficheroServer, PLDAP_CONTEXT stPLDAPContext,
		PLDAP_CONTEXT_OP stPLDAPContextOperations,
		PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, stConfiguracion stConf);
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

/**
 * Esta funcion parsea el get que recibimos del browser y nos devuelve el recurso pedido por el usuario.
 */
char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente);

/**
 * De un string del estilo /grupoDeNoticias/Noticia obtengo grupoDeNoticias.
 */
char* obtenerGrupoDeNoticias(char* sRecursoPedido);

/**
 * Funcion que devuelve 1 en caso de que el recurso sea del estilo /grupoDeNoticias/Noticia. Devolverá 0 si es /grupoDeNoticias nomas.
 */
int llevaNoticia(char* sRecursoPedido);

/**
 * De un string del estilo /grupoDeNoticias/Noticia obtengo Noticia.
 */
char* obtenerNoticia(char* sRecursoPedido);

/************************************************************************************************************
 *	Aca comienzan las definiciones de las funciones															*
 ************************************************************************************************************/
int main(void) {
	/****************************************
	 *	Cargo el archivo de configuracion	*
	 ****************************************/
	stConfiguracion stConf;

	if (!CargaConfiguracion("config.conf\0", &stConf)) {
		printf("Archivo de configuracion no valido.\n");
		LoguearError("Archivo de configuracion no vÃ¡lido.", "HTTPServer");
		return -1;
	} else {
		LoguearInformacion("Archivo de configuracion cargado correctamente.",
				APP_NAME_FOR_LOGGER);
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
		LoguearError("No se pudo conectar a OpenDS.", APP_NAME_FOR_LOGGER);
		return -1;
	} else
		printf("Conectado a OpenDS en: IP=%s; Port=%d\n", stConf.czBDServer,
				stConf.uiBDPuerto);

	/********************************************************
	 *	Creo la conexion con el socket y lo dejo listo		*
	 ********************************************************/
	int ficheroServer; /* los ficheros descriptores */
	struct sockaddr_in server; /* para la informacion de la direccion del servidor */
	if (!crearConexionConSocket(&stConf, &ficheroServer, &server)) {
		LoguearError(
				"No se pudo crear la conexion con el socket y dejarlo listo para escuchar conexiones entrantes.",
				APP_NAME_FOR_LOGGER);
		return -1;
	} else
		printf(
				"Aplicacion levantada en: IP=%s; Port=%d\n\nEscuchando conexiones entrantes...\n",
				inet_ntoa(server.sin_addr), stConf.uiAppPuerto);

	/********************************************************************************
	 *	Itero de manera infinita??? recibiendo conexiones de != clientes			*
	 *******************************************************************************/
	/*	while (1) {*/
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
		LoguearError("Error al aceptar la conexion.", APP_NAME_FOR_LOGGER);
	printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
	/*	}*/

	printf(
			"Le doy al thread 8 segundos para responderle al cliente antes que cierre todo... ;)\n");
	sleep(8);

	printf("Ahora cierro socket, db, etc...\n");
	liberarRecursos(ficheroServer, stPLDAPContext, stPLDAPContextOperations,
			stPLDAPSession, stPLDAPSessionOperations, stConf);

	/*	TODO: Libero lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
}

void* procesarRequestFuncionThread(void* threadParameters) {
	LoguearDebugging("--> procesarRequestFuncionThread()", APP_NAME_FOR_LOGGER);
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);
	printf("---------------- Procesando thread xD -----------------\n");
	int bytesRecibidos;
	char* sResponse;
	char sRecursoPedido[1024];
	char sGrupoDeNoticias[1024];
	char sNoticia[1024];

	char sMensajeHTTPCliente[1024];/*	TODO: esto seria toda la url me parece, y en cada
	 funcion la parseo y creo el criterio por el que voy a buscar en OpenDS!!			*/

	int lenMensajeHTTPCliente;
	lenMensajeHTTPCliente = 1024;
	bytesRecibidos = recv(stParametros.ficheroCliente, sMensajeHTTPCliente,
			lenMensajeHTTPCliente, 0);

	printf("Recibi %d bytes del usuario.\n\n", bytesRecibidos);
	printf("############################################################\n\n");
	printf("%s", sMensajeHTTPCliente);
	printf("############################################################\n");

	strcpy(sRecursoPedido, obtenerRecursoDeCabecera(sMensajeHTTPCliente));

	printf("El usuario pidio el recurso: %s\n", sRecursoPedido);
	if (strlen(sRecursoPedido) == 1) {
		printf("El recurso es una '/', por lo tanto hay que mostrarle el listado de newsgroups.\n");
		/* El gil de nahuel hace el select correspondiente xD */
	} else {
		/* Obtengo el grupo de noticias. */
		strcpy(sGrupoDeNoticias, obtenerGrupoDeNoticias(sRecursoPedido));
		printf("El grupo de noticias es: %s\n", sGrupoDeNoticias);
		
		/* Me fijo si ademas del grupo de noticias viene la noticia */
		if (llevaNoticia(sRecursoPedido)) {
			/* Obtengo la noticia de dicho grupo. */
			strcpy(sNoticia, obtenerNoticia(sRecursoPedido));
			printf("La noticia es: %s\n", sNoticia);
		}
		
	}

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

	LoguearDebugging("<-- procesarRequestFuncionThread()", APP_NAME_FOR_LOGGER);
	thr_exit(0);/*	Termino el thread.*/

	return 0;
}

int crearConexionLDAP(stConfiguracion* stConf, PLDAP_CONTEXT* pstPLDAPContext,
		PLDAP_CONTEXT_OP* pstPLDAPContextOperations,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	LoguearDebugging("--> crearConexionLDAP()", APP_NAME_FOR_LOGGER);

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
	LoguearDebugging("<-- crearConexionLDAP()", APP_NAME_FOR_LOGGER);
	return 1;
}

/**
 * Crea el socket, lo bindea, y lo deja listo para escuchar conexiones entrantes.
 */
int crearConexionConSocket(stConfiguracion* stConf, int* ficheroServer,
		struct sockaddr_in* server) {
	LoguearDebugging("--> crearConexionConSocket()", APP_NAME_FOR_LOGGER);

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

	LoguearDebugging("<-- crearConexionConSocket()", APP_NAME_FOR_LOGGER);
	return 1;
}

/**
 * 1. Cierra el socket.
 * 2. Cierra contexto, sesion y demas "cosas" de OpenDS y LDAP Wrapper.
 */
void liberarRecursos(int ficheroServer, PLDAP_CONTEXT stPLDAPContext,
		PLDAP_CONTEXT_OP stPLDAPContextOperations,
		PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, stConfiguracion stConf) {
	LoguearDebugging("--> liberarRecursos()", APP_NAME_FOR_LOGGER);

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
	LoguearDebugging("<-- liberarRecursos()", APP_NAME_FOR_LOGGER);
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sURL) {
	LoguearDebugging("--> buscarNoticiaEnCache()", APP_NAME_FOR_LOGGER);

	LoguearDebugging("<-- buscarNoticiaEnCache()", APP_NAME_FOR_LOGGER);
	return 0;
}

int buscarNoticiaEnBD(stArticle* pstArticulo, char* sURL,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	LoguearDebugging("--> buscarNoticiaEnBD()", APP_NAME_FOR_LOGGER);

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

	LoguearDebugging("<-- buscarNoticiaEnBD()", APP_NAME_FOR_LOGGER);
	return 1;
}

void guardarNoticiaEnCache(stArticle stArticulo) {
	LoguearDebugging("--> guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);

	printf("Entre a guardarNoticiaEnCache.\n");

	LoguearDebugging("<-- guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);
	return;
}

char* formatearArticuloAHTML(stArticle stArticulo) {
	LoguearDebugging("--> formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);

	char* response;
	strcpy(
			response,
			"<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, pero aca tendria que estar devolviendo una noticia en particular</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>");

	LoguearDebugging("<-- formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);
	return response;
}

char* processRequestTypeNews(char* sRecursoPedido,
		stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeNews()", APP_NAME_FOR_LOGGER);

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

	LoguearDebugging("<-- processRequestTypeNews()", APP_NAME_FOR_LOGGER);
	return formatearArticuloAHTML(stArticulo);
}

char* processRequestTypeNewsgroup(char* sRecursoPedido,
		stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeNewsgroup()", APP_NAME_FOR_LOGGER);

	/*	TODO: Aca proceso.	*/

	LoguearDebugging("<-- processRequestTypeNewsgroup()", APP_NAME_FOR_LOGGER);
	return "<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, aca tendria que haber devuelto el listado de grupos de noticias</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>";
}
char* processRequestTypeNewsList(char* sRecursoPedido,
		stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeNewsList()", APP_NAME_FOR_LOGGER);

	/*	TODO: Aca proceso.	*/

	LoguearDebugging("<-- processRequestTypeNewsList()", APP_NAME_FOR_LOGGER);
	return "<HTML><HEAD><TITLE>este es el titulo de la pagina</TITLE></HEAD><BODY><P>Esto ya es html, aca tendria que haber devuelto el listado de noticias para un grupo de noticias en particular.</P><TABLE><TR><TD>esta es la primer fila</TD></TR><TR><TD>esta es la segunda fila</TD></TR></TABLE></BODY></HTML>";
}

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente) {
	LoguearDebugging("--> obtenerRecursoDeCabecera()", APP_NAME_FOR_LOGGER);

	int i = 0;
	int j = 0;
	int k = 0;
	char recurso[1024];

	/* En este while saco el GET */
	while (sMensajeHTTPCliente[i] != '/') {
		i = i + 1;
	}
	k = i;

	if (sMensajeHTTPCliente[k + 1] == 32) {
		strcpy(recurso, "/");
	} else {
		/* Aca situo a k al final del recurso (donde esta el primer espacio) */
		while (sMensajeHTTPCliente[k] != '.') {
			k = k + 1;
		}

		/* Cuando i = k quiere decir que i llego al .html (al punto en realidad). O sea que ya obtuve el recurso */
		while (i != k) {
			recurso[j] = sMensajeHTTPCliente[i];
			i = i + 1;
			j = j + 1;
		}

		recurso[j] = '\0';

	}

	LoguearDebugging("<-- obtenerRecursoDeCabecera()", APP_NAME_FOR_LOGGER);
	return recurso;

}

char* obtenerGrupoDeNoticias(char* sRecursoPedido) {
	LoguearDebugging("--> obtenerGrupoDeNoticias()", APP_NAME_FOR_LOGGER);

	int i = 1;
	int j = 0;
	char grupoDeNoticias[1024];

	while (sRecursoPedido[i] != '/' && sRecursoPedido[i] != '.') {
		grupoDeNoticias[j] = sRecursoPedido[i];
		j = j + 1;
		i = i + 1;
	}
	grupoDeNoticias[j] = '\0';
	LoguearDebugging("<-- obtenerGrupoDeNoticias()", APP_NAME_FOR_LOGGER);
	return grupoDeNoticias;
}

char* obtenerNoticia(char* sRecursoPedido) {
	LoguearDebugging("--> obtenerDeNoticia()", APP_NAME_FOR_LOGGER);
	int i = 1;
	int j = 0;
	char noticia[1024];

	while (sRecursoPedido[i] != '/') {
		i = i + 1;
	}
	
	while(sRecursoPedido[i+1] != '\0') {
		noticia[j] = sRecursoPedido[i+1];
		i = i + 1;
		j = j + 1;
	}
	
	noticia[j] = '\0';
	LoguearDebugging("<-- obtenerDeNoticia()", APP_NAME_FOR_LOGGER);
	return noticia;
}

int llevaNoticia(char* sRecursoPedido) {
	LoguearDebugging("--> llevaNoticia()", APP_NAME_FOR_LOGGER);
	int i = 1;
	
	while(sRecursoPedido[i] != '/' && sRecursoPedido[i] != '\0' ) {
		i = i + 1;
	}
	
	if(sRecursoPedido[i] == '\0') {
		return 0;
	}
	LoguearDebugging("<-- llevaNoticia()", APP_NAME_FOR_LOGGER);
	return 1;
}

