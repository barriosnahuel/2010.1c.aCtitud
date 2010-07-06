#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <iostream>
#include <cstdlib>
#include <process.h>
#include "funcionesMSMQ.hpp"
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
#define BUFFERSIZE 1024

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

typedef struct stConfiguracion{
	char	appPort[6];
    char	serverIP[16]; 
	char	szDefault[255];
}stConfiguracion;


typedef struct stThreadParameters {
	SOCKET ficheroCliente;    /*el file descriptor de la conexion con el nuevo cliente.	*/
	stIRC_IPC datosRecibidos;
	MsmqProcess colaMsmq; /*Cola MSMQ*/
} stThreadParameters;

int crearConexionSocket(SOCKET* ficheroServer, struct sockaddr_in* server,struct stConfiguracion* configuracion)
{
	if((*ficheroServer = socket (AF_INET, SOCK_STREAM, 0))== INVALID_SOCKET){
		cout<<"No se pudo crear el socket"<<endl;
		return EXIT_FAILURE;
	}
	
	unsigned int puerto = atoi(configuracion->appPort);
	server->sin_family		 = AF_INET;
	server->sin_addr.s_addr  = INADDR_ANY ;//Coloca nuestra direccion IP
    server->sin_port		 = htons((u_short)puerto);
	
	if (bind(*ficheroServer, (SOCKADDR*) &(*server), sizeof(*server))==-1){
	  cout<<"Error al asociar el puerto al socket."<<endl;
	  return EXIT_FAILURE;
	}	
	if(listen(*ficheroServer,1)==-1){
	  cout<<"No se pudo dejar el socket a la escucha de conexiones entrantes"<<endl;
	  return EXIT_FAILURE;
	}
	
	cout <<"Se creo el socket correctamente"<<endl;	
	return EXIT_SUCCESS;

};

