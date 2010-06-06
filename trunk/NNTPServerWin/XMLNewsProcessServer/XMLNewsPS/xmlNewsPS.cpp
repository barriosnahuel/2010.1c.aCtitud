#include <winsock2.h>
#include<windows.h>
#include <winbase.h>
#include<iostream>
#include<cstdlib>
#include<process.h>
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

typedef struct stIRC_IPC{
   char idDescriptor[16];
   char payloadDescriptor[2];
   char payloadLength[3];
   char payloadXML[1024]; // o char * (??) , si lo dejo en char* hasta donde hago el memcpy ?
}stIRC_IPC;

typedef struct stConfiguracion{
	char	appPort[6];
    char	serverIP[15+1]; 
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
	
	server->sin_family		 = AF_INET;
	server->sin_addr.s_addr  = INADDR_ANY ;//Coloca nuestra direccion IP
    server->sin_port		 = htons((u_short)configuracion->appPort);//htons(16000);//*(u_short*)configuracion->appPort); //El puerto
	
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
	---- En este caso se setea 0 0 0 porque no me interesa el tamaño del heap.---- */
	HANDLE handle = HeapCreate( 0, 0, 0 );
	if( handle == NULL ) {
		cout << "HeapCreate error." << endl;
	}

	
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters); //Esto es raro xD, lo saque del HHTPServer
	
	//HANDSHAKE PROTOCOLO IPC/RPC
	
	int bytesRecibidos;
	char* estructuraEnBytesIPCRPC;
	// Reservamos 1024 bytes (BUFFERSIZE) que es lo q corresponde segun restriccion de TP.
	estructuraEnBytesIPCRPC = (char*) HeapAlloc( handle, 0, BUFFERSIZE );
	
	if( estructuraEnBytesIPCRPC == NULL ) {
		cout << "HeapAlloc error." << endl;
	}

	bytesRecibidos = recv(stParametros.ficheroCliente, estructuraEnBytesIPCRPC, BUFFERSIZE, 0);
	
	//PASO LOS BYTES RECIBIDOS A LA ESTRUCTURA IPC/RPC
	//memcpy(stParametros.datosRecibidos.idDescriptor,
	//	   (char*)&bytesRecibidos,strlen(stParametros.datosRecibidos.idDescriptor)/*16*/);
	//memcpy(stParametros.datosRecibidos.payloadDescriptor,
	//	   (char*)&bytesRecibidos+strlen(stParametros.datosRecibidos.idDescriptor),strlen(stParametros.datosRecibidos.payloadDescriptor));
	//memcpy(stParametros.datosRecibidos.payloadLength,
	//	   (char*)&bytesRecibidos+strlen(stParametros.datosRecibidos.idDescriptor)+strlen(stParametros.datosRecibidos.payloadDescriptor),strlen(stParametros.datosRecibidos.payloadLength));
	//memcpy(stParametros.datosRecibidos.payloadXML,
	//	   (char*)&bytesRecibidos+strlen(stParametros.datosRecibidos.idDescriptor)+strlen(stParametros.datosRecibidos.payloadDescriptor)
      //     + strlen(stParametros.datosRecibidos.payloadLength),strlen(stParametros.datosRecibidos.payloadXML));


	// TODO - FGUERRA: Esto posteriormente se cambiara por el xml solo.
	cout << "Recibi el xml: " << estructuraEnBytesIPCRPC << endl;
	
	// Paso el xml a un msj para meter en la cola.
	// TODO - FGUERRA: Por ahora meto todo el xml en el body.
	IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");

	// Aca hay que tratar la estructura para obtener solamente el XML. Seria una onda asi:
	// char* xml;
	// xml = obtenerXMLDeEstructura(estructuraEnBytesIPCRPC);

	pMsg->Body = estructuraEnBytesIPCRPC;

	// Sea lo que sea lo encolo (despues me ocupare de verificar que lo que me mandaron es correcto, aca es al pedo).
	stParametros.colaMsmq.insertarMensaje(pMsg);

	// Si pude insertar el mensaje correctamente cierro todo al carajo.

	// Libero la memoria reservada.
	if( ! HeapFree( handle, 0, estructuraEnBytesIPCRPC ) ) {
		cout << "HeapFree error." << endl;
	}
	// Destruyo el heap.
	if( ! HeapDestroy( handle ) ) {
		cout << "HeapDestroy error." << endl;
	}

	//delete [] xml;
	DeleteObject(pMsg);
	_endthreadex(0);
    return 0;
}


int main(){
	//Carga configuracion --> Ip y puerto del serividor
	struct stConfiguracion configuracion;

	
	LPCSTR archivoConfiguracion = "../configuracion.ini";
	GetPrivateProfileString("configuracion2","IP", 0,configuracion.serverIP,16,archivoConfiguracion);
	GetPrivateProfileString("configuracion2","PUERTO", 0,configuracion.appPort,6,archivoConfiguracion);

//    cout<<"Puerto:"<<configuracion.appPort<<" IP:"<<configuracion.serverIP<<endl;


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
	
	//while(1){
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
	//}*/
	colaMsmq.leerMensajes();
	system("PAUSE");
	return 0;
}