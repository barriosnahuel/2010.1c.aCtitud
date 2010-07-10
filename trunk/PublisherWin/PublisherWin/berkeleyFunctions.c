#include "berkeleyFunctions.h"
#include "logger-win.h"

void createDb(DB** dbp, HANDLE** memoryHandle,char* dbName)
{
	char* rutaDb;
	int ret;
	size_t tamanioRutaDb = strlen(dbName) + 10;
	
	rutaDb = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,tamanioRutaDb); 
	sprintf_s(rutaDb,tamanioRutaDb,"C:\\%s.db",dbName);

	LoguearInformacion("Database path ok.");
	LoguearInformacion(rutaDb);
	
	if (!(ret = db_create(&*dbp, NULL, 0))) {
		/*fprintf(stderr, "db_create: %s\n", db_strerror(ret));
		LoguearError("Fallo en db_create.");*/
	}
		
	if (!(ret = (*dbp)->open(*dbp,NULL, rutaDb , NULL, DB_BTREE, DB_CREATE, 0))){
		LoguearDebugging("ENTRA A  DBP->OPEN");
		//*dbp)->err(*dbp, ret, "%s", rutaDb);
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
	
	noticia = (news*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,sizeof(struct news)); 
	
	LoguearInformacion("Buscando noticias no enviadas.");
	
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
		
		CopyMemory(noticia->transmitted,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen+noticia->largos.headlen+noticia->largos.bodylen,noticia->largos.transmittedlen);

		if(strcmp(noticia->transmitted,"0")==0)
		{	//No fue trasmitida
			
			noticia->newsgroup = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.newsgrouplen);
			CopyMemory(noticia->newsgroup,(char*)data.data + sizeof(newslen), noticia->largos.newsgrouplen);
			
			noticia->head      = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.headlen);
			CopyMemory(noticia->head,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen,noticia->largos.headlen);

			noticia->body      = (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,noticia->largos.bodylen);
			CopyMemory(noticia->body,(char*)data.data+sizeof(newslen)+noticia->largos.newsgrouplen+noticia->largos.headlen,
								  noticia->largos.bodylen);

			
			LoguearInformacion("Key almacenada:");
			LoguearInformacion(key.data);
			LoguearInformacion("NewsGroupNoticia:");
			LoguearInformacion(noticia->newsgroup);
			LoguearInformacion("Head:");
			LoguearInformacion(noticia->head);
			
			//PASAR A FORMATO XML
			doc = crearXML(noticia,key.data);
			xmlDocDumpFormatMemory(doc,&memoriaXML,&tamanioXML,1);
			
			//ENVIAR A NNTP
			transmitioANNTPServer = enviarXML(memoriaXML,tamanioXML,ipNNTP,puertoNNTP,&*memoryHandle);
			
			if(transmitioANNTPServer == 0){ 
				//LA TENGO QUE VOLVER A GUARDAR PERO CON EL TRANSMITTED EN 1 !!!!!
				strcpy_s(noticia->transmitted,2,"1");
				CopyMemory((char*)data.data+ sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen+noticia->largos.bodylen,noticia->transmitted,noticia->largos.transmittedlen);
				dbCursor->put(dbCursor, &key, &data, DB_CURRENT);
	      }
		}
	
	
	}
	if ((ret = dbCursor->c_close(dbCursor)) != 0){
		//(*dbp)->err(*dbp, ret, "DBcursor->close");
		LoguearDebugging("DBcursor->close");
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

	strftime (*buffer, (DWORD)BERKELEY_ID_LEN, "%m%d%H%M%S", &timeinfo);

	//	Borro los ceros a la izquierda.
	while(**buffer=='0')
		*buffer= (*buffer)+1;

}

int lastID(DB** dbp)
{
	int ret;
	DBC* dbCursor;
	DBT key,data;
	int maxID = -1;
	memset (&data, 0, sizeof(data));
	memset (&key, 0, sizeof(key));
	
	LoguearDebugging("Buscando LastId");
	
	if ((ret = (*dbp)->cursor(*dbp,NULL,&dbCursor,0)) != 0) {
		(*dbp)->err(*dbp, ret, "DB->cursor");
	}
	while ((ret = dbCursor->c_get(dbCursor, &key, &data, DB_NEXT)) == 0){
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
	size_t idAuxLen;
	int ret;          
	DBT key, data;
	size_t noticiaEnBytesLargo;
	
	LoguearInformacion("Guardando noticia.");
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	
	
	//NOTICIA
	noticiaEnBytesLargo = sizeof(noticia->largos) + noticia->largos.newsgrouplen + noticia->largos.headlen + noticia->largos.bodylen + noticia->largos.transmittedlen;

	data.data = HeapAlloc(*memoryHandler,HEAP_ZERO_MEMORY,noticiaEnBytesLargo);
    data.size = (unsigned int)noticiaEnBytesLargo;
	
	//ID NOTICIA -- Hago esto porque la id me llega como entero, y dps el memcpy se me simplifica
	idAuxLen = strlen(noticia->id) + 1;
	key.data  = HeapAlloc(*memoryHandler,HEAP_ZERO_MEMORY,idAuxLen);
	CopyMemory((char*)key.data,noticia->id,idAuxLen);
	key.size = (unsigned int)idAuxLen;
	CopyMemory((char*)data.data,(char*)&noticia->largos,sizeof(noticia->largos));
	CopyMemory((char*)data.data + sizeof(noticia->largos),noticia->newsgroup,noticia->largos.newsgrouplen);
	CopyMemory((char*)data.data + sizeof(noticia->largos)+noticia->largos.newsgrouplen,noticia->head,noticia->largos.headlen);
	CopyMemory((char*)data.data + sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen,
		   noticia->body,noticia->largos.bodylen);
	CopyMemory((char*)data.data+ sizeof(noticia->largos)+noticia->largos.newsgrouplen+noticia->largos.headlen+noticia->largos.bodylen,noticia->transmitted,noticia->largos.transmittedlen);
	
	
	switch (ret = (*dbp)->put(*dbp, NULL, &key, &data, DB_NOOVERWRITE)) {
	case 0:
		LoguearInformacion("Articulo guardado:");
		break;
	case DB_KEYEXIST:
		LoguearWarning("El articulo ya se encontraba en la base.");
		break;
	default:
		(*dbp)->err(*dbp, ret, "DB->put");
	}

	HeapFree(*memoryHandler,HEAP_ZERO_MEMORY, data.data);
	HeapFree(*memoryHandler,HEAP_ZERO_MEMORY, key.data );

	return;
}


void closeDb(DB** dbp)
{
	(*dbp)->close(*dbp,0);
	return;
}
