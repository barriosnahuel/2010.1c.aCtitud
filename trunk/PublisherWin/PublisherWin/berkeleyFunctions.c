#include "berkeleyFunctions.h"

void createDb(DB** dbp, HANDLE** memoryHandle,char* dbName)
{
	char* ruta = "C:\\";
	//char* dbName = "aCtitud10.db";	//Aca debe levantar el nombre del newsgroup
	char* extension=".db";
	char* rutaDb = NULL;
	int ret;
	int tamanioRutaDb = strlen(ruta) + strlen(dbName)+strlen(extension)+1;
	
	rutaDb= (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,tamanioRutaDb); 
	printf("strlen(%s): %d \n",ruta,strlen(ruta));
	printf("strlen(%s): %d \n",dbName,strlen(dbName));
	strcat(rutaDb,ruta);	
	strcat(rutaDb,dbName);
	strcat(rutaDb,extension);
	printf("rutaDB: %s \n", rutaDb);
	printf("strlen(%s): %d \n",rutaDb,strlen(rutaDb));
	
	if (!(ret = db_create(&*dbp, NULL, 0))) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
	}
		
	if (!(ret = (*dbp)->open(*dbp,NULL, rutaDb , NULL, DB_BTREE, DB_CREATE, 0))){
		printf("ENTRA A  DBP->OPEN \n");
		(*dbp)->err(*dbp, ret, "%s", rutaDb);
	}
	
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY, rutaDb );
	return;
}

void noticiasNoEnviadas(DB** dbp,HANDLE** memoryHandle, char* ipNNTP, int puertoNNTP)
{
	int ret;
	struct news* noticia;
	int transmitioANNTPServer;
	//berkeley
	DBC* dbCursor;
	DBT key,data;
	//XML
	xmlDocPtr doc;
	xmlChar* memoriaXML; //contiene el XML ARMADO
	int tamanioXML;
			
	memset (&data, 0, sizeof(data));
	memset (&key, 0, sizeof(key));
	
	noticia = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,sizeof(struct news)); 
	
	printf("\n ####### Noticias No enviadas ####### \n");
	
	//Creo el cursor de Berkeley
	if ((ret = (*dbp)->cursor(*dbp,NULL,&dbCursor,0)) != 0) {
		(*dbp)->err(*dbp, ret, "DB->cursor");
	}

	while ((ret = dbCursor->c_get(dbCursor, &key, &data, DB_NEXT)) == 0)
	{
		//Obtengo el tamañano de cada campo
		CopyMemory(&noticia->largos,(char*)data.data,sizeof(newslen));

		//Solo trabajo con aquellas que no estan trasmitidas.
		noticia->transmitted = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.transmittedlen);
		
		CopyMemory(noticia->transmitted,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen+noticia->largos.headlen+
			   noticia->largos.bodylen,noticia->largos.transmittedlen);

		if(strcmp(noticia->transmitted,"0")==0)
		{	//No fue trasmitida
			
			noticia->newsgroup = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.newsgrouplen);
			CopyMemory(noticia->newsgroup,(char*)data.data + sizeof(newslen), noticia->largos.newsgrouplen);
			
			noticia->head      = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.headlen);
			CopyMemory(noticia->head,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen,noticia->largos.headlen);

			noticia->body      = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.bodylen);
			CopyMemory(noticia->body,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen+noticia->largos.headlen,
								  noticia->largos.bodylen);

			
			printf("Key almacenada   : %s  \n",key.data);
			printf("NewsGroupNoticia : %s \n",noticia->newsgroup);
			printf("Head : %s \n",noticia->head);
			printf("Body : %s \n",noticia->body);
			
			//PASAR A FORMATO XML
			doc = crearXML(noticia,key.data);
			xmlDocDumpFormatMemory(doc,&memoriaXML,&tamanioXML,1);
			printf("TAMANIO XML: %d", tamanioXML);
			
			//ENVIAR A NNTP
			transmitioANNTPServer = enviarXML(memoriaXML,tamanioXML,ipNNTP,puertoNNTP,&*memoryHandle);
			
			if(transmitioANNTPServer == 0){ 
				//LA TENGO QUE VOLVER A GUARDAR PERO CON EL TRANSMITTED EN 1 !!!!!
				strcpy(noticia->transmitted,"1");
				CopyMemory((char*)data.data+ sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen+noticia->largos.bodylen,
				noticia->transmitted,noticia->largos.transmittedlen);
				dbCursor->put(dbCursor, &key, &data, DB_CURRENT);
	      }
		}
	
	
	}
	if ((ret = dbCursor->c_close(dbCursor)) != 0){
		(*dbp)->err(*dbp, ret, "DBcursor->close");
	}
	
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,noticia->newsgroup);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,noticia->head);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,noticia->body);
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY,noticia);
	
	return;
}

/**
 *	Genero un nuevo ID con el formato: MMddHHmmss
 *	Si el mes por ejemplo es 07 (Julio), entonces el cero a la izquierda se saca.
 *	El formato para la funcion strftime, es similar al de printf, lo saque de:
 *	http://www.cplusplus.com/reference/clibrary/ctime/strftime/
 */
void generateNewID(DB** dbp, char** buffer){
	time_t rawtime;
	struct tm timeinfo;
	
	time (&rawtime);
    _localtime64_s(&timeinfo,&rawtime);

	strftime (*buffer, 11, "%m%d%H%M%S", &timeinfo);//	El 11 es la longitud, son 10 mas el \0, Deberia haber usado la constante BERKELEY_ID_LEN, pero pincha.

	//	Borro los ceros a la izquierda.
	while(**buffer=='0')
		*buffer= (*buffer)+1;

	printf("+++++++++++++++++++++++++++\n");
	printf("El nuevo ID generado es: %s\n", *buffer);
	printf("+++++++++++++++++++++++++++\n");
}

