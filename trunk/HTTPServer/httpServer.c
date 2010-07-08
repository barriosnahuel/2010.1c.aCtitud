#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../util.h"
#include "../Logger/logger.h"
#include "configuration.h"
#include "../LDAP/LdapWrapperHandler.h"
#include "funcionesMemcached.h"
#include <libmemcached/memcached.h>

#define BACKLOG 3 /* El numero de conexiones permitidas  TODO: Aca no tendriamos que poner por lo menos 20? */
#define REQUEST_TYPE_NEWSGROUP 1	/*	Indica que se esta solicitando el listado de newsgroups.	*/
#define REQUEST_TYPE_NEWS_LIST 2	/*	Indica que se esta solicitando el listado de noticias para un newsgroup especifico. 	*/
#define REQUEST_TYPE_NEWS 3			/*	Indica que se esta solicitando una noticia en particular.	*/
#define MAX_CHARACTERS_FOR_RESPONSE 5000	/*	TODO: La cantidad maxima de caracteres para el response esta bien 5000?	*/

char czNombreProceso[20];
int ficheroServer; 			/* Fichero descriptor de nuestro server. */

/*int thr_create(void *stack_base
 , size_t stack_size
 , void *(*start_routine)(void *)
 , void *arg
 , long flags
 , thread_t* new_thread.
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

	memcached_st* memCluster;
	stConfiguracion* pstConfiguration;
} stThreadParameters;

/************************************************************************************************************
 *	Aca comienzan las declaraciones de las funciones														*
 ************************************************************************************************************/
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
 * Saca los espacios del grupo para poder insertarlo en la clave de la cache		
 */
char* sacarEspaciosEnGrupo(char* grupo);
/**
 * Busca la noticia en la BD, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnBD(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations);
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
 * Funcion que devuelve 1 en caso de que el recurso sea del estilo /grupoDeNoticias/Noticia. Devolverï¿½ 0 si es /grupoDeNoticias nomas.
 */
int llevaNoticia(char* sRecursoPedido);

/**
 * De un string del estilo /grupoDeNoticias/Noticia obtengo Noticia.
 */
char* obtenerNoticia(char* sRecursoPedido);

