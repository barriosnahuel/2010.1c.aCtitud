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

int Valida_IP(const char *ip) {
   int tam, cont, idx;
   char *ptr, ipaux[15+1];
   cont = 0;
   idx= 0;

   if (!ip) return 0;
   strcpy(ipaux, ip);
   ptr = strtok(ipaux, "." );
   while(ptr) {
      tam = strlen(ptr);
      if ( tam < 1 || tam > 3 ) return 0;					/*	Se valida que la longitud sea de 1 a 3	*/

      /*	Valido que cada caracter sea un numero, y no haya letras	*/
      for(idx= 0; idx<tam; idx++){
    	  if(!isdigit(*(ptr+idx)))
    		  return 0;
      }

      if ( atoi(ptr) < 0 || atoi(ptr) > 255 ) return 0;		/*	Se valida que sea un numero entre 0-255*/
      ptr = strtok ( NULL, "." );
      cont = cont + 1;
   }
   if (cont < 4) return 0;									/*	Valido que la IP tenga al menos 4 partes.	*/
   return 1;
}

int ValidaNumero(const char *buffer, int chequeaSigno) {
	int idx= 0;

	/*	Si hay que validar que el numero puede tener signo, entonces valido que el primer caracter sea -/+	*/
	if(chequeaSigno==1){
		idx= 1;

		if(!isdigit(*buffer) && *buffer!='-' && *buffer!='+')
			return 0;
	}
	
	/*	Valido que cada caracter sea un numero, y no haya letras	*/
	while(idx<strlen(buffer)){
		if(!isdigit(*(buffer+idx)))
			return 0;

		idx++;
	}
	
	return 1;
}

unsigned __stdcall publisherFunction(void* threadParameters)
{
	struct news noticia;
	HANDLE* memoryHandler;
	DB* dbHandler;
	char *head;
	char *body;
	char respuesta;
	int respuestaCorrecta= 0;
	DWORD dwThreadId = GetCurrentThreadId();

	stConfigParameters stParametros = *((stConfigParameters*) threadParameters);
	memoryHandler =  HeapCreate(0,0,0);
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

/*
	Asi estaba antes.
		printf("Desea publicar otra noticia S/N:");
		scanf_s("%c",&respuesta,1);
		respuesta = toupper(respuesta);
		fflush(stdin);
*/

		/*	Asi lo deje.	*/
		while(!respuestaCorrecta){
			printf("Desea publicar otra noticia S/N:");
			scanf_s("%c", &respuesta, 1);
			respuesta = toupper(respuesta);
			fflush(stdin);
			if(respuesta=='S' || respuesta=='N')
				respuestaCorrecta= 1;
			else
				respuestaCorrecta= 0;
		}
		respuestaCorrecta= 0;/*	Cuando salgo, tengo que volver a ponerlo en 0, para que vuelva a entrar por "primera vez" luego de haber ingresado la n+1 noticia.	*/

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
	int hayErrorDeConfiguracion= 0;
	stConfigParameters stSender;

	/* THREAD CLIENTE */
	unsigned threadProcesarRequest;
	HANDLE threadCliente; 

	/* THREAD SENDER */
	unsigned threadProcesarSender;
	HANDLE threadSender;

	/* CONFIGURACION */
	LPCSTR archivoConfiguracion = ".\\PublisherWin.ini";

	/*	Esto es para el log	*/
	strcpy_s(czNombreProceso,10,"Publisher\0");

	/*	Levanto las propiedades del archivo de configuracion y las valido	*/
	GetPrivateProfileString("configuracion","IPNNTP",0,stSender.ipNNTP,16,archivoConfiguracion);
	if(!Valida_IP(stSender.ipNNTP)){
		LoguearError("Archivo de configuracion incorrecto, la IP del NNTP no esta bien formada.");
		hayErrorDeConfiguracion= 1;
	}

	GetPrivateProfileString("configuracion","PUERTONNTP",0,tempPuerto,8,archivoConfiguracion);
	if(!ValidaNumero(tempPuerto, 0)){
		LoguearError("Archivo de configuracion incorrecto, el puerto de NNTP no esta bien formado.");
		hayErrorDeConfiguracion= 1;
	}
	else
		stSender.puertoNNTP = atoi(tempPuerto);

	GetPrivateProfileString("configuracion","TIEMPO",0,tempEsperaSender,32,archivoConfiguracion);
	if(!ValidaNumero(tempEsperaSender, 1)){
		LoguearError("Archivo de configuracion incorrecto, El tiempo de intervalo no esta bien formado.");
		hayErrorDeConfiguracion= 1;
	}
	else {
		stSender.tiempoEspera = atoi(tempEsperaSender);
		if(stSender.tiempoEspera<=0){
			LoguearError("Archivo de configuracion incorrecto, El tiempo de intervalo debe ser mayor a cero.");
			hayErrorDeConfiguracion= 1;
		}
	}
	GetPrivateProfileString("configuracion","NEWSGROUP",0,stSender.newsgroup,100,archivoConfiguracion);

	if(hayErrorDeConfiguracion){
		printf("\n\n--\nEl archivo de configuracion contiene errores, corrijalos y vuelva a iniciar el Publisher.\taCtitud.\n\n");
	
		system("PAUSE");	/*	Esto es para que el usuario tenga que tocar una tecla para cerrar la consola.	*/
		return 0;
	}
	
	LoguearInformacion("Archivo de configuracion cargado correctamente con los valores:");
	LoguearInformacion("IPNNTP:");
	LoguearInformacion(stSender.ipNNTP);
	LoguearInformacion("Puerto NNTP Server (XML News Process Server):");
	LoguearInformacion(tempPuerto);
	LoguearInformacion("Intervalo de tiempo: ");
	LoguearInformacion(tempEsperaSender);
	LoguearInformacion("Nombre del newsgroup:");
	LoguearInformacion(stSender.newsgroup);

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