int lastID(DB** dbp)
{
	int ret;
	DBC* dbCursor;
	DBT key,data;
	int maxID = -1;
	memset (&data, 0, sizeof(data));
	memset (&key, 0, sizeof(key));
	
	printf("\n ####### LASTID ####### \n");
	
	if ((ret = (*dbp)->cursor(*dbp,NULL,&dbCursor,0)) != 0) {
		(*dbp)->err(*dbp, ret, "DB->cursor");
	}
	while ((ret = dbCursor->c_get(dbCursor, &key, &data, DB_NEXT)) == 0){
		printf("KEY ALMACENADA : %s  \n",key.data);
		if(maxID<atoi(key.data))
			maxID = atoi(key.data);
		
	}
	if(key.data==NULL)
		return 0; //Para generar la ID la primera vez

	if ((ret = dbCursor->c_close(dbCursor)) != 0){
		(*dbp)->err(*dbp, ret, "DBcursor->close");
	}
	return maxID;
}

void putArticle(struct news* noticia,DB** dbp,HANDLE** memoryHandler)
{
	int idAuxLen,ret;          
	DBT key, data;
	unsigned int noticiaEnBytesLargo;
	
	printf("########### putArticle ###########\n");
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	
	
	//NOTICIA
	noticiaEnBytesLargo = sizeof(noticia->largos) + noticia->largos.newsgrouplen + 
		                  noticia->largos.headlen + noticia->largos.bodylen + noticia->largos.transmittedlen;

	data.data = HeapAlloc(*memoryHandler,HEAP_ZERO_MEMORY,noticiaEnBytesLargo);
    data.size = noticiaEnBytesLargo;
	
	//ID NOTICIA -- Hago esto porque la id me llega como entero, y dps el memcpy se me simplifica
	//itoa(noticia->id,idAux,10);
	//idAuxLen = strlen(idAux) + 1;
	idAuxLen = strlen(noticia->id) + 1;
	key.data  = HeapAlloc(*memoryHandler,HEAP_ZERO_MEMORY,idAuxLen);		
	//CopyMemory((char*)key.data,idAux,idAuxLen);
	CopyMemory((char*)key.data,noticia->id,idAuxLen);
	key.size = idAuxLen;
	printf("^^^^^^^^^^^^^^^^^^ key.data= %s\n", (char*)key.data);
	printf("^^^^^^^^^^^^^^^^^^ key.size= %d\n", key.size);
	CopyMemory((char*)data.data,(char*)&noticia->largos,sizeof(noticia->largos));
	CopyMemory((char*)data.data + sizeof(noticia->largos),noticia->newsgroup,noticia->largos.newsgrouplen);
	CopyMemory((char*)data.data + sizeof(noticia->largos)+noticia->largos.newsgrouplen,noticia->head,noticia->largos.headlen);
	CopyMemory((char*)data.data + sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen,
		   noticia->body,noticia->largos.bodylen);
	CopyMemory((char*)data.data+ sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen+noticia->largos.bodylen,
		   noticia->transmitted,noticia->largos.transmittedlen);
	
	
	switch (ret = (*dbp)->put(*dbp, NULL, &key, &data, DB_NOOVERWRITE)) {
	case 0:
		printf("db: %s: Articulo guardado.\n", (char *)key.data);
		break;
	case DB_KEYEXIST:
		printf("db: %s: El articulo ya se encontraba en la base.\n",(char*)key.data);
		break;
	default:
		(*dbp)->err(dbp, ret, "DB->put");
	}

	HeapFree(*memoryHandler,HEAP_ZERO_MEMORY, data.data);
	HeapFree(*memoryHandler,HEAP_ZERO_MEMORY, key.data );

	/*
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = "34037030";
	key.size = strlen(key.data) + 1 ;
	data.data = "Alan";
	data.size = strlen(data.data) + 1;

	switch (ret = (*dbp)->put(*dbp, NULL, &key, &data, DB_NOOVERWRITE)) {
	case 0:
		printf("db: %s: Articulo guardado.\n", (char *)key.data);
		break;
	case DB_KEYEXIST:
		printf("db: %s: El articulo ya se encontraba en la base.\n",(char*)key.data);
		break;
	default:
		(*dbp)->err(dbp, ret, "DB->put");
	}
	*/
	return;
}

void getArticle(DB** dbp)
{
	int ret;          
	DBT key, data;

	printf("############## Lee en BerkeleyDB ##############\n");
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = "1";
	key.size = strlen(key.data) + 1 ;
/*	data.data = "Alan";
	data.size = strlen(data.data) + 1;
*/	
	if (!(ret = (*dbp)->get(*dbp, NULL, &key, &data, 0)))
		printf("db: %s: key encontrada: datos: %s.\n",(char *)key.data, (char *)data.data);
	else 
		(*dbp)->err(*dbp, ret, "DB->get");

	return;
}
void closeDb(DB** dbp)
{
	(*dbp)->close(*dbp,0);
	return;
}

/**
int main()
{
	/**
		apCreate(0,             // sin banderas: por defecto es sincronizado
                 15 * 1024,     // compromiso inicial: 15 KB
                 25 * 1024 );   // reserva inicial y tamaño máximo:
	**/
/**	HANDLE* memoryHandler = HeapCreate( 0, 1024, 0); //esto debería ir en cada hilo
	DB* dbHandler;
	printf("<------------------- Berkeley aCtitud -------------------> \n");
	
	
	createDb(&dbHandler, &memoryHandler);
	putArticle(&dbHandler);	
	getArticle(&dbHandler);
	closeDb(&dbHandler);
	getchar();
	return 0;
}

**/