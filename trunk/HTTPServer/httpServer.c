#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Logger/logger.h"
#include "configuration.h"
#include "LdapWrapperHandler.h"

#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
#define APP_NAME_FOR_LOGGER "HTTPServer"
#define REQUEST_TYPE_NEWSGROUP 1	/*	Indica que se esta solicitando el listado de newsgroups.	*/
#define REQUEST_TYPE_NEWS_LIST 2	/*	Indica que se esta solicitando el listado de noticias para un newsgroup especifico. 	*/
#define REQUEST_TYPE_NEWS 3			/*	Indica que se esta solicitando una noticia en particular.	*/
#define MAX_CHARACTERS_FOR_RESPONSE 5000	/*	TODO: La cantidad maxima de caracteres para el response esta bien 5000?	*/

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

	stConfiguracion* pstConfiguration;
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
void liberarRecursos( int 				ficheroServer
					, PLDAP_CONTEXT		stPLDAPContext
					, PLDAP_CONTEXT_OP	stPLDAPContextOperations
					, PLDAP_SESSION 	stPLDAPSession
					, PLDAP_SESSION_OP 	stPLDAPSessionOperations
					, stConfiguracion	stConf);
/**
 * Procesa un request del tipo grupo de noticias. Es decir, el listado de grupo de noticias disponibles
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeListadoGruposDeNoticias(stThreadParameters* pstParametros);
/**
 * Procesa un request del tipo news list. Es decir, busca las noticias pertenecientes a un grupo de noticias en particular
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeListadoDeNoticias(char* sGrupoDeNoticias,
		stThreadParameters* pstParametros);
/**
 * Procesa un request del tipo news. Es decir, busca una noticia en particular en base al sCriterio
 * y devuelve un response en formato HTML.
 */
char* processRequestTypeUnaNoticia(char* sGrupoDeNoticias, char* sArticleID,
		stThreadParameters* pstParametros);
/**
 * Busca la noticia en la cache, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID);
/**
 * Busca la noticia en la BD, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnBD(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations);
/**
 * Guarda la noticia que se le pasa como parametro en cache.
 */
void guardarNoticiaEnCache(stArticle stArticulo);
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
 * Funcion que devuelve 1 en caso de que el recurso sea del estilo /grupoDeNoticias/Noticia. Devolver� 0 si es /grupoDeNoticias nomas.
 */
int llevaNoticia(char* sRecursoPedido);

/**
 * De un string del estilo /grupoDeNoticias/Noticia obtengo Noticia.
 */
char* obtenerNoticia(char* sRecursoPedido);

/**
 * Quita los elementos repetidos del array listadoGruposDeNoticias, y pone ceros en las posiciones donde se repiten.
 */
VOID quitarRepetidos(char* listadoGruposDeNoticias[], int iCantidadDeGruposDeNoticias);

/**
 * Pasa los campos que sean distintos de cero del array listadoGrupoNoticiasRepetidos a listadoGrupoNoticiasSinRepetir y retorna la cantidad de campos que posee este
 * ultimo.
 */
unsigned int pasarArrayEnLimpio(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias, char* listadoGrupoNoticiasSinRepetir[]);

/**
 * Reemplaza los %20 por espacios y devuelve la cadena sin %20.
 */
char* formatearEspacios(char* sRecursoPedido, char* sRecursoPedidoSinEspacios);

/************************************************
 *	Declaracion funciones relacionadas al HTML	*
 ************************************************/
/**
 * Formatea la noticia la noticia a un char* en formato HTML para poder enviarle eso al cliente
 * y visualizar bien la pagina HTML.
 */
char* formatearArticuloAHTML(stArticle* pstArticulo);
char* formatearListadoDeNocitiasAHTML(char* sGrupoDeNoticias, stArticle listadoDeNoticias[], unsigned int uiCantidadDeNoticias);
char* formatearListadoDeGruposDeNoticiasAHTML(char* listadoGruposDeNoticias[], int len);
/**
 * Arma un tag a de HTML con un hipervinculo al sURL que se le pasa como parametro,
 * y mostrando el  sNombreDelLink como el hipervinculo.
 */
char* armarLinkCon(char* sURL, char* sNombreDelLink);

/************************************************************************************************************
 *	Aca comienzan las definiciones de las funciones															*
 ************************************************************************************************************/
