#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread.h>
#include <stdio.h>
#include <stdlib.h>
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
unsigned int obtenerTipoOperacionYVariables(char* sURL, char* sGrupoDeNoticias, char* sArticleID);
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

/************************************************
 *	Declaracion funciones relacionadas al HTML	*
 ************************************************/
/**
 * Formatea la noticia la noticia a un char* en formato HTML para poder enviarle eso al cliente
 * y visualizar bien la pagina HTML.
 */
char* formatearArticuloAHTML(stArticle stArticulo);
char* formatearListadoDeGruposDeNoticiasAHTML(stConfiguracion* pstConf, char* listadoGruposDeNoticias[], int len);
/**
 * Arma un tag a de HTML con un hipervinculo al sURL que se le pasa como parametro,
 * y mostrando el  sNombreDelLink como el hipervinculo.
 */
char* armarLinkCon(char* sURL, char* sNombreDelLink);

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
		LoguearError("Archivo de configuracion no válido.", "HTTPServer");
		return -1;
	} else {
		LoguearInformacion("Archivo de configuracion cargado correctamente.", APP_NAME_FOR_LOGGER);
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
	if (!crearConexionConSocket(&stConf, &ficheroServer, &server)){
		LoguearError("No se pudo crear la conexion con el socket y dejarlo listo para escuchar conexiones entrantes.", APP_NAME_FOR_LOGGER);
		return -1;
	}
	else{
		strcpy(stConf.czAppServer, inet_ntoa(server.sin_addr));
		printf("Aplicacion levantada en: IP=%s; Port=%d\n\nEscuchando conexiones entrantes...\n", stConf.czAppServer, stConf.uiAppPuerto);
	}


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
			stParameters.ficheroCliente= ficheroCliente;
			stParameters.pstPLDAPSession= &stPLDAPSession;
			stParameters.pstPLDAPSessionOperations= &stPLDAPSessionOperations;
			stParameters.pstConfiguration= &stConf;

			if (thr_create(0, 0, (void*) &procesarRequestFuncionThread,
					(void*) &stParameters, 0, &threadProcesarRequest) != 0)
				printf(
						"No se pudo crear un nuevo thread para procesar el request.\n");
		} else
			LoguearError("Error al aceptar la conexion.", APP_NAME_FOR_LOGGER);
		printf("Se obtuvo una conexion desde %s...\n", inet_ntoa(client.sin_addr));
/*	}*/

	printf("Le doy al thread 8 segundos para responderle al cliente antes que cierre todo... ;)\n");
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
	char sMensajeHTTPCliente[1024];/*	TODO: No pueden valer lo mismo estos dos.	*/

	int lenMensajeHTTPCliente;
	lenMensajeHTTPCliente = 1024;
	bytesRecibidos = recv(stParametros.ficheroCliente, sMensajeHTTPCliente, lenMensajeHTTPCliente, 0);

	printf("Recibi %d bytes del usuario.\n\n", bytesRecibidos);
	printf("############################################################\n\n");
	printf("%s", sMensajeHTTPCliente);
	printf("############################################################\n");

	char sRecursoPedido[1024];/*	TODO: Esto tendria que ser menos.	*/
	strcpy(sRecursoPedido, obtenerRecursoDeCabecera(sMensajeHTTPCliente));

	printf("El usuario pidio el recurso: %s\n", sRecursoPedido);

	char* sGrupoDeNoticia= (char*)malloc(sizeof(char)*OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT);
	char* sArticleID= (char*)malloc(sizeof(char)*OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT);
	unsigned int uiOperation= obtenerTipoOperacionYVariables(sRecursoPedido, sGrupoDeNoticia, sArticleID);
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

	printf("Le respondo al cliente con el html ya armado... \n");
	if ((bytesEnviados = send(stParametros.ficheroCliente, sResponse, len, 0))
			== -1) {
		printf("El send no funco\n");
	}
	printf("El cliente recibio %d bytes\n", bytesEnviados);

	printf("Voy a cerrar la conexion con el cliente\n");
	close(stParametros.ficheroCliente);

	/*	TODO: Libero la memoria	*/
	free(sResponse);

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
	printf("Libere LDAP/OpenDS\n");

	/*	Cierro el socket	*/
	close(ficheroServer);
	printf("Libere el ficheroServer\n");
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

	/*	Estos dos, son exclusivos para algunas operaciones	*/
	PLDAP_ENTRY_OP entryOp = newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP attribOp = newLDAPAttributeOperations();

	*pstArticulo= getArticle(*pstPLDAPSession, *pstPLDAPSessionOperations, sGrupoDeNoticias, sArticleID);

	LoguearDebugging("<-- buscarNoticiaEnBD()", APP_NAME_FOR_LOGGER);
	return 1;
}

