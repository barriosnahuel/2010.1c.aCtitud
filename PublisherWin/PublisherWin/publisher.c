#include "berkeleyFunctions.h"
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <string.h>

typedef struct stConfigParameters {
	int puertoNNTP;
	char ipNNTP[15+1];
	int tiempoEspera;
	char newsgroup[100];
} stConfigParameters;

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

	for(i=0;(car = getchar())!= '^' ;i++){
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
	HANDLE* memoryHandler;
	DB* dbHandler;
	char *head;
	char *body;
	char respuesta[100];
	DWORD dwThreadId = GetCurrentThreadId();

	stConfigParameters stParametros = *((stConfigParameters*) threadParameters);
	memoryHandler =  HeapCreate(0,1024,0);
	dbHandler = NULL;

	strcpy(respuesta,"S");

	if( memoryHandler == NULL ) 
		printf("HeapCreate error.\n");

	printf("<------------------- Ingreso de Noticia aCtitud -------------------> \n");

	while(strcmp(respuesta,"S")==0){
		head = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,2);
		ZeroMemory(head,2);
		printf("Ingrese el HEAD de la noticia: ");
		lecturaDinamica(&head,&memoryHandler);
		printf("HEAD INGRESADO: %s",head);
		//fflush(stdin);

		body = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,2);
		ZeroMemory(body,2);
		printf("Ingrese el body de la noticia: ");
		lecturaDinamica(&body,&(memoryHandler));
		printf("body INGRESADO: %s",body);
		//fflush(stdin);

		noticia.largos.newsgrouplen = strlen(stParametros.newsgroup)+1;
		noticia.newsgroup = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.newsgrouplen);
		strcpy(noticia.newsgroup,stParametros.newsgroup);

		noticia.largos.headlen = strlen(head)+1;
		noticia.head	  = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.headlen);
		strcpy(noticia.head,head);

		noticia.largos.bodylen = strlen(body)+1;
		noticia.body	  = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.bodylen);
		strcpy(noticia.body,body);

		noticia.largos.transmittedlen = strlen("0")+1;
		noticia.transmitted = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.transmittedlen);
		strcpy(noticia.transmitted,"0");

		noticia.largos.idlen= BERKELEY_ID_LEN;
		noticia.id = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.idlen);
		
		printf("noticia.newsgroup: %s \n",noticia.newsgroup);
		printf("noticia.head: %s \n",noticia.head);
		printf("noticia.body: %s \n",noticia.body);
		printf("noticia.transmitted: %s \n",noticia.transmitted);	
		
			
		printf("<------------------- Acceso a db BERKELEY - aCtitud -------------------> \n");
		
		createDb(&dbHandler, &memoryHandler,&stParametros.newsgroup);
		generateNewID(&dbHandler, &noticia.id);
		
		putArticle(&noticia,&dbHandler,&memoryHandler);	
		closeDb(&dbHandler);

		printf("Desea publicar otra noticia S/N:");
		scanf("%s",&respuesta);
		fflush(stdin);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,head);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,body);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.newsgroup);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.head);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.body);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.transmitted);

	}

	printf("TERMINA DE PROCESAR EL THREAD CLIENTE\n");
	HeapDestroy(memoryHandler);
	_endthreadex(dwThreadId);
	return 0;
}


unsigned __stdcall senderFunction(void* threadParameters)
{
	/**
	 * Busco en Berkeley noticias con 0 , ya que no estan transmitidas.
	 * Las paso a formato XML
	 * Las envio al NNTP 
	**/
	stConfigParameters stParametros = *((stConfigParameters*) threadParameters);
	HANDLE* memoryHandler;
	DB* dbHandler;
	DWORD dwThreadId = GetCurrentThreadId();

	printf("<------------------- SENDER - aCtitud -------------------> \n");
	memoryHandler =  HeapCreate(0,1024,0); 
	dbHandler = NULL;
	if( memoryHandler == NULL ) 
		printf("Sender HeapCreate error.\n");
	
	createDb(&dbHandler,&memoryHandler,&stParametros.newsgroup);
	
	//Se fija las noticias que no estan enviadas, las pasa a XML y las envia a nntp
	noticiasNoEnviadas(&dbHandler, &memoryHandler,(char*)stParametros.ipNNTP,stParametros.puertoNNTP);

	closeDb(&dbHandler);
	HeapDestroy(memoryHandler);
	_endthreadex(dwThreadId);
	return 0;
}

int main(){

	char tempEsperaSender[32];
	char tempPuerto[8];
	stConfigParameters stSender;

	/* THREAD CLIENTE */
	unsigned threadProcesarRequest;
	HANDLE threadCliente; 

	/* THREAD SENDER */
	unsigned threadProcesarSender;
	HANDLE threadSender;

	/* CONFIGURACION */
	LPCSTR archivoConfiguracion = ".\\PublisherWin.ini";
	GetPrivateProfileString("configuracion","IPNNTP",0,stSender.ipNNTP,16,archivoConfiguracion);
	GetPrivateProfileString("configuracion","PUERTONNTP",0,tempPuerto,8,archivoConfiguracion);
	GetPrivateProfileString("configuracion","TIEMPO",0,tempEsperaSender,32,archivoConfiguracion);
	GetPrivateProfileString("configuracion","NEWSGROUP",0,stSender.newsgroup,100,archivoConfiguracion);
	stSender.puertoNNTP = atoi(tempPuerto);
	stSender.tiempoEspera = atoi(tempEsperaSender);

	if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&publisherFunction,(void*)&stSender, 0, &threadProcesarRequest))!= 0){
		CloseHandle(threadCliente);
	}else{
		printf("No se pudo crear el thread para procesar el request\n");
	}

	printf("Sale del thread cliente\n");

	/* TODO nunca puede salir de este bucle. ¿cómo mato al cliente? */
	while(1){
		Sleep(stSender.tiempoEspera);
		if((threadSender = (HANDLE)_beginthreadex(NULL, 0,&senderFunction,(void*)&stSender, 0, &threadProcesarSender))!= 0){
			CloseHandle(threadSender);
		}else{
			printf("No se pudo crear el thread para procesar el envío de xml\n");
		}
	}

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