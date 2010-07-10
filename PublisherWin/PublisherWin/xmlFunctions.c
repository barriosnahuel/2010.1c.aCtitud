#include< winsock2.h>
#include "berkeleyFunctions.h" //Dentro tiene la llamada a #include"xmlFunctions"
#include <time.h>
#include <string.h>
#include "logger-win.h"

//El formato propuesto para los mensajes XML es el siguiente: 
	//	<?xml version="1.0" encoding="iso-8859-1" ?> 
	//		<news> 
	//			<newsgroup>Minuto.com</newsgroup> 
	//			<idNoticia>12345</idNoticia> 
	//			<HEAD>MARTIN PALEMO IDOLO</HEAD> 
	//			<BODY>ARGENTINA CAMPEON MUNDIAL 2010</BODY> 
	//		</news> 

typedef struct largos_IRCIPC{
	size_t lenIdDescriptor;
	size_t lenPayloadDescriptor;
	size_t lenPayloadLength;
}largos_IRCIPC;
typedef struct stIRC_IPC{
   largos_IRCIPC largos;
   char idDescriptor[16+1];
   char payloadDescriptor[1+1];
   char payloadLength[4+1];
   char* payloadXML; 
}stIRC_IPC;

#define LARGOID            17
#define LARGOPAYLOAD	   2
#define LARGOPAYLOADLENGTH 5
#define DEFAULT_BUFLEN 512


xmlDocPtr crearXML(struct news* noticia, char* key)
 {
	xmlNodePtr root;
	xmlDocPtr doc;
	char* NEWS     = "news";
	char* ENCODING = "iso-8859-1";
	
	LoguearInformacion("<------------------- creacion XML - aCtitud ------------------->");
	doc = xmlNewDoc("1.0");
	doc->encoding = ENCODING; //Propiedad del nodo padre
	
	//Nodo padre
	root = xmlNewDocNode (doc, NULL, NEWS, NULL);
	xmlDocSetRootElement (doc, root);
	
	//Hijos
	xmlNewChild (root, NULL, "newsgroup",  noticia->newsgroup);
	xmlNewChild (root, NULL, "idNoticia", key);
	xmlNewChild (root, NULL, "HEAD",  noticia->head);
	xmlNewChild (root, NULL, "BODY",  noticia->body);

	return doc;
}


