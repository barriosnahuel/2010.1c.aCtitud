#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <iostream>
#include <cstdlib>
#include <process.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//	Nuestros includes:
#include "../../funcionesMSMQ.hpp"
#include "LdapWrapperHandler-Win.hpp"
#include "xmlFunctions.hpp"
#include "../../logger-win.hpp"

//	Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#define DEFAULT_BUFLEN 512/*	ESTO CREO QUE NO HACE FALTA.*/

using namespace std;

#define BUFFERSIZE 1024
typedef struct stConfiguracion{
	char	acOpenDSPort[6];
    char	acOpenDSServer[16];
	char	acInterval[10];

	char	szDefault[255];
}stConfiguracion;

typedef struct news{
   char*   head;
   char*   body;
   char*   newsgroup;
   unsigned int id;
}news;

/* nombre del proceso */
char czNombreProceso[20];
Logger logger;

int openDSEstaCaido;

/***********************************************************************************************
							Declaraciones de funciones
 ***********************************************************************************************/ 

/**
 *	Esta funcion se encarga de:
 *	1.	Comprobar que existen mensajes nuevos en la cola MSQM.
 *	2.	Obtener uno a uno los mensajes nuevos (si existen).
 *	3.	Parsear el msj XML utilizando LibXML2 y seteando el objeto stArticle.
 *	4.	Persistir el objeto stArticle en OpenDS por medio de la API LDAPWrapperHandler.
 *	
 *	Si no se pudiera llevar a cabo con alguno de los subprocesos, la funcion retornara 0. Caso
 *	contrario, retornara 1.
 */
int consumirMensajesYAlmacenarEnBD(	MsmqProcess colaMsmq
									, PLDAP_SESSION stPLDAPSession
									, PLDAP_SESSION_OP stPLDAPSessionOperations);

int crearConexionSocket(SOCKET* ficheroServer, struct sockaddr_in* server, struct stConfiguracion* stConfiguracion);

int Valida_IP(const char *ip);
int ValidaNumero(const char *buffer, int chequeaSigno);

/***********************************************************************************************
							Definiciones de funciones
 ***********************************************************************************************/ 
