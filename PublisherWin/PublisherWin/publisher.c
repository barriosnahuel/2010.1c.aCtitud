#include "berkeleyFunctions.h"
#include<windows.h>
typedef struct stIRC_IPC{
   char idDescriptor[16+1];
   char payloadDescriptor[1+1];
   char payloadLength[4+1];
   char payloadXML[1023+1]; // o char * (??) , si lo dejo en char* hasta donde hago el memcpy ?
}stIRC_IPC;

typedef struct stThreadParameters {
	DB* dbHandler;
	HANDLE* memoryHandler;
	char newsgroup[1024];
} stThreadParameters;

typedef struct stSenderParameters {
	DB* dbHandler;
	HANDLE* memoryHandler;
} stSenderParameters;

#define BUFFERSIZE 1024

void LecturaDinamica(char** cadena,HANDLE** memoryHandler)
{
	int i;
	char c;
	int tamanioInicial= sizeof(char);

	*cadena = (char*)HeapAlloc(*memoryHandler,HEAP_ZERO_MEMORY,tamanioInicial);
	
	for(i=0;(c=getchar())!='\n';i++)
	{
		
		*cadena		 = (char*)HeapReAlloc(*memoryHandler,HEAP_ZERO_MEMORY,*cadena,(2*tamanioInicial));
		tamanioInicial += 1;
		(*cadena)[i] = c;
		/*if( *cadena == NULL )
			printf( "HeapReAlloc error.");
		else*/
		
	}

	*cadena     = (char*)HeapReAlloc(*memoryHandler,HEAP_ZERO_MEMORY,*cadena,2*(tamanioInicial+1));
	(*cadena)[i]= '\0';
	//printf("Cadena dentro de la funcion: %s \n",*cadena);
	return;
}



unsigned __stdcall publisherFunction(void* threadParameters)
{
	struct news noticia;
	char head[BUFFERSIZE] = "NACE UN NUEVO BILL GATES";
	char body[BUFFERSIZE] = "LINUX NO EXISTIS";

	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);
	stParametros.memoryHandler =  HeapCreate(0,1024,0); //esto debería ir en cada hilo
	stParametros.dbHandler = NULL;
	
	if( stParametros.memoryHandler == NULL ) 
		printf("HeapCreate error.\n");

	printf("<------------------- Ingreso de Noticia aCtitud -------------------> \n");
	//VOY A USAR CAMPOS ESTATICOS PERO LA FUNCION LECTURADINAMICA ESTA AHIII DE SALIR JAJA
/*	printf("Ingrese el HEAD de la noticia:");
	gets(head);
	printf("Ingrese el BODY de la noticia:");
	gets(body);
*/	
	printf("HEAD LEIDO: %s \n",head);
	printf("BODY LEIDA: %s \n", body);
	noticia.largos.newsgrouplen = strlen(stParametros.newsgroup)+1;
	noticia.largos.headlen = strlen(head)+1;
	noticia.largos.bodylen = strlen(body)+1;
	noticia.largos.transmittedlen = strlen("0")+1;
	
	noticia.newsgroup = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,10/*noticia->largos.newsgrouplen*/);
	noticia.head	  = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.headlen);
	noticia.body	  = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.bodylen);
    noticia.transmitted = (char*)HeapAlloc(stParametros.memoryHandler,HEAP_ZERO_MEMORY,noticia.largos.transmittedlen);
	noticia.newsgroup = stParametros.newsgroup;
	noticia.head      = head;
	noticia.body	  = body;
	noticia.transmitted = "0" ; 

	
	printf("noticia.newsgroup : %s \n",noticia.newsgroup);
	printf("noticia.head : %s \n",noticia.head);
	printf("noticia.body : %s \n",noticia.body);
	printf("noticia.transmitted : %s \n",noticia.transmitted);	
	
		
	printf("<------------------- Acceso a db BERKELEY - aCtitud -------------------> \n");
	
	createDb(&stParametros.dbHandler, &stParametros.memoryHandler);
	noticia.id = lastID(&stParametros.dbHandler) + 1 ;
	putArticle(&noticia,&stParametros.dbHandler,&stParametros.memoryHandler);	
	//getArticle(&stParametros.dbHandler,&stParametros.memoryHandler);
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
	
	createDb(&stParametros.dbHandler, &stParametros.memoryHandler);
	
	//Se fija las noticias que no estan enviadas, las pasa a XML y las envia a nntp
	noticiasNoEnviadas(&stParametros.dbHandler, &stParametros.memoryHandler);	
	
	closeDb(&stParametros.dbHandler);
	
	return 0;
}




int main(){

	//Carga configuracion --> Ip y puerto del serividor
	/*struct stConfiguracion configuracion;
	LPCSTR archivoConfiguracion = "..\\configuracion.ini";
	GetPrivateProfileString("configuracion","IP", configuracion.szDefault,configuracion.serverIP,16,archivoConfiguracion);
	GetPrivateProfileString("configuracion","PUERTO", configuracion.szDefault,configuracion.appPort,6,archivoConfiguracion);
    cout<<"Puerto:"<<configuracion.appPort<<" IP:"<<configuracion.serverIP<<endl;
	*/
	unsigned int segundosEsperaSender;
	//THREAD CLIENTE
	stThreadParameters stParameters;
	unsigned threadProcesarRequest;
	HANDLE threadCliente; 
	//THREAD SENDER
	stSenderParameters stSender;
	unsigned threadProcesarSender;
	HANDLE threadSender;
	
	strcpy(stParameters.newsgroup,"LA NACION");
	if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&publisherFunction,(void*)&stParameters, 
		0, &threadProcesarRequest))!=0){
		CloseHandle(threadCliente);
	}else{
		printf("No se pudo crear el thread para procesar el request\n");
	}

	printf("Sale del thread cliente\n");

	segundosEsperaSender = 10000;
	while(1){
		Sleep(segundosEsperaSender);
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