int main(void) {
	char* sLogMessage;	/*	Es la variable que uso para generar el msj para el log.	*/
	/****************************************
	 *	Cargo el archivo de configuracion	*
	 ****************************************/
	stConfiguracion stConf;

	if (!CargaConfiguracion("config.conf\0", &stConf)) {
		printf("Archivo de configuracion no valido.\n");
		LoguearError("Archivo de configuracion no válido.", "HTTPServer");
		return -1;
	}
	LoguearInformacion("Archivo de configuracion cargado correctamente.", APP_NAME_FOR_LOGGER);
	asprintf(&sLogMessage, "Puerto de la aplicacion: %d.", stConf.uiAppPuerto);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	asprintf(&sLogMessage, "IP LDAP/OpenDS: %s.", stConf.czBDServer);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	asprintf(&sLogMessage, "Puerto de LDAP/OpenDS: %d.", stConf.uiBDPuerto);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);

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
	}
	asprintf(&sLogMessage, "Conectado a OpenDS en: IP=%s; Port=%d.", stConf.czBDServer, stConf.uiBDPuerto);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);


/*	Esto es prueba!!	*/
/*
  	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 1);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 2);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 3);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 4);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 5);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 6);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 7);
	deleteEntry(stPLDAPSession, stPLDAPSessionOperations, 8);
*/
/*		stArticle stArticulo;

		stArticulo.sBody = "un body para la 1";
		stArticulo.sHead = "un head para la 1";
		stArticulo.sNewsgroup = "Clarin";
		stArticulo.uiArticleID = 1;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 2";
		stArticulo.sHead = "un head para la 2";
		stArticulo.sNewsgroup = "Clarin";
		stArticulo.uiArticleID = 2;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 3";
		stArticulo.sHead = "un head para la 3";
		stArticulo.sNewsgroup = "La Nacion";
		stArticulo.uiArticleID = 3;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 4";
		stArticulo.sHead = "un head para la 4";
		stArticulo.sNewsgroup = "Pagina 12";
		stArticulo.uiArticleID = 4;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 5";
		stArticulo.sHead = "un head para la 5";
		stArticulo.sNewsgroup = "Clarin";
		stArticulo.uiArticleID = 5;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 6";
		stArticulo.sHead = "un head para la 6";
		stArticulo.sNewsgroup = "La Nacion";
		stArticulo.uiArticleID = 6;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 7";
		stArticulo.sHead = "un head para la 7";
		stArticulo.sNewsgroup = "Clarin";
		stArticulo.uiArticleID = 7;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 8 (clarin en minuscula)";
		stArticulo.sHead = "un head para la 8 (clarin en minuscula)";
		stArticulo.sNewsgroup = "clarin";
		stArticulo.uiArticleID = 8;

		stArticulo.sBody = "un body para la 9";
		stArticulo.sHead = "un head para la 9";
		stArticulo.sNewsgroup = "Cronica";
		stArticulo.uiArticleID = 9;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 10";
		stArticulo.sHead = "un head para la 10";
		stArticulo.sNewsgroup = "Fruta";
		stArticulo.uiArticleID = 10;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		stArticulo.sBody = "un body para la 11";
		stArticulo.sHead = "un head para la 11";
		stArticulo.sNewsgroup = "Cronica";
		stArticulo.uiArticleID = 11;
		insertEntry(stPLDAPSession, stPLDAPSessionOperations, stArticulo);

		selectAndPrintEntries(stPLDAPSession, stPLDAPSessionOperations, "(utnArticleID=*)");
*/

	/********************************************************
	 *	Creo la conexion con el socket y lo dejo listo		*
	 ********************************************************/
	int ficheroServer; 			/* Fichero descriptor de nuestro server. */
	struct sockaddr_in server;	/* Para la informacion de la direccion del servidor. */
	if (!crearConexionConSocket(&stConf, &ficheroServer, &server)){
		LoguearError("No se pudo crear la conexion con el socket y dejarlo listo para escuchar conexiones entrantes.", APP_NAME_FOR_LOGGER);
		return -1;
	}
	strcpy(stConf.czAppServer, inet_ntoa(server.sin_addr));
	asprintf(&sLogMessage, "Aplicacion levantada en: IP=%s; Port=%d.", stConf.czAppServer, stConf.uiAppPuerto);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	printf("* HTTPServer escuchando conexiones entrantes...\n");

	/********************************************************************************
	 *	Itero de manera infinita??? recibiendo conexiones de != clientes			*
	 *******************************************************************************/
	while (1) {
		int sin_size = sizeof(struct sockaddr_in);
		struct sockaddr_in client; /* para la informacion de la direccion del cliente */

		int ficheroCliente = accept(ficheroServer, (struct sockaddr *) &client, &sin_size);
		if (ficheroCliente != -1) {
			/*	Si no hubo errores aceptando la conexion, entonces la gestiono. */

			thread_t threadProcesarRequest;/*	Declaro un nuevo thread. */

			/*	Le seteo los parametros al nuevo thread.	*/
			stThreadParameters stParameters;
			stParameters.ficheroCliente= ficheroCliente;
			stParameters.pstPLDAPSession= &stPLDAPSession;
			stParameters.pstPLDAPSessionOperations= &stPLDAPSessionOperations;
			stParameters.pstConfiguration= &stConf;

			if (thr_create(0, 0, (void*) &procesarRequestFuncionThread,
					(void*) &stParameters, 0, &threadProcesarRequest) != 0)
				LoguearError("No se pudo crear un nuevo thread para procesar el request.", APP_NAME_FOR_LOGGER);
		} else
			LoguearError("Error al aceptar la conexion.", APP_NAME_FOR_LOGGER);

		asprintf(&sLogMessage, "Se obtuvo una conexion desde %s.", inet_ntoa(client.sin_addr));
		LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	}

	printf("Le doy al thread 8 segundos para responderle al cliente antes que cierre todo... ;)\n");
	sleep(8);

	liberarRecursos(ficheroServer, stPLDAPContext, stPLDAPContextOperations,
			stPLDAPSession, stPLDAPSessionOperations, stConf);

	/*	TODO: Libero lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
}

void* procesarRequestFuncionThread(void* threadParameters) {
	LoguearDebugging("--> procesarRequestFuncionThread()", APP_NAME_FOR_LOGGER);
	char* sLogMessage;
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);

	LoguearInformacion("Comienzo a procesar un nueco thread.", APP_NAME_FOR_LOGGER);
	int bytesRecibidos;
	char sMensajeHTTPCliente[1024];/*	TODO: No pueden valer lo mismo estos dos.	*/

	int lenMensajeHTTPCliente;
	lenMensajeHTTPCliente = 1024;
	bytesRecibidos = recv(stParametros.ficheroCliente, sMensajeHTTPCliente, lenMensajeHTTPCliente, 0);


	asprintf(&sLogMessage, "Recibi %d bytes del usuario.", bytesRecibidos);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	asprintf(&sLogMessage, "El mensaje HTTP que recibimos del cliente es:\n%s", sMensajeHTTPCliente);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);

	char sRecursoPedido[1024];/*	TODO: Esto tendria que ser menos.	*/
	char sRecursoPedidoSinEspacios[1024];
	strcpy(sRecursoPedido, obtenerRecursoDeCabecera(sMensajeHTTPCliente));

	asprintf(&sLogMessage, "El usuario pidio el recurso: \"%s\".", sRecursoPedido);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);
	
	formatearEspacios(sRecursoPedido, sRecursoPedidoSinEspacios);

	/*	En Ubuntu/Chrome, /Firefox (no se en otras situaciones) luego de enviar el response, recibimos un
	    request por este recurso. Decidimos no tratarlo, por lo tanto cerramos la conexion.	*/
	if(strcmp(sRecursoPedido, "/favicon")==0){
		close(stParametros.ficheroCliente);
		LoguearInformacion("Se cerro el fichero descriptor del cliente porque el request pedia el recurso /favicon.ico.", APP_NAME_FOR_LOGGER);

		LoguearInformacion("Termino el thread.", APP_NAME_FOR_LOGGER);
		thr_exit(0);
	}

	char* sGrupoDeNoticia= (char*)malloc(sizeof(char)*OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT);
	char* sArticleID= (char*)malloc(sizeof(char)*OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT);

	
	/*	Obtengo la operacion, el grupo de noticia y noticia	segun corresponda*/
	unsigned int uiOperation;
	if (strlen(sRecursoPedido) == 1)
		uiOperation= REQUEST_TYPE_NEWSGROUP;
	else {
		/* Obtengo el grupo de noticias. */
		
		strcpy(sGrupoDeNoticia, obtenerGrupoDeNoticias(sRecursoPedidoSinEspacios));
		/* Me fijo si ademas del grupo de noticias viene la noticia */
		if (llevaNoticia(sRecursoPedidoSinEspacios)) {
			/* Obtengo la noticia de dicho grupo. */
			strcpy(sArticleID, obtenerNoticia(sRecursoPedidoSinEspacios));
			uiOperation= REQUEST_TYPE_NEWS;
		}
		else
			/*	Como la URL no tiene el sufijo /noticiaID, el cliente me esta pidiendo
				 el listado de noticias para un grupo de noticia dado.					*/
			uiOperation= REQUEST_TYPE_NEWS_LIST;
	}

	char* sResponse= (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	switch (uiOperation) {
	case REQUEST_TYPE_NEWSGROUP:
		LoguearInformacion("Proceso la obtencion del listado de grupos de noticias.", APP_NAME_FOR_LOGGER);
		sResponse = processRequestTypeListadoGruposDeNoticias(&stParametros);
		break;
	case REQUEST_TYPE_NEWS_LIST:
		LoguearInformacion("Proceso la obtencion del listado de noticias para un grupo de noticias.", APP_NAME_FOR_LOGGER);
		sResponse = processRequestTypeListadoDeNoticias(sGrupoDeNoticia, &stParametros);
		break;
	case REQUEST_TYPE_NEWS:
		LoguearInformacion("Proceso la obtencion de una noticia en particular.", APP_NAME_FOR_LOGGER);
		sResponse = processRequestTypeUnaNoticia(sGrupoDeNoticia, sArticleID, &stParametros);
		break;
	default:
		LoguearInformacion("Por default, obtengo el listado de grupos de noticias.", APP_NAME_FOR_LOGGER);
		sResponse = processRequestTypeListadoGruposDeNoticias(&stParametros);
		break;
	}
	free(sGrupoDeNoticia);
	free(sArticleID);

	int len, bytesEnviados;
	len = strlen(sResponse);
	
	char protocolo = (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	protocolo = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
	int lenProtocolo = 41;
	int asd;
	
	if((asd = send(stParametros.ficheroCliente, sResponse, len, 0)) == -1) {
		printf("Error en el send del protocolo\n");
	}

	if ((bytesEnviados = send(stParametros.ficheroCliente, sResponse, len, 0)) == -1)
		LoguearError("No se pudo enviar el response al cliente.", APP_NAME_FOR_LOGGER);

	free(sResponse);

	close(stParametros.ficheroCliente);
	LoguearInformacion("Se cerro el fichero descriptor del cliente.", APP_NAME_FOR_LOGGER);

	LoguearInformacion("Termino el thread.", APP_NAME_FOR_LOGGER);
	thr_exit(0);
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
		LoguearError("No se pudo obtener el fichero descriptor del socket.", APP_NAME_FOR_LOGGER);
		exit(-1);
	}
	LoguearInformacion("Se obtuvo el fichero descriptor correctamente.", APP_NAME_FOR_LOGGER);

	(*server).sin_family = AF_INET;
	(*server).sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra direccion IP automaticamente */
	(*server).sin_port = htons((*stConf).uiAppPuerto); /* htons transforma el short de maquina a short de red */
	bzero(&((*server).sin_zero), 8); /* Escribimos ceros en el resto de la estructura*/

	if (bind(*ficheroServer, (const struct sockaddr *) &(*server),
			sizeof(struct sockaddr)) == -1) {
		LoguearError("Error al asociar el puerto al socket.", APP_NAME_FOR_LOGGER);
		exit(-1);
	}
	LoguearInformacion("Se asocio bien el puerto al socket.", APP_NAME_FOR_LOGGER);

	if (listen(*ficheroServer, BACKLOG) == -1) {
		LoguearError("No se pudo dejar escuchando al puerto.", APP_NAME_FOR_LOGGER);
		exit(-1);
	}

	LoguearDebugging("<-- crearConexionConSocket()", APP_NAME_FOR_LOGGER);
	return 1;
}

