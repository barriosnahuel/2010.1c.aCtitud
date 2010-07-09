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

/***********************************************************************************************
							Definiciones de funciones
 ***********************************************************************************************/ 
int main(int argc, char** argv){
	//	Carga configuracion
	struct stConfiguracion configuracion;
	LPCSTR archivoConfiguracion= "..\\configuracion.ini";
	GetPrivateProfileString("configuracion","OpenDSServer", configuracion.szDefault, configuracion.acOpenDSServer, 16, archivoConfiguracion);
	GetPrivateProfileString("configuracion","OpenDSPort", configuracion.szDefault, configuracion.acOpenDSPort, 6, archivoConfiguracion);
	GetPrivateProfileString("configuracion","Interval", configuracion.szDefault, configuracion.acInterval, 10, archivoConfiguracion);
	
	cout<<"Puerto:"<<configuracion.acOpenDSPort<<"; IP:"<<configuracion.acOpenDSServer<<"; Interval:"<<configuracion.acInterval<<endl;

	//	Creo la cola MSMQ o me fijo que ya exista.
	MsmqProcess colaMsmq;
	colaMsmq.crearCola();

	/****************************************************
	 *	Conecto a OpenDS por medio del LDAP Wrapper		*
	 ****************************************************/
	PLDAP_CONTEXT stPLDAPContext= newLDAPContext();
	PLDAP_CONTEXT_OP stPLDAPContextOperations= newLDAPContextOperations(); /*	Me permite operar sobre un contexto	*/
	PLDAP_SESSION stPLDAPSession;
	PLDAP_SESSION_OP stPLDAPSessionOperations= newLDAPSessionOperations(); /*	Me permite operar sobre una sesion	*/
	if (!crearConexionLDAP(configuracion.acOpenDSServer, atoi(configuracion.acOpenDSPort), &stPLDAPContext, &stPLDAPContextOperations,
			&stPLDAPSession, &stPLDAPSessionOperations)) {
		cout << "No se pudo conectar a OpenDS." << endl;
		//LoguearError("No se pudo conectar a OpenDS.");
		system("PAUSE");
		return -1;
	}
	cout << "Conectado a OpenDS en: IP= " << configuracion.acOpenDSServer << "; Port= " << configuracion.acOpenDSPort << endl;
	openDSEstaCaido = 0;

	//	Comienzo a iterar infinitamente, con un intervalo de X tiempo el cual logro llamando a
	//	la funcion sleep(intervaloDeTiempo); donde intervaloDeTiempo es una variable que cargamos
	//	del archivo de configuracion.
	while(1){/*	ToDo: Ver como salir de aca, hacer algo como el NNTPServerBam porque sino no puedo desvincular el puerto.	*/

		Sleep(atoi(configuracion.acInterval));
		while((consumirMensajesYAlmacenarEnBD(colaMsmq, stPLDAPSession, stPLDAPSessionOperations) != 0) && !openDSEstaCaido) {
			cout << "Se consumio un mensaje de la cola y se guardo en OpenDS" << endl;
		}

		openDSEstaCaido = 0;
	
	}
	
	system("PAUSE");	/*	Esto es para que el usuario tenga que tocar una tecla para cerrar la consola.	*/
	return 0;
}