int main(int argc, char** argv){
	ZeroMemory(czNombreProceso,20);
	strcpy_s(czNombreProceso,20, "NNTP_Process_Srvr\0");
	logger.LoguearDebugging("--> Main()");
	
	//	Carga configuracion
	struct stConfiguracion configuracion;
	LPCSTR archivoConfiguracion= "..\\configuracion.ini";
	int hayErrorDeConfiguracion= 0;

	GetPrivateProfileString("configuracion","OpenDSServer", configuracion.szDefault, configuracion.acOpenDSServer, 16, archivoConfiguracion);
	if(!Valida_IP(configuracion.acOpenDSServer)){
		logger.LoguearError("Archivo de configuracion incorrecto, la IP del NNTP no esta bien formada.");
		hayErrorDeConfiguracion= 1;
	}

	GetPrivateProfileString("configuracion","OpenDSPort", configuracion.szDefault, configuracion.acOpenDSPort, 6, archivoConfiguracion);
	if(!ValidaNumero(configuracion.acOpenDSPort, 0)){
		logger.LoguearError("Archivo de configuracion incorrecto, el puerto de NNTP no esta bien formado.");
		hayErrorDeConfiguracion= 1;
	}

	GetPrivateProfileString("configuracion","Interval", configuracion.szDefault, configuracion.acInterval, 10, archivoConfiguracion);
	if(!ValidaNumero(configuracion.acInterval, 1)){
		logger.LoguearError("Archivo de configuracion incorrecto, El tiempo de intervalo no esta bien formado.");
		hayErrorDeConfiguracion= 1;
	}
	else if(atoi(configuracion.acInterval)<=0){
		logger.LoguearError("Archivo de configuracion incorrecto, El tiempo de intervalo debe ser mayor a cero.");
		hayErrorDeConfiguracion= 1;	
	}

	if(hayErrorDeConfiguracion){
		cout << "\n\n--\nEl archivo de configuracion contiene errores, corrijalos y vuelva a iniciar el Publisher.\taCtitud.\n" << endl;
	
		system("PAUSE");	/*	Esto es para que el usuario tenga que tocar una tecla para cerrar la consola.	*/
		return 0;
	}

	logger.LoguearDebugging("Archivo de configuracion cargado correctamente.");
	logger.LoguearInformacion("Archivo de configuracion cargado con:");
	logger.LoguearInformacion("Puerto OpenDS:");
	logger.LoguearInformacion(configuracion.acOpenDSPort);
	logger.LoguearInformacion("IP OpenDS:");
	logger.LoguearInformacion(configuracion.acOpenDSServer);
	logger.LoguearInformacion("Intervalo de tiempo:");
	logger.LoguearInformacion(configuracion.acInterval);

	cout << "------------>>> NNTP Server Win <<<------------" << endl;
	//	Creo la cola MSMQ o me fijo que ya exista.
	MsmqProcess colaMsmq;
	colaMsmq.crearCola();
	logger.LoguearDebugging("Cola MSMQ Creada.");

	/****************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper		*
	 ****************************************************/
	PLDAP_CONTEXT stPLDAPContext= newLDAPContext();
	PLDAP_CONTEXT_OP stPLDAPContextOperations= newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
	PLDAP_SESSION stPLDAPSession;
	PLDAP_SESSION_OP stPLDAPSessionOperations= newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
	if (!crearConexionLDAP(configuracion.acOpenDSServer, atoi(configuracion.acOpenDSPort), &stPLDAPContext, &stPLDAPContextOperations,
			&stPLDAPSession, &stPLDAPSessionOperations)) {
		logger.LoguearError("No se pudo conectar con OpenDS");

		system("PAUSE");
		return -1;
	}
	openDSEstaCaido = 0;
	logger.LoguearDebugging("Conectado a OpenDS correctamente.");
	logger.LoguearInformacion("OpenDS conectado correctamente en:");
	logger.LoguearInformacion("IP:");
	logger.LoguearInformacion(configuracion.acOpenDSServer);
	logger.LoguearInformacion("Puerto");
	logger.LoguearInformacion(configuracion.acOpenDSPort);
	

	//	Comienzo a iterar infinitamente, con un intervalo de X tiempo el cual logro llamando a
	//	la funcion sleep(intervaloDeTiempo); donde intervaloDeTiempo es una variable que cargamos
	//	del archivo de configuracion.
	while(1){/*	ToDo: Ver como salir de aca, hacer algo como el NNTPServerBam porque sino no puedo desvincular el puerto.	*/

		Sleep(atoi(configuracion.acInterval));

		if(openDSEstaCaido == 1) {
			liberarRecursosLdap(stPLDAPContext, stPLDAPContextOperations, stPLDAPSession, stPLDAPSessionOperations);
			//LoguearInformacion("Se libero la memoria de LDAP/OpenDS correctamente.");
			PLDAP_CONTEXT stPLDAPContext= newLDAPContext();
			PLDAP_CONTEXT_OP stPLDAPContextOperations= newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
			PLDAP_SESSION stPLDAPSession;
			PLDAP_SESSION_OP stPLDAPSessionOperations= newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
			if (!crearConexionLDAP(configuracion.acOpenDSServer, atoi(configuracion.acOpenDSPort), &stPLDAPContext, &stPLDAPContextOperations,
				&stPLDAPSession, &stPLDAPSessionOperations)) {
				logger.LoguearError("No se pudo reconectar con OpenDS");
			} else {
				openDSEstaCaido = 0;
				logger.LoguearDebugging("Reconexion con OpenDS OK.");
			}
		}

		while((consumirMensajesYAlmacenarEnBD(colaMsmq, stPLDAPSession, stPLDAPSessionOperations) != 0) && !openDSEstaCaido) {
			cout << "Se consumio un mensaje de la cola" << endl;
			logger.LoguearInformacion("Se consumio un mensaje de la cola");
		}
	
	}
	
	 /*        Cierro/Libero lo relacionado a la BD        */
	liberarRecursosLdap(stPLDAPContext, stPLDAPContextOperations, stPLDAPSession, stPLDAPSessionOperations);
	logger.LoguearDebugging("Libere los recursos de LDAP.");
	
	system("PAUSE");	/*	Esto es para que el usuario tenga que tocar una tecla para cerrar la consola.	*/
	return 0;
}