void guardarNoticiaEnCache(stArticle stArticulo) {
	LoguearDebugging("--> guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);

	LoguearDebugging("<-- guardarNoticiaEnCache()", APP_NAME_FOR_LOGGER);
	return;
}

char* formatearArticuloAHTML(stArticle stArticulo) {
	LoguearDebugging("--> formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);

	char* response= (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	sprintf(response, "<HTML><HEAD><TITLE>%s</TITLE></HEAD><BODY><P>%s<P></BODY></HTML>", stArticulo.sHead, stArticulo.sBody);

	LoguearDebugging("<-- formatearArticuloAHTML()", APP_NAME_FOR_LOGGER);
	return response;
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
	return formatearArticuloAHTML(stArticulo);
}

char* processRequestTypeListadoGruposDeNoticias(stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoGrupoDeNoticias()", APP_NAME_FOR_LOGGER);

	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+1+1];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, "*");

	int len= 2;
	char* listadoGrupoNoticias[len];

	/*	TODO: Aca hago la busqueda.	*/
	listadoGrupoNoticias[0]= "Clarin";
	listadoGrupoNoticias[1]= "La Nacion";

	LoguearDebugging("<-- processRequestTypeListadoGrupoDeNoticias()", APP_NAME_FOR_LOGGER);
	return formatearListadoDeGruposDeNoticiasAHTML((*pstParametros).pstConfiguration, listadoGrupoNoticias, len);
}

char* formatearListadoDeGruposDeNoticiasAHTML(stConfiguracion* pstConf, char* listadoGruposDeNoticias[], int len){
	LoguearDebugging("--> formatearListadoDeGruposDeNoticiasAHTML()", APP_NAME_FOR_LOGGER);

	char* sURL= (char*)malloc(sizeof(char)*(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+30));
	char* response= (char*)malloc(sizeof(char)*MAX_CHARACTERS_FOR_RESPONSE);
	strcpy(response, "<HTML><HEAD><TITLE>Listado de grupos de noticias</TITLE></HEAD><BODY>");

	int i;
	for(i=0; i<len; i++){
		sprintf(sURL, "%s:%d/%s", (*pstConf).czAppServer, (*pstConf).uiAppPuerto, listadoGruposDeNoticias[i]);
		sprintf(response, "%s%s", response, armarLinkCon(sURL, listadoGruposDeNoticias[i]));
	}
	free(sURL);
	sprintf(response, "%s%s", response, "</BODY></HTML>");

	LoguearDebugging("<-- formatearListadoDeGruposDeNoticiasAHTML()", APP_NAME_FOR_LOGGER);
	return response;
}

char* armarLinkCon(char* sURL, char* sNombreDelLink){
	LoguearDebugging("--> armarLinkCon()", APP_NAME_FOR_LOGGER);
printf("url vale: %s\n", sURL);
	/*	+50 Porque tengo que tener en cuenta ip:puerto/grupoNoticia/noticiaID	*/
	char* sTag= (char*)malloc(sizeof(char)*(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50));
	memset(sTag, 0, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT+50);
	sprintf(sTag, "<A href=\"/%s\">%s</A><BR />", sURL, sNombreDelLink);
printf("tag vale: %s\n", sTag);
	LoguearDebugging("<-- armarLinkCon()", APP_NAME_FOR_LOGGER);
	return sTag;
}

char* processRequestTypeListadoDeNoticias(char* sGrupoDeNoticias, stThreadParameters* pstParametros) {
	LoguearDebugging("--> processRequestTypeListadoDeNoticias()", APP_NAME_FOR_LOGGER);

	/*	El limite impuesto por la bd, mas el largo del nombre del atributo, mas el igual.	*/
	unsigned int uiNumberOfCharacters= strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT;

	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, sGrupoDeNoticias);

	printf("sCriterio quedo en: %s\n", sCriterio);
	/*	TODO: Aca hago la busqueda	*/


	LoguearDebugging("<-- processRequestTypeListadoDeNoticias()", APP_NAME_FOR_LOGGER);
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


unsigned int obtenerTipoOperacionYVariables(char* sURL, char* sGrupoDeNoticias, char* sArticleID){
	unsigned int uiOperation;
	if (strlen(sURL) == 1)
		uiOperation= REQUEST_TYPE_NEWSGROUP;
	else {
		/* Obtengo el grupo de noticias. */
		strcpy(sGrupoDeNoticias, obtenerGrupoDeNoticias(sURL));

		/* Me fijo si ademas del grupo de noticias viene la noticia */
		if (llevaNoticia(sURL)) {
			/* Obtengo la noticia de dicho grupo. */
			strcpy(sArticleID, obtenerNoticia(sURL));
			uiOperation= REQUEST_TYPE_NEWS;
		}
		else
			/*	Como la URL no tiene el sufijo /noticiaID, el cliente me esta pidiendo
				 el listado de noticias para un grupo de noticia dado.					*/
			uiOperation= REQUEST_TYPE_NEWS_LIST;
	}
	return uiOperation;
}
