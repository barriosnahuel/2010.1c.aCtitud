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
	char* newsgroup;
} stThreadParameters;

void LeerTeclado(char* szCadena,HANDLE** memoryHandler){
   
   char c;
   char* szAuxiliar;
   int LONGITUD_INICIAL =   15;
   int INCREMENTO       =   5;
   int iAllocSize=LONGITUD_INICIAL;
   int iAllocUsed=0;
   
   szCadena = malloc(LONGITUD_INICIAL*sizeof(char));
   if (szCadena==NULL) {
        return;
    }

    while ((c=getchar())!='\n') {
        if (iAllocUsed < iAllocSize-1) {
			szAuxiliar = HeapReAlloc( memoryHandler,HEAP_ZERO_MEMORY,0,(iAllocSize+INCREMENTO)*sizeof(char));
			szAuxiliar = realloc(szCadena,(iAllocSize+INCREMENTO)*sizeof(char));
            if (szAuxiliar==NULL) {
                free(szCadena);
                return;
            }
            iAllocSize+=INCREMENTO;
            szCadena = szAuxiliar;
        }
        szCadena[iAllocUsed++]=c;
    }

    szCadena[iAllocUsed]='\0';
/*    printf("szCadena: %s\n",szCadena);*/
    return;
}

unsigned __stdcall publisherFunction(void* threadParameters)
{
	
	char* head;
	stThreadParameters stParametros = *((stThreadParameters*) threadParameters);
	stParametros.memoryHandler =  HeapCreate( 0,1024, 0); //esto debería ir en cada hilo
	stParametros.dbHandler = NULL;
	
	if( stParametros.memoryHandler == NULL ) 
		printf("HeapCreate error.\n");
	
	printf("<------------------- Berkeley aCtitud -------------------> \n");
	printf("Ingrese el HEAD de la noticia:\n");
/*	LeerTeclado(&head,&stParametros.memoryHandler);
	printf("CADENA LEIDA: %s",head);
	getchar();
*/
	createDb(&stParametros.dbHandler, &stParametros.memoryHandler);
	putArticle(&stParametros.dbHandler);	
	getArticle(&stParametros.dbHandler);
	closeDb(&stParametros.dbHandler);
	
	
	printf("TERMINA DE PROCESAR EL THREAD CLIENTE\n");
	getchar();
	_endthreadex(0);
	return 0;
}


unsigned __stdcall senderFunction()
{
	printf("Captura de nuevas noticias, conversion a xml y envio\n");
	getchar();
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
	unsigned threadProcesarSender;
	HANDLE threadSender;

	if((threadCliente = (HANDLE)_beginthreadex(NULL, 0,&publisherFunction,(void*)&stParameters, 
		0, &threadProcesarRequest))!=0){
		CloseHandle(threadCliente);
	}else{
		printf("No se pudo crear el thread para procesar el request\n");
	}

	printf("Sale del thread cliente \n");
/*
	segundosEsperaSender = 3000;
	//while(1){
		Sleep(segundosEsperaSender);
		if((threadSender = (HANDLE)_beginthreadex(NULL, 0,&senderFunction,NULL, 
			0, &threadProcesarSender))!=0){
			CloseHandle(threadSender);
		}else{
			printf("No se pudo crear el thread para procesar el envío de xml\n");
		}
	//}

*/
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
	
	getchar();
	return 0;
}