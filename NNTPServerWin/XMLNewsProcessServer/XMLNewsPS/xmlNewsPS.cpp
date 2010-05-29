#include <winsock2.h>
#include<windows.h>
#include<iostream>
#include<cstdlib>
#include<process.h>
using namespace std;
/*
#include<libxml/tree.h>
#include<libxml/parser.h>


class xmlProcess
{
	//Lo privado

public:
	
}*/

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


unsigned __stdcall clientFunction(void* pArguments)
{
	cout<<"Entro al threadCliente"<<endl;
	
	//HANDSHAKE PROTOCOLO IPC/RPC
	
	//RECIBE EL XML, LA APLICACION QUE LO GUARDA EN OPENDS ES LA ENCARGADA DE PARSEAR EL XML

	//GUARDA EL XML EN LA MSMQ
	
	

	_endthreadex(0);
    return 0;
}


int main(){


	//Se inicializa la biblioteca winsock.
	WSADATA WsaData;
	WORD wVersionRequerida = MAKEWORD (2, 2); //Esto es para determinar hasta que version se puede soportar.
	WSAStartup (wVersionRequerida, &WsaData);
	SOCKET ficheroServer;
	struct sockaddr_in server;
	
	if(crearConexionSocket(&ficheroServer,&server)==1){
		cout<<"No se pudo crear la conexion"<<endl;
		return EXIT_FAILURE;
	}
	
	//Queda a la escucha de conexiones
	//while(1){ (??)
		int  addrlen = sizeof(struct sockaddr_in);
		struct sockaddr_in cliente;
		SOCKET ficheroCliente = accept(ficheroServer,(sockaddr*)&cliente,&addrlen);
		if(ficheroCliente != -1){
			cout<<"Conexion entrante.."<<endl;
			unsigned threadProcesarRequest;
			HANDLE threadCliente;
			threadCliente = (HANDLE)_beginthreadex(NULL, 0,&clientFunction,
							 NULL, 0, &threadProcesarRequest);
			
			CloseHandle(threadCliente);
		}
		else{
			cout<<"Problemas al aceptar conexion cliente."<<endl;
		}
	//}
	system("PAUSE");
	return 0;
}