int consumirMensajesYAlmacenarEnBD(	MsmqProcess colaMsmq
									, PLDAP_SESSION stPLDAPSession
									, PLDAP_SESSION_OP stPLDAPSessionOperations){
	cout << "--> consumirMensajesYAlmacenarEnBD()" << endl;
	
	HANDLE handle = HeapCreate( 0, 1024, 0 );
	if( handle == NULL ) {
		cout << "HeapCreate error." << endl;
	}

	xmlDocPtr doc;
	xmlNodePtr root; 
	xmlNodePtr child;
	stArticle articulo;
	char *xmlCompleto = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	IMSMQMessagePtr pMsg = colaMsmq.desencolarMensaje();

	if(pMsg == NULL) {
		// No hay mensajes en la cola.
		cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
		return 0;
	}

	strcpy(xmlCompleto, (char *)(_bstr_t) pMsg->Body);
	cout << "Para logger: El xml completo es: " << xmlCompleto << "\n" << endl;
	
	cout << "Para logger: --> Comienzo a parsear el XML para obtener sus valores." << endl;
	doc = xmlParseMemory(xmlCompleto, strlen(xmlCompleto));
	if(doc!=NULL){ //si la variable doc devuelve un valor diferente a NULL, se dice que el documento se ha parseado correctamente.
		cout << "Se parseo la memoria correctamente y se obtuvo el documento XML" << endl;
	}
	else {
		cout << "El xml no se pudo parsear." << endl;
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
					cout << "Para logger: proceso el nodo newsgroup" << endl;
					articulo.sNewsgroup= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sNewsgroup, (char*)xmlNodeGetContent(cur_node));					
					break;
				case 2:
					cout << "Para logger: proceso el nodo articleID" << endl;
					articulo.uiArticleID= atoi((char*)xmlNodeGetContent(cur_node));
					break;
				case 3:
					cout << "Para logger: proceso el nodo head" << endl;
					articulo.sHead= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sHead, (char*)xmlNodeGetContent(cur_node));
					break;
				case 4:
					cout << "Para logger: proceso el nodo body" << endl;
					articulo.sBody= (char*) HeapAlloc(handle, 0, BUFFERSIZE);
					strcpy(articulo.sBody, (char*)xmlNodeGetContent(cur_node));
					break;
			}
			contadorNodosReales++;//	Incremento el contador solo cuando trabaje con alguno de los nodos que me importan del XML.
        }
    }//	Fin for (cur_node = root->children; cur_node; cur_node = cur_node->next)

	//	Libera las variables globales de la biblioteca que puedan haber sido usadas en el parseo
	xmlCleanupParser();


	cout << "\nPara logger: En la BD se intentara insertar:" << endl;
	cout << "Newsgroup del articulo: " << articulo.sNewsgroup << endl;
	cout << "Id del articulo: " << articulo.uiArticleID << endl;
	cout << "Head del articulo: " << articulo.sHead << endl;
	cout << "Body del articulo: " << articulo.sBody << endl;

	//	Persisto el articulo en la BD.
	insertEntry(stPLDAPSession, stPLDAPSessionOperations, articulo);
	if(stPLDAPSession->errorCode!=0) {
		cout << "Debido a que hubo un problema con OpenDS el mensaje se reencolara para su posterior procesamiento." << endl;
		colaMsmq.insertarMensaje(pMsg);
		openDSEstaCaido = 1;
		cout << "El NNTP Process Server se cerrara. Por favor, levante OpenDS y vuelva a correr este servidor." << endl;
		if( ! HeapFree( handle, 0, articulo.sNewsgroup ) ) {
			cout << "HeapFree error en articulo.sNewsgroup." << endl;
		}
		if( ! HeapFree( handle, 0, articulo.sHead ) ) {
				cout << "HeapFree error en articulo.sHead." << endl;
		}
		if( ! HeapFree( handle, 0, articulo.sBody) ) {
				cout << "HeapFree error en articulo.sBody." << endl;
		}
		if( ! HeapFree( handle, 0, xmlCompleto ) ) {
				cout << "HeapFree error en xmlCompleto." << endl;
		}

		// Destruyo el heap.
		if( ! HeapDestroy( handle ) ) {
			cout << "HeapDestroy error." << endl;
		}
		cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
		exit(1);
	} else {
		cout << "\nPara logger: Se inserto el siguiente mensaje en OpenDS: " << endl;
		cout << "Newsgroup del articulo: " << articulo.sNewsgroup << endl;
		cout << "Id del articulo: " << articulo.uiArticleID << endl;
		cout << "Head del articulo: " << articulo.sHead << endl;
		cout << "Body del articulo: " << articulo.sBody << endl;	
	}

	if( ! HeapFree( handle, 0, articulo.sNewsgroup ) ) {
			cout << "HeapFree error en articulo.sNewsgroup." << endl;
	}
	if( ! HeapFree( handle, 0, articulo.sHead ) ) {
			cout << "HeapFree error en articulo.sHead." << endl;
	}
	if( ! HeapFree( handle, 0, articulo.sBody) ) {
			cout << "HeapFree error en articulo.sBody." << endl;
	}
	if( ! HeapFree( handle, 0, xmlCompleto ) ) {
			cout << "HeapFree error en xmlCompleto." << endl;
	}

	// Destruyo el heap.
	if( ! HeapDestroy( handle ) ) {
		cout << "HeapDestroy error." << endl;
	}
	cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
	return 1;
}