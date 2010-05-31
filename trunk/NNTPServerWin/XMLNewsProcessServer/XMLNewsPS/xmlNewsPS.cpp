#include <winsock2.h>
#include<windows.h>
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

typedef struct stIRC_IPC{
   char idDescriptor[16];
   char payloadDescriptor[2];
   char payloadLength[3];
   char payloadXML[1024]; // o char * (??) , si lo dejo en char* hasta donde hago el memcpy ?
}stIRC_IPC;


typedef struct stThreadParameters {
	int ficheroCliente;    /*el file descriptor de la conexion con el nuevo cliente.	*/
	stIRC_IPC datosRecibidos;
	MsmqProcess colaMsmq; /*Cola MSMQ*/
} stThreadParameters;

int crearConexionSocket(SOCKET* ficheroServer, struct sockaddr_in* server)
{
	if((*ficheroServer = socket (AF_INET, SOCK_STREAM, 0))== INVALID_SOCKET){
		cout<<"No se pudo crear el socket"<<endl;
		return EXIT_FAILURE;
	}
	
	server->sin_family		 = AF_INET;
	server->sin_addr.s_addr  = INADDR_ANY ;//Coloca nuestra direccion IP
    server->sin_port		 = htons(16000); //El puerto
	
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
	
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters); //Esto es raro xD, lo saque del HHTPServer
	
	//HANDSHAKE PROTOCOLO IPC/RPC
	
int bytesRecibidos;
	char* estructuraEnBytesIPCRPC;
	estructuraEnBytesIPCRPC = new char[100];

	bytesRecibidos = recv(stParametros.ficheroCliente, estructuraEnBytesIPCRPC, (int)strlen(estructuraEnBytesIPCRPC), 0);
	
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
	// TODO - FGUERRA: Por ahora meto todo el xml en el label. ¿Es correcto esto? :S
	IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");

	// Aca hay que tratar la estructura para obtener solamente el XML. Seria una onda asi:
	// char* xml;
	// xml = obtenerXMLDeEstructura(estructuraEnBytesIPCRPC);

	// Esto no anda ni a palos... Pero es la idea :D
	pMsg->Label = "Este mensaje indica que me llego un mensaje del publisher y en este lugar deberia ir dicho mensaje. El problema es que Fernando no tiene idea de como setearle el string del publisher que le llega a este label que te estoy mostrando :)";

	// Sea lo que sea lo encolo (despues me ocupare de verificar que lo que me mandaron es correcto, aca es al pedo).
	stParametros.colaMsmq.insertarMensaje(pMsg);

	// Si pude insertar el mensaje correctamente cierro todo al carajo.
	delete [] estructuraEnBytesIPCRPC;
	//delete [] xml;
	DeleteObject(pMsg);
	_endthreadex(0);
    return 0;
}


int main(){
	

	//Creo la cola MSMQ o me fijo que ya exista.
	MsmqProcess colaMsmq;
	colaMsmq.crearCola();

	/* Prueba de insercion de 1 msj 
	IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");
	pMsg->Label = "Label de la prueba de Fer";                     //Agrego el Label y Body y envío el mensaje
	pMsg->Body = "Body de la prueba de Fer";
	cout << "pMsg creado correctamente" << endl;
	//colaMsmq.insertarMensaje(pMsg);
	cout << "Inserte el mensaje en la cola" << endl;

	cout << "Voy a leer los msjs" << endl;
	colaMsmq.leerMensajes();
	*/

	//Se inicializa la biblioteca winsock.
	WSADATA WsaData;
	WORD wVersionRequerida = MAKEWORD (2, 2); //Esto es para determinar hasta que version se puede soportar.
	WSAStartup (wVersionRequerida, &WsaData);
	SOCKET ficheroServer;
	struct sockaddr_in server;
	
	/* Se crea el socket que quedara a la escucha de conexiones */
	if(crearConexionSocket(&ficheroServer,&server)==1){
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