/**
 * 1. Cierra el socket.
 * 2. Cierra contexto, sesion y demas "cosas" de OpenDS y LDAP Wrapper.
 */
void liberarRecursos(int 				ficheroServer
					, PLDAP_CONTEXT		stPLDAPContext
					, PLDAP_CONTEXT_OP	stPLDAPContextOperations
					, PLDAP_SESSION 	stPLDAPSession
					, PLDAP_SESSION_OP	stPLDAPSessionOperations
					, stConfiguracion	stConf) {
	LoguearDebugging("--> liberarRecursos()", APP_NAME_FOR_LOGGER);

	/*	Cierro/Libero lo relacionado a la BD	*/
	stPLDAPSessionOperations->endSession(stPLDAPSession);
	freeLDAPSessionOperations(stPLDAPSessionOperations);
	freeLDAPSession(stPLDAPSession);
	freeLDAPContext(stPLDAPContext);
	freeLDAPContextOperations(stPLDAPContextOperations);
	LoguearInformacion("Se libero la memoria de LDAP/OpenDS correctamente.", APP_NAME_FOR_LOGGER);

	/*	Cierro el socket	*/
	close(ficheroServer);
	LoguearInformacion("Libere el fichero descriptor de nuestro server correctamente.", APP_NAME_FOR_LOGGER);

	LoguearDebugging("<-- liberarRecursos()", APP_NAME_FOR_LOGGER);
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticia, char* sArticleID) {
	LoguearDebugging("--> buscarNoticiaEnCache()", APP_NAME_FOR_LOGGER);

	LoguearDebugging("<-- buscarNoticiaEnCache()", APP_NAME_FOR_LOGGER);
	return 0;/*	TODO: Esta hardcodeado el false para que entre a buscar a la BD	*/
}

