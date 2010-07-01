#include<winsock2.h>
#include"berkeleyFunctions.h" //Dentro tiene la llamada a #include"xmlFunctions"
#include <time.h>
//El formato propuesto para los mensajes XML es el siguiente: 
	//	<?xml version="1.0" encoding="iso-8859-1" ?> 
	//		<news> 
	//			<newsgroup>Minuto.com</newsgroup> 
	//			<idNoticia>12345</idNoticia> 
	//			<HEAD>MARTIN PALEMO IDOLO</HEAD> 
	//			<BODY>ARGENTINA CAMPEON MUNDIAL 2010</BODY> 
	//		</news> 
typedef struct largos_IRCIPC{
	int lenIdDescriptor;
	int lenPayloadDescriptor;
	int lenPayloadLength;
}largos_IRCIPC;
typedef struct stIRC_IPC{
   largos_IRCIPC largos;
   char idDescriptor[16+1];
   char payloadDescriptor[1+1];
   char payloadLength[4+1];
   char* payloadXML; // o char * (??) , si lo dejo en char* hasta donde hago el memcpy ?
}stIRC_IPC;

#define LARGOID            17
#define LARGOPAYLOAD	   2
#define LARGOPAYLOADLENGTH 5
#define DEFAULT_BUFLEN 512


xmlDocPtr crearXML(struct news* noticia, char* key)
 {
	xmlNodePtr root,news;
	xmlDocPtr doc;
	char* NEWS     = "news";
	char* ENCODING = "iso-8859-1";
	
	printf("<------------------- creacion XML - aCtitud -------------------> \n");
	doc = xmlNewDoc("1.0");
	doc->encoding = ENCODING; //Propiedad del nodo padre
	
	//Nodo padre
	root = xmlNewDocNode (doc, NULL, NEWS, NULL);
	xmlDocSetRootElement (doc, root);
	
	//Hijos
	xmlNewChild (root, NULL, "newsgroup",  noticia->newsgroup);
	xmlNewChild (root, NULL,"idNoticia", key);
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
	int iResult;
	
	char recvbuf[DEFAULT_BUFLEN];
	char recvbufXML[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//PARA PROTOCOLO
	struct stIRC_IPC* pkg;
	time_t seconds = time(NULL);
	char* handshakeEnBytes;
	char* xmlEnBytes;
	char* responseNNTP;
	int  largoHandshake;
	int largoXmlEnBytes;
	int i;
	printf("################ ENVIO DE XML A NNTPSERVER ################\n");
	printf("PUERTO: %d IP: %s \n",puertoNNTP,ipNNTP);
	pkg = HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,sizeof(struct stIRC_IPC));
	
	//################################# HANDSHAKE #################################  
	//VARIABLES PARA EL HANDSHAKE
	printf((char*)memoriaXML);
	sprintf(pkg->idDescriptor, "%d", seconds);
	strcpy(pkg->payloadDescriptor,"1"); //1=REQUEST;
	strcpy(pkg->payloadLength,"0000");
	strcat(pkg->idDescriptor,"123456");

	printf("idDescriptor: %s \n", pkg->idDescriptor);
	printf("payloadDescriptor : %s \n",pkg->payloadDescriptor);
	printf("payloadLength: %s \n ",pkg->payloadLength);

	pkg->largos.lenIdDescriptor      = strlen(pkg->idDescriptor) +1;
	pkg->largos.lenPayloadDescriptor = strlen(pkg->payloadDescriptor) + 1;
	pkg->largos.lenPayloadLength     = strlen(pkg->payloadLength) + 1;
	//ARMO ESTRUCTURA EN BYTES
	ZeroMemory(handshakeEnBytes,0);
	largoHandshake = sizeof(pkg->largos) + pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor + pkg->largos.lenPayloadLength;//LOS LARGOS SON SIEMPRE FIJOS
	handshakeEnBytes = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,largoHandshake);

	CopyMemory(handshakeEnBytes,(char*)&pkg->largos,sizeof(pkg->largos));
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos),pkg->idDescriptor,pkg->largos.lenIdDescriptor);
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos)+pkg->largos.lenIdDescriptor,pkg->payloadDescriptor,pkg->largos.lenPayloadDescriptor);
	CopyMemory(handshakeEnBytes + sizeof(pkg->largos)+pkg->largos.lenIdDescriptor + pkg->largos.lenPayloadDescriptor,pkg->payloadLength, pkg->largos.lenPayloadLength);

	
	//HAGO LA CONEXION CON EL NNTP SERVER
	if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0){
		printf("Error en inicializacion de Socket");
        return 1;
    }
    lhSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(lhSocket == INVALID_SOCKET){
      printf("Socket Invalido ");
	  return 1;
	}

	memset(&lSockAddr,0, sizeof(lSockAddr));
    lSockAddr.sin_family = AF_INET;
    lSockAddr.sin_port = htons(puertoNNTP);
    lSockAddr.sin_addr.s_addr = inet_addr(ipNNTP);
    lConnect = connect(lhSocket,(SOCKADDR *)&lSockAddr,sizeof(SOCKADDR_IN));
    if(lConnect != 0){
		printf("Connect Error\n");
        return 1;
    }
    lLength = send(lhSocket,handshakeEnBytes,largoHandshake,0);
    
	if (lLength == SOCKET_ERROR) {
        printf("Fallo el send del Handshake: %d\n", WSAGetLastError());
        closesocket(lhSocket);
        WSACleanup();
        return 1;
    }
	//Recibo la respuesta del NNTPServer
	lLength = recv(lhSocket, recvbuf, recvbuflen, 0);