unsigned __stdcall clientFunction(void* threadParameters)
{
	cout<<"Entro al threadCliente"<<endl;
	
	/* HeapCreate( 
					Opciones del heap, 
					tamaño inicial del heap (en bytes). Si es cero provee el tamaño de una pagina,
					tamaño maximo del heap (en bytes). Si es cero puede crecer y su unica restriccion es la memoria disponible )
	---- En este caso se setea 0 1024 0 porque quiero q arranque en 1024.---- */

	HANDLE handle = HeapCreate( 0, 1024, 0 );
	if( handle == NULL ) {
		cout << "HeapCreate error." << endl;
	}

	
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters); //Esto es raro xD, lo saque del HHTPServer
	
	int bytesRecibidos;
	int bytesEnviados;
	// Reservamos 1024 bytes (BUFFERSIZE) que es lo q corresponde segun restriccion de TP.
	char* handshake = (char*) HeapAlloc( handle, 0, BUFFERSIZE);
	if( handshake == NULL ) {
		cout << "HeapAlloc error." << endl;
	}
	char *estructuraEnBytesIPCRPC = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	if( estructuraEnBytesIPCRPC == NULL ) {
		cout << "HeapAlloc error." << endl;
	}

	// ################## Variables para el response ####################################
	char *idDescriptor = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *payloadDescriptor = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *payloadLengthHandshake = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *payloadLengthXml = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *payloadXMLResponse = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *xmlResponse = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	char *handshakeResponse;
	// ################## FIN Variables para el response ####################################
	
	// ################## INICIO MENSAJE HANDSHAKE ###################################
	cout << "---------------- Arranco a procesar el Handshake ----------------" << endl;
	bytesRecibidos = recv(stParametros.ficheroCliente, handshake, BUFFERSIZE, 0);
	
	CopyMemory(&stParametros.datosRecibidos.largos,handshake,sizeof(largos_IRCIPC));
	CopyMemory(&stParametros.datosRecibidos.idDescriptor,handshake + sizeof(largos_IRCIPC),stParametros.datosRecibidos.largos.lenIdDescriptor);
	CopyMemory(&stParametros.datosRecibidos.payloadDescriptor, handshake+ sizeof(largos_IRCIPC) + stParametros.datosRecibidos.largos.lenIdDescriptor , stParametros.datosRecibidos.largos.lenPayloadDescriptor);
	CopyMemory(&stParametros.datosRecibidos.payloadLength,handshake+ sizeof(largos_IRCIPC) + stParametros.datosRecibidos.largos.lenIdDescriptor + stParametros.datosRecibidos.largos.lenPayloadDescriptor, stParametros.datosRecibidos.largos.lenPayloadLength);
	
	cout << "Id descriptor handshake: " << stParametros.datosRecibidos.idDescriptor << endl;
	cout << "payloadDescriptor handshake: " << stParametros.datosRecibidos.payloadDescriptor << endl;
	cout << "payloadLength handshake: " << stParametros.datosRecibidos.payloadLength << endl;
	
	//cout << "payload handshake: " << stParametros.datosRecibidos.payloadXML << endl;

	idDescriptor = stParametros.datosRecibidos.idDescriptor;
	payloadLengthHandshake = "0000";
	
	// ################## INICIO VALIDACION HANDSHAKE ##################

	if((strcmp(stParametros.datosRecibidos.payloadLength, "0000") != 0) || (strlen(stParametros.datosRecibidos.idDescriptor) != 16)
		|| (strlen(stParametros.datosRecibidos.payloadDescriptor) != 1) || (strlen(stParametros.datosRecibidos.payloadLength) != 4)) {
	
		// Si el handshake es invalido mando 1, caso contrario 2.
		payloadDescriptor = "1";
		handshakeResponse = (char*) HeapAlloc( handle, 0, strlen(idDescriptor)+strlen(payloadDescriptor)+strlen(payloadLengthHandshake)+3);
		// Concateno los valores para responderle al publisher handshake OK.
		handshakeResponse = strcat(idDescriptor, payloadDescriptor);
		handshakeResponse = strcat(handshakeResponse, payloadLengthHandshake);

		if ((bytesEnviados = send(stParametros.ficheroCliente, handshakeResponse, (int)strlen(handshakeResponse), 0)) == -1) {
				cout << "Error al enviar response handshake" << endl;
		}
		if( ! HeapFree( handle, 0, estructuraEnBytesIPCRPC ) ) {
			cout << "HeapFree error en estructuraEnBytesIPCRPC." << endl;
		}
		if( ! HeapFree( handle, 0, handshake ) ) {
			cout << "HeapFree error en handshake." << endl;
		}

		// Destruyo el heap.
		if( ! HeapDestroy( handle ) ) {
			cout << "HeapDestroy error." << endl;
		}

		cout << "Se cerrara el thread ya que el handshake es invalido." << endl;

		_endthreadex(0);
		return 1;
	}
	// ################## FIN VALIDACION HANDSHAKE ##################

	// Si el handshake es invalido mando 1, caso contrario 2.
	payloadDescriptor = "2";
	handshakeResponse = (char*) HeapAlloc( handle, 0, strlen(idDescriptor)+strlen(payloadDescriptor)+strlen(payloadLengthHandshake)+1);//el strcat deja un solo /0 ese es el problema
	// Concateno los valores para responderle al publisher handshake OK.
	handshakeResponse = strcat(idDescriptor, payloadDescriptor);
	handshakeResponse = strcat(handshakeResponse, payloadLengthHandshake);
	cout << "Enviare al publisher el siguiente response del handshake: " << handshakeResponse << endl;

	if ((bytesEnviados = send(stParametros.ficheroCliente, handshakeResponse, (int)strlen(handshakeResponse), 0)) == -1)
		cout << "Error al enviar response handshake" << endl;
	
	cout << "---------------- Termino de procesar el Handshake ----------------" << endl;
	// ################## FIN MENSAJE HANDSHAKE ###################################

	// ################## INICIO MENSAJE CON XML ###################################
	cout << "---------------- Arranco a procesar el mensaje con XML ----------------" << endl;
	bytesRecibidos = recv(stParametros.ficheroCliente, estructuraEnBytesIPCRPC, BUFFERSIZE, 0);

	CopyMemory(&stParametros.datosRecibidos.largos,estructuraEnBytesIPCRPC,sizeof(largos_IRCIPC));
	CopyMemory(&stParametros.datosRecibidos.idDescriptor,estructuraEnBytesIPCRPC + sizeof(largos_IRCIPC),stParametros.datosRecibidos.largos.lenIdDescriptor);
	CopyMemory(&stParametros.datosRecibidos.payloadDescriptor, estructuraEnBytesIPCRPC+ sizeof(largos_IRCIPC) + stParametros.datosRecibidos.largos.lenIdDescriptor , stParametros.datosRecibidos.largos.lenPayloadDescriptor);
	CopyMemory(&stParametros.datosRecibidos.payloadLength,estructuraEnBytesIPCRPC + sizeof(largos_IRCIPC) + stParametros.datosRecibidos.largos.lenIdDescriptor + stParametros.datosRecibidos.largos.lenPayloadDescriptor, stParametros.datosRecibidos.largos.lenPayloadLength);
	
	stParametros.datosRecibidos.payloadXML = (char*)HeapAlloc(handle,0,atoi(stParametros.datosRecibidos.payloadLength));
	
	CopyMemory(stParametros.datosRecibidos.payloadXML,estructuraEnBytesIPCRPC+ sizeof(largos_IRCIPC) + stParametros.datosRecibidos.largos.lenIdDescriptor + stParametros.datosRecibidos.largos.lenPayloadDescriptor + stParametros.datosRecibidos.largos.lenPayloadLength,
			   atoi(stParametros.datosRecibidos.payloadLength));
		
	cout << "Id descriptor: " << stParametros.datosRecibidos.idDescriptor << endl;
	cout << "payloadDescriptor: " << stParametros.datosRecibidos.payloadDescriptor << endl;
	cout << "payloadLength: " << stParametros.datosRecibidos.payloadLength << endl;
	cout << "Payload: " << stParametros.datosRecibidos.payloadXML << endl;

	
	idDescriptor = stParametros.datosRecibidos.idDescriptor;
	payloadXMLResponse = "RequestOK";
	payloadLengthXml = "0009";

	strcpy(xmlResponse, idDescriptor);
	xmlResponse = strcat(xmlResponse, payloadDescriptor);
	xmlResponse = strcat(xmlResponse, payloadLengthXml);
	xmlResponse = strcat(xmlResponse, payloadXMLResponse);
	cout << "Enviare al publisher el siguiente response del xml: " << xmlResponse << endl;

	if ((bytesEnviados = send(stParametros.ficheroCliente, xmlResponse, (int)strlen(xmlResponse), 0)) == -1)
		cout << "Error al enviar response" << endl;

	cout << "---------------- Termino de procesar el mensaje con XML ----------------" << endl;

	// ################## FIN MENSAJE CON XML ###################################

	// Paso el xml a un msj para meter en la cola.
	IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");

	pMsg->Body = stParametros.datosRecibidos.payloadXML;

	// Sea lo que sea lo encolo (despues me ocupare de verificar que lo que me mandaron es correcto, aca es al pedo).
	stParametros.colaMsmq.insertarMensaje(pMsg);

	// Si pude insertar el mensaje correctamente cierro todo al carajo.

	// Libero la memoria reservada.
	if( ! HeapFree( handle, 0, estructuraEnBytesIPCRPC ) ) {
		cout << "HeapFree error en estructuraEnBytesIPCRPC." << endl;
	}
	if( ! HeapFree( handle, 0, handshake ) ) {
		cout << "HeapFree error en handshake." << endl;
	}

	// Destruyo el heap.
	if( ! HeapDestroy( handle ) ) {
		cout << "HeapDestroy error." << endl;
	}

	DeleteObject(pMsg);
	_endthreadex(0);
    return 0;
}