int enviarXML(xmlChar* memoriaXML,int tamanioXML,char* ipNNTP,int puertoNNTP,HANDLE** memoryHandle)
{

	//Handshake 
	//Para realizar los handshake entre los procesos se deberá enviar un mensaje de request del protocolo IPC/IRC 
	//con los siguientes valores: 
	//Descriptor ID:  
	//Identificador de 16 bytes único descriptor en la red. 
	//PayloadDescriptor:  
	//Nro. identificando el proceso que se está conectando, reservado para iniciar la conexión. 
	//PayLoad Lenght: 0, indicando que no hay payload. 
	
	//PARA SOCKETS
	SOCKET lhSocket;
    SOCKADDR_IN lSockAddr;
    WSADATA wsaData;
	int lConnect;
	int lLength;
		
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//PARA PROTOCOLO
	struct stIRC_IPC* pkg;
	time_t seconds = time(NULL);
	char* handshakeEnBytes;
	char* xmlEnBytes;
	char* responseNNTP;
	size_t  largoHandshake;
	size_t largoXmlEnBytes;

	LoguearInformacion("ENVIO DE XML A NNTPSERVER");
	pkg = HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,sizeof(struct stIRC_IPC));
	
	//################################# HANDSHAKE #################################  
	//VARIABLES PARA EL HANDSHAKE
	//LoguearInformacion((char*)memoriaXML);
	sprintf_s(pkg->idDescriptor, 16, "%d", seconds);
	strcpy(pkg->payloadDescriptor,"1"); //1=REQUEST;
	strcpy(pkg->payloadLength,"0000");
	strcat(pkg->idDescriptor,"123456");

	pkg->largos.lenIdDescriptor      = strlen(pkg->idDescriptor) +1;
	pkg->largos.lenPayloadDescriptor = strlen(pkg->payloadDescriptor) + 1;
	pkg->largos.lenPayloadLength     = strlen(pkg->payloadLength) + 1;
	//ARMO ESTRUCTURA EN BYTES
	largoHandshake = sizeof(pkg->largos) + pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor + pkg->largos.lenPayloadLength;//LOS LARGOS SON SIEMPRE FIJOS
	handshakeEnBytes = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,largoHandshake);
	ZeroMemory(handshakeEnBytes,largoHandshake);
	CopyMemory(handshakeEnBytes,(char*)&pkg->largos,sizeof(pkg->largos));
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos),pkg->idDescriptor,pkg->largos.lenIdDescriptor);
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos)+pkg->largos.lenIdDescriptor,pkg->payloadDescriptor,pkg->largos.lenPayloadDescriptor);
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos)+pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor,pkg->payloadLength, pkg->largos.lenPayloadLength);

	
	//HAGO LA CONEXION CON EL NNTP SERVER
	if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0){
		LoguearDebugging("Error en inicializacion de Socket");
        return 1;
    }
    lhSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(lhSocket == INVALID_SOCKET){
      LoguearDebugging("Socket Invalido");
	  return 1;
	}

	memset(&lSockAddr,0, sizeof(lSockAddr));
    lSockAddr.sin_family = AF_INET;
    lSockAddr.sin_port = htons(puertoNNTP);
    lSockAddr.sin_addr.s_addr = inet_addr(ipNNTP);
    lConnect = connect(lhSocket,(SOCKADDR *)&lSockAddr,sizeof(SOCKADDR_IN));
    if(lConnect != 0){
		LoguearDebugging("Connect Error");
        return 1;
    }
    lLength = send(lhSocket,handshakeEnBytes,largoHandshake,0);
    
	if (lLength == SOCKET_ERROR) {
        LoguearDebugging("Fallo el send del Handshake");
        closesocket(lhSocket);
        WSACleanup();
        return 1;
    }
	
	//Recibo la respuesta del NNTPServer
	lLength = recv(lhSocket, recvbuf, recvbuflen, 0);
	if ( lLength > 0 )
            LoguearInformacion("Recepción ok.");
        else if ( lLength == 0 )
            LoguearDebugging("Coneccion cerrada");
        else
            LoguearDebugging("Fallo al recibir datos (recv)");

	LoguearInformacion("Se recibio respuesta del XML Process server.");
	//Me interesa que payload recibi, 1 si es un es inválido , si es 2 es válido
	CopyMemory(&pkg->payloadDescriptor,recvbuf + (LARGOID-1),(LARGOPAYLOAD-1)); //PORQUE ME LO MANDA CON STRCAT
	//El NNTP no me pasa los largos, pero como son estáticos los sé de antemano. 
		//Por eso acá uso constantes.
	if(strcmp(pkg->payloadDescriptor,"1")==0){
		LoguearInformacion("Hanshake invalido: se cerrara la conexión. La noticia no fue transmitida");
		return 1;}	
	else LoguearInformacion("Handshake valido: se procedera a enviar la noticia en XML");

	
	//################################# ENVIO EL XML #################################
	sprintf_s(pkg->payloadLength,5, "%d",tamanioXML+1);

	pkg->largos.lenPayloadLength = strlen(pkg->payloadLength)+1;	

	largoXmlEnBytes = sizeof(pkg->largos) + pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor + pkg->largos.lenPayloadLength + pkg->payloadLength;//LOS LARGOS SON SIEMPRE FIJOS
	xmlEnBytes = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,largoXmlEnBytes);

	//COPIO EL MENSAJE XML AL PAYLOAD XML
	pkg->payloadXML = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,tamanioXML);
	strcpy(pkg->payloadXML,memoriaXML);
	
	CopyMemory(xmlEnBytes,(char*)&pkg->largos,sizeof(largos_IRCIPC));
	CopyMemory(xmlEnBytes + sizeof(largos_IRCIPC),pkg->idDescriptor,pkg->largos.lenIdDescriptor);
	CopyMemory(xmlEnBytes + sizeof(largos_IRCIPC)+pkg->largos.lenIdDescriptor,pkg->payloadDescriptor,pkg->largos.lenPayloadDescriptor);
	CopyMemory(xmlEnBytes + sizeof(largos_IRCIPC)+pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor,pkg->payloadLength, pkg->largos.lenPayloadLength);
	CopyMemory(xmlEnBytes + sizeof(largos_IRCIPC)+pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor +pkg->largos.lenPayloadLength,
		       pkg->payloadXML,tamanioXML);
	
	
	lLength = send(lhSocket,xmlEnBytes,largoXmlEnBytes,0);
    
	if (lLength == SOCKET_ERROR) {
        LoguearDebugging("Fallo el send del XML");
        closesocket(lhSocket);
        WSACleanup();
        return 1;
    }

	//Recibo la respuesta del NNTPServer
	lLength = recv(lhSocket, recvbuf, recvbuflen, 0);
        if ( lLength > 0 )
           LoguearInformacion("Respuesta recibida ok.");
		else if ( lLength == 0 ){
			LoguearInformacion("Coneccion cerrada.");return 1;}
		else{
			LoguearDebugging("Fallo al recibir datos del NNTP Server (recv)."); return 1;}
	
      //Reutilizo la estructura del envio para manejar el Response
	CopyMemory(&pkg->idDescriptor,(char*)recvbuf,LARGOID);
	CopyMemory(&pkg->payloadDescriptor,(char*)recvbuf + LARGOID,LARGOPAYLOAD); 
	CopyMemory(&pkg->payloadLength,(char*)recvbuf + LARGOID + LARGOPAYLOAD,LARGOPAYLOADLENGTH);
	
	responseNNTP= (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,10);
	CopyMemory((char*)responseNNTP,(char*)recvbuf + (LARGOID - 1) + (LARGOPAYLOAD -1)+(LARGOPAYLOADLENGTH-1),9); //responde "RequestOK"
	//printf("Response de NNTP: %s",responseNNTP);    
	closesocket(lhSocket);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,pkg);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,xmlEnBytes);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,handshakeEnBytes);
	if(strcmp(responseNNTP,"RequestOK")==0){
		HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,responseNNTP);return 0;} //TODO SE TRANSMITIO CORRECTAMENTE
	else
		HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,responseNNTP);return 1;
}