/**
  * SI EL NNTPSERVER NUNCA ME RESPONDE ME QUEDO BLOQUEADO ? COMO HAGO PARA SALIR, COMO HAGO UN TIME OUT (??)
 **/
	if ( iResult > 0 )
            printf("Bytes recividos: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Coneccion cerrada\n");
        else
            printf("recv fallo: %d\n", WSAGetLastError());

	printf("Recibi del XML Process server como response del handshake lo siguiente -> %s\n", recvbuf);
	//Me interesa que payload recibi, 1 si es un es inválido , si es 2 es válido
	CopyMemory(&pkg->payloadDescriptor,recvbuf + (LARGOID-1),(LARGOPAYLOAD-1)); //PORQUE ME LO MANDA CON STRCAT
	printf("PAYLOADDESCRIPTORRRRRRRRRRRRRRRR : %s \n",pkg->payloadDescriptor);
	//El NNTP no me pasa los largos, pero como son estáticos los sé de antemano. 
		//Por eso acá uso constantes.
	if(pkg->payloadDescriptor == 1){
		printf("Hanshake invalido: se cerrara la conexión. La noticia no fue transmitida");
		return 1;}	
	else printf("Handshake valido: se procedera a enviar la noticia en XML");

	
	//################################# ENVIO EL XML #################################
	sprintf(pkg->payloadLength,"%d",tamanioXML+1);
	printf("tamanioXML: %d \n",tamanioXML+1);
	printf("pkg->payloadLength: %s \n",pkg->payloadLength);	

	printf("idDescriptor: %s \n", pkg->idDescriptor);
	printf("payloadDescriptor : %s \n",pkg->payloadDescriptor);
	printf("payloadLength: %s \n ",pkg->payloadLength);
	
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
        printf("Fallo el send del XML: %d\n", WSAGetLastError());
        closesocket(lhSocket);
        WSACleanup();
        return 1;
    }

	//Recibo la respuesta del NNTPServer
	lLength = recv(lhSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes recividos: %d\n", iResult);
		else if ( iResult == 0 ){
			printf("Coneccion cerrada\n");return 1;}
		else{
			printf("recv fallo: %d\n", WSAGetLastError()); return 1;}
	
      //Reutilizo la estructura del envio para manejar el Response
	CopyMemory(&pkg->idDescriptor,(char*)recvbuf,LARGOID);
	CopyMemory(&pkg->payloadDescriptor,(char*)recvbuf + LARGOID,LARGOPAYLOAD); 
	CopyMemory(&pkg->payloadLength,(char*)recvbuf + LARGOID + LARGOPAYLOAD,LARGOPAYLOADLENGTH);
	
	responseNNTP= (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,10);
	CopyMemory((char*)responseNNTP,(char*)recvbuf + (LARGOID - 1) + (LARGOPAYLOAD -1)+(LARGOPAYLOADLENGTH-1),9); //responde "RequestOK"
	printf("Response de NNTP: %s",responseNNTP);    
	closesocket(lhSocket);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,pkg);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,xmlEnBytes);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,handshakeEnBytes);
	if(strcmp(responseNNTP,"RequestOK")==0){
		HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,responseNNTP);return 0;} //TODO SE TRANSMITIO CORRECTAMENTE
	else
		HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,responseNNTP);return 1;
}