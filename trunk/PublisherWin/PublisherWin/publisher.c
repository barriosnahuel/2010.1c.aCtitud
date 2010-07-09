#include "berkeleyFunctions.h"
#include <windows.h>
#include <stdio.h>

typedef struct stThreadParameters {
	DB* dbHandler;
	HANDLE* memoryHandler;
	char newsgroup[100];
} stThreadParameters;

typedef struct stSenderParameters {
	DB* dbHandler;
	HANDLE* memoryHandler;
	int puertoNNTP;
	char ipNNTP[16];
	int tiempoEspera;
	char newsgroup[100];
} stSenderParameters;

#define BUFFERSIZE 1024
#define BERKELEY_ID_LEN 11
#define BUFFERCADSIZE 9

void lecturaDinamica(char** cadena, HANDLE** handler){

	char tempCad[BUFFERCADSIZE+1];
	char car;
	int i,largo,size;
	largo = 0;
	*cadena[0]='\0';
	size=1;

	for(i=0;(car = getchar())!='\n';i++){
		tempCad[i]=car;
		if( i ==BUFFERCADSIZE){
			tempCad[BUFFERCADSIZE]='\0';
			size = size + BUFFERCADSIZE;
			HeapReAlloc(*handler,0,*cadena,(DWORD)size);
			strcat(*cadena,tempCad);
			tempCad[0]=car;
			i=0;
		}
	}
	if(i!=0){
		size = size + i;
		tempCad[i]='\0';
		HeapReAlloc(*handler,0,*cadena,(DWORD)size );
		strcat(*cadena,tempCad);
	}
}



unsigned __stdcall publisherFunction(void* threadParameters)
{
	struct news noticia;
	char *head;
	char *body;

	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);
	stParametros.memoryHandler =  HeapCreate(0,1024,0); //esto debería ir en cada hilo
	stParametros.dbHandler = NULL;
	
	if( stParametros.memoryHandler == NULL ) 
		printf("HeapCreate error.\n");

	printf("<------------------- Ingreso de Noticia aCtitud -------------------> \n");

	head = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,2);
	ZeroMemory(head,2);
	printf("Ingrese el HEAD de la noticia: ");
	lecturaDinamica(&head,&(stParametros.memoryHandler));
	printf("HEAD INGRESADO: %s",head);

	body = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,2);
	ZeroMemory(body,2);
	printf("Ingrese el body de la noticia: ");
	lecturaDinamica(&body,&(stParametros.memoryHandler));
	printf("body INGRESADO: %s",body);

	noticia.largos.newsgrouplen = strlen(stParametros.newsgroup)+1;
	noticia.largos.headlen = strlen(head)+1;
	noticia.largos.bodylen = strlen(body)+1;
	noticia.largos.transmittedlen = strlen("0")+1;
	noticia.largos.idlen= BERKELEY_ID_LEN;
	
	noticia.newsgroup = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,10/*noticia->largos.newsgrouplen*/);
	noticia.head	  = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.headlen);
	noticia.body	  = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.bodylen);
    noticia.transmitted = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.transmittedlen);
	noticia.id = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.idlen);
	noticia.newsgroup = stParametros.newsgroup;
	noticia.head      = head;
	noticia.body	  = body;
	noticia.transmitted = "0" ; 

	
	printf("noticia.newsgroup: %s \n",noticia.newsgroup);
	printf("noticia.head: %s \n",noticia.head);
	printf("noticia.body: %s \n",noticia.body);
	printf("noticia.transmitted: %s \n",noticia.transmitted);	
	
		
	printf("<------------------- Acceso a db BERKELEY - aCtitud -------------------> \n");
	
	createDb(&stParametros.dbHandler, &stParametros.memoryHandler,&stParametros.newsgroup);
	generateNewID(&stParametros.dbHandler, &noticia.id);
	
	putArticle(&noticia,&stParametros.dbHandler,&stParametros.memoryHandler);	
	closeDb(&stParametros.dbHandler);


	printf("TERMINA DE PROCESAR EL THREAD CLIENTE\n");
	getchar();
	_endthreadex(0);
	return 0;
}