void gestionarSenialCtrlC(int senial);

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
int main(int argn, char *argv[]) {
	char* sLogMessage;	/*	Es la variable que uso para generar el msj para el log.	*/

    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso, "HTTPServer\0");
    strcpy(argv[0], czNombreProceso);


	/****************************************
	 *	Cargo el archivo de configuracion	*
	 ****************************************/
	stConfiguracion stConf;

	signal(SIGINT, gestionarSenialCtrlC);

	if (!CargaConfiguracion("config.conf\0", &stConf)) {
		printf("Archivo de configuracion no valido.\n");
		LoguearError("Archivo de configuracion no vÃ¡lido.");
		return -1;
	}
	LoguearInformacion("Archivo de configuracion cargado correctamente.");
	asprintf(&sLogMessage, "Puerto de la aplicacion: %d.", stConf.uiAppPuerto);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage, "IP LDAP/OpenDS: %s.", stConf.czBDServer);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage, "Puerto de LDAP/OpenDS: %d.", stConf.uiBDPuerto);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage,"\tIP memcachedServer 1: %s\n",stConf.memcachedServer1);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage,"\tPuerto memcachedServer 1: %d\n",stConf.memcachedServer1Puerto);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage,"\tIP memcachedServer 2: %s\n",stConf.memcachedServer2);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage,"\tPuerto memcachedServer2: %d\n",stConf.memcachedServer2Puerto);
	
	memcached_st * memc;
	iniciarClusterCache(&memc,stConf.memcachedServer1,stConf.memcachedServer1Puerto,stConf.memcachedServer2,stConf.memcachedServer2Puerto);
	
	/****************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper		*
	 ****************************************************/
	PLDAP_CONTEXT stPLDAPContext = newLDAPContext();
	PLDAP_CONTEXT_OP stPLDAPContextOperations = newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
	PLDAP_SESSION stPLDAPSession;
	PLDAP_SESSION_OP stPLDAPSessionOperations = newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
	if (!crearConexionLDAP(stConf.czBDServer, stConf.uiBDPuerto, &stPLDAPContext, &stPLDAPContextOperations,
			&stPLDAPSession, &stPLDAPSessionOperations)) {
		LoguearError("No se pudo conectar a OpenDS.");
		return -1;
	}
	asprintf(&sLogMessage, "Conectado a OpenDS en: IP=%s; Port=%d.", stConf.czBDServer, stConf.uiBDPuerto);
	LoguearInformacion(sLogMessage);


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
	struct sockaddr_in server;	/* Para la informacion de la direccion del servidor. */
	if (!crearConexionConSocket(&stConf, &ficheroServer, &server)){
		LoguearError("No se pudo crear la conexion con el socket y dejarlo listo para escuchar conexiones entrantes.");
		return -1;
	}
	strcpy(stConf.czAppServer, inet_ntoa(server.sin_addr));
	asprintf(&sLogMessage, "Aplicacion levantada en: IP=%s; Port=%d.", stConf.czAppServer, stConf.uiAppPuerto);
	LoguearInformacion(sLogMessage);
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
			stParameters.memCluster = memc;
			if (thr_create(0, 0, (void*) &procesarRequestFuncionThread,
					(void*) &stParameters, 0, &threadProcesarRequest) != 0)
				LoguearError("No se pudo crear un nuevo thread para procesar el request.");
		} else
			LoguearError("Error al aceptar la conexion.");

		asprintf(&sLogMessage, "Se obtuvo una conexion desde %s.", inet_ntoa(client.sin_addr));
		LoguearInformacion(sLogMessage);
	}

	printf("Le doy al thread 8 segundos para responderle al cliente antes que cierre todo... ;)\n");
	sleep(8);

	liberarRecursos(ficheroServer, stPLDAPContext, stPLDAPContextOperations,
			stPLDAPSession, stPLDAPSessionOperations, stConf);
	memcached_free(memc);
	/*	TODO: Libero lo ultimo que pueda llegar a quedar de memoria pedida. */
	return 1;
}

void* procesarRequestFuncionThread(void* threadParameters) {
	LoguearDebugging("--> procesarRequestFuncionThread()");
	char* sLogMessage;
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);

	LoguearInformacion("Comienzo a procesar un nuevo thread.");
	int bytesRecibidos;
	char sMensajeHTTPCliente[1024];/*	TODO: No pueden valer lo mismo estos dos.	*/

	int lenMensajeHTTPCliente;
	lenMensajeHTTPCliente = 1024;
	bytesRecibidos = recv(stParametros.ficheroCliente, sMensajeHTTPCliente, lenMensajeHTTPCliente, 0);


	asprintf(&sLogMessage, "Recibi %d bytes del usuario.", bytesRecibidos);
	LoguearInformacion(sLogMessage);
	asprintf(&sLogMessage, "El mensaje HTTP que recibimos del cliente es:\n%s", sMensajeHTTPCliente);
	LoguearInformacion(sLogMessage);

	char sRecursoPedido[1024];/*	TODO: Esto tendria que ser menos.	*/
	char sRecursoPedidoSinEspacios[1024];
	strcpy(sRecursoPedido, obtenerRecursoDeCabecera(sMensajeHTTPCliente));

	asprintf(&sLogMessage, "El usuario pidio el recurso: \"%s\".", sRecursoPedido);
	LoguearInformacion(sLogMessage);
	
	formatearEspacios(sRecursoPedido, sRecursoPedidoSinEspacios);

	/*	En Ubuntu/Chrome, /Firefox (no se en otras situaciones) luego de enviar el response, recibimos un
	    request por este recurso. Decidimos no tratarlo, por lo tanto cerramos la conexion.	*/
	if(strcmp(sRecursoPedido, "/favicon")==0){
		close(stParametros.ficheroCliente);
		LoguearInformacion("Se cerro el fichero descriptor del cliente porque el request pedia el recurso /favicon.ico.");

		LoguearInformacion("Termino el thread.");
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
		LoguearInformacion("Proceso la obtencion del listado de grupos de noticias.");
		sResponse = processRequestTypeListadoGruposDeNoticias(&stParametros);
		break;
	case REQUEST_TYPE_NEWS_LIST:
		LoguearInformacion("Proceso la obtencion del listado de noticias para un grupo de noticias.");
		sResponse = processRequestTypeListadoDeNoticias(sGrupoDeNoticia, &stParametros);
		break;
	case REQUEST_TYPE_NEWS:
		LoguearInformacion("Proceso la obtencion de una noticia en particular.");
		sResponse = processRequestTypeUnaNoticia(sGrupoDeNoticia, sArticleID, &stParametros);
		break;
	default:
		LoguearInformacion("Por default, obtengo el listado de grupos de noticias.");
		sResponse = processRequestTypeListadoGruposDeNoticias(&stParametros);
		break;
	}
	free(sGrupoDeNoticia);
	free(sArticleID);

	int len, bytesEnviados;
	len = strlen(sResponse);	
	
	if ((bytesEnviados = send(stParametros.ficheroCliente, sResponse, len, 0)) == -1)
		LoguearError("No se pudo enviar el response al cliente.");

	free(sResponse);

	close(stParametros.ficheroCliente);
	LoguearInformacion("Se cerro el fichero descriptor del cliente.");

	LoguearInformacion("Termino el thread.");
	thr_exit(0);
}

