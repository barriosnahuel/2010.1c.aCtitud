#include "berkeleyFunctions.h"
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <string.h>
#include "logger-win.h"

typedef struct stConfigParameters {
	int puertoNNTP;
	char ipNNTP[15+1];
	int tiempoEspera;
	char newsgroup[100];
} stConfigParameters;

#define BUFFERCADSIZE 9

char czNombreProceso[20];

void lecturaDinamica(char** cadena, HANDLE** handler){

	char tempCad[BUFFERCADSIZE+1];
	char car;
	int i,largo,size;
	largo = 0;
	*cadena[0]='\0';
	size=1;

	fflush(stdin);
	for(i=0;(car = getchar())!= '^' ;i++){
		tempCad[i]=car;
		if( i ==BUFFERCADSIZE){
			tempCad[BUFFERCADSIZE]='\0';
			size = size + BUFFERCADSIZE;
			HeapReAlloc(*handler,0,*cadena,(DWORD)size);
			strcat_s(*cadena,(DWORD)size,tempCad);
			tempCad[0]=car;
			i=0;
		}
	}
	if(i!=0){
		size = size + i;
		tempCad[i]='\0';
		HeapReAlloc(*handler,0,*cadena,(DWORD)size );
		strcat_s(*cadena,(DWORD)size,tempCad);
	}
	fflush(stdin);
}




unsigned __stdcall publisherFunction(void* threadParameters)
{
	struct news noticia;
	HANDLE* memoryHandler;
	DB* dbHandler;
	char *head;
	char *body;
	char respuesta;
	DWORD dwThreadId = GetCurrentThreadId();

	stConfigParameters stParametros = *((stConfigParameters*) threadParameters);
	memoryHandler =  HeapCreate(0,1024,0);
	dbHandler = NULL;

	respuesta = 'S';

	if( memoryHandler == NULL ) 
		LoguearError("HeapCreate error.");

	printf("<------------------- Ingreso de Noticia aCtitud -------------------> \n");

	while(respuesta == 'S'){
		head = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,2);
		ZeroMemory(head,2);
		printf("Ingrese el HEAD de la noticia: ");
		lecturaDinamica(&head,&memoryHandler);
		LoguearInformacion("Head ingresado:");
		LoguearInformacion(head);

		body = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,2);
		ZeroMemory(body,2);
		printf("Ingrese el body de la noticia: ");
		lecturaDinamica(&body,&(memoryHandler));
		LoguearInformacion("Body ingresado:");
		LoguearInformacion(body);

		noticia.largos.newsgrouplen = strlen(stParametros.newsgroup)+1;
		noticia.newsgroup = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.newsgrouplen);
		strcpy_s(noticia.newsgroup,noticia.largos.newsgrouplen,stParametros.newsgroup);

		noticia.largos.headlen = strlen(head)+1;
		noticia.head = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.headlen);
		strcpy_s(noticia.head,noticia.largos.headlen,head);

		noticia.largos.bodylen = strlen(body)+1;
		noticia.body = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.bodylen);
		strcpy_s(noticia.body,noticia.largos.bodylen,body);

		noticia.largos.transmittedlen = strlen("0")+1;
		noticia.transmitted = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.transmittedlen);
		strcpy_s(noticia.transmitted,2,"0");

		noticia.id = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,BERKELEY_ID_LEN);
		
		LoguearDebugging(noticia.newsgroup);
		LoguearDebugging(noticia.head);
		LoguearDebugging(noticia.body);
		LoguearDebugging(noticia.transmitted);	
		
			
		LoguearInformacion("<------------------- Acceso a db BERKELEY - aCtitud -------------------> \n");
		
		createDb(&dbHandler, &memoryHandler,stParametros.newsgroup);
		generateNewID(&dbHandler, &noticia.id);
		
		putArticle(&noticia,&dbHandler,&memoryHandler);	
		closeDb(&dbHandler);

		printf("Desea publicar otra noticia S/N:");
		scanf_s("%c",&respuesta,1);
		respuesta = toupper(respuesta);
		fflush(stdin);

		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,head);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,body);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.newsgroup);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.head);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.body);
		HeapFree(memoryHandler,HEAP_ZERO_MEMORY,noticia.transmitted);

	}

	LoguearInformacion("TERMINA DE PROCESAR EL THREAD CLIENTE");
	printf("-- Finalizado --\n");
	printf("-- aCtitud (c) 2010 --");
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

	LoguearInformacion("<------------------- SENDER - aCtitud ------------------->");
	memoryHandler =  HeapCreate(0,1024,0); 
	dbHandler = NULL;
	if( memoryHandler == NULL ) 
		LoguearError("Sender HeapCreate error.");
	
	createDb(&dbHandler,&memoryHandler,stParametros.newsgroup);
	
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

	strcpy_s(czNombreProceso,10,"Publisher\0");
	LoguearInformacion("--------- Publisher. Inicio OK.");

	if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&publisherFunction,(void*)&stSender, 0, &threadProcesarRequest))!= 0){
		CloseHandle(threadCliente);
	}else{
		LoguearError("No se pudo crear el thread para procesar el request.");
	}

	/* TODO nunca puede salir de este bucle. ¿cómo mato al cliente? */
	while(threadCliente != 0){
		Sleep(stSender.tiempoEspera);
		if((threadSender = (HANDLE)_beginthreadex(NULL, 0,&senderFunction,(void*)&stSender, 0, &threadProcesarSender))!= 0){
			CloseHandle(threadSender);
		}else{
			LoguearWarning("No se pudo crear el thread para procesar el envío de xml\n");
		}
	}
	Sleep(5000);
	return 0;
}