int consumirMensajesYAlmacenarEnBD(	MsmqProcess colaMsmq
									, PLDAP_SESSION stPLDAPSession
									, PLDAP_SESSION_OP stPLDAPSessionOperations){
	logger.LoguearDebugging("--> consumirMensajesYAlmacenarEnBD()");
	
	HANDLE handle = HeapCreate( 0, 1024, 0 );
	if( handle == NULL ) {
		logger.LoguearDebugging("Error en HeapCreate()");
	}

	xmlDocPtr doc;
	xmlNodePtr root;
	stArticle articulo;
	char *xmlCompleto = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	IMSMQMessagePtr pMsg = colaMsmq.desencolarMensaje();

	if(pMsg == NULL) {
		// No hay mensajes en la cola.
		logger.LoguearInformacion("No hay mensajes en la cola.");
		logger.LoguearDebugging("<-- consumirMensajesYAlmacenarEnBD()");
		return 0;
	}

	strcpy(xmlCompleto, (char *)(_bstr_t) pMsg->Body);
	logger.LoguearDebugging("El XML recibido completo es:");
	logger.LoguearDebugging(xmlCompleto);

	logger.LoguearDebugging("Comienzo a parsear el XML para obtener sus valores.");
	doc = xmlParseMemory(xmlCompleto, strlen(xmlCompleto));
	if(doc!=NULL){ //si la variable doc devuelve un valor diferente a NULL, se dice que el documento se ha parseado correctamente.
		logger.LoguearInformacion("Se parseo la memoria correctamente y se obtuvo el documento XML.");
	}
	else {
		logger.LoguearError("El xml no se pudo parsear.");
		return 0;
	}

	//	Preparo la estructura stArticle que preciso para persistir en la BD.
	root = xmlDocGetRootElement(doc);

	xmlNodePtr cur_node = NULL;
	int contadorNodosReales= 1;//	Esta variable la uso para trabajar con los nodos que son de tipo elemento, ya que la biblioteca usa algunos mas medios raros.
	for (cur_node = root->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			switch (contadorNodosReales){
				case 1:
					logger.LoguearDebugging("Se procesa el nodo newsgroup.");
					articulo.sNewsgroup= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sNewsgroup, (char*)xmlNodeGetContent(cur_node));					
					break;
				case 2:
					logger.LoguearDebugging("Se procesa el nodo articleID.");
					articulo.uiArticleID= atoi((char*)xmlNodeGetContent(cur_node));
					break;
				case 3:
					logger.LoguearDebugging("Se procesa el nodo head.");
					articulo.sHead= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sHead, (char*)xmlNodeGetContent(cur_node));
					break;
				case 4:
					logger.LoguearDebugging("Se procesa el nodo body.");
					articulo.sBody= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sBody, (char*)xmlNodeGetContent(cur_node));
					break;
			}
			contadorNodosReales++;//	Incremento el contador solo cuando trabaje con alguno de los nodos que me importan del XML.
        }
    }//	Fin for (cur_node = root->children; cur_node; cur_node = cur_node->next)

	//	Libera las variables globales de la biblioteca que puedan haber sido usadas en el parseo
	xmlCleanupParser();

	logger.LoguearDebugging("En la Bd se insertara:");
	logger.LoguearDebugging("Newsgroup:\t");
	logger.LoguearDebugging(articulo.sNewsgroup);
	logger.LoguearDebugging("ArticleID:\t");
	char sArticleID[20];
	itoa(articulo.uiArticleID, sArticleID, 10);
	logger.LoguearDebugging(sArticleID);
	logger.LoguearDebugging("Head:\t");
	logger.LoguearDebugging(articulo.sHead);
	logger.LoguearDebugging("Body:\t");
	logger.LoguearDebugging(articulo.sBody);

	//	Persisto el articulo en la BD.
	insertEntry(stPLDAPSession, stPLDAPSessionOperations, articulo);
	if(stPLDAPSession->errorCode == 81) {
		logger.LoguearError("Debido a que hubo un problema con OpenDS el mensaje se reencolara para su posterior procesamiento.");
		colaMsmq.insertarMensaje(pMsg);
		openDSEstaCaido = 1;
	} 
	if((stPLDAPSession->errorCode != 0) && (stPLDAPSession->errorCode != 81)) {
		logger.LoguearError("Problema al insertar el articulo. El articulo se descartara");
	}
	else {
		logger.LoguearDebugging("Se inserto correctamente el articulo.");
	}

	if( ! HeapFree( handle, 0, articulo.sNewsgroup ) ) {
		logger.LoguearError("HeapFree error en articulo.sNewsgroup.");
	}
	if( ! HeapFree( handle, 0, articulo.sHead ) ) {
		logger.LoguearError("HeapFree error en articulo.sHead.");
	}
	if( ! HeapFree( handle, 0, articulo.sBody) ) {
		logger.LoguearError("HeapFree error en articulo.sBody.");
	}
	if( ! HeapFree( handle, 0, xmlCompleto ) ) {
		logger.LoguearError("HeapFree error en xmlCompleto.");
	}

	// Destruyo el heap.
	if( ! HeapDestroy( handle ) ) {
		logger.LoguearError("Hubo un error en HeapDestroy()");
	}

	logger.LoguearDebugging("<-- consumirMensajesYAlmacenarEnBD()");
	return 1;
}

int Valida_IP(const char *ip) {
   int tam, cont, idx;
   char *ptr, ipaux[15+1];
   cont = 0;
   idx= 0;

   if (!ip) return 0;
   strcpy(ipaux, ip);
   ptr = strtok(ipaux, "." );
   while(ptr) {
      tam = strlen(ptr);
      if ( tam < 1 || tam > 3 ) return 0;					/*	Se valida que la longitud sea de 1 a 3	*/

      /*	Valido que cada caracter sea un numero, y no haya letras	*/
      for(idx= 0; idx<tam; idx++){
    	  if(!isdigit(*(ptr+idx)))
    		  return 0;
      }

      if ( atoi(ptr) < 0 || atoi(ptr) > 255 ) return 0;		/*	Se valida que sea un numero entre 0-255*/
      ptr = strtok ( NULL, "." );
      cont = cont + 1;
   }
   if (cont < 4) return 0;									/*	Valido que la IP tenga al menos 4 partes.	*/
   return 1;
}

int ValidaNumero(const char *buffer, int chequeaSigno) {
	int idx= 0;

	/*	Si hay que validar que el numero puede tener signo, entonces valido que el primer caracter sea -/+	*/
	if(chequeaSigno==1){
		idx= 1;

		if(!isdigit(*buffer) && *buffer!='-' && *buffer!='+')
			return 0;
	}
	
	/*	Valido que cada caracter sea un numero, y no haya letras	*/
	while(idx<strlen(buffer)){
		if(!isdigit(*(buffer+idx)))
			return 0;

		idx++;
	}
	
	return 1;
}