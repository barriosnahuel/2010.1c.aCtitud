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
#include "funcionesMSMQ.hpp"
#include "LdapWrapperHandler-Win.hpp"
/*extern "C"{
    #include "LdapWrapperHandler.h"
}
*/

//	Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#define DEFAULT_BUFLEN 512/*	ESTO CREO QUE NO HACE FALTA.*/

using namespace std;

/*
#include<libxml/tree.h>
#include<libxml/parser.h>
class xmlProcess
{
	//Lo privado
public:
}
*/

typedef struct stConfiguracion{
	char	acOpenDSPort[6];
    char	acOpenDSServer[16];
	char	acInterval[10];

	char	szDefault[255];
}stConfiguracion;

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

	// Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

	/*	heap????? Que es esto?	*/
	struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family= AF_UNSPEC;
    hints.ai_socktype= SOCK_STREAM;
    hints.ai_protocol= IPPROTO_TCP;

    // Resolve the server address and port
	struct addrinfo *result= NULL;
	iResult = getaddrinfo(argv[1], configuracion.acOpenDSPort, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Itero intentando conectarme, hasta que consiga una conexion.
	SOCKET ConnectSocket= INVALID_SOCKET;
	struct addrinfo *ptr= NULL;
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Creo un socket para conectarme.
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Me conecto.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

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
		return -1;
	}
	//asprintf(&sLogMessage, "Conectado a OpenDS en: IP=%s; Port=%d.", stConf.czOpenDSServer, stConf.uiOpenDSPort);
	//LoguearInformacion(sLogMessage);
	cout << "Conectado a OpenDS en: IP= " << configuracion.acOpenDSServer << "; Port= " << configuracion.acOpenDSPort << endl;

	//	Comienzo a iterar infinitamente, con un intervalo de X tiempo el cual logro llamando a
	//	la funcion sleep(intervaloDeTiempo); donde intervaloDeTiempo es una variable que cargamos
	//	del archivo de configuracion.
	while(1){/*	ToDo: Ver como salir de aca, hacer algo como el NNTPServerBam porque sino no puedo desvincular el puerto.	*/

		if(/*ToDo: Hay nuevos mensajes en la cola MSMQ*/true){
			if(!consumirMensajesYAlmacenarEnBD(colaMsmq, stPLDAPSession, stPLDAPSessionOperations))
				cout << "Ocurrio un error y la aplicacion no pudo continuar normalmente." << endl;
			else
				cout << "Se consumieron los mensajes y se persistieron correctamente." << endl;
		}
		else
			cout << "Como no hay mensajes nuevos, no hago nada." << endl;
	
		Sleep(atoi(configuracion.acInterval));
	}

	/*	ToDo: Cerrar la conexion (socket).	*/

	/*	ToDo: Ver la memoria que haya que liberar.	*/
	
	system("PAUSE");//	ToDo:	Esto para que es??
	return 0;
}


int consumirMensajesYAlmacenarEnBD(	MsmqProcess colaMsmq
									, PLDAP_SESSION stPLDAPSession
									, PLDAP_SESSION_OP stPLDAPSessionOperations){
	cout << "--> consumirMensajesYAlmacenarEnBD()" << endl;
	
	while(/*Haya mas mensajes*/true){
		if(!/*	ToDo: Obtengo el primer msj de la cola.	*/true){
			cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
			return 0;
		}

		if(!/*	ToDo: Parseo el msj usando seguramente libxml2 y voy armando el objeto article.	*/true){
			cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
			return 0;
		}

		if(!/*	ToDo: Persisto el msj usando las funciones del LDAPWrapperHandler. (El parametro es un objeto stArticle)	*/true){
			cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
			return 0;
		}
	}

	cout << "<-- consumirMensajesYAlmacenarEnBD()" << endl;
	return 1;
}