void gestionarSenialCtrlC(int senial){
	printf("\nHa pulsado CTRL + C (señal numero %d)\n", senial);
	printf("Se cerrará el servidor.\n");
	printf("Valor de ficher antes del close: %d\n", ficheroServer);
	close(ficheroServer);
	printf("Valor de ficher dps del close: %d\n", ficheroServer);
	/*int resultadoClose = close(&ficheroServer);
	if(resultadoClose == 0) {
		printf("Pase por el close y cerro joya\n");
	} else {
		printf("Pase por el close y cerro mal\n");
	}*/
	exit(1);
}


/**
 * Crea el socket, lo bindea, y lo deja listo para escuchar conexiones entrantes.
 */
int crearConexionConSocket(stConfiguracion* stConf, int* ficheroServer,
		struct sockaddr_in* server) {
	LoguearDebugging("--> crearConexionConSocket()");

	if ((*ficheroServer = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		LoguearError("No se pudo obtener el fichero descriptor del socket.");
		exit(-1);
	}
	LoguearInformacion("Se obtuvo el fichero descriptor correctamente.");

	(*server).sin_family = AF_INET;
	(*server).sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY coloca nuestra direccion IP automaticamente */
	(*server).sin_port = htons((*stConf).uiAppPuerto); /* htons transforma el short de maquina a short de red */
	bzero(&((*server).sin_zero), 8); /* Escribimos ceros en el resto de la estructura*/

	if (bind(*ficheroServer, (const struct sockaddr *) &(*server),
			sizeof(struct sockaddr)) == -1) {
		LoguearError("Error al asociar el puerto al socket.");
		exit(-1);
	}
	LoguearInformacion("Se asocio bien el puerto al socket.");

	if (listen(*ficheroServer, BACKLOG) == -1) {
		LoguearError("No se pudo dejar escuchando al puerto.");
		exit(-1);
	}

	LoguearDebugging("<-- crearConexionConSocket()");
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
	LoguearDebugging("--> liberarRecursos()");

	/*	Cierro/Libero lo relacionado a la BD	*/
	stPLDAPSessionOperations->endSession(stPLDAPSession);
	freeLDAPSessionOperations(stPLDAPSessionOperations);
	freeLDAPSession(stPLDAPSession);
	freeLDAPContext(stPLDAPContext);
	freeLDAPContextOperations(stPLDAPContextOperations);
	LoguearInformacion("Se libero la memoria de LDAP/OpenDS correctamente.");

	/*	Cierro el socket	*/
	close(ficheroServer);
	LoguearInformacion("Libere el fichero descriptor de nuestro server correctamente.");

	LoguearDebugging("<-- liberarRecursos()");
}

int buscarNoticiaEnBD(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	LoguearDebugging("--> buscarNoticiaEnBD()");

	*pstArticulo= getArticle(*pstPLDAPSession, *pstPLDAPSessionOperations, sGrupoDeNoticias, sArticleID);

	LoguearDebugging("<-- buscarNoticiaEnBD()");
	return 1;
}

char* formatearArticuloAHTML(stArticle* pstArticulo) {
	LoguearDebugging("--> formatearArticuloAHTML()");

	char* response;
	asprintf(&response, "<HTML><HEAD><TITLE>%s</TITLE></HEAD><BODY><P><B>Grupo de noticias: %s</B></P><P>%s</P></BODY></HTML>", (*pstArticulo).sHead, (*pstArticulo).sNewsgroup, (*pstArticulo).sBody);

	LoguearDebugging("<-- formatearArticuloAHTML()");
	return response;
}

char* sacarEspaciosEnGrupo(char* grupo)
{ 
	int i ;
	int j;
	char* grupoSinEspacios = malloc(strlen(grupo)+1);
	for(i=0,j=0;i<=strlen(grupo);i++){
		if(!isspace(grupo[i])){
			grupoSinEspacios[j] = grupo[i];
			j++;
		}
	}
	grupoSinEspacios[j]='\0';
	return grupoSinEspacios;
}


char* processRequestTypeUnaNoticia(char* sGrupoDeNoticias, char* sArticleID,
		stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeUnaNoticia()");

	stArticle stArticulo;
	char* grupoSinEspacios = sacarEspaciosEnGrupo(sGrupoDeNoticias);
	if (!buscarNoticiaEnCache(&stArticulo,sGrupoDeNoticias, sArticleID,grupoSinEspacios,&pstParametros->memCluster)) {
		/*	Como no encontre la noticia en Cache, la busco en la BD	*/
		buscarNoticiaEnBD(&stArticulo, sGrupoDeNoticias, sArticleID,
				(*pstParametros).pstPLDAPSession,
				(*pstParametros).pstPLDAPSessionOperations);
		/*	Como no la encontre en Cache, ahora la guardo en cache para que este la proxima vez.	*/
		guardarNoticiaEnCache(stArticulo,sGrupoDeNoticias,grupoSinEspacios,&pstParametros->memCluster);
	}
	if(stArticulo.uiArticleID != -1) {
		
		char* aviso200;
		asprintf(&aviso200, "HTTP/1.1 200 OK\nContent-type: text/html\n\n");
		int lenAviso200 = strlen(aviso200);
		int bytesEnviadosDeAviso;
			
		if ((bytesEnviadosDeAviso = send(pstParametros->ficheroCliente, aviso200, lenAviso200, 0)) == -1) {
			LoguearError("No se pudo enviar el 200 OK al cliente.");
		}

		LoguearDebugging("<-- processRequestTypeUnaNoticia()");
		return formatearArticuloAHTML(&stArticulo);
	}
	else {
		char* error404;
		asprintf(&error404, "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n");
		LoguearDebugging("<-- processRequestTypeUnaNoticia()");
		return error404;
	}
}

char* processRequestTypeListadoGruposDeNoticias(stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoGrupoDeNoticias()");
	
	/* Apenas entro aca mando un 200 OK porque siempre se va a poder procesar el listado de grupos de noticias. */
	char* aviso200;
	asprintf(&aviso200, "HTTP/1.1 200 OK\nContent-type: text/html\n\n");
	int lenAviso200 = strlen(aviso200);
	int bytesEnviadosDeAviso;
	
	if ((bytesEnviadosDeAviso = send(pstParametros->ficheroCliente, aviso200, lenAviso200, 0)) == -1) {
		LoguearError("No se pudo enviar el 200 OK al cliente.");
	}

	char* listadoGrupoNoticiasSinRepetir[1000];
	unsigned int cantidadDeGruposSinRepetir= obtenerListadoGruposDeNoticias(listadoGrupoNoticiasSinRepetir, (*(*pstParametros).pstPLDAPSession), (*(*pstParametros).pstPLDAPSessionOperations));

	LoguearDebugging("<-- processRequestTypeListadoGrupoDeNoticias()");
	return formatearListadoDeGruposDeNoticiasAHTML(listadoGrupoNoticiasSinRepetir, cantidadDeGruposSinRepetir);
}


char* formatearListadoDeGruposDeNoticiasAHTML(char* listadoGrupoDeNoticiasSinRepetir[], int iCantidadDeGruposDeNoticias){
	LoguearDebugging("--> formatearListadoDeGruposDeNoticiasAHTML()");

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

	LoguearDebugging("<-- formatearListadoDeGruposDeNoticiasAHTML()");
	return response;
}

char* armarLinkCon(char* sURL, char* sNombreDelLink){
	LoguearDebugging("--> armarLinkCon()");

	/*	+50 Porque tengo que tener en cuenta ip:puerto/grupoNoticia/noticiaID	*/
	/*	TODO: Ver si se puede cambiar el malloc por el uso de asprintf	*/
	char* sTag= (char*)malloc(sizeof(char)*(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50));
	memset(sTag, 0, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50);
	sprintf(sTag, "<A href=\"/%s\">%s</A><BR />", sURL, sNombreDelLink);

	LoguearDebugging("<-- armarLinkCon()");
	return sTag;
}

char* processRequestTypeListadoDeNoticias(char* sGrupoDeNoticias, stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoDeNoticias()");


	stArticle listadoNoticias[1000];/*	TODO: Chequear este 1000, ver como deshardcodearlo	*/

	LoguearDebugging("Llamo al metodo obtenerListadoNoticiasParaUnGrupo() provisto por aCtitud.");
	unsigned int uiCantidadDeNoticias= obtenerListadoNoticiasParaUnGrupo(listadoNoticias, (*(*pstParametros).pstPLDAPSession), (*(*pstParametros).pstPLDAPSessionOperations), sGrupoDeNoticias);
	
	/* Si el diario no tiene noticias significa que no existe dicho diario. Por lo tanto es un 404. */
	if(uiCantidadDeNoticias == 0) {
		char* error404;
		asprintf(&error404, "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n");
		LoguearDebugging("<-- processRequestTypeListadoDeNoticias()");
		return error404;
	}
	else {
		char* aviso200;
		asprintf(&aviso200, "HTTP/1.1 200 OK\nContent-type: text/html\n\n");
		int lenAviso200 = strlen(aviso200);
		int bytesEnviadosDeAviso;
			
		if ((bytesEnviadosDeAviso = send(pstParametros->ficheroCliente, aviso200, lenAviso200, 0)) == -1) {
			LoguearError("No se pudo enviar el 200 OK al cliente.");
		}
		LoguearDebugging("<-- processRequestTypeListadoDeNoticias()");
		return formatearListadoDeNocitiasAHTML(sGrupoDeNoticias, listadoNoticias, uiCantidadDeNoticias);
	}

}

char* formatearListadoDeNocitiasAHTML(char* sGrupoDeNoticias, stArticle listadoDeNoticias[], unsigned int uiCantidadDeNoticias){
	LoguearDebugging("--> formatearListadoDeNocitiasAHTML()");

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

	LoguearDebugging("<-- formatearListadoDeNocitiasAHTML()");
	return response;
}

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente) {
	LoguearDebugging("--> obtenerRecursoDeCabecera()");

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

	LoguearDebugging("<-- obtenerRecursoDeCabecera()");
	return recurso;

}

char* obtenerGrupoDeNoticias(char* sRecursoPedido) {
	LoguearDebugging("--> obtenerGrupoDeNoticias()");

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

	LoguearDebugging("<-- obtenerGrupoDeNoticias()");

	return grupoDeNoticias;
}

char* obtenerNoticia(char* sRecursoPedido) {
	LoguearDebugging("--> obtenerDeNoticia()");
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
	LoguearDebugging("<-- obtenerDeNoticia()");
	return noticia;
}

int llevaNoticia(char* sRecursoPedido) {
	LoguearDebugging("--> llevaNoticia()");
	int i = 1;

	while(sRecursoPedido[i] != '/' && sRecursoPedido[i] != '\0' ) {
		i = i + 1;
	}

	if(sRecursoPedido[i] == '\0') {
		return 0;
	}
	LoguearDebugging("<-- llevaNoticia()");
	return 1;
}
