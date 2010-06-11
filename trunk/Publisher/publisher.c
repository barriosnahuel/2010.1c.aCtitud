#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#define DEFAULT_BUFLEN 512
#define PUERTO_SERVERXML "16000"

int __cdecl main(int argc, char **argv) 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
	//Estos son los campos con los que voy a formar sendbuf segun el protocolo IPC/IRC.
/*	char* descriptorID = "1234567812345678"; //Identificador de 16 bytes único descriptor en la red.
	char* payloadDescriptor = "1"; //1 byte. Identificador de nro de protocolo.
	char* payload = "Esta es una prueba del payload\0"; //La carga de datos que se necesite pasar. Queda libre al usuario del protocolo.
	char* payloadLenght = (char*)strlen(payload);//La longitud del descriptor inmediatamente seguido del header.*/
	
	// Aca armo el mensaje a enviar.
	// TODO - FGUERRA: como carajo concateno los char* de arriba para armar el sendbuff? :P
	char *sendbufHandshake = "123456781234567810000";
	char *sendbufXML = "123456781234567810237<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?><news><newsgroup>Minuto.com</newsgroup><idNoticia>12345</idNoticia><HEAD>El tp de operativos no deja dormir a los alumnos!!!</HEAD><BODY>UFFF tengo 32 tecnologías en la cabeza</BODY></news>";
	
	char recvbuf[DEFAULT_BUFLEN];
	char recvbufXML[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], PUERTO_SERVERXML, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Connect to server.
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

	// ##################### HANDSHAKE ###############################################
    // Send an initial buffer
    iResult = send( ConnectSocket, sendbufHandshake, (int)strlen(sendbufHandshake), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("Fallo el send: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes enviados: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
  /*  iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }*/

    // Receive until the peer closes the connection

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

		printf("Recibi del XML Process server como response del handshake lo siguiente -> %s\n", recvbuf);
		// ##################### FIN HANDSHAKE ###############################################

		// ##################### XML ###############################################
    // Send an initial buffer
    iResult = send( ConnectSocket, sendbufXML, (int)strlen(sendbufXML), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("Fallo el send: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes enviados: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


        iResult = recv(ConnectSocket, recvbufXML, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());

		printf("Recibi del XML Process server como response del xml lo siguiente -> %s\n", recvbufXML);
		// ##################### FIN XML ###############################################
		



    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