unsigned __stdcall senderFunction(void* threadParameters)
{
	/**
	 * Busco en Berkeley noticias con 0 , ya que no estan transmitidas.
	 * Las paso a formato XML
	 * Las envio al NNTP 
	**/
	stSenderParameters stParametros = *((stSenderParameters*) threadParameters);
	printf("<------------------- SENDER - aCtitud -------------------> \n");
	stParametros.memoryHandler =  HeapCreate(0,1024,0); 
	stParametros.dbHandler = NULL;
	if( stParametros.memoryHandler == NULL ) 
		printf("Sender HeapCreate error.\n");
	
	createDb(&stParametros.dbHandler,&stParametros.memoryHandler,&stParametros.newsgroup);
	
	//Se fija las noticias que no estan enviadas, las pasa a XML y las envia a nntp
	//noticiasNoEnviadas(&stParametros.dbHandler, &stParametros.memoryHandler);	
	noticiasNoEnviadas(&stParametros.dbHandler, &stParametros.memoryHandler,&stParametros.ipNNTP,stParametros.puertoNNTP);	
	
	closeDb(&stParametros.dbHandler);
	HeapDestroy(stParametros.memoryHandler);
	_endthreadex(0);
	return 0;
}




int main(){

	char tempEsperaSender[32];
	char tempPuerto[8];

	//THREAD CLIENTE
	stThreadParameters stParameters;
	unsigned threadProcesarRequest;
	HANDLE threadCliente; 

	//THREAD SENDER
	stSenderParameters stSender;
	unsigned threadProcesarSender;
	HANDLE threadSender;

	//CONFIGURACION PARA EL SENDER
	LPCSTR archivoConfiguracion = "../configuracion.ini";
	GetPrivateProfileString("configuracion","IPNNTP",0,stSender.ipNNTP,16,archivoConfiguracion);
	GetPrivateProfileString("configuracion","PUERTONNTP",0,tempPuerto,8,archivoConfiguracion);
	GetPrivateProfileString("configuracion","TIEMPO",0,tempEsperaSender,32,archivoConfiguracion);
	stSender.puertoNNTP = atoi(tempPuerto);
	stSender.tiempoEspera = atoi(tempEsperaSender);

	//CONFIGURACION THREAD CLIENTE INTERFAZ
	GetPrivateProfileString("configuracion","NEWSGROUP",0,stParameters.newsgroup,256,archivoConfiguracion);
	strcpy(stSender.newsgroup,stParameters.newsgroup); //el sender tambien necesita el nombre del newsgroup para levantar db

	if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&publisherFunction,(void*)&stParameters, 
		0, &threadProcesarRequest))!=0){
		CloseHandle(threadCliente);
	}else{
		printf("No se pudo crear el thread para procesar el request\n");
	}

	printf("Sale del thread cliente\n");

	
	while(1){	
		Sleep(stSender.tiempoEspera);
		if((threadSender = (HANDLE)_beginthreadex(NULL, 0,&senderFunction,(void*)&stSender, 
			0, &threadProcesarSender))!=0){
			CloseHandle(threadSender);
		}else{
			printf("No se pudo crear el thread para procesar el envío de xml\n");
		}
	}


/** Para probar lo de BERKELEY
	
	HANDLE* memoryHandler = HeapCreate( 0, 1024, 0); //esto debería ir en cada hilo
	DB* dbHandler;
	printf("<------------------- Berkeley aCtitud -------------------> \n");
	
	
	createDb(&dbHandler, &memoryHandler);
	putArticle(&dbHandler);	
	getArticle(&dbHandler);
	closeDb(&dbHandler);
	getchar();
	return 0;


**/

/**	

	Leo archivo de configuracion para saber el newsgroup
    Creo la base Berkeley para dicho newsgroup

    Creo el thread GUIthread
        - El usuario crea la noticia
        - Se inserta en la base, si no existía una noticia con ese newsgroup e id!
        - Queda corriendo esto (??), es decir, inserta la noticia le dice "che inserté", y se queda a la espera de mas insersiones (??), porque si lo finalizo dps como hago para levantar el otro thread (??)

    Creo el XMLsender cada x cantidad de tiempo
        - Se fija si hay noticias nuevas en la base berkeley del newsgroup
        - Si hay noticias nuevas, las pasa a formato XML
        - Se hace el handshake , y luego se envía el XML al NTTPNewsServerProcess
        - Si no hubo problemas marco la noticia en Berkeley como transmitted (1)
        - Finaliza el thread
**/
	Sleep(10000000);
	return 0;
}