int buscarNoticiaEnBD(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	LoguearDebugging("--> buscarNoticiaEnBD()", APP_NAME_FOR_LOGGER);

	*pstArticulo= getArticle(*pstPLDAPSession, *pstPLDAPSessionOperations, sGrupoDeNoticias, sArticleID);

	LoguearDebugging("<-- buscarNoticiaEnBD()", APP_NAME_FOR_LOGGER);
	return 1;
}

void guardarNoticiaEnCache(stArticle stArticulo) {
	LoguearDebugging("--> guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);

	LoguearDebugging("<-- guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);
	return;
}

char* formatearArticuloAHTML(stArticle* pstArticulo) {
	LoguearDebugging("--> formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);

	char* response;
	asprintf(&response, "<HTML><HEAD><TITLE>%s</TITLE></HEAD><BODY><P><B>Grupo de noticias: %s</B></P><P>%s</P></BODY></HTML>", (*pstArticulo).sHead, (*pstArticulo).sNewsgroup, (*pstArticulo).sBody);

	LoguearDebugging("<-- formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);
	return response;
}

char* formatearEspacios(char* sRecursoPedido, char* sRecursoPedidoSinEspacios) {
	LoguearDebugging("--> formatearEspacios()", APP_NAME_FOR_LOGGER);
	int i = 0;
	int j = 0;
	
	while(sRecursoPedido[i] != '\0') {
		if(sRecursoPedido[i] == '%') {
			sRecursoPedidoSinEspacios[j] = ' ';
			i = i + 3;
			j++;
		}
		else {
			sRecursoPedidoSinEspacios[j] = sRecursoPedido[i];
			i++;
			j++;
		}
	}
	sRecursoPedidoSinEspacios[j] = '\0';
	
	LoguearDebugging("<-- formatearEspacios()", APP_NAME_FOR_LOGGER);
}


char* processRequestTypeUnaNoticia(char* sGrupoDeNoticias, char* sArticleID,
		stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeUnaNoticia()", APP_NAME_FOR_LOGGER);

	stArticle stArticulo;
	if (!buscarNoticiaEnCache(&stArticulo, sGrupoDeNoticias, sArticleID)) {
		/*	Como no encontre la noticia en Cache, la busco en la BD	*/
		buscarNoticiaEnBD(&stArticulo, sGrupoDeNoticias, sArticleID,
				(*pstParametros).pstPLDAPSession,
				(*pstParametros).pstPLDAPSessionOperations);

		/*	Como no la encontre en Cache, ahora la guardo en cache para que este la proxima vez.	*/
		guardarNoticiaEnCache(stArticulo);
	}
	/*	Para este momento ya tengo la noticia que tengo que responderle al cliente seteada	*/

	LoguearDebugging("<-- processRequestTypeUnaNoticia()", APP_NAME_FOR_LOGGER);
	return formatearArticuloAHTML(&stArticulo);
}

char* processRequestTypeListadoGruposDeNoticias(stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoGrupoDeNoticias()", APP_NAME_FOR_LOGGER);

	/*	Sumo 3 por el =* y el \0	*/
	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+1+1];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, "*");

	int q;
	int k;
	unsigned int cantidadDeGrupos= 0;
	unsigned int cantidadDeGruposSinRepetir= 0;
	char* listadoGrupoNoticiasRepetidos[1000];/*	TODO: Chequear este 1000, ver como deshardcodearlo	*/
	char* listadoGrupoNoticiasSinRepetir[1000];
	/* Este memset es importantisimo, ya que si no le seteamos ceros al array, y queremos ingresar a una posicion que no tiene nada tira Seg Fault */
	memset(listadoGrupoNoticiasSinRepetir, 0, 1000);
	LoguearDebugging("Hago el select a OpenDS", APP_NAME_FOR_LOGGER);
	selectEntries(listadoGrupoNoticiasRepetidos, &cantidadDeGrupos, (*(*pstParametros).pstPLDAPSession), (*(*pstParametros).pstPLDAPSessionOperations), sCriterio, OPENDS_SELECT_GRUPO_DE_NOTICIA);
	
	printf("La cantidad total de grupos de noticias repetidos es: %d\n", cantidadDeGrupos);
	
	quitarRepetidos(&listadoGrupoNoticiasRepetidos, cantidadDeGrupos);
	
	/*	TODO: Esto se borra o se loguea?	*/
	for(q = 0; q < cantidadDeGrupos; q++) printf("Contenido de la posicion %d del array es: %s\n", q, listadoGrupoNoticiasRepetidos[q]);
	
	cantidadDeGruposSinRepetir = pasarArrayEnLimpio(&listadoGrupoNoticiasRepetidos, cantidadDeGrupos, &listadoGrupoNoticiasSinRepetir);

	/*	TODO: Esto se borra o se loguea?	*/
	for(k = 0; k < cantidadDeGruposSinRepetir; k++) printf("Contenido de la posicion %d del array LIMPIO es: %s\n", k, listadoGrupoNoticiasSinRepetir[k]);
	
	printf("La cantidad total de grupos de noticias SIN repetir es: %d\n", cantidadDeGruposSinRepetir);

	LoguearDebugging("<-- processRequestTypeListadoGrupoDeNoticias()", APP_NAME_FOR_LOGGER);
	return formatearListadoDeGruposDeNoticiasAHTML(listadoGrupoNoticiasSinRepetir, cantidadDeGruposSinRepetir);
}

VOID quitarRepetidos(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias) {
	int i;
	int j;
	char grupoDeNoticias[70];/*	TODO: 70???	*/
	
	for(i = 0; i < iCantidadDeGruposDeNoticias; i++) {
		strcpy(grupoDeNoticias, listadoGrupoNoticiasRepetidos[i]);
		for(j = i+1; j < iCantidadDeGruposDeNoticias; j++) {
			if(strcmp(grupoDeNoticias, listadoGrupoNoticiasRepetidos[j]) == 0) {
				listadoGrupoNoticiasRepetidos[j] = "0\0";
			}
		}
	}
}

unsigned int pasarArrayEnLimpio(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias, char* listadoGrupoNoticiasSinRepetir[]) {
	int p;
	int l = 0;
	
	for(p = 0; p < iCantidadDeGruposDeNoticias; p++) {
		if(strcmp("0", listadoGrupoNoticiasRepetidos[p]) != 0) {
			listadoGrupoNoticiasSinRepetir[l] = listadoGrupoNoticiasRepetidos[p];
			l++;
		}
	}
	return l;
}

char* formatearListadoDeGruposDeNoticiasAHTML(char* listadoGrupoDeNoticiasSinRepetir[], int iCantidadDeGruposDeNoticias){
	LoguearDebugging("--> formatearListadoDeGruposDeNoticiasAHTML()", APP_NAME_FOR_LOGGER);

	/*	1+OPEN...+5+1 Es igual a: /nombreGrupoNoticia.html\0	*/
	char* sURL= (char*)malloc(sizeof(char)*(1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+5+1));
	char* response= (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	strcpy(response, "<HTML><HEAD><TITLE>Listado de grupos de noticias</TITLE></HEAD><BODY><P><B>Listado de grupos de noticias</B></P><OL>");

	int i;
	for(i=0; i<iCantidadDeGruposDeNoticias; i++){
		sprintf(sURL, "%s%s", listadoGrupoDeNoticiasSinRepetir[i], ".html");
		sprintf(response, "%s<LI>%s</LI>", response, armarLinkCon(sURL, listadoGrupoDeNoticiasSinRepetir[i]));
	}
	free(sURL);

	sprintf(response, "%s%s", response, "</OL></BODY></HTML>");

	LoguearDebugging("<-- formatearListadoDeGruposDeNoticiasAHTML()", APP_NAME_FOR_LOGGER);
	return response;
}

char* armarLinkCon(char* sURL, char* sNombreDelLink){
	LoguearDebugging("--> armarLinkCon()", APP_NAME_FOR_LOGGER);

	/*	+50 Porque tengo que tener en cuenta ip:puerto/grupoNoticia/noticiaID	*/
	/*	TODO: Ver si se puede cambiar el malloc por el uso de asprintf	*/
	char* sTag= (char*)malloc(sizeof(char)*(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50));
	memset(sTag, 0, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50);
	sprintf(sTag, "<A href=\"/%s\">%s</A><BR />", sURL, sNombreDelLink);

	LoguearDebugging("<-- armarLinkCon()", APP_NAME_FOR_LOGGER);
	return sTag;
}

char* processRequestTypeListadoDeNoticias(char* sGrupoDeNoticias, stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoDeNoticias()", APP_NAME_FOR_LOGGER);
	char* sLogMessage;

	/*	El limite impuesto por la bd, mas el largo del nombre del atributo, mas el igual.	*/
	unsigned int uiNumberOfCharacters= strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT;

	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, sGrupoDeNoticias);

	asprintf(&sLogMessage, "El criterio para buscar en OpenDS es: %s", sCriterio);
	LoguearInformacion(sLogMessage, APP_NAME_FOR_LOGGER);

	LoguearDebugging("Hago el select a OpenDS", APP_NAME_FOR_LOGGER);
	unsigned int uiCantidadDeNoticias= 0;
	stArticle listadoNoticias[1000];/*	TODO: Chequear este 1000, ver como deshardcodearlo	*/
	selectArticles(listadoNoticias, &uiCantidadDeNoticias, (*(*pstParametros).pstPLDAPSession), (*(*pstParametros).pstPLDAPSessionOperations), sCriterio);

	LoguearDebugging("<-- processRequestTypeListadoDeNoticias()", APP_NAME_FOR_LOGGER);
	return formatearListadoDeNocitiasAHTML(sGrupoDeNoticias, listadoNoticias, uiCantidadDeNoticias);
}

char* formatearListadoDeNocitiasAHTML(char* sGrupoDeNoticias, stArticle listadoDeNoticias[], unsigned int uiCantidadDeNoticias){
	LoguearDebugging("--> formatearListadoDeNocitiasAHTML()", APP_NAME_FOR_LOGGER);

	/*	1+OPEN...+1+OPEN...5+1 Es igual a: /nombreGrupoNoticia/noticiaID.html\0	*/
	/*	TODO: Chequear si se puede sacar el malloc usando asprintf ;)	*/
	char* sURL= (char*)malloc(sizeof(char)*(1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+1+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+5+1));
	char* response= (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	sprintf(response, "<HTML><HEAD><TITLE>Listado de noticias de %s</TITLE></HEAD><BODY><P><B>Listado de noticias de %s</B></P><OL>", sGrupoDeNoticias, sGrupoDeNoticias);

	int i;
	for(i=0; i<uiCantidadDeNoticias; i++){
		stArticle stArticle= listadoDeNoticias[i];

		sprintf(sURL, "%s/%d%s", sGrupoDeNoticias, stArticle.uiArticleID, ".html");
		sprintf(response, "%s<LI>%s</LI>", response, armarLinkCon(sURL, stArticle.sHead));
	}
	free(sURL);
	sprintf(response, "%s%s", response, "</OL></BODY></HTML>");

	LoguearDebugging("<-- formatearListadoDeNocitiasAHTML()", APP_NAME_FOR_LOGGER);
	return response;
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

	/*sprintf(sGrupoDeNoticias, "%s", grupoDeNoticias);*/

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