int main(){
	//Carga configuracion --> Ip y puerto del serividor
	struct stConfiguracion configuracion;
	LPCSTR archivoConfiguracion = "..\\configuracion.ini";
	GetPrivateProfileString("configuracion","IP", configuracion.szDefault,configuracion.serverIP,16,archivoConfiguracion);
	GetPrivateProfileString("configuracion","PUERTO", configuracion.szDefault,configuracion.appPort,6,archivoConfiguracion);
	cout<<"Proceso levantado en la ip: "<<configuracion.serverIP<<" y en el puerto: "<<configuracion.appPort<<endl;


	//Creo la cola MSMQ o me fijo que ya exista.
	MsmqProcess colaMsmq;
	colaMsmq.crearCola();

	//Se inicializa la biblioteca winsock.
	WSADATA WsaData;
	WORD wVersionRequerida = MAKEWORD (2, 2); //Esto es para determinar hasta que version se puede soportar.
	WSAStartup (wVersionRequerida, &WsaData);
	SOCKET ficheroServer;
	struct sockaddr_in server;
	
	/* Se crea el socket que quedara a la escucha de conexiones */
	if(crearConexionSocket(&ficheroServer,&server,&configuracion)==1){
		cout<<"No se pudo crear la conexion"<<endl;
		return EXIT_FAILURE;
	}
	
	while(1){
		int  addrlen = sizeof(struct sockaddr_in);
		struct sockaddr_in cliente;
		SOCKET ficheroCliente = accept(ficheroServer,(sockaddr*)&cliente,&addrlen);
		if(ficheroCliente != -1){
			cout<<"Conexion entrante.."<<endl;
			
		    //Le seteo los parametros al nuevo thread
			
			stThreadParameters stParameters;
			stParameters.ficheroCliente= ficheroCliente;
			stParameters.colaMsmq = colaMsmq;;

			unsigned threadProcesarRequest;
			HANDLE threadCliente;
			if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&clientFunction,
				(void*)&stParameters, 0, &threadProcesarRequest))!=0){
			
			
				CloseHandle(threadCliente);
			}else{
				cout<<"No se pudo crear el thread para procesar el request"<<endl;
			}
			
		}
		else{
			cout<<"Problemas al aceptar conexion cliente."<<endl;
		}
	}
	
	system("PAUSE");
	return 0;
}