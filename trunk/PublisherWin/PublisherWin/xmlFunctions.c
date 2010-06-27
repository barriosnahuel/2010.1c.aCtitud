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
typedef struct stIRC_IPC{

   char idDescriptor[16+1];
   char payloadDescriptor[1+1];
   char payloadLength[4+1];
   char payloadXML[1023+1]; // o char * (??) , si lo dejo en char* hasta donde hago el memcpy ?
}stIRC_IPC;

#define LARGOID            17
#define LARGOPAYLOAD	   2
#define LARGOPAYLOADLENGHT 5


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

int enviarXML(xmlChar* memoriaXML,int* tamanioXML,char* ipNNTP,int puertoNNTP,HANDLE** memoryHandle)
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

	//PARA PROTOCOLO
	struct stIRC_IPC* pkg;
	time_t seconds = time(NULL);
	long milisec = seconds * 1000;

	char* handshakeEnBytes;
	int  largoHandshake;
	printf("################ ENVIO DE XML A NNTPSERVER ################\n");
	
	pkg = HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,sizeof(struct stIRC_IPC));
	
	//VARIABLES PARA EL HANDSHAKE
	sprintf(pkg->idDescriptor, "%d", milisec);
	strcpy(pkg->payloadDescriptor,"1"); //1=REQUEST;
	strcpy(pkg->payloadLength,"0000");

	printf("idDescriptor: %s \n", pkg->idDescriptor);
	printf("payloadDescriptor : %s \n",pkg->payloadDescriptor);
	printf("payloadLength: %s \n ",pkg->payloadLength);
	
	//ARMO ESTRUCTURA EN BYTES
	ZeroMemory(handshakeEnBytes,0);
	largoHandshake = LARGOID+LARGOPAYLOAD+LARGOPAYLOADLENGHT;//LOS LARGOS SON SIEMPRE FIJOS
	handshakeEnBytes = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,largoHandshake);
	CopyMemory(handshakeEnBytes,pkg->idDescriptor,LARGOID);
	CopyMemory(handshakeEnBytes + LARGOID,pkg->payloadDescriptor,LARGOPAYLOAD);
	CopyMemory(handshakeEnBytes + LARGOID + LARGOPAYLOAD,pkg->payloadLength,LARGOPAYLOADLENGHT);

	printf("PUERTO: %d IP: %s \n",puertoNNTP,ipNNTP);
	//HAGO LA CONEXION CON EL NNTP SERVER
	if(WSAStartup(MAKEWORD(2,0),&wsaData) != 0){
		printf("Socket Initialization Error. Program aborted\n");
        return 1;
    }
    lhSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(lhSocket == INVALID_SOCKET){
      printf("Invalid Socket ");
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
    
	closesocket(lhSocket);

	Sleep(100000);
	//Tengo que hacer el request
	printf((char*)memoriaXML);
	return 0;
}